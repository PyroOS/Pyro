#include "mainwindow.h"
#include "server.h"
#include "settings.h"
#include "settingsmenu.h"
#include "messages.h"
#include "progresswindow.h"
#include "connectbar.h"
#include "containerview.h"
#include "requesters.h"

#include <gui/requesters.h>
#include <gui/spinner.h>
#include <gui/stringview.h>

#include <util/regexp.h>

#include <pyro/time.h>	/* For get_system_time() */

using namespace os;

/* The following grammar was modified from from RFC-3986.
* This is the actual grammar written using the regexp syntax along with the
* bnf syntax for grammars. The ones used by this class are defined as Strings
* using static variables.
*
* Syntax Grammar:
* ; COLON | SLASH | QUESTION | HASH | LBRACKET | RBRACKET | AT
* gen-delims ::= ':' | '/' | '?' | '#' | '[' | ']' | '@'
*
* ; EXCLAMATION | DOLLAR | AMPERSAND | SQUOTE | LPAREN | RPAREN | STAR | PLUS | COMMA | SEMI | EQ
* sub-delims ::= '!' | '$' | '&' | "'" | "(" | ')' | '*' | '+' | ',' | ";" | '=' 
*
* ; ALPHA | DIGIT | DASH | DOT | UNDERSCORE | TILDE
* unreserved ::= [a-zA-Z] | [0-9] | '-' | '.' | '_' | '~'
*
* URI ::= scheme COLON hier-part (QUESTION query)? (HASH fragment)?
* scheme ::= ALPHA(ALPHA | DIGIT | PLUS | DASH | DOT)*
* hier-part ::= SLASH SLASH authority path-abempty
*             | path-absolute
*             | path-rootless
*             | path-empty
*
* authority = ( userinfo AT )? host (COLON port)?
*
* userinfo = (unreserved | percent-encoded | sub-delims | COLON)*
* ; Note: the username:password syntax has been depricated so 
* ; there shouldn't be anything following a COLON (due to IPv6).
* ; however this is still supported by most syntaxes for ftp so we
* ; leave this alone for the grammar.
*
* ; In the code the password is parsed using this but disallowing COLON's
*
* host ::= IP-literal | IPv4address | reg-name
* IP-literal ::= LBRACKET (IPv6address | IPvFuture) RBRACKET
* IPvFuture ::= "v" (HEXDIGIT)+ DOT (unreserved | sub-delims | COLON)+
* IPv6address ::= (h16 COLON){6,6} ls32
*               | COLON COLON (h16 COLON){5,5} ls32
*               | (h16)? COLON COLON (h16 COLON){4,4} ls32
*               | ( (h16 COLON)? h16 )? COLON COLON (h16 COLON){3,3} ls32
*               | ( (h16 COLON){0,2} h16 )? COLON COLON (h16 COLON){2,2} ls32
*               | ( (h16 COLON){0,3} h16 )? COLON COLON h16 COLON ls32
*               | ( (h16 COLON){0,4} h16 )? COLON COLON ls32
*               | ( (h16 COLON){0,5} h16 )? COLON COLON h16
*               | ( (h16 COLON){0,6} h16 )? COLON COLON
* ls32 ::= ( h16 COLON h16 ) | IPv4address
* h16  ::= (HEXDIGIT){1,4}
*
* IPv4address ::= dec-octet DOT dec-octet DOT dec-octet DOT dec-octet
* dec-octet   ::= DIGIT           # AKA 0-255
*               | [1-9] DIGIT
*               | 1 DIGIT{2,2}
*               | 2 [0-4] DIGIT
*               | 25	[0-5]
*
* reg-name    ::= ( unreserved | percent-encoded | sub-delims)*
*
*
* path ::= path-abempty | path-absolute | path-noscheme | path-rootless | path-empty
*
* path-abempty = ( SLASH segement)*
* path-absolute = SLASH ( segment-nz ( SLASH segment)* )?
* path-noscheme segment-nz-nc ( SLASH segment )*
* path-rootless segment-nz ( SLASH segment )*
* path-empty = pchar{0,0}
*
* segment = pchar*
* segment-nz = pchar+
* segment-nz-nc = (unreserved | percent-encoded | sub-delims | COLON | AT)+
*
* pchar = unreserved | percent-encoded | sub-delims | COLON | AT
*
* query ::= (pchar | SLASH | QUESTION)*
* fragment ::= (pchar | SLASH | QUESTION)*
*
* percent-encoded = PERCENT HEXDIGIT HEXDIGIT
* unreserved = ALPHA | DIGIT | - | . | _ | ~
*/

