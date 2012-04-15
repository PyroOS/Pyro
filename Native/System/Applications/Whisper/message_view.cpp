/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */

#include <message_view.h>
#include <messages.h>
#include <mail.h>
#include <qp_codec.h>
#include <base64_codec.h>
#include <strfuncs.h>
#include <resources/Whisper.h>

#include <debug.h>

#include <gui/guidefines.h>
#include <gui/font.h>
#include <gui/image.h>
#include <util/message.h>
#include <util/resources.h>
#include <util/application.h>
#include <util/settings.h>
#include <storage/registrar.h>
#include <storage/memfile.h>

#include <algorithm>
#include <stdlib.h>

/* Mouse-cursor I-beam */
#define POINTER_WIDTH  7
#define POINTER_HEIGHT 14
static uint8 g_anMouseImg[] = {
	0x02, 0x02, 0x02, 0x00, 0x02, 0x02, 0x02,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x02, 0x02, 0x02, 0x00, 0x02, 0x02, 0x02
};

MessageView::InnerView::InnerView( Rect cFrame, const Handler *pcAppWindow, const Settings *pcVisualSettings ) : View( cFrame, "message_inner_view", CF_FOLLOW_ALL )
{
	m_pcMessage = NULL;

	m_pcAppWindow = pcAppWindow;
	m_pcVisualSettings = pcVisualSettings;
	m_pcMessenger = new Messenger( m_pcAppWindow );

	m_cSize.x = cFrame.Width();
	m_cSize.y = cFrame.Height();

	m_vContentHeight = 0;
	m_bIBeamActive = false;

	Rect cStringFrame;
	Point cStringSize;

	cStringFrame = cFrame;
	cStringFrame.top += 10;
	cStringFrame.left += 10;
	cStringFrame.right -= 10;
	cStringFrame.bottom = cStringFrame.top + 20;

	SetBgColor( get_default_color( COL_SHINE ) );
	SetEraseColor( get_default_color( COL_SHINE ) );
	SetFgColor( get_default_color( COL_MENU_TEXT ) );

	m_pcSubjectView = new StringView( cStringFrame, "message_subject", "<Subject>" );
	cStringSize = m_pcSubjectView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcSubjectView->SetFrame( cStringFrame );

	Font *pcFont;
	font_properties cFontProperties;

	pcFont = new Font();
	pcFont->GetDefaultFont( DEFAULT_FONT_BOLD, &cFontProperties );
	cFontProperties.m_vSize += 4.0f;
	pcFont->SetProperties( cFontProperties );
	m_pcSubjectView->SetFont( pcFont );
	pcFont->Release();

	AddChild( m_pcSubjectView );

	cStringFrame.top = cStringFrame.bottom + 5;
	m_pcFromView = new StringView( cStringFrame, "message_from", "<From>" );
	cStringSize = m_pcFromView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcFromView->SetFrame( cStringFrame );

	pcFont = new Font();
	pcFont->GetDefaultFont( DEFAULT_FONT_REGULAR, &cFontProperties );
	m_vDefualtFontHeight = cFontProperties.m_vSize;
	cFontProperties.m_vSize -= 1.0f;
	pcFont->SetProperties( cFontProperties );
	m_pcFromView->SetFont( pcFont );

	AddChild( m_pcFromView );

	pcFont->Release();

	cStringFrame.top = cStringFrame.bottom + 5;
	m_pcToView = new StringView( cStringFrame, "message_to", "<To>" );
	cStringSize = m_pcToView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcToView->SetFrame( cStringFrame );
	AddChild( m_pcToView );

	m_vContentHeight = cStringFrame.bottom + 5;

	m_pcAttachmentView = NULL;
}

MessageView::InnerView::~InnerView()
{
	RemoveChild( m_pcSubjectView );
	delete( m_pcSubjectView );

	RemoveChild( m_pcFromView );
	delete( m_pcFromView );

	RemoveChild( m_pcToView );
	delete( m_pcToView );

	std::vector<ChildView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
	{
		RemoveChild( (*i) );
		delete( (*i) );
	}
	m_vViews.clear();

	if( m_pcMessage )
		delete( m_pcMessage );

	if( m_pcMessenger )
		delete( m_pcMessenger );
}

Point MessageView::InnerView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return m_cSize;
	else
		return View::GetPreferredSize( false );
}

void MessageView::InnerView::FrameSized( const Point &cDelta )
{
	_Layout();
}

void MessageView::InnerView::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );

	if( m_bDrawSeperator && m_vSeperatorFrame.DoIntersect( cUpdateRect ) )
		DrawLine( Point( m_vSeperatorFrame.left, m_vSeperatorFrame.bottom ),  Point( m_vSeperatorFrame.right, m_vSeperatorFrame.bottom ) );
}

void MessageView::InnerView::HandleMessage( Message *pcMessage )
{
	/* Messages sent by any AttachmentViews end up here. */
	switch( pcMessage->GetCode() )
	{
		case ID_ATTACHMENT_SAVE:
		{
			int64 nId = 0;
			int i = 0;
			while( pcMessage->FindInt64( "id", &nId, i++ ) == EOK )
				_SaveAttachment( nId );
			break;
		}

		case ID_ATTACHMENT_OPEN:
		{
			int64 nId = 0;
			int i = 0;
			while( pcMessage->FindInt64( "id", &nId, i++ ) == EOK )
				_OpenAttachment( nId );
			break;
		}

		case M_SAVE_REQUESTED:
		{
			const char* pzFilename;
			int64 nId = 0;

			if( pcMessage->FindString( "file/path", &pzFilename ) == EOK && pcMessage->FindInt64( "id", &nId ) == EOK )
				_DoSaveAttachment( nId, pzFilename );

			FileRequester *pcRequester;
			if( pcMessage->FindPointer( "source", (void**)&pcRequester ) == EOK )
				pcRequester->Close();

			break;
		}

		case M_FILE_REQUESTER_CANCELED:
		{
			FileRequester *pcRequester;
			if( pcMessage->FindPointer( "source", (void**)&pcRequester ) == EOK )
				pcRequester->Close();

			break;
		}

		case ID_ATTACHMENT_BEGIN_DRAG:
		{
			Message *pcReply = new Message( ID_ATTACHMENT_BEGIN_DRAG );
			String cFilename;
			int64 nId = 0;
			int i = 0;
			while( pcMessage->FindInt64( "id", &nId, i++ ) == EOK )
			{
				debug( "got drag item #%d\n", i );
				_SaveAttachmentTmp( nId, cFilename );
				pcReply->AddString( "file/path", cFilename );
				pcReply->AddInt64( "id", nId );
			}
			pcMessage->SendReply( pcReply );
			break;
		}

		case ID_EDIT_COPY:
		{
			Copy();
			break;
		}

		case M_ENABLE_EDIT_COPY:
		{
			m_pcMessenger->SendMessage( pcMessage );
			break;
		}

		default:
			View::HandleMessage( pcMessage );
	}
}

void MessageView::InnerView::MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData )
{
	/* XXXKV: I'd prefer to have this handled by the FlowView, which is the only View where
	   the IBeam is valid. The problem is that it does not work as intended. InnerView is the
	   "highest" View in the stack that recieves ENTERED & EXITED messages on all four edges.
	   The downside to this is that the IBeam is on even when the mouse is not over a FlowView.
	   I don't see an easy fix for this. */

	if( nCode == MOUSE_ENTERED )
	{
		if( m_bIBeamActive == false )
		{
			Application::GetInstance()->PushCursor( MPTR_MONO, g_anMouseImg, POINTER_WIDTH, POINTER_HEIGHT, IPoint( POINTER_WIDTH / 2, POINTER_HEIGHT / 2 ) );
			m_bIBeamActive = true;
		}
	}
	else if( nCode == MOUSE_EXITED )
	{
		if( m_bIBeamActive )
		{
			m_bIBeamActive = false;
			Application::GetInstance()->PopCursor();
		}
	}
}

