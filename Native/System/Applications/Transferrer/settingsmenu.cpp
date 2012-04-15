#include "settingsmenu.h"
#include "messages.h"

/** \brief SettingsMenu Constructor.
 * Initialize the settings menu object. This creates the actual menu items and
 * keeps track of the AppSettings for it.
 *
 */
SettingsMenu::SettingsMenu( const String& zName, AppSettings* pcSettings )
	: Menu( Rect( 0,0,1,1 ), zName, ITEMS_IN_COLUMN  )
{
	m_pcPassive = new CheckMenu( "Use passive mode", new Message( M_SETTINGS_PASSIVE ), true );
	m_pcSaveHistory = new CheckMenu( "Remember server addresses", new Message( M_SETTINGS_SAVE_HISTORY ), true );
	m_pcSavePasswords = new CheckMenu( "Remember passwords", new Message( M_SETTINGS_SAVE_PASSWORDS ), true );
	m_pcMaxConnections = new MenuItem( "Maximum connections: 2", new Message( M_SETTINGS_MAX_CONNECTIONS_MENU ) );
	m_pcDebugMode = new CheckMenu( "Enable debug mode", new Message( M_SETTINGS_DEBUG_MODE ), false );
	
	AddItem( m_pcSaveHistory );
	AddItem( m_pcSavePasswords );
	AddItem( m_pcPassive );
	AddItem( m_pcMaxConnections );
	AddItem( new MenuSeparator() );
	AddItem( m_pcDebugMode );
	
	if( pcSettings ) SetSettings( pcSettings );
}

/** \brief SettingsMenu Deconstructor.
 * Menu items are deleted by ~Menu.
 */
SettingsMenu::~SettingsMenu()
{
	
}

/** \brief Assign the settings for the menu.
 * This checks the AppSettings so that the menu corresponds to the settings within
 * the AppSettings object.
 *
 */
void SettingsMenu::SetSettings( AppSettings* pcSettings )
{
	m_pcPassive->SetChecked( pcSettings->GetPassive() );
	m_pcSaveHistory->SetChecked( pcSettings->GetSaveHistory() );
	m_pcSavePasswords->SetChecked( pcSettings->GetSavePasswords() );
	m_pcDebugMode->SetChecked( pcSettings->GetDebugMode() );
	String zTmp;
	zTmp.Format( "Maximum simultaneous connections: %i", pcSettings->GetMaxConnections() );
	m_pcMaxConnections->SetLabel( zTmp );
}