const os::String MainWindow::sUnreserved = "[-a-zA-Z0-9\\_\\~.]";
const os::String MainWindow::sPctEncoded = "%[0-9a-fA-F][0-9a-fA-F]";
const os::String MainWindow::sSubDelims = "[!$&'()*+,;=]";
const os::String MainWindow::sUserinfo_nc = os::String("(") + MainWindow::sUnreserved +
									os::String("|") + MainWindow::sPctEncoded + 
									os::String("|") + MainWindow::sSubDelims +
									os::String(")*");
const os::String MainWindow::sDecOctet = "([0-9]|[1-9][0-9]|1[0-9]{2,2}|2[0-4][0-9]|25[0-5])";

// Old version: "([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})";
const os::String MainWindow::sIPv4address = os::String("(") + sDecOctet +
											os::String("\\.") + sDecOctet +
											os::String("\\.") + sDecOctet +
											os::String("\\.") + sDecOctet +
											os::String(")");
											
const os::String MainWindow::sH16 = "([0-9a-fA-F]{1,4})";
const os::String MainWindow::sLS32 = os::String("((") + MainWindow::sH16 + os::String(":")
									+ MainWindow::sH16 + os::String(")|") 
									+ MainWindow::sIPv4address + os::String(")");

/* IPvFuture ::= "v" (HEXDIGIT)+ DOT (unreserved | sub-delims | COLON)+ */
const os::String MainWindow::sIPvFuture = os::String("v[0-9a-fA-F]+\\.(") + 
									MainWindow::sUnreserved + os::String("|") +
									MainWindow::sSubDelims + os::String("|:)+");
																																				
									/* (h16 COLON){6,6} ls32 */
const os::String MainWindow::sIPv6address = os::String("(") + MainWindow::sH16 +
									os::String(":){6,6}") + MainWindow::sLS32 +
									
									/* | COLON COLON (h16 COLON){5,5} ls32 */
									os::String("|::(") + MainWindow::sH16 +
									os::String(":){5,5}") + MainWindow::sLS32 +
									
									/* | (h16)? COLON COLON (h16 COLON){4,4} ls32 */
									os::String("|(") + MainWindow::sH16 +
									os::String(")?::(") + MainWindow::sH16 + 
									os::String(":){4,4}") + MainWindow::sLS32 +
									
									/*| ( (h16 COLON)? h16 )? COLON COLON (h16 COLON){3,3} ls32*/
									os::String("|((") + MainWindow::sH16 +
									os::String(":)?") + MainWindow::sH16 +
									os::String(")?::(") + MainWindow::sH16 + 
									os::String(":){3,3}") + MainWindow::sLS32 +
				
									/*| ( (h16 COLON){0,2} h16 )? COLON COLON (h16 COLON){2,2} ls32 */
									os::String("|((") + MainWindow::sH16 +
									os::String(":){0,2}") + MainWindow::sH16 +
									os::String(")?::(") + MainWindow::sH16 + 
									os::String(":){2,2}") + MainWindow::sLS32 +
									
									/*| ( (h16 COLON){0,3} h16 )? COLON COLON h16 COLON ls32  */
									os::String("|((") + MainWindow::sH16 +
									os::String(":){0,3}") + MainWindow::sH16 +
									os::String(")?::") + MainWindow::sH16 + 
									os::String(":") + MainWindow::sLS32 +

									/*| ( (h16 COLON){0,4} h16 )? COLON COLON ls32 */
									os::String("|((") + MainWindow::sH16 +
									os::String(":){0,4}") + MainWindow::sH16 +
									os::String(")?::") + MainWindow::sLS32 +
									
									/* | ( (h16 COLON){0,5} h16 )? COLON COLON h16 */
									os::String("|((") + MainWindow::sH16 +
									os::String(":){0,5}") + MainWindow::sH16 +
									os::String(")?::") + MainWindow::sH16 +
									
									/* | ( (h16 COLON){0,6} h16 )? COLON COLON */
									os::String("|((") + MainWindow::sH16 +
									os::String(":){0,6}") + MainWindow::sH16 +
									os::String(")?::");