void MessageView::InnerView::Display( Mailmessage *pcMessage )
{
	if( m_pcMessage )
		delete( m_pcMessage );
	m_pcMessage = pcMessage;

	/* Reset the view position before the new message is displayed.  This
	   ensures that the InnerView is always at the correct position when
	   it's cleared and the new message drawn. */
	if( GetScrollOffset().y < 0 )
		ScrollTo( Point( 0, 2 ) );

	std::vector<ChildView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
	{
		RemoveChild( (*i) );
		delete( (*i) );
	}
	m_vViews.clear();
	m_pcAttachmentView = NULL;

	m_pcSubjectView->SetString( m_pcMessage->GetSubject() );
	m_pcFromView->SetString( m_pcMessage->GetFrom() );
	m_pcToView->SetString( String( "To: " ) + m_pcMessage->GetTo() );

	m_bDrawSeperator = true;

	for( int j = 0; j < m_pcMessage->GetPartCount(); j++ )
	{
		Multipart cPart = m_pcMessage->GetPartInfo(j);
		switch( cPart.eDisposition )
		{
			case BODY:
			case BODY_ALTERNATIVE:
			{
				if( cPart.eDisposition == BODY )
					debug( "part #%d is BODY.\n", j );
				else
					debug( "part #%d is BODY_ALTERNATIVE.\n", j );

				Multipart cBody = m_pcMessage->GetPart( j );
				AddBody( cBody );
				break;
			}

			case BODY_SIGNED:
			{
				debug( "part #%d is BODY_SIGNED.\n", j );
				AddSigned( cPart );
				break;
			}

			case BODY_ALTERNATIVE_HIDDEN:
			{
				debug( "part #%d is BODY_ALTERNATIVE_HIDDEN.\n", j );
				break;
			}

			case INLINE:
			{
				debug( "part #%d is INLINE.\n", j );
				AddInline( cPart );
				break;
			}

			case ATTACHMENT:
			{
				debug( "part #%d is ATTACHMENT.\n", j );

				/* If the part is an image and "Always inline images" is on, treat it as inline */
				if( cPart.cMimeType.cSuperType == "image" && m_pcVisualSettings->GetBool( "inline_images", false ) )
				{
					debug( "treating image as INLINE\n" );
					AddInline( cPart );
				}
				else
					AddAttachment( cPart );

				break;
			}

			case MULTIPART:
			{
				debug( "part #%d is MULTIPART.\n", j );
				AddMultipart( cPart );
				break;
			}

			case ENCRYPTION_KEY:
			{
				debug( "part #%d is ENCRYPTION_KEY.\n", j );
				break;
			}

			default:
			{
				debug( "part #%d is unknown.\n", j );
				break;
			}
		}
	}

	/* Recalculate and redraw */
	_Layout();
}

void MessageView::InnerView::AddBody( Multipart &cPart )
{
	char *pzRawBody = NULL, *pzBody = NULL;

	if( cPart.GetDataSize() > 0 )
	{
		size_t nSize;

		if( cPart.cEncoding == "quoted-printable" )
		{
			QpCodec cCodec;
			nSize = cCodec.Decode( cPart.GetData(), cPart.GetDataSize(), &pzRawBody );
		}
		else if( cPart.cEncoding == "base64" )
		{
			Base64Codec cCodec;
			nSize = cCodec.Decode( cPart.GetData(), cPart.GetDataSize(), &pzRawBody );
		}
		else
		{
			nSize = cPart.GetDataSize();
			pzRawBody = (char*)calloc( 1, nSize + 1 );
			if( NULL == pzRawBody )
			{
				debug( "out of memory\n" );
				return;
			}
			pzRawBody = (char*)memcpy( pzRawBody, cPart.GetData(), nSize );
		}

		/* Strip carriage returns from the body text */
		pzBody = (char*)calloc( 1, nSize );
		if( NULL == pzBody )
		{
			debug( "out of memory\n" );
			return;
		}
		pzBody = xstrncpy_to_unix( pzBody, pzRawBody, nSize );
		free( pzRawBody );
	}

	FlowView *pcView = new FlowView( Rect( 0, 0, GetBounds().Width(), 0 ), pzBody, m_pcVisualSettings );
	AddChild( pcView );
	m_vViews.push_back( pcView );
}

void MessageView::InnerView::AddAttachment( Multipart &cPart )
{
	BitmapImage *pcImage = new BitmapImage();
	Resources cRes( get_image_id() );		
	ResStream *pcStream = cRes.GetResourceStream( "attachment48x48.png" );
	pcImage->Load( pcStream );
	delete ( pcStream );

	if( NULL == m_pcAttachmentView )
	{
		m_pcAttachmentView = new AttachmentView( Rect( 0, 0, 0, 0 ) );
		AddChild( m_pcAttachmentView );
		m_vViews.push_back( m_pcAttachmentView );
	}

	m_pcAttachmentView->AddIcon( pcImage, cPart.cFilename, cPart.GetId() );
}

void MessageView::InnerView::AddInline( Multipart &cPart )
{
	InlineView *pcView = new InlineView( Rect( 0, 0, GetBounds().Width(), 0 ), cPart, m_pcAppWindow, m_pcVisualSettings );
	AddChild( pcView );
	m_vViews.push_back( pcView );
}

void MessageView::InnerView::AddSigned( Multipart &cPart )
{
	/* XXXKV: Most of this is a placeholder; we'll need both the signed part
	   & the key, properly decode/check etc. for this to all work */
	SignedView *pcView = new SignedView( Rect( 0, 0, GetBounds().Width(), 0 ), cPart, SIGNED_UNKNOWN, m_pcAppWindow, m_pcVisualSettings );
	AddChild( pcView );
	m_vViews.push_back( pcView );
}

void MessageView::InnerView::AddMultipart( Multipart &cPart )
{
	int nChildren = cPart.GetChildCount();
	debug( "this part has %d child parts\n", nChildren );

	for( int k = 0; k < nChildren; k++ )
	{
		Multipart cChildPart = cPart.GetChild(k);

		debug( "child #%d is %s/%s\n", k, cChildPart.cMimeType.cSuperType.c_str(), cChildPart.cMimeType.cSubType.c_str() );

		switch( cChildPart.eDisposition )
		{
			case BODY:
			case BODY_ALTERNATIVE:
			{
				if( cChildPart.eDisposition == BODY )
					debug( "child part #%d is BODY.", k );
				else
					debug( "child part #%d is BODY_ALTERNATIVE.\n", k );

				AddBody( cChildPart );
				break;
			}

			case BODY_SIGNED:
			{
				debug( "child part #%d is BODY_SIGNED.\n", k );
				AddSigned( cChildPart );
				break;
			}

			case BODY_ALTERNATIVE_HIDDEN:
			{
				debug( "child part #%d is BODY_ALTERNATIVE_HIDDEN.\n", k );
				break;
			}

			case ATTACHMENT:
			{
				debug( "child part #%d is ATTACHMENT.\n", k );

				/* If the part is an image and "Always inline images" is on, treat it as inline */
				if( cChildPart.cMimeType.cSuperType == "image" && m_pcVisualSettings->GetBool( "inline_images", false ) )
				{
					debug( "treating image as INLINE\n" );
					AddInline( cChildPart );
				}
				else
					AddAttachment( cChildPart );

				break;
			}

			case INLINE:
			{
				debug( "child part #%d is INLINE.\n", k );
				AddInline( cChildPart );
				break;
			}

			case MULTIPART:
			{
				debug( "child part #%d is MULTIPART.\n", k );
				AddMultipart( cChildPart );
				break;
			}

			case ENCRYPTION_KEY:
			{
				debug( "child part #%d is ENCRYPTION_KEY.\n", k );
				break;
			}
		}
	}
}

void MessageView::InnerView::Copy( void )
{
	std::vector<ChildView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
		(*i)->GetSelection();
}

