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

#include <gui/button.h>
#include <gui/image.h>
#include <gui/stringview.h>
#include <util/resources.h>

#include <settings_window.h>
#include <messages.h>
#include <resources/Whisper.h>

#include <debug.h>

#include <stdint.h>

IconList::IconList( Rect cFrame, int nFlags, Handler *pcHandler ) : View( cFrame, "icon_list", nFlags )
{
	m_pcHandler = pcHandler;

	Rect cInnerBounds = GetBounds();
	cInnerBounds.top += 2;
	cInnerBounds.left += 2;
	cInnerBounds.right -= 2 + IL_SCROLL_WIDTH;
	cInnerBounds.bottom -= 2;
	m_pcInnerView = new InnerView( cInnerBounds );
	AddChild( m_pcInnerView );

	m_pcVScrollBar = new ScrollBar( Rect( cFrame.Width() - ( IL_SCROLL_WIDTH + 2 ), 2, cFrame.Width() - 2, cFrame.Height() - 2 ), "icon_list_vscroll", new Message( M_SCROLL_V ), 0, FLT_MAX, VERTICAL, CF_FOLLOW_RIGHT | CF_FOLLOW_TOP | CF_FOLLOW_BOTTOM );
	m_pcVScrollBar->SetScrollTarget( m_pcInnerView );
	AddChild( m_pcVScrollBar );
}

IconList::~IconList()
{
	RemoveChild( m_pcInnerView );
	delete( m_pcInnerView );

	RemoveChild( m_pcVScrollBar );
	delete( m_pcVScrollBar );
}

void IconList::AttachedToWindow( void )
{
	SetFgColor( get_default_color( COL_MENU_TEXT ) );
	SetEraseColor( get_default_color( COL_SHINE ) );
	SetBgColor( get_default_color( COL_SHINE ) );

	_Layout();
}

Point IconList::GetPreferredSize( bool bLargest ) const
{
	return m_pcInnerView->GetPreferredSize( bLargest );
}

void IconList::Paint( const Rect &cUpdateRect )
{
	if( GetFrame().DoIntersect( cUpdateRect ) )
		DrawFrame( GetBounds(), FRAME_RECESSED );
}

void IconList::WheelMoved( const Point &cDelta )
{
	m_pcVScrollBar->WheelMoved( cDelta );
}

void IconList::FrameSized( const Point &cDelta )
{
	_Layout();
}

void IconList::HandleMessage( Message *pcMessage )
{
	m_pcHandler->HandleMessage( pcMessage );
}

void IconList::AddIcon( Image *pcImage, String cName )
{
	m_pcInnerView->AddIcon( pcImage, cName );
}

status_t IconList::Select( int nIndex, bool bNotify )
{
	return m_pcInnerView->Select( nIndex, bNotify );
}

void IconList::_Layout( void )
{
	/* Adjust scrollbar */
	float vIVHeight = m_pcInnerView->GetPreferredSize(false).y;
	m_pcVScrollBar->SetValue( 0 );
	m_pcVScrollBar->SetMinMax( 0.0f, vIVHeight - GetBounds().Height() );
	m_pcVScrollBar->SetProportion( GetBounds().Height() / vIVHeight );
	m_pcVScrollBar->SetSteps( vIVHeight / 50, vIVHeight / 10 );
}

IconList::InnerView::InnerView( Rect cFrame ) : View( cFrame, "icon_list_inner_view", CF_FOLLOW_ALL )
{
	m_cSize.x = cFrame.Width();
	m_cSize.y = cFrame.Height();

	m_nSelected = -1;
}

IconList::InnerView::~InnerView()
{
	if( m_vIcons.size() > 0 )
		for( uint i = 0; i < m_vIcons.size();i ++ )
			RemoveChild( m_vIcons[i] );

	std::vector<Icon*>::iterator i;
	for( i = m_vIcons.begin(); i != m_vIcons.end(); i++ )
		delete( (*i) );
	m_vIcons.clear();
}

void IconList::InnerView::AttachedToWindow( void )
{
	View *pcParent = GetParent();
	SetFgColor( pcParent->GetFgColor() );
	SetEraseColor( pcParent->GetEraseColor() );
	SetBgColor( pcParent->GetBgColor() );

	_Layout();
}