/* IP-literal ::= LBRACKET (IPv6address | IPvFuture) RBRACKET. See RFC 3986. */
const os::String MainWindow::sIP_literal = os::String("\\[(") + MainWindow::sIPvFuture +
										  os::String("|") + MainWindow::sIPv6address +
										  os::String(")\\]");


/** \brief Main Window Constructor.
 * This creates and registers the main window and all of its widgets.
 * It also loads the settings from the settings file and all the required
 * icons. Then it creates the progress window for listing downloads in queue.
 */
MainWindow::MainWindow()
: os::Window( os::Rect( 0, 0, 700, 300 ), "main_wnd", "Transferrer" )
{
	m_pcSettings = new AppSettings();
	m_pcSettings->Load();		/* Load settings from the default file */
	g_bDebug = m_pcSettings->GetDebugMode();

	SetupMenus();
	SetupToolBar();
	
//	SetupStatusBar();	/* Status bar currently isn't useful */
	Layout();	

	/* Create the progress window. */
	m_pcProgressWindow = new ProgressWindow( this, m_pcSettings );
	PositionWindows();
	m_pcProgressWindow->Show();
	
	/* Set Icon */
	os::Resources cCol( get_image_id() );
	os::ResStream *pcStream = cCol.GetResourceStream( "icon48x48.png" );
	os::BitmapImage *pcIcon = new os::BitmapImage( pcStream );
	SetIcon( pcIcon->LockBitmap() );
	delete( pcIcon );
	
	/* Load most recently used server address */
	if( m_pcSettings->GetSaveHistory() ) {
		String zHost;
		String zPort;
		String zUser;
		String zPassword;
		m_pcSettings->LoadHistory( &zHost, &zPort, &zUser, &zPassword );

		m_pcConnectBar->SetHost(zHost);
		m_pcConnectBar->SetPort(zPort);
		m_pcConnectBar->SetUser(zUser);
		m_pcConnectBar->SetPass(zPassword);
	}
	
	m_pcServer = NULL;
}

/** \brief MainWindow Deconstructor.
 * This closes the server connection.
 */
MainWindow::~MainWindow()
{
	/* TODO: should make sure that the progress window has quit before returning, since the progress window assumes that the MainWindow still exists */
	if( m_pcSettings ) {
		/* Save settings */
		m_pcSettings->SetMainWindowFrame( GetFrame() );
		/* SafeLock() in case the ProgressWindow has already closed. */
		/* TODO: Should save progress window settings properly even if the progress window closes first. */
		if( m_pcProgressWindow->SafeLock() == 0 ) {
			m_pcSettings->SetProgressWindowFrame( m_pcProgressWindow->GetFrame() );
			m_pcProgressWindow->Unlock();
		}
		m_pcSettings->Save();
	}


	if( m_pcServer ) m_pcServer->Close();
}

/** \brief Open Connection.
 * This is used by the application to attempt to open a window after startup.
 *
 * \param zScheme The scheme of the protocol (eg. ftp, http, etc).
 * \param zHost The host to connect to.
 * \param zPort The port number string.
 * \param zUser The username to login using.
 * \param zPassword The password to login with.
 * 
 */
void MainWindow::OpenConnection(const String& zScheme, const String& zHost,
								const String& zPort, const String& zUser,
								const String& zPassword )
{
	
	// Write the data into the connect bar.
	m_pcConnectBar->SetHost(zHost);
	m_pcConnectBar->SetPort(zPort);
	m_pcConnectBar->SetUser(zUser);
	m_pcConnectBar->SetPass(zPassword);
	
	// Invoke the connect bar button's action.
	PostMessage( M_CONNECT );
}

/** \brief Setup Menus
 * This generates the contents of the main menu.
 */