void MessageView::InnerView::_Layout( void )
{
	Rect cFrame = GetFrame();

	/* Re-position StringViews */
	Rect cStringFrame;
	Point cStringSize;

	cStringFrame = cFrame;
	cStringFrame.top += 10;
	cStringFrame.left += 10;
	cStringFrame.right -= 10;
	cStringSize = m_pcSubjectView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcSubjectView->SetFrame( cStringFrame );

	cStringFrame.top = cStringFrame.bottom + 5;
	cStringSize = m_pcFromView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcFromView->SetFrame( cStringFrame );

	cStringFrame.top = cStringFrame.bottom + 5;
	cStringSize = m_pcToView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcToView->SetFrame( cStringFrame );

	cStringFrame.bottom += 10;
	m_vSeperatorFrame = cStringFrame;

	m_vContentHeight = ( m_vSeperatorFrame.bottom + 5 ) + (int)m_vDefualtFontHeight;

	Rect cContentFrame = m_vSeperatorFrame;
	cContentFrame.top = m_vContentHeight;
	cContentFrame.right -= MV_SCROLL_WIDTH;

	for( uint i = 0; i < m_vViews.size(); i++ )
	{
		Point cContentSize = m_vViews[i]->GetPreferredSize( true );
		cContentFrame.bottom = cContentFrame.top + cContentSize.y;
		m_vViews[i]->SetFrame( cContentFrame );

		m_cSize.x = std::max( m_cSize.x, cContentSize.x );

		cContentFrame.top += cContentSize.y + 5;
		m_vContentHeight += cContentSize.y + 5;
	}

	m_cSize.y = m_vContentHeight + 5;

	/* Force a redraw */
	Invalidate();
}

void MessageView::InnerView::_SaveAttachment( uint64 nId )
{
	/* Decode and save every currently select attachment */
	Multipart cPart;
	char *pzHome = getenv( "HOME" );

	if( m_pcMessage->GetPartById( cPart, nId ) != EOK )
		return;

	if( cPart.cFilename == "" )
		return;

	os::String cFilename = os::String( pzHome ) + "/" + cPart.cFilename;

	Message *pcMessage = new Message( M_SAVE_REQUESTED );
	pcMessage->AddInt64( "id", nId );

	FileRequester *pcSaveRequester = new FileRequester( FileRequester::SAVE_REQ, new Messenger( this ), cFilename, FileRequester::NODE_FILE, false, pcMessage );
	pcSaveRequester->Show();
}

void MessageView::InnerView::_DoSaveAttachment( uint64 nId, const char *pzFilename )
{
	Multipart cPart;
	if( m_pcMessage->GetPartById( cPart, nId ) != EOK )
		return;

	try
	{
		os::File cFile( pzFilename, O_WRONLY | O_CREAT );

		ssize_t nOut = 0;
		char *pBuffer;

		debug( "Attachment encoding is %s\n", cPart.cEncoding.c_str() );

		/* Find the encoding type and pass to the appropriate codec; don't assume Base64 */
		if( cPart.cEncoding == "base64" )
		{
			Base64Codec cCodec;
			nOut = cCodec.Decode( cPart.GetData(), cPart.GetDataLen(), &pBuffer );
		}
		else if( cPart.cEncoding == "quoted-printable" )
		{
			QpCodec cCodec;
			nOut = cCodec.Decode( cPart.GetData(), cPart.GetDataLen(), &pBuffer );
		}
		else
		{
			pBuffer = (char*)calloc( 1, cPart.GetDataLen() );
			if( NULL != pBuffer )
			{
				memcpy( pBuffer, cPart.GetData(), cPart.GetDataLen() );
				nOut = cPart.GetDataLen();
			}
		}

		if( nOut > 0 )
		{
			cFile.Write( pBuffer, nOut );
			if( cPart.cMimeType.cSuperType != "" && cPart.cMimeType.cSuperType != "" )
			{
				os::String cType = cPart.cMimeType.cSuperType + "/" + cPart.cMimeType.cSubType;
				cFile.WriteAttr( "os::MimeType", 0, ATTR_TYPE_STRING, cType.c_str(), 0, cType.Length() );
			}
			cFile.Flush();
			free( pBuffer );
		}
	}
	catch( std::exception &e )
	{
		debug( "%s: %s\n", pzFilename, e.what() );
	}
}

void MessageView::InnerView::_OpenAttachment( uint64 nId )
{
	/* XXXKV: Display a dialog asking the user to confirm open,
	   or alternativly save the file */

	char zTemplate[23] = "/tmp/attachment_XXXXXX";
	char *pzTempfile = mktemp( zTemplate );
	if( NULL == pzTempfile )
		return;

	_DoSaveAttachment( nId, pzTempfile );

	try
	{
		RegistrarManager *pcRegistrar = RegistrarManager::Get();
		pcRegistrar->Launch( NULL, pzTempfile );
		pcRegistrar->Put();

		/* XXXKV: Is there a better way to do this?  Not sleeping means that this code
		   races with the launched application and usually wins, so the application fails
		   to open the file */
		sleep(1);
	}
	catch( std::exception &e )
	{
		debug( "%s: %s\n", pzTempfile, e.what() );
	}

	unlink( pzTempfile );
}

status_t MessageView::InnerView::_SaveAttachmentTmp( uint64 nId, String &cFilename )
{
	Multipart cPart;

	if( m_pcMessage->GetPartById( cPart, nId ) != EOK )
		return EINVAL;

	if( cPart.cFilename == "" )
		return EINVAL;

	cFilename = String( "/tmp/" ) + cPart.cFilename;
	_DoSaveAttachment( nId, cFilename.c_str() );

	return EOK;
}

void MessageView::InnerView::Clear( void )
{
	std::vector<ChildView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
	{
		RemoveChild( (*i) );
		delete( (*i) );
	}
	m_vViews.clear();

	m_pcSubjectView->SetString( "" );
	m_pcFromView->SetString( "" );
	m_pcToView->SetString( "" );

	if( GetScrollOffset().y < 0 )
		ScrollTo( Point( 0, 2 ) );

	m_bDrawSeperator = false;

	_Layout();
}

MessageView::MessageView( Rect cFrame, const char *pzName, const Handler *pcAppWindow, const Settings *pcVisualSettings ) : View( cFrame, pzName, CF_FOLLOW_ALL )
{
	/* The size and position of the InnerView and Scrollbar are adjusted to
	   allow for the size of the frame drawn around the View */
	Rect cBounds = GetBounds();
	m_pcInnerView = new InnerView( Rect( 2, 2, cBounds.Width() - MV_SCROLL_WIDTH, cBounds.Height() ), pcAppWindow, pcVisualSettings );

	AddChild( m_pcInnerView );

	m_pcVScrollBar = new ScrollBar( Rect( cFrame.Width() - ( MV_SCROLL_WIDTH + 2 ), 2, cFrame.Width() - 1, cFrame.Height() ), "message_view_vscroll", new Message( ID_MV_SCROLL_V ), 0, FLT_MAX, VERTICAL, CF_FOLLOW_RIGHT | CF_FOLLOW_TOP | CF_FOLLOW_BOTTOM );
	m_pcVScrollBar->SetScrollTarget( m_pcInnerView );
	AddChild( m_pcVScrollBar );

	/* Start with the ScrollBar hidden or we may call Show() twice */
	m_pcVScrollBar->Hide();

	SetEraseColor( get_default_color( COL_SHINE ) );

	m_eScrollDirection = AUTOSCROLL_NONE;
}

MessageView::~MessageView( )
{
	RemoveChild( m_pcInnerView );
	delete( m_pcInnerView );

	RemoveChild( m_pcVScrollBar );
	delete( m_pcVScrollBar );
}

Point MessageView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return m_pcInnerView->GetPreferredSize( true );
	else
		return View::GetPreferredSize( false );
}

void MessageView::FrameSized( const Point &cDelta )
{
	Invalidate();
	_Layout();
}

