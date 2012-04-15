#ifndef CONTROL_BAR_H
#define CONTROL_BAR_H

#include <gui/layoutview.h>
#include <gui/textview.h>
#include <gui/stringview.h>
#include <gui/dropdownmenu.h>
#include <gui/button.h>
#include <util/message.h>
#include <util/string.h>


/** \brief Connection Bar.
 * This is the connection bar that is displayed in the window.
 * It is used to get the host, user, password and port information
 * and to pass this information to the various Server objects.
 *
 * \todo Need to add a function for retrieving the value of the 
 * DropdownMenu that contains the desired connection type.
 */
class ConnectBar : public os::LayoutView
{
public:
	ConnectBar();
	
public:

	/** \brief Get Server Type.
	 * This retreives the type of server selected by the user.
	 */
	os::String GetServerType()
	{
		os::String result;
		int selected = -1;
		
		if ( (selected = m_pcConnectionTypeDropdown->GetSelection()) >= 0 )
		{
			result = m_pcConnectionTypeDropdown->GetItem(selected);
		}

		return result;
	}
	
		
	/** \brief Get Host Address.
	 * This retrieves the host address from the host address text box.
	 */
	os::String GetHost()
	{
		return m_pcHostText->GetBuffer()[0];
	}
	
	/** \brief Set Host Address.
	 * Sets the host address for the connection bar.
	 */
	void SetHost(os::String zHost)
	{
		m_pcHostText->SetValue(zHost);
	}
	
	/** \brief Get Username.
	 * This retrieves the user name from the user name text box.
	 */
	os::String GetUser()
	{
		return m_pcUserText->GetBuffer()[0];
	}
	
	/** \brief Set Username.
	 * Sets the username for the connection bar.
	 */
	void SetUser(os::String zUser)
	{
		m_pcUserText->SetValue(zUser);
	}

	/** \brief Get Password.
	 * This retrieves the password from the password text box.
	 */
	os::String GetPassword()
	{
		return m_pcPassText->GetBuffer()[0];
	}

	/** \brief Set Password.
	 * Sets the password for the connection bar.
	 */
	void SetPass(os::String zPass)
	{
		m_pcPassText->SetValue(zPass);
	}

	/** \brief Get Port.
	 * This retrieves the port string from the port text box.
	 */	
	os::String GetPort()
	{
		return m_pcPortText->GetBuffer()[0];
	}

	/** \brief Set Port.
	 * Sets the port for the connection bar.
	 */
	void SetPort(os::String zPort)
	{
		m_pcPortText->SetValue(zPort);
	}
	
private:

	void _Layout();
	
	
	//libsyllable functions
	os::Point GetPreferredSize(bool) const;
	void AllAttached();

private:
	
	/** Root layout node. */
	os::HLayoutNode* m_pcRoot;

	/** The drop down containing the connection type (eg. ftp) */
	os::DropdownMenu* m_pcConnectionTypeDropdown;
	
	/** Host text label. */	
	os::StringView* m_pcHostString;
	/** Host string entry text box. */
	os::TextView* m_pcHostText;
	
	/** Username text label. */
	os::StringView* m_pcUserString;
	/** Username string entry text box. */
	os::TextView* m_pcUserText;
	
	/** Password text label. */
	os::StringView* m_pcPassString;
	/** Password string entry text box. */
	os::TextView* m_pcPassText;
	
	/** Port text label. */
	os::StringView* m_pcPortString;
	/** Port string entry text box. */
	os::TextView* m_pcPortText;
	
	/** Connect button. */
	os::Button* m_pcButton;
};

#endif










