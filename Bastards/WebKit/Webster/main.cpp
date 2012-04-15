/* Webster (C)opyright	2008 Kristian Van Der Vliet
 * 						2004-2007 Arno Klenke
 *						2001 Kurt Skauen
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <unistd.h>

#include <gui/window.h>
#include <gui/rect.h>
#include <gui/textview.h>
#include <gui/requesters.h>
#include <gui/statusbar.h>
#include <gui/toolbar.h>
#include <gui/imagebutton.h>
#include <gui/tabview.h>
#include <gui/menu.h>
#include <util/application.h>
#include <util/message.h>
#include <util/string.h>
#include <util/locker.h>
#include <util/resource.h>
#include <util/settings.h>
#include <storage/file.h>
#include <storage/directory.h>
#include <storage/registrar.h>

#include <browserwebview.h>
#include <browserchromeclient.h>
#include <urledit.h>
#include <progresspanel.h>
#include <urlfilter.h>
#include <bookmarks.h>
#include <settings_window.h>
#include <messages.h>
#include <version.h>

#include <webview/websettings.h>

using namespace os;

class BrowserWindow : public Window
{
	public:
		BrowserWindow( const Rect & cFrame );
		~BrowserWindow();

		void HandleMessage( Message * pcMsg );
		bool OkToQuit( void );

		void OpenURL( String cURL );
		void UpdateButtonState( bool bLoadStarted );

	private:
		uint CreateTab( String cURL );
		void ChangeTab( uint nIndex );
		void DestroyTab( uint nIndex );

		BrowserWebView* GetCurrentWebView( void ) const;

		Menu *m_pcMenuBar;
		UrlEdit *m_pcUrlEdit;
		StatusBar *m_pcStatusBar;
		ProgressPanel *m_pcProgress;
		ToolBar *m_pcToolBar;
		TabView *m_pcTabView;

		ImageButton *m_pcBackButton;
		ImageButton *m_pcForwardButton;
		ImageButton *m_pcReloadButton;
		ImageButton *m_pcStopButton;
		ImageButton *m_pcHomeButton;

		BrowserChromeClient *m_pcChromeClient;

		WebSettings *m_pcWebSettings;
		Settings *m_pcSettings, *m_pcGuiSettings;
		Rect m_cWindowFrame, m_cSettingsFrame;

		BookmarksManager *m_pcBookmarksManager;
};

/* The global mutex is supplied by WebCore */
extern Locker g_cGlobalMutex;