void MainWindow::SetupMenus()
{
	m_pcMainMenu = new os::Menu(os::Rect(0,0,1,1),"main_menu", os::ITEMS_IN_ROW);
	
	os::Menu* file = new os::Menu(os::Rect(0,0,1,1),"File",os::ITEMS_IN_COLUMN);
//	file->AddItem("About",new os::Message(M_APP_ABOUT));
//	file->AddItem(new os::MenuSeparator());
	file->AddItem("Quit",new os::Message(M_APP_QUIT));
	
	
//	os::Menu* edit = new os::Menu(os::Rect(0,0,1,1),"Edit",os::ITEMS_IN_COLUMN);	/* Edit menu is currently unused */

	m_pcSettingsMenu = new SettingsMenu( "Settings", m_pcSettings );

//	os::Menu* help = new os::Menu(os::Rect(0,0,1,1),"Help",os::ITEMS_IN_COLUMN);	/* Help menu is currently unsed */

	m_pcMainMenu->AddItem(file);
//	m_pcMainMenu->AddItem(edit);
	m_pcMainMenu->AddItem(m_pcSettingsMenu);
//	m_pcMainMenu->AddItem(help);
		
	
	m_pcMainMenu->SetTargetForItems(this);
	m_pcMainMenu->ResizeTo(os::Point(GetBounds().Width(),m_pcMainMenu->GetPreferredSize(false).y));
	AddChild(m_pcMainMenu);
}

/** \brief Setup ToolBar
 * This generates the toolbar with the required tools.
 */
void MainWindow::SetupToolBar()
{
	m_pcToolBar = new os::ToolBar(os::Rect(),"toolbar");
	m_pcToolBar->AddChild(m_pcConnectBar = new ConnectBar(),os::ToolBar::TB_FIXED_WIDTH);
	m_pcToolBar->SetFrame( os::Rect(0,m_pcMainMenu->GetBounds().Height()+1,GetBounds().Width(),m_pcMainMenu->GetBounds().Height()+1+m_pcToolBar->GetPreferredSize(false).y));
	AddChild(m_pcToolBar);
}

/** \brief Setup Status Bar
 * Setup for status display at bottom of the window.
 */
void MainWindow::SetupStatusBar()
{
	m_pcStatusBar = new os::StatusBar(os::Rect(0,GetBounds().Height()-20,GetBounds().Width(),GetBounds().Height()),"",os::CF_FOLLOW_LEFT | os::CF_FOLLOW_RIGHT | os::CF_FOLLOW_BOTTOM);
	m_pcStatusBar->AddPanel("status","status");
	AddChild(m_pcStatusBar);
}

/** \brief Restore the windows to their saved positions.
 * Loads the position of the main and progress windows from the settings file, and sets the window frame.
 */
void MainWindow::PositionWindows()
{
	Rect cTmp = m_pcSettings->GetMainWindowFrame();
	if( cTmp == Rect(0,0,0,0) ) {
		/* No saved settings, so center the window */
		CenterInScreen();
	} else {
		SetFrame( cTmp );
	}
	
	cTmp = m_pcSettings->GetProgressWindowFrame();
	if( cTmp == Rect(0,0,0,0) ) {
		/* No saved settings, so place it below the main window */
		m_pcProgressWindow->MoveTo( (GetFrame().right + GetFrame().left - m_pcProgressWindow->GetBounds().Width())/2, GetFrame().bottom + 15 );
	} else {
		m_pcProgressWindow->SetFrame( cTmp );
	}
}

/** \brief Layout
 * Generate the main widgets for the window and add them to the window.
 */
void MainWindow::Layout()
{
	m_pcView = new ContainerView(os::Rect(0,(int)m_pcMainMenu->GetBounds().Height() + (int)m_pcToolBar->GetBounds().Height() + 1,GetBounds().Width(),GetBounds().Height()-20),"container_view");
	AddChild(m_pcView);
}

/** \brief Is Valid Host.
 * Verifies that the input host is valid.
 *
 * \param zHost Host string to validate.
 *
 */
