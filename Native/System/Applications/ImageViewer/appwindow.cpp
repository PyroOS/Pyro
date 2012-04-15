// ImageViewer (C)opyright 2008 Jonas Jarvoll
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <vector>

#include <util/application.h>
#include <util/settings.h>
#include <storage/operations.h>
#include <util/event.h>
#include <gui/layoutview.h>
#include <gui/frameview.h>
#include <gui/slider.h>
#include <gui/button.h>
#include <gui/image.h>
#include <util/resources.h>
#include <storage/operations.h>  // To delete files
#include <gui/desktop.h>
#include <gui/filerequester.h>

#include "appwindow.h"
#include "iview.h"
#include "layouter.h"
#include "imagekeeper.h"
#include "iconbar.h"
#include "messages.h"
#include "main.h"
#include "resources/lang.h"

using namespace os;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

class AppWindow :: _Private
{
public:
	_Private()
	{		
		_Image = NULL;
		Fullscreen = false;
		Slideshow = false;
		SlideshowTimeout = 5;
	}

	void SetTitle( Window* win, String filename )
	{
		String title = ID_TITLE;
		if( filename != "" )
			title = filename + " - " + title;

		win->SetTitle( title ); 
	}

	void LoadImage( String file )
	{
		File* tmp;

		if( _Image != NULL )
			delete( _Image );
		_Image = NULL;

		tmp = new File( file );
	
		_Image = new BitmapImage( tmp );

		delete( tmp );
	}

	BitmapImage* _Image; // The current image we are watching

	bool Fullscreen;	// If true we are in full screen mode
	bool Slideshow;	// True if we are in slideshow mode
	int SlideshowTimeout;		// How many seconds between pictures
	Rect WindowSize;	// Save window size if we go into fullscreen

	FileRequester* _LoadRequester;
	ImageKeeper* _ImageKeeper;