BrowserWindow::BrowserWindow( const Rect &cFrame ) : Window( cFrame, "webview_wnd", "Webster" )
{
	Rect cMenuFrame, cToolFrame, cStatusFrame, cTabFrame;

	m_pcChromeClient = new BrowserChromeClient( this );
	m_pcWebSettings = new WebSettings();
	m_pcWebSettings->Load();

	m_pcSettings = new Settings();
	m_pcSettings->Load();

	/* Get window layouts */
	m_pcGuiSettings = new Settings();
	m_pcGuiSettings->SetFile( "Gui" );
	m_pcGuiSettings->Load();

	m_cWindowFrame = m_pcGuiSettings->GetRect( "webster", cFrame );
	m_cSettingsFrame = m_pcGuiSettings->GetRect( "settings", Rect( 25, 25, 600, 450 ) );

	SetFrame( m_cWindowFrame );
	cMenuFrame = cToolFrame = cStatusFrame = cTabFrame = GetBounds();

	/* DANGER WILL ROBINSON! See the note in the BrowserApp constructor about this mutex! */
	g_cGlobalMutex.Lock();
	SetMutex( &g_cGlobalMutex );

	m_pcMenuBar = new Menu( cMenuFrame, "menubar", ITEMS_IN_ROW );

	Menu *pcApplicationMenu = new Menu( Rect(), "Application", ITEMS_IN_COLUMN );
	pcApplicationMenu->AddItem( "Quit", new Message( M_TERMINATE ), "Ctrl+Q" );
	pcApplicationMenu->AddItem( new MenuSeparator() );
	pcApplicationMenu->AddItem( "About Webster", new Message( ID_MENU_APP_ABOUT ) );
	m_pcMenuBar->AddItem( pcApplicationMenu );

	Menu *pcWindowMenu = new Menu( Rect(), "Window", ITEMS_IN_COLUMN );
	pcWindowMenu->AddItem( "New window", new Message( ID_CREATE_WINDOW ), "Ctrl+N" );
	pcWindowMenu->AddItem( "Close window", new Message( M_QUIT ), "Ctrl+W" );
	pcWindowMenu->AddItem( new MenuSeparator() );
	pcWindowMenu->AddItem( "New tab", new Message( ID_MENU_WIN_NEW_TAB ), "Ctrl+T" );
	pcWindowMenu->AddItem( "Close tab", new Message( ID_MENU_WIN_CLOSE_TAB ) );
	m_pcMenuBar->AddItem( pcWindowMenu );

	Menu *pcEditMenu = new Menu( Rect(), "Edit", ITEMS_IN_COLUMN );
	pcEditMenu->AddItem( "Cut", new Message( ID_MENU_EDIT_CUT ), "Ctrl+X" );
	pcEditMenu->AddItem( "Copy", new Message( ID_MENU_EDIT_COPY ), "Ctrl+C" );
	pcEditMenu->AddItem( "Paste", new Message( ID_MENU_EDIT_PASTE ), "Ctrl+V" );
	pcEditMenu->AddItem( new MenuSeparator() );
	pcEditMenu->AddItem( "Delete", new Message( ID_MENU_EDIT_DELETE ) );
	m_pcMenuBar->AddItem( pcEditMenu );

	Menu *pcSettingsMenu = new Menu( Rect(), "Settings", ITEMS_IN_COLUMN );
	pcSettingsMenu->AddItem( "Configure", new Message( ID_MENU_SETTINGS_CONFIGURE ) );
	m_pcMenuBar->AddItem( pcSettingsMenu );

	m_pcBookmarksManager = new BookmarksManager();
	BookmarksMenu *pcBookmarksMenu = m_pcBookmarksManager->CreateMenu( "Bookmarks", Path( "" ) );
	m_pcMenuBar->AddItem( pcBookmarksMenu );

	m_pcMenuBar->SetTargetForItems( this );

	cMenuFrame.bottom = m_pcMenuBar->GetPreferredSize( false ).y;
	m_pcMenuBar->SetFrame( cMenuFrame );
	AddChild( m_pcMenuBar );

	/* Setup the toolbar */
	bool bShowButtonText = m_pcSettings->GetBool( "show_button_text", true );
	m_pcToolBar = new ToolBar( Rect(), "toolbar", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	ResStream *pcStream;
	File cSelf( open_image_file( get_image_id() ) );
	Resources cCol( &cSelf );		

	m_pcBackButton = new ImageButton( Rect(), "back", "Back", new Message(ID_BUTTON_BACK), NULL, ImageButton::IB_TEXT_BOTTOM, true, bShowButtonText, true );
	pcStream = cCol.GetResourceStream( "back.png" );
	m_pcBackButton->SetImage( pcStream );
	delete( pcStream );

	m_pcForwardButton = new ImageButton( Rect(), "foward", "Forward", new Message(ID_BUTTON_FORWARD), NULL, ImageButton::IB_TEXT_BOTTOM, true, bShowButtonText, true );
	pcStream = cCol.GetResourceStream( "forward.png" );
	m_pcForwardButton->SetImage( pcStream );
	delete( pcStream );

	m_pcReloadButton = new ImageButton( Rect(), "reload", "Reload", new Message(ID_BUTTON_RELOAD), NULL, ImageButton::IB_TEXT_BOTTOM, true, bShowButtonText, true );
	pcStream = cCol.GetResourceStream( "reload.png" );
	m_pcReloadButton->SetImage( pcStream );
	delete( pcStream );

	m_pcStopButton = new ImageButton( Rect(), "stop", "Stop", new Message(ID_BUTTON_STOP), NULL, ImageButton::IB_TEXT_BOTTOM, true, bShowButtonText, true );
	pcStream = cCol.GetResourceStream( "stop.png" );
	m_pcStopButton->SetImage( pcStream );
	delete( pcStream );

	m_pcHomeButton = new ImageButton( Rect(), "home", "Home", new Message(ID_BUTTON_HOME), NULL, ImageButton::IB_TEXT_BOTTOM, true, bShowButtonText, true );
	pcStream = cCol.GetResourceStream( "home.png" );
	m_pcHomeButton->SetImage( pcStream );
	delete( pcStream );

	m_pcBackButton->SetEnable( false );
	m_pcForwardButton->SetEnable( false );
	m_pcStopButton->SetEnable( false );

	m_pcToolBar->AddChild( m_pcBackButton, ToolBar::TB_FIXED_WIDTH );
	m_pcToolBar->AddChild( m_pcForwardButton, ToolBar::TB_FIXED_WIDTH );
	m_pcToolBar->AddChild( m_pcReloadButton, ToolBar::TB_FIXED_WIDTH );
	m_pcToolBar->AddChild( m_pcStopButton, ToolBar::TB_FIXED_WIDTH );
	m_pcToolBar->AddChild( m_pcHomeButton, ToolBar::TB_FIXED_WIDTH );

	m_pcUrlEdit = new UrlEdit( Rect(), "urledit", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );
	m_pcUrlEdit->SetMinPreferredSize( 32 );
	m_pcUrlEdit->SetMaxPreferredSize( 256 );
	m_pcUrlEdit->SetEditMessage( new Message( ID_URL_CHANGED ) );
	m_pcUrlEdit->SetSelectionMessage( new Message( ID_URL_CHANGED ) );
	m_pcUrlEdit->SetTarget( this, this );

	m_pcToolBar->AddChild( m_pcUrlEdit, ToolBar::TB_FREE_WIDTH );

	cToolFrame.top = cMenuFrame.bottom + 1.0f;
	cToolFrame.bottom = cToolFrame.top + m_pcToolBar->GetPreferredSize(false).y;
	m_pcToolBar->SetFrame( cToolFrame );
	AddChild( m_pcToolBar );

	m_pcStatusBar = new StatusBar( Rect(), "statusbar", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_BOTTOM );
	m_pcStatusBar->AddPanel( "text", "" );

	m_pcProgress = new ProgressPanel( "progress", 10 );
	m_pcStatusBar->AddPanel( m_pcProgress );

	cStatusFrame.top = cStatusFrame.bottom - 20;
	m_pcStatusBar->SetFrame( cStatusFrame );
	AddChild( m_pcStatusBar );

	cTabFrame.top = cToolFrame.bottom + 1.0f;
	cTabFrame.bottom = cStatusFrame.top - 1.0f;

	m_pcTabView = new TabView( cTabFrame, "webviewtabs", CF_FOLLOW_ALL );
	m_pcTabView->SetMessage( new Message( ID_TAB_CHANGED ) );
	AddChild( m_pcTabView );

	/* Create a tab and open the homepage, if one is configured */
	CreateTab( m_pcSettings->GetString( "homepage", "about:blank" ) );

	/* Set Window icon */
	pcStream = cCol.GetResourceStream( "icon24x24.png" );
	BitmapImage *pcIcon = new BitmapImage( pcStream );
	delete( pcStream );
	SetIcon( pcIcon->LockBitmap() );
	delete( pcIcon );

	/* Nothing is being loaded at this point */
	UpdateButtonState( false );
}

BrowserWindow::~BrowserWindow()
{
	RemoveChild( m_pcToolBar );
	delete m_pcToolBar;
	RemoveChild( m_pcStatusBar );
	delete m_pcStatusBar;

	RemoveChild( m_pcTabView );
	delete m_pcTabView;

	delete m_pcBookmarksManager;

	m_pcWebSettings->Save();
	delete m_pcWebSettings;

	m_pcGuiSettings->Save();
	delete m_pcGuiSettings;
}

void BrowserWindow::HandleMessage( Message * pcMsg )
{
	BrowserWebView *pcWebView = GetCurrentWebView();

	switch( pcMsg->GetCode() )
	{
		case ID_URL_CHANGED:
		{
			bool bFinal = false;
			pcMsg->FindBool( "final", &bFinal );

			if( bFinal )
			{
				String cURL = "";
				int nSelection = -1;
				if( pcMsg->FindInt32( "selection", &nSelection ) == EOK )
					cURL = m_pcUrlEdit->GetItem( nSelection );
				else
				{
					cURL = m_pcUrlEdit->GetCurrentString();
					m_pcUrlEdit->AppendItem( cURL );
				}

				OpenURL( cURL );
			}
			break;
		}
		case ID_SET_STATUS_BAR_TEXT:
		{
			String cText;
			if( pcMsg->FindString( "text", &cText ) == EOK )
				m_pcStatusBar->SetText( "text", cText );
			break;
		}
		case ID_CLEAR_STATUS_BAR_TEXT:
		{
			m_pcStatusBar->SetText( "text", "" );
			break;
		}
		case ID_CREATE_WINDOW:
		{
			BrowserWindow *pcWindow;
			String cURL;
			Rect cFrame;

			/* Use the supplied dimensions if they've been provided, otherwise
			   clone the current Window */
			if( pcMsg->FindRect( "frame", &cFrame ) != EOK )
				cFrame = GetFrame();

			pcWindow = new BrowserWindow( cFrame );

			if( pcMsg->FindString( "url", &cURL ) == EOK )
				pcWindow->OpenURL( cURL );

			pcWindow->Show();
			pcWindow->MakeFocus();

			/* Tell BrowserApp about the new window */
			Application::GetInstance()->PostMessage( ID_WINDOW_OPENED );

			if( pcMsg->IsSourceWaiting() )
			{
				WebCore::Page *pcPage;
				View *pcTab;

				/* The new window will only have one tab at this stage so it's safe to always get tab #0 */
				pcTab = pcWindow->m_pcTabView->GetTabView( 0 );
				pcPage = static_cast<BrowserWebView*>( pcTab )->GetWebCoreFrame()->page();

				Message cReply( ID_CREATE_WINDOW_REPLY );
				cReply.AddPointer( "page", pcPage );

				pcMsg->SendReply( &cReply );
			}

			break;
		}
		case ID_CREATE_TAB:
		{
			String cURL;
			if( pcMsg->FindString( "url", &cURL ) == EOK )
			{
				uint nTabIndex;

				nTabIndex = CreateTab( cURL );

				if( pcMsg->IsSourceWaiting() )
				{
					WebCore::Page *pcPage;
					View *pcTab;

					pcTab = m_pcTabView->GetTabView( nTabIndex );
					pcPage = static_cast<BrowserWebView*>( pcTab )->GetWebCoreFrame()->page();

					Message cReply( ID_CREATE_WINDOW_REPLY );
					cReply.AddPointer( "page", pcPage );

					pcMsg->SendReply( &cReply );
				}
			}

			break;
		}
		case ID_BUTTON_BACK:
		{
			pcWebView->GoBack();

			UpdateButtonState( false );
			break;
		}
		case ID_BUTTON_FORWARD:
		{
			pcWebView->GoForward();

			UpdateButtonState( false );
			break;
		}
		case ID_BUTTON_RELOAD:
		{
			pcWebView->Reload();
			break;
		}
		case ID_BUTTON_STOP:
		{
			pcWebView->Stop();

			UpdateButtonState( false );
			break;
		}
		case ID_BUTTON_HOME:
		{
			OpenURL( m_pcSettings->GetString( "homepage", "about:blank" ) );
			break;
		}
		case ID_MENU_APP_ABOUT:
		{
			Alert* pcAbout = new Alert( "About Webster",
										"Webster " WEBSTER_VERSION " (Alpha)\nA WebCore based web browser\n\n© Copyright Kristian Van Der Vliet, 2008\nArno Klenke, 2004-2007\nKurt Skauen, 2001\n"
										"\nWebster is released under the Gnu Public License (GPL)\n",
										Alert::ALERT_INFO, 0x00, "Ok", NULL );
			pcAbout->Go( NULL );

			break;
		}
		case ID_MENU_WIN_NEW_TAB:
		{
			CreateTab( "" );
			break;
		}
		case ID_MENU_WIN_CLOSE_TAB:
		{
			DestroyTab( m_pcTabView->GetSelection() );
			break;
		}
		case ID_MENU_EDIT_CUT:
		{
			pcWebView->Cut();
			break;
		}
		case ID_MENU_EDIT_COPY:
		{
			pcWebView->Copy();
			break;
		}
		case ID_MENU_EDIT_PASTE:
		{
			pcWebView->Paste();
			break;
		}
		case ID_MENU_EDIT_DELETE:
		{
			pcWebView->Delete();
			break;
		}
		case ID_MENU_SETTINGS_CONFIGURE:
		{
			Rect cSettingsFrame = m_pcGuiSettings->GetRect( "settings", m_cSettingsFrame );
			SettingsWindow *pcSettingsWindow = new SettingsWindow( cSettingsFrame, "Webster settings", this, m_pcWebSettings, m_pcSettings, m_pcGuiSettings );
			pcSettingsWindow->Show();
			pcSettingsWindow->MakeFocus();

			break;
		}
		case ID_SETTINGS_SAVE:
		{
			Settings *pcSettings;
			if( pcMsg->FindPointer( "settings", (void**)&pcSettings ) == EOK )
			{
				delete m_pcSettings;
				m_pcSettings = pcSettings;

				m_pcSettings->Save();
			}

			WebSettings *pcWebSettings;
			if( pcMsg->FindPointer( "web_settings", (void**)&pcWebSettings ) == EOK )
			{
				delete m_pcWebSettings;
				m_pcWebSettings = pcWebSettings;
			}

			break;
		}
		case ID_SETTINGS_APPLY:
		{
			Settings *pcSettings;
			if( pcMsg->FindPointer( "settings", (void**)&pcSettings ) == EOK )
			{
				delete m_pcSettings;
				m_pcSettings = pcSettings;
			}

			WebSettings *pcWebSettings;
			if( pcMsg->FindPointer( "web_settings", (void**)&pcWebSettings ) == EOK )
			{
				delete m_pcWebSettings;
				m_pcWebSettings = pcWebSettings;
			}

			break;
		}
		case ID_MENU_BOOKMARKS_ADD:
		{
			String cShortTitle, cLongTitle, cURL;

			pcWebView->GetTitle( cShortTitle, cLongTitle );
			cURL = pcWebView->GetCurrentURL();

			m_pcBookmarksManager->AddBookmark( cLongTitle, cURL );

			break;
		}
		case ID_MENU_BOOKMARKS_MANAGE:
		{
			String cPath;
			if( pcMsg->FindString( "path", &cPath ) != EOK )
				break;

			/* XXXKV: This is a really rubbish way to manage bookmarks: we should have
			   a proper in-application dialog to do it */

			/* Open a filebrowser window */
			if( fork() == 0 )
			{
				set_thread_priority( -1, 0 );
				execlp( "/system/bin/FileBrowser", "/system/bin/FileBrowser", cPath.c_str(), NULL );
			}
			break;
		}
		case ID_BOOKMARK_GO:
		{
			String cURL;

			if( pcMsg->FindString( "url", &cURL ) == EOK )
				OpenURL( cURL );

			break;
		}
		case ID_TAB_CHANGED:
		{
			ChangeTab( m_pcTabView->GetSelection() );
			break;
		}
		case ID_WEBVIEW_SET_TITLE:
		{
			String cShortTitle, cLongTitle;
			uint nTabIndex;

			if( pcMsg->FindString( "short", &cShortTitle ) == EOK &&
				pcMsg->FindString( "long", &cLongTitle ) == EOK &&
				pcMsg->FindInt32( "index", (int32*)&nTabIndex ) == EOK )
			{
				m_pcTabView->SetTabTitle( nTabIndex, cShortTitle );

				/* If this is the currently selected tab, set the Window title to the full page title */
				if( nTabIndex == m_pcTabView->GetSelection() )
					SetTitle( cLongTitle );
			}

			break;
		}
		case ID_WEBVIEW_LOAD_STARTED:
		{
			String cURL;
			uint nTabIndex;

			if( pcMsg->FindString( "url", &cURL ) == EOK &&
				pcMsg->FindInt32( "index", (int32*)&nTabIndex ) == EOK )
			{
				if( nTabIndex == m_pcTabView->GetSelection() )
				{
					m_pcUrlEdit->SetCurrentString( cURL.c_str() );
					UpdateButtonState( true );
				}
			}
			break;
		}
		case ID_WEBVIEW_LOAD_FINISHED:
		{
			uint nTabIndex;
			if( pcMsg->FindInt32( "index", (int32*)&nTabIndex ) == EOK )
				if( nTabIndex == m_pcTabView->GetSelection() )
					UpdateButtonState( false );
			break;
		}
		default:
		{
			fprintf( stderr, "Unknown message with code #%d\n", pcMsg->GetCode() );
			Window::HandleMessage( pcMsg );
			break;
		}
	}
}

bool BrowserWindow::OkToQuit( void )
{
	/* Store current GUI settings */
	m_pcGuiSettings->SetRect( "webster", GetFrame() );

	/* Tell BrowserApp the window is closing */
	Application::GetInstance()->PostMessage( ID_WINDOW_CLOSED );
	return true;
}

void BrowserWindow::OpenURL( String cURL )
{
	BrowserWebView *pcWebView = GetCurrentWebView();

	url_protocol_e nProtocol = UrlFilter::Filter( cURL );
	if( nProtocol != URL_IS_UNKNOWN && nProtocol != URL_IS_EMPTY )
		pcWebView->OpenURL( cURL );
	else
		/* XXXKV: Eventually we should pass the URLs on to the Registrar: perhaps
		   an external program knows how to handle it? */
		fprintf( stderr, "Don't know how to open URL %s\n", cURL.c_str() );
}

void BrowserWindow::UpdateButtonState( bool bLoadStarted )
{
	BrowserWebView *pcWebView = GetCurrentWebView();

	if( bLoadStarted )
	{
		m_pcStopButton->SetEnable( true );
		m_pcReloadButton->SetEnable( false );

		m_pcProgress->SetProgress( true );
	}
	else
	{
		m_pcStopButton->SetEnable( false );
		m_pcReloadButton->SetEnable( true );

		m_pcProgress->SetProgress( false );
	}

	if( pcWebView->GetForwardListCount() > 0 )
		m_pcForwardButton->SetEnable( true );
	else
		m_pcForwardButton->SetEnable( false );

	if( pcWebView->GetBackListCount() > 0 )
		m_pcBackButton->SetEnable( true );
	else
		m_pcBackButton->SetEnable( false );
}

uint BrowserWindow::CreateTab( String cURL )
{
	BrowserWebView *pcWebView;
	uint nTabIndex;

	nTabIndex = m_pcTabView->GetTabCount();
	pcWebView = new BrowserWebView( m_pcTabView->GetBounds(), "webview", nTabIndex, m_pcChromeClient, m_pcWebSettings );

	m_pcTabView->AppendTab( "about:blank", pcWebView );
	pcWebView->MakeFocus();

	/* Open the URL if the user middle-clicked on a link, but let it load in the background. Otherwise, focus the new tab */
	if( cURL != "" )
		pcWebView->OpenURL( cURL );
	else
		m_pcTabView->SetSelection( nTabIndex );

	return nTabIndex;
}

void BrowserWindow::ChangeTab( uint nIndex )
{
	String cURL, cShortTitle, cLongTitle;
	BrowserWebView *pcWebView = static_cast<BrowserWebView*>( m_pcTabView->GetTabView( nIndex ) );

	cURL = pcWebView->GetCurrentURL();
	m_pcUrlEdit->SetCurrentString( cURL.c_str() );

	pcWebView->GetTitle( cShortTitle, cLongTitle );
	m_pcTabView->SetTabTitle( nIndex, cShortTitle );
	SetTitle( cLongTitle );

	/* Set the "is loading" flag according to the state of the tab */
	UpdateButtonState( pcWebView->IsLoading() );
}

void BrowserWindow::DestroyTab( uint nIndex )
{
	BrowserWebView *pcWebView = static_cast<BrowserWebView*>( m_pcTabView->GetTabView( nIndex ) );

	m_pcTabView->DeleteTab( nIndex );
	delete( pcWebView );

	/* Any tabs "above" this one will change their index we need to inform them all. */
	uint nTotal, nCurrentTab;

	nTotal = m_pcTabView->GetTabCount();
	if( nTotal > 0 )
	{
		for( nCurrentTab = 0; nCurrentTab < nTotal; nCurrentTab++ )
		{
			pcWebView = static_cast<BrowserWebView*>( m_pcTabView->GetTabView( nCurrentTab ) );
			pcWebView->SetTabIndex( nCurrentTab );
		}
	}
	else
	{
		/* User closed the last tab in this window. Close the window */
		PostMessage( M_QUIT );
	}
}

BrowserWebView* BrowserWindow::GetCurrentWebView( void ) const
{
	/* Get the WebView for the current tab */
	uint nCurrentTab;

	nCurrentTab = m_pcTabView->GetSelection();
	return static_cast<BrowserWebView*>( m_pcTabView->GetTabView( nCurrentTab ) );
}

class BrowserApp : public Application
{
	public:
		BrowserApp( const char *pzName, String cURL ) : Application( pzName )
		{
			/* Register types */
			m_pcManager = NULL;
			try
			{
				char zPath[PATH_MAX] = {0};
				m_pcManager = RegistrarManager::Get();
		
				m_pcManager->RegisterType( "text/x-bookmark", "Bookmark" );
				m_pcManager->RegisterTypeIconFromRes( "text/x-bookmark", "text_bookmark.png" );
				m_pcManager->RegisterAsTypeHandler( "text/x-bookmark" );
		
				m_pcManager->RegisterType( "text/html", "HTML Page" );
				m_pcManager->RegisterTypeExtension( "text/html", "htm" );
				m_pcManager->RegisterTypeExtension( "text/html", "html" );
				m_pcManager->RegisterTypeIconFromRes( "text/html", "text_html.png" );
				m_pcManager->RegisterAsTypeHandler( "text/html" );
			}
			catch( ... )
			{
			}

			/* Create bookmarks folder */
			char *pzHome = getenv( "HOME" );
			if( NULL != pzHome )
				try
				{
					Directory cDir( pzHome );
					Directory cBookmarkDir;
					cDir.CreateDirectory( "Bookmarks", &cBookmarkDir, cDir.GetMode() );
				}
				catch( ... )
				{
				}

			m_pcWindow = new BrowserWindow( Rect( 0, 0, 600, 400 ) );
			if( m_pcWindow->GetBounds() == Rect( 0, 0, 600, 400 ) )
				m_pcWindow->CenterInScreen();

			/* DANGER WILL ROBINSON! The renderer MUST be started and running before we
			   call Window::Show(), because everything shares the same mutex and it is
			   VERY VERY PICKY about the order in which the locks are taken. If Show() is
			   called before the renderer has started it will lead to a deadlock on
			   g_cGlobalMutex. THIS IS A VERY BAD DESIGN, and I shall try to find a
			   sensible fix.

			   Yes, it did take me quite some time before I realised this was the case. */

			if( cURL != "" )
				m_pcWindow->OpenURL( cURL );

			m_nWindows = 1;

			m_pcWindow->Show();
			m_pcWindow->MakeFocus();
		}
		~BrowserApp()
		{
			WebView::Shutdown();

			if( m_pcManager )
				m_pcManager->Put();
		}
		void HandleMessage( Message * pcMsg )
		{
			switch( pcMsg->GetCode() )
			{
				case ID_WINDOW_OPENED:
				{
					++m_nWindows;
					break;
				}
				case ID_WINDOW_CLOSED:
				{
					if(--m_nWindows == 0 )
						PostMessage( M_QUIT );
					break;
				}
				default:
				{
					Application::HandleMessage( pcMsg );
					break;
				}
			}
		}

	private:
		RegistrarManager* m_pcManager;

		BrowserWindow *m_pcWindow;
		int m_nWindows;
};

int main( int argc, char *argv[] )
{
	String cURL = "";
	if( argc > 1 )
		cURL = argv[1];

	BrowserApp *pcApp = new BrowserApp( "Webster", cURL );
	pcApp->Run();

	return 0;
}