void IconList::InnerView::MouseDown( const Point &cPosition, uint32 nButtons )
{
	if( m_vIcons.size() > 0 )
		for( uint i = 0; i < m_vIcons.size(); i++ )
		{
			Rect cIconFrame = m_vIcons[i]->GetFrame();
			cIconFrame.bottom = cIconFrame.top + m_vIcons[i]->GetPreferredSize( false ).y;
			if( cIconFrame.DoIntersect( cPosition ) )
				Select( i );
		}
}

Point IconList::InnerView::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return View::GetPreferredSize( true );
	else
		return m_cSize;
}

void IconList::InnerView::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );
}

void IconList::InnerView::FrameSized( const Point &cDelta )
{
	Rect cFrame = GetFrame();

	m_cSize.x = cFrame.Width();
	m_cSize.y = cFrame.Height();

	_Layout();
}

void IconList::InnerView::HandleMessage( Message *pcMessage )
{
	pcMessage->AddInt32( "selection", m_nSelected );
	GetParent()->HandleMessage( pcMessage );
}

void IconList::InnerView::AddIcon( Image *pcImage, String cName )
{
	Icon *pcIcon = new Icon( Rect(), pcImage, cName );
	AddChild( pcIcon );
	m_vIcons.push_back( pcIcon );

	_Layout();
}

status_t IconList::InnerView::Select( int nIndex, bool bNotify )
{
	if( nIndex == m_nSelected )
		return EOK;

	if( nIndex < 0 || nIndex > (int)m_vIcons.size() )
		return EINVAL;

	std::vector<Icon*>::iterator i;
	for( i = m_vIcons.begin(); i != m_vIcons.end(); i++ )
		(*i)->Select( false );

	m_vIcons[nIndex]->Select( true );
	m_nSelected = nIndex;

	if( bNotify )
	{
		Messenger cMessenger( this );
		cMessenger.SendMessage( M_SETTINGS_SELECT, this );
	}

	return EOK;
}

void IconList::InnerView::_Layout( void )
{
	Rect cIconFrame;

	if( m_vIcons.size() > 0 )
	{
		cIconFrame.top = 10;

		for( uint i = 0; i < m_vIcons.size(); i++ )
		{
			Point cIconSize = m_vIcons[i]->GetPreferredSize( false );

			cIconFrame.bottom = cIconFrame.top + cIconSize.y + 10;
			cIconFrame.left = (int)( ( GetBounds().Width() - cIconSize.x ) / 2 );
			cIconFrame.right = cIconFrame.left + cIconSize.x;

			m_vIcons[i]->SetFrame( cIconFrame );

			cIconFrame.top = cIconFrame.bottom + 10;
		}
	}

	m_cSize.y = cIconFrame.top;

	Invalidate();
}