void MessageView::Paint( const Rect &cUpdateRect )
{
	DrawFrame( GetBounds(), FRAME_RECESSED );
}

void MessageView::WheelMoved( const Point &cDelta )
{
	m_pcVScrollBar->WheelMoved( cDelta );
}

void MessageView::MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData )
{
	if( nButtons & MOUSE_BUT_LEFT )
	{
		Rect cBounds = GetBounds();
		if( cNewPos.y < cBounds.top )
			_StartScroll( AUTOSCROLL_UP );
		else if( cNewPos.y > cBounds.bottom )
			_StartScroll( AUTOSCROLL_DOWN );
		else
			_StopScroll();
	}
	else
		_StopScroll();

	View::MouseMove( cNewPos, nCode, nButtons, pcData );
}

void MessageView::MouseUp( const Point &cPosition, uint32 nButtons, Message *pcData )
{
	if( nButtons & MOUSE_BUT_LEFT )
		_StopScroll();

	View::MouseUp( cPosition, nButtons, pcData );
}

void MessageView::TimerTick( int nId )
{
	if( nId != AUTOSCROLL_TIMER )
		return View::TimerTick( nId );

	/* Most of this was adapted from ListViewContainer.  The basic idea is to calculate a small step
	   to scroll the InnerView by, using the current mouse position as a reference.  The way scrolling
	   works means that everything is expressed in negative values, which is the major cause of
	   confusion. */

	Rect cBounds = GetBounds();
	float vInnerHeight = m_pcInnerView->GetPreferredSize(true).y;

	if( cBounds.Height() > vInnerHeight )
		return;

	Point cMousePos;
	GetMouse( &cMousePos, NULL );

	float vPrevScroll = m_pcInnerView->GetScrollOffset().y;
	float vCurScroll = vPrevScroll;

	if( m_eScrollDirection == AUTOSCROLL_DOWN )
	{
		float vScrollStep = cBounds.top - cMousePos.y;
		vScrollStep = ( vScrollStep / 10.0f ) + 1.0f;

		vCurScroll += vScrollStep;
		if( vCurScroll > 0 )
			vCurScroll = 0;
	}
	else if( m_eScrollDirection == AUTOSCROLL_UP )
	{
		float vMaxScroll = -( vInnerHeight - cBounds.Height() );
		float vScrollStep = cMousePos.y - cBounds.bottom;
		vScrollStep = ( vScrollStep / 10.0f ) + 1.0f;

		vCurScroll -= vScrollStep;
		if( vCurScroll < vMaxScroll )
			vCurScroll = vMaxScroll;
	}

	/* Do not scroll off the top */
	if( vCurScroll > 0.0f )
		vCurScroll = 0.0f;

	/* Do not scroll off the bottom */
	if( vCurScroll < -( vInnerHeight - cBounds.Height() ) )
		vCurScroll = -( vInnerHeight - cBounds.Height() );

	if( vCurScroll != vPrevScroll )
		m_pcInnerView->ScrollTo( 0, vCurScroll );

	return;
}

status_t MessageView::Display( Mailmessage *pcMessage )
{
	m_pcInnerView->Display( pcMessage );

	_Layout();
	return EOK;
}

void MessageView::Clear( void )
{
	m_pcInnerView->Clear();
	_Layout();
}

void MessageView::Copy( void )
{
	m_pcInnerView->Copy();
}

void MessageView::_Layout()
{
	/* Adjust scrollbar */
	float vHeight, vInnerHeight;

	vHeight = GetBounds().Height();
	vInnerHeight = m_pcInnerView->GetPreferredSize(true).y;

	if( vInnerHeight - vHeight > 0 )
	{
		m_pcVScrollBar->SetMinMax( 0.0f, vInnerHeight - vHeight );
		m_pcVScrollBar->SetProportion( vHeight / vInnerHeight );
		m_pcVScrollBar->SetSteps( vInnerHeight / 50, vInnerHeight / 10 );

		if( m_pcVScrollBar->IsVisible() == false )
		{
			m_pcVScrollBar->SetScrollTarget( m_pcInnerView );
			m_pcVScrollBar->Show();
		}
	}
	else
		if( m_pcVScrollBar->IsVisible() )
		{
			m_pcVScrollBar->Hide();
			m_pcVScrollBar->SetScrollTarget( NULL );
		}
}

void MessageView::_StartScroll( scroll_direction eDirection )
{
	if( m_eScrollDirection == AUTOSCROLL_NONE )
	{
		m_eScrollDirection = eDirection;

		Looper *pcLooper = GetLooper();
		if( pcLooper )
			pcLooper->AddTimer( this, AUTOSCROLL_TIMER, 50000, false );
	}
}

void MessageView::_StopScroll( void )
{
	if( m_eScrollDirection != AUTOSCROLL_NONE )
	{
		Looper *pcLooper = GetLooper();
		if( pcLooper )
			pcLooper->RemoveTimer( this, AUTOSCROLL_TIMER );

		m_eScrollDirection = AUTOSCROLL_NONE;
	}
}

FlowView::FlowView( Rect cFrame, char *pzText, const Settings *pcVisualSettings ) : ChildView( cFrame, "flow_view", CF_FOLLOW_ALL )
{
	m_cSize.x = cFrame.Width();
	m_cSize.y = cFrame.Height();

	/* Select the prefered font */
	font_properties cFontProperties;
	Font *pcFont = new Font();
	pcFont->GetDefaultFont( pcVisualSettings->GetString( "font", DEFAULT_FONT_REGULAR ), &cFontProperties );
	pcFont->SetProperties( cFontProperties );
	SetFont( pcFont );
	pcFont->Release();

	m_pzText = pzText;
	if( m_pzText )
		m_cSize.y = GetTextExtent( m_pzText, DTF_ALIGN_BOTTOM | DTF_WRAP_SOFT, (int)m_cSize.x ).y;

	m_bIBeamActive = 
	m_bSawClick =
	m_bSelect = false;

	m_pcMessenger = NULL;
	m_pcMenu = NULL;
}

FlowView::~FlowView()
{
	if( m_pzText )
		free( m_pzText );

	if( m_pcMessenger )
		delete( m_pcMessenger );

	if( m_pcMenu )
		delete( m_pcMenu );
}

void FlowView::AttachedToWindow( void )
{
	View *pcParent = GetParent();
	m_pcMessenger = new Messenger( pcParent );

	SetBgColor( pcParent->GetBgColor() );
	SetEraseColor( pcParent->GetEraseColor() );
	SetFgColor( pcParent->GetFgColor() );
}

Point FlowView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return m_cSize;
	else
		return View::GetPreferredSize( false );
}

void FlowView::FrameSized( const Point &cDelta )
{
	m_cSize.x += cDelta.x;
	m_cSize.y = GetTextExtent( m_pzText, DTF_ALIGN_TOP | DTF_WRAP_SOFT, (int)m_cSize.x ).y;

	Invalidate();
}

void FlowView::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );
	if( m_pzText )
	{
		if( m_bSelect )
			DrawSelectedText( Rect( 0, 10, m_cSize.x, m_cSize.y ), m_pzText, m_cSel1, m_cSel2, SEL_CHAR, DTF_ALIGN_TOP | DTF_WRAP_SOFT );
		else
			DrawText( Rect( 0, 10, m_cSize.x, m_cSize.y ), m_pzText, DTF_ALIGN_TOP | DTF_WRAP_SOFT );
	}
}

void FlowView::HandleMessage( Message *pcMessage )
{
	m_pcMessenger->SendMessage( pcMessage );
}

