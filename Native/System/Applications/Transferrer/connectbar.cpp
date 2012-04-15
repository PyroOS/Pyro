#include "connectbar.h"
#include "messages.h"
#include <util/looper.h>

/** \brief ConnectBar Constructor.
 * This generates the initial layout of the window by calling _Layout().
 */
ConnectBar::ConnectBar() : os::LayoutView(os::Rect(),"")
{
	_Layout();
}

/** \brief Overloads the virtual GetPreferredSize(bool) method.
 * Simply returns the prefered size of the root layout bar.
 */
os::Point ConnectBar::GetPreferredSize(bool) const 
{
	return m_pcRoot->GetPreferredSize(false);
}


/** \brief Layout Generator.
 * This generates the layout of the connectbar. Elements include:
 * Host Name, User Name, Password, and Port
 *
 */
void ConnectBar::_Layout()
{
	m_pcRoot = new os::HLayoutNode("connect_bar_layout");
	
	os::HLayoutNode* host = new os::HLayoutNode("host_node");
	host->AddChild(m_pcHostString = new os::StringView(os::Rect(),"host_string","Host:"));
	host->AddChild(new os::HLayoutSpacer("",2,2));
	host->AddChild(m_pcHostText = new os::TextView(os::Rect(),"host_text",""),100);
	m_pcHostText->SetMinPreferredSize(10,1);
	m_pcHostText->SetMaxPreferredSize(10,1);
	m_pcHostText->SetTabOrder(1);	/* Best to set the values explicitly since it looks like there are bugs with using NEXT_TAB_ORDER */

	os::HLayoutNode* user = new os::HLayoutNode("user_node");
	user->AddChild(m_pcUserString = new os::StringView(os::Rect(),"user_string","User:"));
	user->AddChild(new os::HLayoutSpacer("",2,2));
	user->AddChild(m_pcUserText = new os::TextView(os::Rect(),"user_text",""),100);
	m_pcUserText->SetMinPreferredSize(10,1);
	m_pcUserText->SetMaxPreferredSize(10,1);
	m_pcUserText->SetTabOrder(2);
	
	os::HLayoutNode* pass = new os::HLayoutNode("pass_node");
	pass->AddChild(m_pcPassString = new os::StringView(os::Rect(),"pass_string","Password:"));
	pass->AddChild(new os::HLayoutSpacer("",2,2));
	pass->AddChild(m_pcPassText = new os::TextView(os::Rect(),"pass_text",""),100);
	m_pcPassText->SetMinPreferredSize(10,1);
	m_pcPassText->SetMaxPreferredSize(10,1);
	m_pcPassText->SetPasswordMode(true);
	m_pcPassText->SetTabOrder(3);

	os::HLayoutNode* port = new os::HLayoutNode("port_node");
	port->AddChild(m_pcPortString = new os::StringView(os::Rect(),"port_string","Port:"));
	port->AddChild(new os::HLayoutSpacer("",2,2));
	port->AddChild(m_pcPortText = new os::TextView(os::Rect(),"port_text",""),100);
	m_pcPortText->SetMinPreferredSize(4,1);
	m_pcPortText->SetMaxPreferredSize(4,1);
	m_pcPortText->SetNumeric(true);
	m_pcPortText->SetTabOrder(4);

	// Add the connection type node
	os::HLayoutNode* connectionType = new os::HLayoutNode("connection_type_node");
	connectionType->AddChild(new os::HLayoutSpacer("",2,2));
	connectionType->AddChild(m_pcConnectionTypeDropdown = new os::DropdownMenu(os::Rect(), "Drop"));
	m_pcConnectionTypeDropdown->AppendItem("ftp");
	m_pcConnectionTypeDropdown->SetSelection(0);
	m_pcConnectionTypeDropdown->SetReadOnly(true);
	m_pcConnectionTypeDropdown->SetTabOrder(10);
	m_pcConnectionTypeDropdown->SetEnable( false );	/* Currently only one choice, so best not to confuse users */
	
	m_pcRoot->AddChild(host);
	m_pcRoot->AddChild(new os::HLayoutSpacer("",5,5));
	m_pcRoot->AddChild(user);
	m_pcRoot->AddChild(new os::HLayoutSpacer("",5,5));
	m_pcRoot->AddChild(pass);
	m_pcRoot->AddChild(new os::HLayoutSpacer("",5,5));
	m_pcRoot->AddChild(port);
	m_pcRoot->AddChild(new os::HLayoutSpacer("",5,5));
	m_pcRoot->AddChild(connectionType);
	m_pcRoot->AddChild(new os::HLayoutSpacer("",5,5));
	m_pcRoot->AddChild(m_pcButton = new  os::Button(os::Rect(),"connect","Connect",new os::Message(M_CONNECT)));
	m_pcButton->SetTabOrder(6);
	SetRoot(m_pcRoot);
}

/** \brief AllAttached Overloaded Function.
 * This attaches the looper that the button should send messages
 * to.
 */
void ConnectBar::AllAttached()
{
	m_pcButton->SetTarget(GetLooper());
}