bool MainWindow::isValidHost(const String& zHost)
{
	/* There are a couple different forms for the host name:
	 * 1) IPv4address = dec-octet '.' dec-octet '.' dec-octet '.' dec-octet
	 * 2) reg-name = sUserinfo_nc
	 * 3) IP-literal = "[" (PvFuture | IPv6address) "]"
	 */	 

	RegExp cHostRE = RegExp();
	
	// Next check for IPv4address.
	if ( cHostRE.Compile(String("^") + sIPv4address + String("$"), false, true) )
	{
		fprintf(stderr, "ERROR(%s, %d): Failed to compile host regular expression!\n",
		        __FILE__, __LINE__);
		return false;
	}
	
	// If it matches, accept it as a good host string.
	if ( cHostRE.Match(zHost) )
	{
		// fprintf(stderr, "Matched as IPv4address.\n");
		return true;
	}

	// Check for IP-literal
	if ( cHostRE.Compile(sIP_literal, false, true) )
	{
		fprintf(stderr, "ERROR(%s, %d): Failed to compile host regular expression!\n",
		        __FILE__, __LINE__);
		return false;
	}
	
	// If it matches, accept it as a good host string.
	if ( cHostRE.Match(zHost) )
	{
		// fprintf(stderr, "SUCCESS: Matched IP-literal\n");
		return true;
	}

	// This must be checked last (reg-name)
	if ( cHostRE.Compile(sUserinfo_nc, false, true) )
	{
		fprintf(stderr, "ERROR(%s, %d): Failed to compile host regular expression!\n",
		        __FILE__, __LINE__);
		return false;
	}
	
	// If it matches, accept it as a good host string.
	if ( cHostRE.Match(zHost) )
	{
		// fprintf(stderr, "Matched as reg-name.\n");
		return true;
	}
	
	// Return false for anything that doesn't pass.
	os::Alert* pcMyAlert = new os::Alert("Invalid Host Name", "Please verify that the host name is correct.", Alert::ALERT_WARNING, "OK", NULL);
	pcMyAlert->Go( new os::Invoker( 0 ) ); 
	return false;
}

/** \brief Is Valid Port.
 * Verifies that the input port is valid.
 *
 * \param nPort Port number to validate.
 */
bool MainWindow::isValidPort(int nPort)
{
	if ( nPort < 0 || nPort >= 65536 )
	{
		os::Alert* pcMyAlert = new os::Alert("Invalid Port", "Please verify that the port number is between 0 and 65,535.", Alert::ALERT_WARNING, "OK", NULL);
		pcMyAlert->Go( new os::Invoker( 0 ) ); 
		return false;
	}
		
	return true;
}

/** \brief Construct URL from widgit data.
 * This will generate the ftp string for 
 *
 * \param zHost Host to connect to.
 * \param zPort The port number of the connection.
 * \param zUser Username.
 * \param zPassword Password for the username.
 *
 * \todo This should be changed so that it can decide whether to create a
 * a URL string for ftp or whatever other protocol that is desired.
 */
String MainWindow::ConstructURL( const String& zHost, const String& zPort )
{
	String zURL = m_pcConnectBar->GetServerType() + "://";
	
	int nPort = atoi( zPort.c_str() );
	
	/* Validate the input and construct the URL string. */
	if( isValidHost(zHost) && isValidPort(nPort) )
	{
		/* Append the host name. */
		zURL += zHost;

		if( nPort > 0 ) {
			/* Append the port. If nPort == 0 then libcurl will use the default port. */
			String zTmp;
			zTmp.Format( ":%i", nPort );
			zURL += zTmp;
		}

	}
	else
	{
		/* Return a blank string. */
		zURL = "";
	}
	
	return( zURL );
}

/** \brief Default Message Handler.
 * This currently handles the creation of any message boxes and also the
 * creation of the connection when the "CONNECT" button is pressed in the
 * ConnectBar. It also does settings changes.
 *
 * \param pcMessage Pointer to the message that needs to be handled.
 *
 * \todo Need code to determine the type of server connection to create
 * depending on the type of connection (This will become more important
 * when there are more server types).
 */
