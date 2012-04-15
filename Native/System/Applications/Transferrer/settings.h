#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <util/settings.h>

using namespace os;

/** \brief Stores the user's settings for the app.
 * This is used to create a settings file for the app. See os::Settings.
 *
 */
class AppSettings : public Settings
{
public:
	
	/** \brief Return if the connection is passive.
	 * Returns the value for the setting 'passive' or true if it isn't found.
	 */
	bool GetPassive()
	{
		return( GetBool( "passive", true ) );
	}
	
	/** \brief Set the value for passive.
	 * \param bPassive Whether or not this is a passive connection.
	 */
	status_t SetPassive( bool bPassive )
	{
		return( SetBool( "passive", bPassive ) );
	}
	
	/** \brief Return the maximum number of connections.
	 * This returns the maximum number of allowable connections or 2 by default.
	 */
	int GetMaxConnections()
	{
		return( GetInt32( "max_connections", 2 ) );
	}
	
	/** \brief Set the maximum number of connections.
	 *
	 * \param nMax The maximum number of connections to allow at one time.
	 */
	status_t SetMaxConnections( int nMax )
	{
		return( SetInt32( "max_connections", nMax ) );
	}
	
	/** \brief Return the value of the 'Remember server addresses' option.
	 * Returns the value of the save_history option or true if it doesn't exist.
	 */
	bool GetSaveHistory()
	{
		return( GetBool( "save_history", true ) );
	}
	
	/** \brief Set the value for the 'Remember server addresses' option.
	 * \param bSave Value to set the save_history option to.
	 */
	status_t SetSaveHistory( bool bSave )
	{
		return( SetBool( "save_history", bSave ) ); 
	}
	
	/** \brief Return the value of the 'Remember passwords' option.
	 */
	bool GetSavePasswords()
	{
		return( GetBool( "save_passwords", true ) );
	}
	
	/** \brief Set the value for the 'Remember passwords' option.
	 *
	 * \param bSave The new value for the 'Remember passwords' option.
	 */
	status_t SetSavePasswords( bool bSave )
	{
		/* TODO: erase any previously saved passwords when bSave == false. */
		return( SetBool( "save_passwords", bSave ) );
	}
	
	/** \brief Save the server address & login details to the history.
	 *
	 * \param zHost, zPort, zUser, zPassword The host, etc of the server to save.
	 */
	status_t AddHistory( const String& zHost, const String& zPort, const String& zUser, const String& zPassword )
	{
		Message cMsg;
		cMsg.AddString( "host", zHost );
		cMsg.AddString( "port", zPort );
		cMsg.AddString( "user", zUser );
		cMsg.AddString( "password", zPassword );
		return( SetMessage( "history", cMsg ) );
	}
	
	/** \brief Load the most-recently used server address & login details from the history.
	 *
	 * \param pzHost, pzPort, pzUser, pzPassword - pointers to locations in which the corresponding data should be stored.
	 */
	status_t LoadHistory( String* pzHost, String* pzPort, String* pzUser, String* pzPassword )
	{
		Message cMsg;
		status_t nResult = FindMessage( "history", &cMsg );
		cMsg.FindString( "host", pzHost );
		cMsg.FindString( "port", pzPort );
		cMsg.FindString( "user", pzUser );
		cMsg.FindString( "password", pzPassword );
		return( nResult );
	}
	
	
	/** \brief Get the saved main window position.
	 * Returns the saved size & position of the main window, if any; returns Rect(0,0,0,0) if no position is saved.
	 */
	Rect GetMainWindowFrame()
	{
		return( GetRect( "main_frame", Rect(0,0,0,0) ) );
	}
	
	/** \brief Save the main window position.
	 * Saves the size & position of the main window.
	 */
	status_t SetMainWindowFrame( const Rect& cFrame )
	{
		return( SetRect( "main_frame", cFrame ) );
	}

	/** \brief Get the saved progress window position.
	 * Returns the saved size & position of the progress window, if any; returns Rect(0,0,0,0) if no position is saved.
	 */
	Rect GetProgressWindowFrame()
	{
		return( GetRect( "progress_frame", Rect(0,0,0,0) ) );
	}
	
	/** \brief Save the progress window position.
	 * Saves the size & position of the progress window.
	 */
	status_t SetProgressWindowFrame( const Rect& cFrame )
	{
		return( SetRect( "progress_frame", cFrame ) );
	}
	
	/** \brief Get debug mode setting.
	 * If true, we should print extra information for debugging, and set libcurl to use verbose mode.
	 */
	bool GetDebugMode()
	{
		return( GetBool( "debug", false ) );
	}
	
	/** \brief Set debug mode setting.
	 * See GetDebugMode().
	 */
	status_t SetDebugMode( bool bDebug )
	{
		return( SetBool( "debug", bDebug ) );
	}
};


#endif	/* __SETTINGS_H__ */