void FlowView::MouseMove( const Point &cNewPos, int nCode, uint32 nButtons, Message *pcData )
{
#if 0
	/* XXXKV: This would be the prefered way of handling the IBeam, if it worked.
	   The problem is that when the View is scrolled, the MOUSE_EXITED message
	   is never sent if the mouse pases out of the parent View along the scrolled
	   edge, leaving the pointer in a permanent IBeam. */
	if( nCode == MOUSE_ENTERED )
	{
		if( m_bIBeamActive == false )
		{
			Application::GetInstance()->PushCursor( MPTR_MONO, g_anMouseImg, POINTER_WIDTH, POINTER_HEIGHT, IPoint( POINTER_WIDTH / 2, POINTER_HEIGHT / 2 ) );
			m_bIBeamActive = true;
		}
	}
	else if( nCode == MOUSE_EXITED )
	{
		if( m_bIBeamActive )
		{
			m_bIBeamActive = false;
			Application::GetInstance()->PopCursor();
		}
	}
#endif

	if( ( nButtons & MOUSE_BUT_LEFT ) && m_bSawClick )
	{
		/* XXXKV: It seems that MOUSE_BUT_RIGHT (Er, 0x02) doesn't get cleared after the context menu has
		   closed, so we may get 0x03 : MOUSE_BUT_LEFT & MOUSE_BUT_RIGHT (Er, 0x02) */
		m_bSelect = true;

		m_cSel2.x = (int)cNewPos.x;
		m_cSel2.y = (int)cNewPos.y;

		/* Try to reduce flicker by only redrawing if the mouse has moved more than 5 pixels in any direction */
		int dx = abs( m_cSel2.x - m_cOldSel.x ), dy = abs( m_cSel2.y - m_cOldSel.y );
		if( ( dx > 5 ) || ( dy > 5 ) )
		{
			m_cOldSel = m_cSel2;

			Invalidate();
			Flush();
		}
	}

	View::MouseMove( cNewPos, nCode, nButtons, pcData );
}

void FlowView::MouseDown( const Point &cPosition, uint32 nButtons )
{
	if( nButtons & MOUSE_BUT_LEFT )
	{
		m_bSawClick = true;
		m_bSelect = false;
		_EnableEdit( false );

		m_cOldSel.x = m_cSel1.x = m_cSel2.x = (int)cPosition.x;
		m_cOldSel.y = m_cSel1.y = m_cSel2.y = (int)cPosition.y;

		Invalidate();
		Flush();
	}
	else if( nButtons == 2 )	/* XXXKV: Not MOUSE_BUT_RIGHT; MID & RIGHT seem to be swapped in the appserver */
		_OpenMenu( cPosition );

	View::MouseDown( cPosition, nButtons );
}

void FlowView::MouseUp( const Point &cPosition, uint32 nButtons, Message *pcData )
{
	if( m_bSelect && ( nButtons & MOUSE_BUT_LEFT ) )
	{
		m_cSel2.x = (int)cPosition.x;
		m_cSel2.y = (int)cPosition.y;

		m_bSawClick = false;
		_EnableEdit( true );

		Invalidate();
		Flush();
	}

	View::MouseUp( cPosition, nButtons, pcData );
}

void FlowView::_OpenMenu( const Point &cPosition )
{
	if( NULL == m_pcMenu )
	{
		m_pcMenu = new Menu( Rect(), "Edit", ITEMS_IN_COLUMN );
		m_pcCopy = new MenuItem( MSG_MAINWND_MENU_EDIT_COPY, new Message( ID_EDIT_COPY ) );
		m_pcCopy->SetEnable( m_bSelect );
		m_pcMenu->AddItem( m_pcCopy );
		m_pcMenu->SetTargetForItems( this );
	}

	m_pcMenu->Open( ConvertToScreen( cPosition ) );
}

void FlowView::_EnableEdit( bool bEnable )
{
	if( m_pcMenu != NULL )
		m_pcCopy->SetEnable( bEnable );

	/* The path to get a Message from here all the way to the application Window is a little complex.  This message
	   get's sent to our parent view (MessageView::InnerView) which in turn passes it directly to it's parent (The
	   application Window).  This involves at least two different Messengers. */

	Message *pcMessage = new Message( M_ENABLE_EDIT_COPY );
	pcMessage->AddBool( "enable", bEnable );
	m_pcMessenger->SendMessage( pcMessage );
}

InlineView::InlineView( Rect cFrame, Multipart &cPart, const Handler *pcAppWindow, const Settings *pcVisualSettings ) : ChildView( cFrame, "inline_view", CF_FOLLOW_ALL )
{
	m_cPart = cPart;

	m_pcInlineText = NULL;
	m_pcInlineImage = NULL;
	m_pcInlineMessage = NULL;

	SetBgColor( get_default_color( COL_SHINE ) );
	SetEraseColor( get_default_color( COL_SHINE ) );
	SetFgColor( get_default_color( COL_MENU_TEXT ) );

	m_pcAttachmentView = new StringView( Rect(), "attachment", MSG_MAINWND_MAIL_ATTACHMENT );

	Font *pcFont;
	font_properties cFontProperties;

	pcFont = new Font();
	pcFont->GetDefaultFont( DEFAULT_FONT_BOLD, &cFontProperties );
	pcFont->SetProperties( cFontProperties );
	m_pcAttachmentView->SetFont( pcFont );
	pcFont->Release();

	AddChild( m_pcAttachmentView );

	m_pcFilenameView = new StringView( Rect(), "filename", m_cPart.cFilename );
	AddChild( m_pcFilenameView );

	m_pcSaveButton = new Button( Rect(), "attachment_save", MSG_MAINWND_MAIL_ATTACHMENT_SAVE, new Message( ID_ATTACHMENT_SAVE ) );
	AddChild( m_pcSaveButton );

	m_cSize.y = m_pcSaveButton->GetPreferredSize( false ).y + 10;

	/* Display the attachment if it's an acceptable type */
	m_bDisplay = false;

	if( m_cPart.GetDataSize() > 0 )
	{
		size_t nSize;

		if( m_cPart.cMimeType.cSuperType == "text" && m_cPart.cMimeType.cSubType != "html" )
		{
			char *pzRawText, *pzText = NULL;

			nSize = _Decode( &m_cPart, &pzRawText );

			/* Strip carriage returns from the text */
			pzText = (char*)calloc( 1, nSize );
			if( NULL == pzText )
				debug( "out of memory\n" );
			else
			{
				pzText = xstrncpy_to_unix( pzText, pzRawText, nSize );
				free( pzRawText );
			}

			m_pcInlineText = new FlowView( Rect( 0, 0, cFrame.Width() - 10, 0 ), pzText, pcVisualSettings );
			AddChild( m_pcInlineText );

			m_bDisplay = true;
		}
		else if( m_cPart.cMimeType.cSuperType == "image" )
		{
			/* Decode the attached image to an in-memory file */
			uint8 *pnData = NULL;
			nSize = _Decode( &m_cPart, (char**)&pnData );

			try
			{
				MemFile cImageFile( pnData, nSize );
				BitmapImage *pcImage = new BitmapImage( &cImageFile );
				m_pcInlineImage = new ImageView( Rect(), "inline_image", pcImage );
				AddChild( m_pcInlineImage );

				m_bDisplay = true;
			}
			catch( std::exception &e )
			{
				debug( "failed to display inline image: %s\n", e.what() );
			}
		}
		else if( m_cPart.cMimeType.cSuperType == "message" && m_cPart.cMimeType.cSubType == "rfc822" )
		{
			char *pzRawMessage = NULL, *pzMessage = NULL;
			status_t nError;

			nSize = _Decode( &m_cPart, &pzRawMessage );

			/* We have to cheat a bit here.  The Parse() method expects uses the \r\n.\r\n sequence, present
			   at the end of every email, to recognise the end of the mail body.  When an email is attached
			   to a message, this sequence is removed.  We need to re-add it to the end of the message we've
			   just extracted from the attachment. */
			pzMessage = (char*)calloc( 1, nSize + 5 );
			pzMessage = strncpy( pzMessage, pzRawMessage, nSize );
			pzMessage = strncat( pzMessage, "\r\n.\r\n", 5 );
			free( pzRawMessage );

			nSize += 5;

			Mailmessage *pcMailmessage = new Mailmessage( pzMessage, nSize );
			nError = pcMailmessage->Parse();

			if( nError == EOK )
			{
				m_pcInlineMessage = new MessageView( Rect(0,0,1,1), "inline_message", pcAppWindow, pcVisualSettings );
				m_pcInlineMessage->Display( pcMailmessage );
				AddChild( m_pcInlineMessage );

				m_bDisplay = true;
			}
			else
			{
				debug( "failed to display inline message\n" );
				delete( pcMailmessage );
			}
		}
	}
}

