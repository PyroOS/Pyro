// EFileBrowser (C)opyright 2007 Jonas Jarvoll
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

#include <gui/imagebutton.h>
#include <gui/stringview.h>
#include <gui/layoutview.h>
#include "address_field.h"
#include "address_field_button.h"

using namespace os;

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

#if 0	/* Not using these images currently */
static uint8 g_aCDImage[] = {
#include "images/cd.h"
};

static uint8 g_aDiskImage[] = {
#include "images/disk.h"
};

static uint8 g_aFloppyImage[] = {
#include "images/floppy.h"
};

uint8 g_aFileImage[] = {
#include "images/file.h"
};
#endif

/** \todo We should add them as resources, rather than as raw data */
uint8 g_aFolderImage[] = {
#include "images/folder.h"
};

/** \brief Private class used by AddressField.
 * This class is used to generate the different buttons for each level
 * of the path. It also stores all of the handler and looper information
 * for the buttons.
 */
class AddressField :: _Private
{
public:
	
	/** \brief Constructor for the class.
	 * Attempts to load the Image* object for folders.
	 */
	_Private()
	{
		StreamableIO* pcSource;
		try {
			pcSource = new File( "/system/icons/folder.png" );
		} catch( ... ) {
			pcSource = new MemFile( g_aFolderImage, sizeof( g_aFolderImage ) );
		}

		m_pcDirIcon = new BitmapImage( pcSource );
		m_pcDirIcon->SetSize( Point( 24,24 ) );
		delete( pcSource );
	};
	
	/** \brief Deconstructor for the class.
	 * Delete the generated icon.
	 */
	~_Private()
	{
		delete( m_pcDirIcon );
	};

	/** \brief Add a button with the specific label and path.
	 *
	 * \param label Label for the button to display.
	 * \param path Path that the label corresponds to.
	 *
	 * \todo Use proper icons.
	 */
	void _AddButton( String label, String path )
	{
		// Make message for button
		Message* msg = NULL;

		if( m_theMessage != NULL )
		{
			msg = new Message( *m_theMessage );
			msg->AddString( "file/path", path );
		}

		/* Make a copy of the icon since the AddressFieldButton deletes it when done */
		BitmapImage* pcCopy = new BitmapImage( *m_pcDirIcon );

		// Create new button		
		AddressFieldButton* button = new AddressFieldButton( Rect(), label, pcCopy, msg );
		m_ListOfButtons.push_back( button );
		m_RootNode->AddChild( button );
	}

	/** \brief Add Distance
	 * Put a space between the buttons.
	 */
	void _AddDistance()
	{
		HLayoutSpacer* space = new HLayoutSpacer( "space" );
		space->LimitMaxSize( Point( 10, 100 ) );
		m_RootNode->AddChild( space );
	}

	/** \brief Clean Up Address Field.
	 * Remove the buttons and replace it with the root node.
	 */
	void _Clean()
	{
		// Clear old buttons
		m_ListOfButtons.clear();
		//delete m_RootView->GetRoot();
		m_RootNode = new HLayoutNode( "pcRootNode" );
		m_RootNode->SetBorders( Rect( 4, 4, 4, 4 ) );		
		m_RootView->SetRoot( m_RootNode );
	}

	/** \brief Update the Path.
	 * Rebuild the buttons for the current path.
	 */
	void _UpdatePath()
	{
		_Clean();
		
		if( m_Path == "" ) return;
	
		String path = m_Path;
		String button_path;

#if 0		/* Not relevant to us since we might use ftp paths relative to user's home dir */
		// Sanity check
		if( path[0] != '/' )
			throw String("Not a valid path");
#endif

		// Remove the very first separator
		path = String( path, 1, path.size() );

		// Create root button
		_AddButton( String( "/" ), String( "/" ) );
		
		while( path != "" )
		{
			_AddDistance();

			// Find next directory splitter "/"
			size_t s = path.find( "/" );

			// Extract folder
			String label = String(path, 0, s);

			// Remove it from the path
			path = String( path, s+1, path.size() );

			// Create nice path for the buttons
			button_path +=  String("/") + label;

			_AddButton( label, button_path );
	
			// No more path separator found
			if( s == std::string::npos)
				break;
		}

		m_RootView->InvalidateLayout();
	}


	std::vector< AddressFieldButton* > m_ListOfButtons; /**< Buttons for the address field. */

	LayoutView*	m_RootView; /**< Root view of the window. */
	LayoutNode*	m_RootNode; /**< Root layout node. */
	String m_Path; /**< The bath that this address field represents. */
	Message* m_theMessage; /**< Message for the button. */
	Handler* m_theHandler; /**< Message handler for the buttons. */
	Looper* m_theLooper; /**< Looper for the buttons. */
	
	BitmapImage* m_pcDirIcon; /**< BitmapImage containing the directory icon. */
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////

/** \brief AddressField Constructor.
 * Initializes the private class for drawing the buttons and creates the other widgets.
 * It also initializes the handler and looper to NULL.
 *
 * \param path The path of the address field.
 * \param msg Message to send to the AddressField objects.
 */
AddressField :: AddressField( const os::String path, os::Message* msg ) : View( Rect(), "" )
{
	// Create the private class
	m = new _Private();

	// Create widgets
	m->m_RootNode = new HLayoutNode( "pcRootNode" );
	m->m_RootNode->SetBorders( Rect( 4, 4, 4, 4 ) );

	m->m_RootView = new LayoutView( GetBounds(), "pcRootView" );
	m->m_RootView->SetRoot( m->m_RootNode );
	AddChild( m->m_RootView );

	m->m_Path = path;
	m->m_theHandler=NULL;
	m->m_theLooper=NULL;
	m->m_theMessage = msg;
	m->_UpdatePath();	

	SetTarget( m->m_theHandler, m->m_theLooper );
}

/** \brief Deconstructor.
 * Deletes the message that was sent to the AddressField object and
 * deletes the _Private object being used for drawing.
 */
AddressField :: ~AddressField()
{
	delete m->m_theMessage;
	delete m;
}

/** \brief Set Path of AddressField.
 * Sets the file path that the address path corresponds to.
 *
 * \param path Path using the unix-style directory path format.
 */
void AddressField :: SetPath( os::String path )
{
	m->m_Path = path;
	m->_UpdatePath();
	SetTarget( m->m_theHandler, m->m_theLooper );
}

/** \brief Return the path of the AddressField.
 */
String AddressField :: GetPath()
{
	return m->m_Path;
}

/** \brief Return the preferred size of the object.
 * \param bLargest If bLargest is set it gives the largest possible size, otherwise it sets it to max x and 35 for y.
 */
Point AddressField :: GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return Point( COORD_MAX, COORD_MAX );

	return Point( COORD_MAX, 35 );
}

/** \brief Set the Target for the button.
 * This sets the Handler and Looper objects for the buttons and then sets
 * the target for all of the buttons.
 *
 * \param pcHandler Handler object for the address buttons.
 * \param pcLooper The looper for the address buttons.
 */
status_t AddressField :: SetTarget( const os::Handler* pcHandler, const os::Looper* pcLooper )
{
	m->m_theHandler = (os::Handler*) pcHandler;
	m->m_theLooper = (os::Looper*) pcLooper;

	// Set target for the all button
	for( uint i = 0 ; i < m->m_ListOfButtons.size() ; i++ )
		m->m_ListOfButtons[ i ]->SetTarget( m->m_theHandler, m->m_theLooper );

	return Invoker::SetTarget( m->m_theHandler, m->m_theLooper );
}