	String WallpaperExtension;
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////
AppWindow :: AppWindow( int argc, char* argv[], const Rect& cFrame ) : Window( cFrame, "main_window", "NView - an image view" )
{	
	NView::SetAppWindow( this );

	// Set up start title
	m->SetTitle( this, "" );

	// Create the private class
	m = new _Private();

	// Set an application icon for Dock
	BitmapImage* pcImage = new BitmapImage();
	Resources cRes( get_image_id() );
	pcImage->Load( cRes.GetResourceStream( "icon24x24.png" ) );
	SetIcon( pcImage->LockBitmap() );	

	Layouter::GetInstance()->SetFrame( GetBounds() );
	AddChild( Layouter::GetInstance() );
		
	// There are two ways of showing images
	// 1) A single image: Load and show the image but allow the user to switch images in the image path
	// 2) Several images: Load the first image and allow the user to switch between the selected images
	// 3) Open via file requester
	if( argc == 2 )		// one file (note the first one is the name of the application )
		m->_ImageKeeper = new SingleImageKeeper( argv[ 1 ] );	
	else if( argc == 1 )
		m->_ImageKeeper = NULL;
	else
		m->_ImageKeeper = new SeveralImageKeeper( argc - 1, &argv[ 1 ] );	
	
	if( m->_ImageKeeper != NULL )
		_LoadImage( m->_ImageKeeper->GetNextImage() );

	// Create file requester
	m->_LoadRequester = new FileRequester( FileRequester::LOAD_REQ, new Messenger( this ), "" );

	// Add shortcuts
	AddShortcut( ShortcutKey( VK_SPACE ), new Message( MSG_NEXT_IMAGE ) );
	AddShortcut( ShortcutKey( VK_LEFT_ARROW ), new Message( MSG_PREV_IMAGE ) );
	AddShortcut( ShortcutKey( VK_RIGHT_ARROW ), new Message( MSG_NEXT_IMAGE ) );
	AddShortcut( ShortcutKey( VK_PAGE_DOWN ), new Message( MSG_NEXT_IMAGE ) );		
	AddShortcut( ShortcutKey( VK_PAGE_UP ), new Message( MSG_PREV_IMAGE ) );
	AddShortcut( ShortcutKey( VK_DELETE ), new Message( MSG_DELETE ) );
	AddShortcut( ShortcutKey( VK_BACKSPACE ), new Message( MSG_FLIP_FULLSCREEN ) );
	AddShortcut( ShortcutKey( VK_ESCAPE ), new Message( MSG_WANTS_TO_QUIT ) );		
	AddShortcut( ShortcutKey( "q" ), new Message( MSG_ROTATE_CCW ) );
	AddShortcut( ShortcutKey( "w" ), new Message( MSG_ROTATE_CW ) );
	AddShortcut( ShortcutKey( "p" ), new Message( MSG_SLIDESHOW_START ) );
	AddShortcut( ShortcutKey( "s" ), new Message( MSG_SLIDESHOW_STOP ) );
	AddShortcut( ShortcutKey( "Q" ), new Message( MSG_ROTATE_CCW ) );
	AddShortcut( ShortcutKey( "W" ), new Message( MSG_ROTATE_CW ) );
	AddShortcut( ShortcutKey( "P" ), new Message( MSG_SLIDESHOW_START ) );
	AddShortcut( ShortcutKey( "S" ), new Message( MSG_SLIDESHOW_STOP ) );
	AddShortcut( ShortcutKey( "+" ), new Message( MSG_ZOOM_IN ) );
	AddShortcut( ShortcutKey( "-" ), new Message( MSG_ZOOM_OUT ) );
	AddShortcut( ShortcutKey( "Ctrl+O" ), new Message( MSG_OPEN ) );
	AddShortcut( ShortcutKey( "Ctrl+o" ), new Message( MSG_OPEN ) );
}

AppWindow :: ~AppWindow()
{
	delete m->_Image;
	delete m->_ImageKeeper;

	delete m;
}

void AppWindow :: HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )	//Get the message code from the message
	{	
		case MSG_NEXT_IMAGE:
		{
			_StopSlideshowTimer();
			if( m->_ImageKeeper != NULL )
				_LoadImage( m->_ImageKeeper->GetNextImage() );
			break;
		}
		case MSG_PREV_IMAGE:
		{
			_StopSlideshowTimer();
			if( m->_ImageKeeper != NULL )
				_LoadImage( m->_ImageKeeper->GetPreviousImage() );
			break;
		}
		case MSG_DELETE:
		{
			_StopSlideshowTimer();
			if( m->_ImageKeeper != NULL && m->_ImageKeeper->GetCurrentImage() != "" )
			{
				std::vector< String > files;
				files.push_back( m->_ImageKeeper->GetCurrentImage() );
				delete_files( files, Messenger( this ), new Message( MSG_DELETED_FILE ) );
			}
			break;
		}
		case MSG_DELETED_FILE:
		{
			bool success = false;
			if( pcMessage->FindBool( "success", &success ) == EOK && success )
			{
				if( m->_ImageKeeper != NULL )
				{
					m->_ImageKeeper->RemoveCurrentImage();
					_LoadImage( m->_ImageKeeper->GetCurrentImage() );
				}
			}
			break;
		}
		case MSG_FLIP_FULLSCREEN:
		{
			_FlipFullscreen();
			break;
		}
		case MSG_WANTS_TO_QUIT:
			OkToQuit();
			break;
		case MSG_VIEW_100:
			Layouter::GetInstance()->Set100();
			break;
		case MSG_VIEW_FIT:
			Layouter::GetInstance()->SetFit();
			break;
		case MSG_ZOOM_SLIDER:
			{
				Layouter::GetInstance()->SetZoom( Layouter::GetInstance()->GetZoomSlider() );
				break;
			}		
		case MSG_ROTATE_CCW:
			_RotateImage( true );
			Layouter::GetInstance()->UpdateAllViews();
			break;
		case MSG_ROTATE_CW:
			_RotateImage( false );
			Layouter::GetInstance()->UpdateAllViews();
			break;
		case MSG_SLIDESHOW_START:
			_Slideshow( true );
			break;
		case MSG_SLIDESHOW_STOP:
			_Slideshow( false );
			break;
		case MSG_TIMEOUT_SLIDER:
		{
			Variant value;
			if( pcMessage->FindVariant( "value", &value ) == EOK )
				m->SlideshowTimeout = (int) value.AsInt32();
			break;
		}	
		case MSG_ZOOM_IN:
			Layouter::GetInstance()->SetZoom( Layouter::GetInstance()->GetZoom() * 1.1f );
			break;	
		case MSG_ZOOM_OUT:
			Layouter::GetInstance()->SetZoom( Layouter::GetInstance()->GetZoom() * 0.9f );
			break;	
		case MSG_OPEN:
			m->_LoadRequester->Show();
			break;
		case M_LOAD_REQUESTED:
		{
			int num = 0;

			if( pcMessage->GetNameInfo( "file/path", NULL, &num ) == 0 )
			{
				const char* pzFilename;
				int nIndex = 0;

				delete m->_ImageKeeper;
				m->_ImageKeeper = NULL;		
			
				// Has the user selected one file only?
				if( num == 1 )
					m->_ImageKeeper = new SingleImageKeeper();
				else
					m->_ImageKeeper = new SeveralImageKeeper();

				while( pcMessage->FindString( "file/path", &pzFilename, nIndex++ ) == 0 )
				{
					m->_ImageKeeper->AddImage( pzFilename );
				}

				if( m->_ImageKeeper != NULL )
					_LoadImage( m->_ImageKeeper->GetNextImage() );
			}
			break;
		}
		case MSG_SET_DESKTOP:
		{
				string image = m->_ImageKeeper->GetCurrentImage();
				
				// We need to copy this image to system place
				vector<String> src, dst;
				src.push_back( image );
				// Create image name
				m->WallpaperExtension = image.substr( image.rfind( '.' ), image.size() - image.rfind( '.' ) );
				dst.push_back( String( "/system/resources/wallpapers/ImageViewer" ) + m->WallpaperExtension );

				copy_files( dst, src, Messenger( this ), new Message( MSG_COPY_FINISHED ), true );
			}
			break;
		case MSG_COPY_FINISHED:
		{
			try
			{
				Event cEvent;
				Message cBackgroundImage;
				cBackgroundImage.AddString( "background_image", String( "ImageViewer" ) + m->WallpaperExtension  );
				if( cEvent.SetToRemote( "os/Desktop/SetBackgroundImage", 0 ) == 0 )
					cEvent.PostEvent( &cBackgroundImage, NULL, 0 );
			}
			catch(...)
			{
			}

			break;
		}
		default:
		{
			Window::HandleMessage( pcMessage );
			break;
		}
	}
}