InlineView::~InlineView()
{
	RemoveChild( m_pcSaveButton );
	delete( m_pcSaveButton );

	RemoveChild( m_pcFilenameView );
	delete( m_pcFilenameView );

	RemoveChild( m_pcAttachmentView );
	delete( m_pcAttachmentView );

	if( m_pcInlineText )
	{
		RemoveChild( m_pcInlineText );
		delete( m_pcInlineText );
	}

	if( m_pcInlineImage )
	{
		RemoveChild( m_pcInlineImage );
		delete( m_pcInlineImage );
	}

	if( m_pcInlineMessage )
	{
		RemoveChild( m_pcInlineMessage );
		delete( m_pcInlineMessage );
	}
}

void InlineView::AttachedToWindow( void )
{
	View *pcParent = GetParent();

	SetBgColor( pcParent->GetBgColor() );
	SetEraseColor( pcParent->GetEraseColor() );
	SetFgColor( pcParent->GetFgColor() );

	/* Make sure all children have calculated their preferred size by forcing a layout */
	/* XXXKV: This only works if we call _Layout() in both AttachedToWindow() & AllAttached().
	   This is a mystery to me; I would have thought AllAttached() would be the correct place to do it? */
	_Layout();
}

void InlineView::AllAttached()
{
	View::AllAttached();
	m_pcSaveButton->SetTarget( this );

	/* Make sure all children have calculated their preferred size by forcing a layout */
	/* XXXKV: See above */
	_Layout();
}

Point InlineView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return m_cSize;
	else
		return View::GetPreferredSize( false );
}

void InlineView::FrameSized( const Point &cDelta )
{
	Invalidate();
	_Layout();
}

void InlineView::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );

	Rect cBounds = GetBounds();
	DrawFrame( cBounds, FRAME_RAISED );

	if( m_bDisplay )
	{
		SetFgColor( get_default_color( COL_SHADOW ) );
		DrawLine( Point( cBounds.left, m_cHeaderSize.y ), Point( cBounds.right, m_cHeaderSize.y ) );
	}
}

void InlineView::HandleMessage( Message *pcMessage )
{
	pcMessage->AddInt64( "id", m_cPart.GetId() );
	GetParent()->HandleMessage( pcMessage );
}

void InlineView::_Layout( void )
{
	/* Move children to fit */
	Rect cBounds, cStringFrame, cButtonFrame;
	Point cStringSize;

	cBounds = GetBounds();

	/* XXXKV: Doing fixed-position layout of the StringViews isn't ideal, but GetPreferredSize()/GetTextExtent()
	   doesn't give us a useful width so we have to guess */

	cStringFrame = cBounds;
	cStringFrame.top += 5;
	cStringFrame.left += 5;
	cStringFrame.right = cStringFrame.left + 80;
	cStringFrame.bottom = cStringFrame.top + 20;

	cStringSize = m_pcAttachmentView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcAttachmentView->SetFrame( cStringFrame );

	cStringFrame.left = cStringFrame.right + 5;
	cStringFrame.right = cBounds.right - 55;

	cStringSize = m_pcFilenameView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcFilenameView->SetFrame( cStringFrame );

	cButtonFrame = cBounds;
	cButtonFrame.top += 5;
	cButtonFrame.right -= 5;
	cButtonFrame.left = cButtonFrame.right - 50;
	cButtonFrame.bottom = cButtonFrame.top + 20;

	m_pcSaveButton->SetFrame( cButtonFrame );

	m_cSize.x = m_cHeaderSize.x = cBounds.Width();
	m_cSize.y = m_cHeaderSize.y = m_pcSaveButton->GetPreferredSize( false ).y + 10;

	if( m_bDisplay )
	{
		View *pcView = NULL;
		if( m_pcInlineText )
			pcView = m_pcInlineText;
		else if( m_pcInlineImage )
			pcView = m_pcInlineImage;
		else if( m_pcInlineMessage )
			pcView = m_pcInlineMessage;

		if( pcView )
		{
			Rect cContentFrame;
			Point cContentSize = pcView->GetPreferredSize( true );

			cContentFrame.left = 5;
			cContentFrame.top = m_cHeaderSize.y + 5;
			cContentFrame.bottom = cContentFrame.top + cContentSize.y;

			if( m_pcInlineMessage )
				cContentFrame.right = m_cSize.x - 5;
			else
				cContentFrame.right = cContentFrame.left + cContentSize.x;

			pcView->SetFrame( cContentFrame );

			m_cSize.y += cContentSize.y + 10;
		}
	}

	/* Force a redraw */
	Invalidate();
}

size_t InlineView::_Decode( Multipart *pcPart, char **ppDecoded )
{
	size_t nSize;

	if( pcPart->cEncoding == "quoted-printable" )
	{
		QpCodec cCodec;
		nSize = cCodec.Decode( pcPart->GetData(), pcPart->GetDataSize(), ppDecoded );
	}
	else if( pcPart->cEncoding == "base64" )
	{
		Base64Codec cCodec;
		nSize = cCodec.Decode( pcPart->GetData(), pcPart->GetDataSize(), ppDecoded );
	}
	else
	{
		nSize = pcPart->GetDataSize();
		*ppDecoded = (char*)calloc( 1, nSize + 1 );
		if( NULL == *ppDecoded )
			debug( "out of memory\n" );
		else
			*ppDecoded = (char*)memcpy( *ppDecoded, pcPart->GetData(), nSize );
	}

	return nSize;
}

SignedView::SignedView( Rect cFrame, Multipart &cPart, int nMode, const Handler *pcAppWindow, const Settings *pcVisualSettings ) : ChildView( cFrame, "signed_view", CF_FOLLOW_ALL )
{
	m_cPart = cPart;
	m_nMode = nMode;

	m_pcInlineText = NULL;
	m_pcInlineMessage = NULL;

	SetBgColor( get_default_color( COL_SHINE ) );
	SetEraseColor( get_default_color( COL_SHINE ) );
	SetFgColor( get_default_color( COL_MENU_TEXT ) );

	m_pcSignedView = new StringView( Rect(), "signed", MSG_MAINWND_MAIL_SIGNEDWITHKEY + " " );

	Font *pcFont;
	font_properties cFontProperties;

	pcFont = new Font();
	pcFont->GetDefaultFont( DEFAULT_FONT_BOLD, &cFontProperties );
	pcFont->SetProperties( cFontProperties );
	m_pcSignedView->SetFont( pcFont );
	pcFont->Release();

	AddChild( m_pcSignedView );

	/* XXXKV: Handle key properly etc. */
	m_pcKeyView = new StringView( Rect(), "key", MSG_MAINWND_MAIL_UNKNOWNKEY );
	AddChild( m_pcKeyView );

	m_cSize.y = m_pcKeyView->GetPreferredSize( false ).y + 10;

	/* Display the attachment if it's an acceptable type */
	m_bDisplay = false;

	if( m_cPart.GetDataSize() > 0 )
	{
		size_t nSize;

		if( m_cPart.cMimeType.cSuperType == "text" && m_cPart.cMimeType.cSubType != "html" )
		{
			char *pzRawText, *pzText = NULL;

			nSize = _Decode( &m_cPart, &pzRawText );

			/* Strip carriage returns from the text */
			pzText = (char*)calloc( 1, nSize );
			if( NULL == pzText )
				debug( "out of memory\n" );
			else
			{
				pzText = xstrncpy_to_unix( pzText, pzRawText, nSize );
				free( pzRawText );
			}


			m_pcInlineText = new FlowView( Rect( 0, 0, cFrame.Width()- 10, 0 ), pzText, pcVisualSettings );
			AddChild( m_pcInlineText );

			m_bDisplay = true;
		}
		else if( m_cPart.cMimeType.cSuperType == "message" && m_cPart.cMimeType.cSubType == "rfc822" )
		{
			char *pzRawMessage = NULL, *pzMessage = NULL;
			status_t nError;

			nSize = _Decode( &m_cPart, &pzRawMessage );

			/* We have to cheat a bit here.  The Parse() method expects uses the \r\n.\r\n sequence, present
			   at the end of every email, to recognise the end of the mail body.  When an email is attached
			   to a message, this sequence is removed.  We need to re-add it to the end of the message we've
			   just extracted from the attachment. */
			pzMessage = (char*)calloc( 1, nSize + 5 );
			pzMessage = strncpy( pzMessage, pzRawMessage, nSize );
			pzMessage = strncat( pzMessage, "\r\n.\r\n", 5 );
			free( pzRawMessage );

			nSize += 5;

			Mailmessage *pcMailmessage = new Mailmessage( pzMessage, nSize );
			nError = pcMailmessage->Parse();

			if( nError == EOK )
			{
				m_pcInlineMessage = new MessageView( Rect(0,0,1,1), "inline_message", pcAppWindow, pcVisualSettings );
				m_pcInlineMessage->Display( pcMailmessage );
				AddChild( m_pcInlineMessage );

				m_bDisplay = true;
			}
			else
			{
				debug( "failed to display inline message\n" );
				delete( pcMailmessage );
			}
		}
	}
}