SettingsWindow::SettingsWindow( const Rect &cFrame, const String cTitle, Handler *pcParent, Settings *pcGuiSettings, Identity *pcIdentity ) : Window( cFrame, "whisper_settings", cTitle )
{
	m_pcParent = pcParent;
	m_pcGuiSettings = pcGuiSettings;

	SetSizeLimits( Point( 500, 400 ), Point( 4096, 4096 ) );

	Rect cBounds = GetBounds();

	Rect cIconListFrame( 10, 10, 140, cBounds.Height() - 45 );
	m_pcIconList = new IconList( cIconListFrame, CF_FOLLOW_TOP | CF_FOLLOW_BOTTOM, this );
	AddChild( m_pcIconList );

	/* The selected panel is displayed inside this FrameView */
	Rect cFrameViewFrame( 150, 10, cBounds.Width() - 10, cBounds.Height() - 45 );
	m_pcFrameView = new FrameView( cFrameViewFrame, "settings_window_frame", MSG_CFGWND_SETTINGS, CF_FOLLOW_ALL );
	AddChild( m_pcFrameView );

	/* Add panel icons & associated views */
	BitmapImage *pcImage;
	ResStream *pcStream;
	Resources cRes( get_image_id() );		

	SettingsView *pcView;
	Rect cViewFrame( 10, 20, cFrameViewFrame.Width() - 10, cFrameViewFrame.Height() - 10 );

	/* Identity */
	pcImage = new BitmapImage();
	pcStream = cRes.GetResourceStream( "identity.png" );
	pcImage->Load( pcStream );
	m_pcIconList->AddIcon( pcImage, MSG_CFGWND_YOURDETAILS );
	delete( pcStream );

	pcView = new IdentityView( cViewFrame, pcIdentity, m_pcParent );
	m_vViews.push_back( pcView );

	/* Signature */
	pcImage = new BitmapImage();
	pcStream = cRes.GetResourceStream( "sign.png" );
	pcImage->Load( pcStream );
	m_pcIconList->AddIcon( pcImage, MSG_CFGWND_SIGNATURES );
	delete( pcStream );

	pcView = new SignatureView( cViewFrame, pcIdentity, m_pcParent );
	m_vViews.push_back( pcView );

	/* Outbound */
	pcImage = new BitmapImage();
	pcStream = cRes.GetResourceStream( "outbound.png" );
	pcImage->Load( pcStream );
	m_pcIconList->AddIcon( pcImage, MSG_CFGWND_SENDINGEMAIL );
	delete( pcStream );

	pcView = new OutboundView( cViewFrame, pcIdentity, m_pcParent );
	m_vViews.push_back( pcView );

	/* Inbound */
	pcImage = new BitmapImage();
	pcStream = cRes.GetResourceStream( "inbound.png" );
	pcImage->Load( pcStream );
	m_pcIconList->AddIcon( pcImage, MSG_CFGWND_RECIEVINGEMAIL );
	delete( pcStream );

	pcView = new InboundView( cViewFrame, pcIdentity, m_pcParent );
	m_vViews.push_back( pcView );

	/* Filters */
	pcImage = new BitmapImage();
	pcStream = cRes.GetResourceStream( "filters.png" );
	pcImage->Load( pcStream );
	m_pcIconList->AddIcon( pcImage, MSG_CFGWND_RULEFILTER );
	delete( pcStream );

	pcView = new FiltersView( cViewFrame, pcIdentity, m_pcParent );
	m_vViews.push_back( pcView );

	/* XXXKV: More to follow:
	   o Fonts/Display options
	   o Add/Remove mailboxes & mailbox options
	   o Add/Remove identities
	   o etc. as needed
	 */

	/* The Apply/Save/Cancel buttons live in a layoutview of their own */
	Rect cButtonFrame = cBounds;
	cButtonFrame.top = cButtonFrame.bottom - 25;

	m_pcLayoutView = new LayoutView( cButtonFrame, "settings_window" );

	VLayoutNode *pcNode = new VLayoutNode( "settings_window_root" );
	pcNode->SetBorders( Rect( 5, 4, 5, 4 ) );
	pcNode->AddChild( new VLayoutSpacer( "settings_window_v_spacer", 1.0f ) );

	HLayoutNode *pcButtons = new HLayoutNode( "settings_window_buttons" );
	pcButtons->AddChild( new HLayoutSpacer( "settings_window_h_spacer" ) );
	Button *pcOkButton = new Button( Rect(), "settings_window_save", MSG_CFGWND_BUTTON_SAVE, new Message( M_SETTINGS_SAVE ) );
	pcButtons->AddChild( pcOkButton, 0.0f );
	pcButtons->AddChild( new HLayoutSpacer( "settings_window_h_spacer", 0.5f, 0.5f, pcButtons, 0.1f ) );
	pcButtons->AddChild( new Button( Rect(), "settings_window_apply", MSG_CFGWND_BUTTON_APPLY, new Message( M_SETTINGS_APPLY ) ), 0.0f );
	pcButtons->AddChild( new HLayoutSpacer( "settings_window_h_spacer", 0.5f, 0.5f, pcButtons, 0.1f ) );
	pcButtons->AddChild( new Button( Rect(), "settings_window_cancel", MSG_CFGWND_BUTTON_CANCEL, new Message( M_SETTINGS_CANCEL ) ), 0.0f );

	pcNode->AddChild( pcButtons );

	m_pcLayoutView->SetRoot( pcNode );
	AddChild( m_pcLayoutView );

	/* Focus the controls */
	/* XXXKV: The default button always swallows Enter, which is no use if we have a
	   multiline TextView (E.g. the signature text).
	SetDefaultButton( pcOkButton );
	*/

	/* Select first view by default */
	m_pcSelected = NULL;
	m_pcIconList->Select( 0, true );
}

