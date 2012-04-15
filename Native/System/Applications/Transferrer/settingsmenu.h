#ifndef __SETTINGSMENU_H__
#define __SETTINGSMENU_H__

#include <gui/menu.h>
#include <gui/checkmenu.h>
#include "settings.h"

using namespace os;

/** \brief Class for storing the current menu options.
 * This class is used to keep track of menu items that can change when settings
 * are changed.
 *
 * \todo Currently some of the information in the menu is specific
 * to FTPServer objects.
 *
 * \todo Should have a way to show and hide the progress window.
 */
class SettingsMenu : public Menu
{
public:
	SettingsMenu( const String& zName, AppSettings* pcSettings );
	~SettingsMenu();
	
	void SetSettings( AppSettings* pcSettings );

private:
	CheckMenu* m_pcPassive; /**< Pointer to menu item for whether the connection is passive or not. */
	CheckMenu* m_pcSaveHistory; /**< Pointer to 'Remember server addresses' menu item . */
	CheckMenu* m_pcSavePasswords;	/** < Pointer to the 'Remember passwords' menu item. */
	MenuItem* m_pcMaxConnections; /**< Pointer to menu item listing the number of allowable simultaneous connections. */
	CheckMenu* m_pcDebugMode;	/**< Pointer to 'Enable debug mode' menu item. */
};


#endif	/* __SETTINGSMENU_H__ */