SignedView::~SignedView()
{
	RemoveChild( m_pcSignedView );
	delete( m_pcSignedView );

	RemoveChild( m_pcKeyView );
	delete( m_pcKeyView );

	if( m_pcInlineText )
	{
		RemoveChild( m_pcInlineText );
		delete( m_pcInlineText );
	}

	if( m_pcInlineMessage )
	{
		RemoveChild( m_pcInlineMessage );
		delete( m_pcInlineMessage );
	}
}

void SignedView::AttachedToWindow( void )
{
	View *pcParent = GetParent();

	SetBgColor( pcParent->GetBgColor() );
	SetEraseColor( pcParent->GetEraseColor() );
	SetFgColor( pcParent->GetFgColor() );

	/* Make sure all children have calculated their preferred size by forcing a layout */
	/* XXXKV: This only works if we call _Layout() in both AttachedToWindow() & AllAttached().
	   This is a mystery to me; I would have though AllAttached() would be the correct place to do it? */
	_Layout();
}

void SignedView::AllAttached()
{
	View::AllAttached();

	switch( m_nMode )
	{
		case SIGNED_OK:
		{
			m_pcSignedView->SetBgColor( Color32_s( 0, 90, 190 ) );
			m_pcKeyView->SetBgColor( Color32_s( 0, 90, 190 ) );
			break;
		}

		case SIGNED_UNKNOWN:
		{
			m_pcSignedView->SetBgColor( Color32_s( 240, 240, 60 ) );
			m_pcKeyView->SetBgColor( Color32_s( 240, 240, 60 ) );
			break;
		}
	}

	/* Make sure all children have calculated their preferred size by forcing a layout */
	/* XXXKV: See above */
	_Layout();
}

Point SignedView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return m_cSize;
	else
		return View::GetPreferredSize( false );
}

void SignedView::FrameSized( const Point &cDelta )
{
	Invalidate();
	_Layout();
}

void SignedView::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );

	Rect cBounds = GetBounds();
	DrawFrame( cBounds, FRAME_RAISED );

	if( m_bDisplay )
	{
		SetFgColor( get_default_color( COL_SHADOW ) );
		DrawLine( Point( cBounds.left, m_cHeaderSize.y ), Point( cBounds.right, m_cHeaderSize.y ) );
	}

	switch( m_nMode )
	{
		case SIGNED_OK:
		{
			FillRect( Rect( cBounds.left + 1, cBounds.top + 1, cBounds.right - 2, m_cHeaderSize.y - 1 ), Color32_s( 0, 90, 190 ) );
			break;
		}

		case SIGNED_UNKNOWN:
		{
			FillRect( Rect( cBounds.left + 1, cBounds.top + 1, cBounds.right - 2, m_cHeaderSize.y - 1 ), Color32_s( 240, 240, 60 ) );
			break;
		}
	}
}

void SignedView::_Layout( void )
{
	/* Move children to fit */
	Rect cBounds, cStringFrame;
	Point cStringSize;

	cBounds = GetBounds();

	/* XXXKV: Doing fixed-position layout of the StringViews isn't ideal, but GetPreferredSize()/GetTextExtent()
	   doesn't give us a useful width so we have to guess */

	cStringFrame = cBounds;
	cStringFrame.top += 5;
	cStringFrame.left += 5;
	cStringFrame.right = cStringFrame.left + 100;
	cStringFrame.bottom = cStringFrame.top + 20;

	cStringSize = m_pcSignedView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcSignedView->SetFrame( cStringFrame );

	cStringFrame.left = cStringFrame.right + 5;
	cStringFrame.right = cBounds.right - 55;

	cStringSize = m_pcKeyView->GetPreferredSize( false );
	cStringFrame.bottom = cStringFrame.top + cStringSize.y;
	m_pcKeyView->SetFrame( cStringFrame );

	m_cSize.x = m_cHeaderSize.x = cBounds.Width();
	m_cSize.y = m_cHeaderSize.y = m_pcKeyView->GetPreferredSize( false ).y + 10;

	if( m_bDisplay )
	{
		View *pcView = NULL;
		if( m_pcInlineText )
			pcView = m_pcInlineText;
		else if( m_pcInlineMessage )
			pcView = m_pcInlineMessage;

		if( pcView )
		{
			Rect cContentFrame;
			Point cContentSize = pcView->GetPreferredSize( true );

			cContentFrame.left = 5;
			cContentFrame.top = m_cHeaderSize.y + 5;
			cContentFrame.bottom = cContentFrame.top + cContentSize.y;

			if( m_pcInlineMessage )
				cContentFrame.right = m_cSize.x - 5;
			else
				cContentFrame.right = cContentFrame.left + cContentSize.x;

			pcView->SetFrame( cContentFrame );

			m_cSize.y += cContentSize.y + 10;
		}
	}

	/* Force a redraw */
	Invalidate();
}

size_t SignedView::_Decode( Multipart *pcPart, char **ppDecoded )
{
	size_t nSize;

	if( pcPart->cEncoding == "quoted-printable" )
	{
		QpCodec cCodec;
		nSize = cCodec.Decode( pcPart->GetData(), pcPart->GetDataSize(), ppDecoded );
	}
	else if( pcPart->cEncoding == "base64" )
	{
		Base64Codec cCodec;
		nSize = cCodec.Decode( pcPart->GetData(), pcPart->GetDataSize(), ppDecoded );
	}
	else
	{
		nSize = pcPart->GetDataSize();
		*ppDecoded = (char*)calloc( 1, nSize + 1 );
		if( NULL == *ppDecoded )
			debug( "out of memory\n" );
		else
			*ppDecoded = (char*)memcpy( *ppDecoded, pcPart->GetData(), nSize );
	}

	return nSize;
}

AttachmentView::AttachmentIcon::AttachmentIcon( Rect cFrame, Image *pcImage, String cName, uint64 nPartId ) : Icon( cFrame, pcImage, cName )
{
	m_nPartId = nPartId;
	m_nSelectionTime = 0;
}

AttachmentView::AttachmentView( Rect cFrame ) : ChildView( cFrame, "attachment_view", CF_FOLLOW_ALL )
{
	m_cSize.x = cFrame.Width();
	m_cSize.y = cFrame.Height();

	m_pcMenu = NULL;
	m_pcMessenger = NULL;

	m_bMouseDown = m_bDragging = false;
}