SettingsWindow::~SettingsWindow()
{
	RemoveChild( m_pcIconList );
	delete( m_pcIconList );

	if( NULL != m_pcSelected )
		m_pcFrameView->RemoveChild( m_pcSelected );

	RemoveChild( m_pcFrameView );
	delete( m_pcFrameView );

	RemoveChild( m_pcLayoutView );
	delete( m_pcLayoutView );

	std::vector<SettingsView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
		delete( (*i) );
	m_vViews.clear();
}

void SettingsWindow::FrameSized( const Point &cDelta )
{
	/* Ensure the buttons follow the bottom of the window */
	Rect cButtonBounds = GetBounds();
	cButtonBounds.top = cButtonBounds.bottom - 25;
	m_pcLayoutView->SetFrame( cButtonBounds );

	/* Resize the views which are currently not visible */
	std::vector<SettingsView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
		if( (*i) != m_pcSelected )
			(*i)->ResizeBy( cDelta );
}

void SettingsWindow::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_SETTINGS_SELECT:
		{
			int32 nSelected;
			if( pcMessage->FindInt32( "selection", &nSelected ) == EOK )
			{
				/* Collect the current information entered in all the views */
				Identity *pcIdentity = new Identity();
				std::vector<SettingsView*>::iterator i;
				for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
					(*i)->Store( pcIdentity );

				/* Hide the old view */
				if( NULL != m_pcSelected )
					m_pcFrameView->RemoveChild( m_pcSelected );

				/* Select the new view and allow it to update with the latest information */
				m_pcSelected = m_vViews[nSelected];
				m_pcSelected->Update( pcIdentity );

				m_pcFrameView->AddChild( m_pcSelected );

				delete( pcIdentity );
			}
			break;
		}

		case M_SETTINGS_APPLY:
		{
			/* Inform the Views that Apply was clicked */
			std::vector<SettingsView*>::iterator i;
			for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
				(*i)->Apply();

			_Save( pcMessage->GetCode() );
			break;
		}

		case M_SETTINGS_SAVE:
		{
			/* Inform the Views that Save was clicked */
			std::vector<SettingsView*>::iterator i;
			for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
				(*i)->Save();

			_Save( pcMessage->GetCode() );
			break;
		}

		case M_SETTINGS_CANCEL:
		{
			_SaveGui();
			Close();
			break;
		}

		default:
			Window::HandleMessage( pcMessage );
	}
}

bool SettingsWindow::OkToQuit( void )
{
	_SaveGui();

	return true;
}

void SettingsWindow::_Save( int nCode )
{
	/* XXXKV: The identity management is too complex.  We have to pass the original identity to the views at
	   creation to allow them to retrieve the current configuration, but then we have to create an empty
	   identity here and have each view write their configuration to it before over-writing the existing identity
	   configuration.  This is a hack.  Identities need to be flexible enough to easily modify on the fly. */

	Identity *pcIdentity = new Identity();
	std::vector<SettingsView*>::iterator i;
	for( i = m_vViews.begin(); i != m_vViews.end(); i++ )
		(*i)->Store( pcIdentity );

	/* XXXKV: Create the default mailbox.  This isn't needed once we have a mailbox dialog to get the boxes from */
	MailboxId cBoxId( "syllable", "Mail" );
	pcIdentity->AddMailboxId( cBoxId );

	/* Pass the identity information back to the application */
	Message *pcNewMessage = new Message();
	if( nCode == M_SETTINGS_APPLY )
		pcNewMessage->SetCode( M_SETTINGS_CONFIGURE_APPLY );
	else
		pcNewMessage->SetCode( M_SETTINGS_CONFIGURE_SAVE );

	pcNewMessage->AddPointer( "identity", (void*)pcIdentity );

	Messenger cMessenger( m_pcParent );
	cMessenger.SendMessage( pcNewMessage );

	_SaveGui();
	Close();
}

void SettingsWindow::_SaveGui( void )
{
	/* Store current GUI settings */
	m_pcGuiSettings->SetRect( "settings", GetFrame() );
}

