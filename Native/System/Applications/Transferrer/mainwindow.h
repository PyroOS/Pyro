#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gui/window.h>
#include <gui/image.h>
#include <util/message.h>
#include <util/resources.h>
#include <util/string.h>
#include <gui/toolbar.h>
#include <gui/menu.h>
#include <gui/statusbar.h>

class ContainerView;
class ConnectBar;
class SettingsMenu;
class Server;
class AppSettings;

using namespace os;

#ifndef DEBUG
extern bool g_bDebug;
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif


/** \brief Main Window class for Transferrer.
 * Populates the main window and handles its events.
 */
class MainWindow : public os::Window
{
public:
	MainWindow();
	~MainWindow();
	void HandleMessage( os::Message* );
	void OpenConnection(const String& zScheme, const String& zHost, const String& nPort, const String& zUser, const String& zPassword );

private:
	bool OkToQuit();
	void Layout();
	void SetupMenus();
	void SetupToolBar();
	void SetupStatusBar();
	void UpdateStatusBar(Message* update);
	void PositionWindows();
	
	String ConstructURL( const String& zHost, const String& zPort );
	bool isValidHost(const String& zHost);
	bool isValidPort(int nPort);
			
private:

	/** \brief The progress window for Transferrer. */
	os::Window* m_pcProgressWindow;
	
	/** \brief Main menu bar. */
	os::Menu* m_pcMainMenu;
	
	/** \brief Toolbar containing the ConnectBar. */
	os::ToolBar* m_pcToolBar;
	
	/** \brief Status bar for the window to show status information. */
	os::StatusBar* m_pcStatusBar;
	
	/** \brief Container class for the local directory and remote directory views. */
	ContainerView* m_pcView;
	
	/** \brief ConnectBar containing buttons that allow for connecting to a server. */
	ConnectBar* m_pcConnectBar;
	
	/** \brief The current server being used by the Transferrer. */
	Server* m_pcServer;
	
	/** \brief Settings object which stores the app's global settings. */
	AppSettings* m_pcSettings;
	
	/** \brief Container for the Settings menu item (for callback purposes to update the actual menu). */
	SettingsMenu* m_pcSettingsMenu;
	
	// These were created for ease of syntax checking.
	// They correspond to the grammar for URI's expressed
	// in RFC 3986.
	static const os::String sUnreserved; /**< Unreserved characters. See RFC 3986. */
	static const os::String sPctEncoded; /**< Percent Encoded numbers. See RFC 3986. */
	static const os::String sSubDelims; /**< Subexpression Delimiters. See RFC 3986. */
	static const os::String sUserinfo_nc; /**< Userinfo without colon. See RFC 3986. */
	static const os::String sDecOctet; /**< Decimal Octet [0, 255]. See RFC 3986. */
	static const os::String sIPv4address; /**< IPv4 Addresses (123.123.123.123). Not as specific as RFC 3986, so need to check that values are between 0 and 255. */
	static const os::String sH16; /**< 16 bit hex value. See RFC 3986. */
	static const os::String sLS32; /**< (h16:h16) | IPv4 Address. See RFC 3986. */
	static const os::String sIPvFuture; /**< Future IP versions. See RFC 3986. */
	static const os::String sIPv6address; /**< IPv6 Address. See RFC 3986. */
	static const os::String sIP_literal; /**< IP-literal ::= LBRACKET (IPv6address | IPvFuture) RBRACKET. See RFC 3986. */
	
};

#endif	/* MAINWINDOW_H */