void MainWindow::HandleMessage( os::Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_APP_ABOUT:
		{
			break;
		}
		
		case M_APP_QUIT:
		{
			PostMessage( os::M_QUIT );
			break;
		}
		
		case M_CONNECT:
		{
			String zHost = m_pcConnectBar->GetHost();
			String zPort = m_pcConnectBar->GetPort();
			String zUser = m_pcConnectBar->GetUser();
			String zPassword = m_pcConnectBar->GetPassword();
			String zURL = ConstructURL( zHost, zPort );

			// Don't parse it if the url is bad.
			if ( zURL == "" )
			{
				break;
			}
				
			DEBUG( "Creating connection to %s\n", zURL.c_str() );
			Server* pcServer = new Server( zURL, zUser, zPassword, this, m_pcSettings );
			if( !pcServer ) {
				fprintf( stderr, "Transferrer: ERROR: Could not create Server( %s )!\n", zURL.c_str() );
				break;
			}
			/** \todo Check that the new url is actually different to the old one, and only proceed if so */
			m_pcView->SetServer( pcServer );
			
			if( m_pcServer ) {
				m_pcServer->Close();
				/* We can forget about the old Server now - its transfer thread will clean up and delete itself. */
			}
			m_pcServer = pcServer;
			
			/* Add this server to the history */
			if( m_pcSettings->GetSaveHistory() ) {
				m_pcSettings->AddHistory( zHost, zPort, zUser, (m_pcSettings->GetSavePasswords() ? zPassword : "") );
			}
			break;
		}
		
		/* Messages from the backend */
		case M_JOB_UPDATED:
		{
			if( m_pcProgressWindow ) m_pcProgressWindow->PostMessage( pcMessage, m_pcProgressWindow );
			break;
		}
		case M_JOB_ERROR:
		{
			String zMsg;
			int nID;
			pcMessage->FindInt32( "jobID", &nID );
			pcMessage->FindString( "message", &zMsg );
			DEBUG( "Job %i reports error: %s\n", nID, zMsg.c_str() );
			
			Alert* pcAlert = new Alert( "Transfer error", zMsg, Alert::ALERT_WARNING, "OK", NULL );
			pcAlert->CenterInWindow( this );
			pcAlert->Show();
			pcAlert->MakeFocus();
			break;
		}
		case M_JOB_OVERWRITE_QUERY:
		{
			String zPath;
			int nID;
			bool bMultiJob;
			int nType;
			if( pcMessage->FindInt32( "jobID", &nID ) != 0 || pcMessage->FindInt32( "type", &nType ) != 0 || pcMessage->FindBool( "multi", &bMultiJob ) != 0 ) {
				DEBUG( "Warning: MainWindow: got M_JOB_OVERWRITE_QUERY without expected parameters!\n" );
				break;
			}
			if( nType == JOB_DOWNLOAD ) {
				if( pcMessage->FindString( "localPath", &zPath ) != 0 ) {
					DEBUG( "Warning: MainWindow: got M_JOB_OVERWRITE_QUERY without expected localPath parameter!\n" );
					break;
				}
			} else if( nType == JOB_UPLOAD ) {
				if( pcMessage->FindString( "remotePath", &zPath ) != 0 ) {
					DEBUG( "Warning: MainWindow: got M_JOB_OVERWRITE_QUERY without expected remotePath parameter!\n" );
					break;
				}
			} else {
				DEBUG( "Warning: MainWindow: got M_JOB_OVERWRITE_QUERY with unexpected type %i!\n", nType );
				break;
			}

			OverwriteRequester* pcRequester = new OverwriteRequester( zPath, nID, bMultiJob, this );
			pcRequester->CenterInWindow( this );
			pcRequester->Show();
			pcRequester->MakeFocus( true );
			break;
		}
		
		/* Messages from requesters */
		case M_GUI_OVERWRITE_REPLY:
		case M_GUI_FAILURE_REPLY:
		{
			if( m_pcServer ) m_pcServer->SendGUIReply( pcMessage );
			break;
		}
		
		/* Messages from the progress window (pause, resume, cancel). */
		case M_GUI_PAUSE:
		{
			int jobID = 0;
			int nIndex = 0;
			if( m_pcServer )
			{
				while( pcMessage->FindInt32( "jobID", &jobID, nIndex ) == 0 )
				{
					m_pcServer->PauseTransfer(jobID);
					nIndex++;
				}
			}
			break;
		}
		case M_GUI_RESUME:
		{
			int jobID = 0;
			int nIndex = 0;
			if ( m_pcServer )
			{
				while( pcMessage->FindInt32( "jobID", &jobID, nIndex ) == 0 )
				{
					m_pcServer->ResumeTransfer(jobID);
					nIndex++;
				}
			}
			break;
		}
		case M_GUI_CANCEL:
		{
			int jobID = 0;
			int nIndex = 0;
			if ( m_pcServer )
			{
				while( pcMessage->FindInt32( "jobID", &jobID, nIndex ) == 0 )
				{
					m_pcServer->CancelTransfer(jobID);
					nIndex++;
				}
			}
			break;
		}

		/* Settings Callbacks. */			
		case M_SETTINGS_PASSIVE:
		{
			bool bPassive;
			if( pcMessage->FindBool( "status", &bPassive ) == 0 ) {
				m_pcSettings->SetPassive( bPassive );
			}
			break;
		}
		case M_SETTINGS_SAVE_HISTORY:
		{
			bool bSave;
			if( pcMessage->FindBool( "status", &bSave ) == 0 ) {
				m_pcSettings->SetSaveHistory( bSave );
			}
			/* TODO: disable the 'Remember passwords' menu option when this is disabled */
			break;
		}
		case M_SETTINGS_SAVE_PASSWORDS:
		{
			bool bSave;
			static bool bWarned = false;
			if( pcMessage->FindBool( "status", &bSave ) == 0 ) {
				m_pcSettings->SetSavePasswords( bSave );
				if( bSave && !bWarned )
				{
					Alert* pcAlert = new Alert( "Save passwords",
						"Note: passwords will be saved in clear text in the settings file.\n" \
						"If you do not want your passwords stored in this way, disable this setting.", Alert::ALERT_INFO, "OK", NULL );
					pcAlert->CenterInWindow( this );
					pcAlert->Show();
					pcAlert->MakeFocus( true );
					bWarned = true;
				}
			}
			break;
		}
		case M_SETTINGS_MAX_CONNECTIONS_MENU:
		{
			LayoutView* pcView = new LayoutView( Rect( 0,0,100,50 ), "max_connections_layout" );
			HLayoutNode* pcNode = new HLayoutNode( "max_connections_node" );
			pcNode->AddChild( new StringView( Rect( 0,0,80,50 ), "max_connections_label", "Maximum simultaneous connections:" ) );
			Spinner* pcSpinner = new Spinner( Rect( 81,0,100,50 ), "max_connections_spinner", m_pcSettings->GetMaxConnections(), new Message( M_SETTINGS_MAX_CONNECTIONS ) );
			pcSpinner->SetMinMax( 1, 100 );
			pcSpinner->SetStep( 1 );
			pcSpinner->SetFormat( "%.0f" );
			pcSpinner->EnableStatusChanged( true );
			pcSpinner->SetTarget( this );
			pcNode->AddChild( pcSpinner );
			pcView->SetRoot( pcNode );
			
			Alert* pcAlert = new Alert( "Maximum simultaneous connections", pcView );
//			pcAlert->ResizeTo( pcView->GetPreferredSize( false ) );		/* Spinner doesn't give an appropriate size? */
			pcAlert->ResizeTo( pcView->GetPreferredSize( false ) + Point( 30, 12) );
			pcAlert->CenterInWindow( this );
			pcAlert->Show();
			pcAlert->MakeFocus( true );
			break;
		}
		case M_SETTINGS_MAX_CONNECTIONS:
		{
			Variant cValue;
			if( pcMessage->FindVariant( "value", &cValue ) == 0 ) {
				int nMax = cValue.AsInt32();
				if( nMax > 0 ) m_pcSettings->SetMaxConnections( nMax );
				m_pcSettingsMenu->SetSettings( m_pcSettings );
			} else {
				/* Libsyllable 0.6.5 Spinner doesn't include its current value in the status changed message :( */
				DEBUG( "User changed the max connections setting, but because of a libsyllable bug we don't know what the new value is, sorry!\n" );
			}
			break;
		}
		case M_SETTINGS_DEBUG_MODE:
		{
			bool bDebug;
			if( pcMessage->FindBool( "status", &bDebug ) == 0 ){
				g_bDebug = bDebug;
				m_pcSettings->SetDebugMode( bDebug );
				DEBUG( "Debug mode %s\n", bDebug?"true":"false" );
			}
			break;
			/* TODO: Notify the Server object of the settings change so it can adopt the new settings */
		}
	}
}


/** \brief Update the status bar for the main window.
 */
void MainWindow::UpdateStatusBar(Message* update)
{
	String result;
	int totalDownloads = 0;
	int totalUploads = 0;
	int totalQueued = 0;
	
	if ( update )
	{
		update->FindInt32( "total_downloads", &totalDownloads );
		update->FindInt32( "total_uploads", &totalUploads );
		update->FindInt32( "total_queued", &totalQueued );
	}
	
	// Update the status bar.
	m_pcStatusBar->SetText("status", result.Format("%d Downloads / %d Uploads / %d in Queue", totalDownloads,
    	totalUploads, totalQueued));

}


/** \brief Quit the app when the user closes the main window.
 * Tells the app to quit, when the user closes the window.
 */
bool MainWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return( true );
}