AttachmentView::~AttachmentView()
{
	if( m_vAttachments.size() > 0 )
		for( uint i = 0; i < m_vAttachments.size();i ++ )
			RemoveChild( m_vAttachments[i] );

	m_vSelected.clear();

	std::vector<AttachmentIcon*>::iterator i;
	for( i = m_vAttachments.begin(); i != m_vAttachments.end(); i++ )
		delete( (*i) );
	m_vAttachments.clear();

	if( m_pcMessenger )
		delete( m_pcMessenger );

	if( m_pcMenu )
		delete( m_pcMenu );
}

void AttachmentView::AttachedToWindow( void )
{
	View *pcParent = GetParent();

	SetBgColor( pcParent->GetBgColor() );
	SetEraseColor( pcParent->GetEraseColor() );
	SetFgColor( pcParent->GetFgColor() );

	m_cParentBounds = pcParent->GetBounds();
	m_pcMessenger = new Messenger( pcParent );

	_Layout();
}

void AttachmentView::MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData )
{
	if( m_bMouseDown && m_bDragging == false )
	{
		Message *pcMessage = new Message( ID_ATTACHMENT_BEGIN_DRAG );

		std::list<int>::const_iterator i;
		for( i = m_vSelected.begin(); i != m_vSelected.end(); i++ )
			pcMessage->AddInt64( "id", m_vAttachments[*i]->GetId() );

		m_pcMessenger->SendMessage( pcMessage, this );
		delete( pcMessage );

		m_bDragging = true;
	}

	View::MouseMove( cNewPos, nCode, nButtons, pcData );
}

void AttachmentView::MouseDown( const Point &cPosition, uint32 nButtons )
{
	if( m_vAttachments.size() > 0 )
	{
		for( uint i = 0; i < m_vAttachments.size(); i++ )
		{
			Rect cIconFrame = m_vAttachments[i]->GetFrame();
			cIconFrame.bottom = cIconFrame.top + m_vAttachments[i]->GetPreferredSize( false ).y;
			if( cIconFrame.DoIntersect( cPosition ) )
			{
				if( GetQualifiers() & QUAL_CTRL )
					_AddToSelection( i );
				else
					_Select( i );

				if( nButtons == 1 )
					m_bMouseDown = true;
				else if( nButtons == 2 )
					_OpenMenu( cPosition );
			}
		}
	}

	View::MouseDown( cPosition, nButtons );
}

void AttachmentView::MouseUp( const Point& cPosition, uint32 nButtons, Message* pcData )
{
	m_bMouseDown = m_bDragging = false;
	View::MouseUp( cPosition, nButtons, pcData );
}

Point AttachmentView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return m_cSize;
	else
		return View::GetPreferredSize( false );
}

void AttachmentView::FrameSized( const Point &cDelta )
{
	m_cParentBounds = GetParent()->GetBounds();
	_Layout();
}

void AttachmentView::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );
}

void AttachmentView::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_ATTACHMENT_BEGIN_DRAG:
		{
			pcMessage->SetCode( 1234 );

			int64 nId = 0;
			int i = 0, nCount = 0;
			while( pcMessage->FindInt64( "id", &nId, i++ ) == EOK )
				nCount++;

			debug( "counted %d attachments to drag", nCount );

			if( nCount > 1 )
			{
				/* Drag multiple */
			}
			else
			{
				/* Drag single */
				BitmapImage *pcImage = new BitmapImage();
				Resources cRes( get_image_id() );		
				ResStream *pcStream = cRes.GetResourceStream( "attachment48x48.png" );
				pcImage->Load( pcStream );
				delete ( pcStream );

				Rect cImageBounds( 0, 0, (int)pcImage->GetSize().x + 1, (int)pcImage->GetSize().y );
				Bitmap cBitmap( (int)pcImage->GetSize().x + 1, (int)pcImage->GetSize().y + 1, CS_RGB32, ( Bitmap::ACCEPT_VIEWS | Bitmap::SHARE_FRAMEBUFFER ) );
				View *pcView = new View( cImageBounds, "" );
				cBitmap.AddChild( pcView );
				
				pcView->SetDrawingMode( DM_COPY );
				pcView->DrawBitmap( pcImage->LockBitmap(), cImageBounds, cImageBounds );
				pcImage->UnlockBitmap();
				delete( pcImage );

				BeginDrag( pcMessage, Point( 0, 0 ), &cBitmap );
			}

			break;
		}

		default:
		{
			std::list<int>::const_iterator i;
			for( i = m_vSelected.begin(); i != m_vSelected.end(); i++ )
				pcMessage->AddInt64( "id", m_vAttachments[*i]->GetId() );

			m_pcMessenger->SendMessage( pcMessage );
		}
	}
}

void AttachmentView::AddIcon( Image *pcImage, String cName, uint64 nPartId )
{
	AttachmentIcon *pcAttachment = new AttachmentIcon( Rect( 0, 0, 0, 0 ), pcImage, cName, nPartId );
	AddChild( pcAttachment );
	m_vAttachments.push_back( pcAttachment );

	_Layout();
}

void AttachmentView::_Layout( void )
{
	m_cSize.x =
	m_cSize.y = 0;

	if( m_vAttachments.size() > 0 )
	{	
		Rect cIconFrame = GetBounds();

		for( uint i = 0; i < m_vAttachments.size(); i++ )
		{
			Point cIconSize = m_vAttachments[i]->GetPreferredSize( false );
			if( cIconSize.y > m_cSize.y )
				m_cSize.y = cIconSize.y;

			if( cIconFrame.left + cIconSize.x > m_cParentBounds.Width() )
			{
				/* Create another row */
				m_cSize.y += cIconSize.y + 10;
				cIconFrame.top += cIconSize.y + 10;
				cIconFrame.left = 0;
			}

			cIconFrame.right = cIconFrame.left + cIconSize.x;
			m_vAttachments[i]->SetFrame( cIconFrame );
			cIconFrame.left += cIconSize.x + 10;

			m_cSize.x += cIconSize.x + 10;
		}
	}

	/* Force a redraw */
	Invalidate();
}

void AttachmentView::_AddToSelection( int nIndex )
{
	/* Was the attachment double-clicked? */
	if( get_system_time() < m_vAttachments[nIndex]->GetSelectionTime() + 1000000 )
	{
		Messenger cMessenger( this );
		cMessenger.SendMessage( ID_ATTACHMENT_OPEN, this );
	}

	m_vAttachments[nIndex]->SetSelectionTime( get_system_time() );

	/* Don't add the same attachment to the selection list multiple times */
	std::list<int>::const_iterator i;
	for( i = m_vSelected.begin(); i != m_vSelected.end(); i++ )
		if( *i == nIndex )
			return;

	m_vAttachments[nIndex]->Select( true );
	m_vSelected.push_back( nIndex );
}

void AttachmentView::_Select( int nIndex )
{
	if( m_vSelected.size() > 0 )
	{
		std::list<int>::const_iterator i;
		for( i = m_vSelected.begin(); i != m_vSelected.end(); i++ )
			m_vAttachments[*i]->Select( false );

		m_vSelected.clear();
	}
	_AddToSelection( nIndex );
}

void AttachmentView::_OpenMenu( const Point &cPosition )
{
	/* Only show the menu if something is selected */
	if( m_vSelected.size() == 0 )
		return;

	if( NULL == m_pcMenu )
	{
		m_pcMenu = new Menu( Rect(), "Attachment", ITEMS_IN_COLUMN );
		m_pcMenu->AddItem( MSG_MAINWND_MAIL_CONMENU_SAVE, new Message( ID_ATTACHMENT_SAVE ) );
		m_pcMenu->AddItem( MSG_MAINWND_MAIL_CONMENU_OPEN, new Message( ID_ATTACHMENT_OPEN ) );
		m_pcMenu->SetTargetForItems( this );
	}

	m_pcMenu->Open( ConvertToScreen( cPosition ) );
}