bool AppWindow :: OkToQuit( void )
{
	// Save application settings
	Settings cSettings;
	cSettings.AddRect( "window_position", GetFrame() );
	cSettings.Save();

	Application::GetInstance()->PostMessage( M_QUIT );
	return true;	
}

void AppWindow :: TimerTick( int nID )
{
	// our slideshow timer
	if( nID == 1 )
	{
		if( m->_ImageKeeper != NULL )
			_LoadImage( m->_ImageKeeper->GetNextImage() );

		_StartSlideshowTimer();
	}
}

int AppWindow :: GetTimeoutValue()
{
	return m->SlideshowTimeout;
}


//////////////////////////////////////////////////////////////////7
//
// PROTECTED METHODS
//
//////////////////////////////////////////////////////////////////7
void AppWindow :: _Slideshow( bool active )
{
	if( active && !m->Slideshow )
	{
		Layouter::GetInstance()->SetSlideshow( true );
		m->Slideshow = true;
		_StartSlideshowTimer();
		
	}
	else if( !active && m->Slideshow )
	{
		Layouter::GetInstance()->SetSlideshow( false );
		m->Slideshow = false;
		_StopSlideshowTimer();
	}
}

void AppWindow :: _StartSlideshowTimer()
{
	AddTimer( this, 1, m->SlideshowTimeout * 1000000L );
}

void AppWindow :: _StopSlideshowTimer()
{
	RemoveTimer( this, 1 );
}

void AppWindow :: _LoadImage( os::String filename )
{
	try
	{
		m->LoadImage( filename );
		m->SetTitle( this, filename );
	}
	catch( ... )
	{
 		m->SetTitle( this, ID_NO_VIEW );
	}

	// Display the new Image
	Layouter::GetInstance()->SetImage( m->_Image );
}

void AppWindow :: _FlipFullscreen()
{
	m->Fullscreen = !m->Fullscreen;

	if( m->Fullscreen )
	{
		m->WindowSize = GetFrame();

		SetFlags( WND_NO_BORDER | WND_FRONTMOST );
		Desktop cDesktop;
		SetFrame( Rect( Point( 0, 0 ), Point( cDesktop.GetResolution() ) ) );
		Layouter::GetInstance()->SetFullscreenMode( true );
	}
	else
	{
		SetFlags( 0 );	
		SetFrame( m->WindowSize );
		
		SetTitle( GetTitle() );		// This will force a window border redraw
		FrameSized( Point( 0, 0 ) );  // and this will force a window content redraw :)
		Layouter::GetInstance()->SetFullscreenMode( false );
	}
}

void AppWindow :: _RotateImage( bool ccw )
{
	// Rotation algorithm inspired from gdk_pixbuf_rotate_simple (www.gtk.org)

#define OFFSET(bmp, x, y) ( (x) * bytes_per_pixel + (y) * bmp->GetBytesPerRow() )

	if( m->_Image == NULL )
		return;

	int src_width = (int) m->_Image->GetBounds().Width() + 1;
	int src_height = (int) m->_Image->GetBounds().Height() + 1;

	Bitmap* src_bitmap = m->_Image->LockBitmap();
	uint8 bytes_per_pixel = BitsPerPixel( m->_Image->GetColorSpace() ) / 8;
	uint8* p;
	uint8* q;

	if( bytes_per_pixel < 1 || bytes_per_pixel > 4)
		return;

	Bitmap* dst_bitmap;dst_bitmap = new Bitmap( src_height, src_width, m->_Image->GetColorSpace() );

	if( ccw )
	{
		for( int y = 0 ; y < src_height ; y++ )
		{
			for( int x = 0 ; x < src_width ; x++ )
			{
				p = src_bitmap->LockRaster() + OFFSET( src_bitmap, x, y );
				q = dst_bitmap->LockRaster() + OFFSET( dst_bitmap, y, src_width - x - 1 );

				memcpy( q, p, bytes_per_pixel );
			}
		}
	}
	else
	{
		for( int y = 0 ; y < src_height ; y++ )
		{
			for( int x = 0 ; x < src_width ; x++ )
			{
				p = src_bitmap->LockRaster() + OFFSET( src_bitmap, x, y );
				q = dst_bitmap->LockRaster() + OFFSET( dst_bitmap, src_height - y - 1, x );

				memcpy( q, p, bytes_per_pixel );
			}
		}
	}

	m->_Image->SetBitmapData( dst_bitmap->LockRaster(), IPoint( (int) dst_bitmap->GetBounds().Width() + 1, (int) dst_bitmap->GetBounds().Height() + 1 ), 
							  dst_bitmap->GetColorSpace() );		
}

