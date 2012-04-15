// NView (C)opyright 2008 Jonas Jarvoll
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
#include <unistd.h>  // For getting the current directory name
#include <storage/path.h>
#include <storage/directory.h>
#include <storage/registrar.h>
#include "imagekeeper.h"
#include "resources/ImageViewer.h"

using namespace os;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
// Bse class
// I m a g e K e e p e r
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

class ImageKeeper :: _Private
{
public:
	_Private()
	{		
		ImageStartIndex = 0;
		ImageIndex = -1;
	}

	RegistrarManager* Registrar;

	vector< String > ImagesList;
	int ImageIndex;
	int ImageStartIndex;
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S 
//
///////////////////////////////////////////////////////////////////////////////

ImageKeeper :: ImageKeeper()
{
	// Create the private class
	_m = new _Private();

	//  Get the registrar so that we can find out the mime type of the file
	_m->Registrar = RegistrarManager::Get();
}

ImageKeeper :: ~ImageKeeper()
{
	if( _m->Registrar )
		_m->Registrar->Put();

	delete _m;
}

String ImageKeeper :: _GetMime( String file )
{
	// Get mime type of the file
	String mime;
	String temp;
	Image* icon;
	Message msg;

	if( _m->Registrar && _m->Registrar->GetTypeAndIcon( file, Point( 24, 24 ), &temp, &icon, &msg ) != 0 )
		return "";

	// We dont need the icon
	delete icon;

	// Check if we can find the mime type the message
	msg.FindString( "mimetype", &mime );

	return mime;
}

void ImageKeeper :: _AddImage( String file )
{
	_FixCompletePath( file );

	String mime = _GetMime( file );

	if( mime.find( "image/" ) == 0 )
		_m->ImagesList.push_back( file );
}

void ImageKeeper :: _SetImageStartIndex( int index )
{
	_m->ImageStartIndex = index;
}

int ImageKeeper :: _GetImageStartIndex()
{
	return _m->ImageStartIndex;
}

void ImageKeeper :: _FixCompletePath( String& file )
{
	if( file[ 0 ] != '/' )
	{
		char* buf = get_current_dir_name();

		file = String( buf ) + "/" + file;

		free( buf );
	}
}


String ImageKeeper :: GetNextImage()
{
	if( _m->ImagesList.empty() )
		return "";

	if( _m->ImageIndex == -1 )
		_m->ImageIndex = _m->ImageStartIndex;
	else
	{
		_m->ImageIndex++;
		if( _m->ImageIndex >= (int) _m->ImagesList.size() )
			_m->ImageIndex = 0;
	}

	return _m->ImagesList[ _m->ImageIndex ];	
}

String ImageKeeper :: GetPreviousImage()
{
	if( _m->ImagesList.empty() )
		return "";

	if( _m->ImageIndex == -1 )
		_m->ImageIndex = _m->ImageStartIndex;
	else
	{
		_m->ImageIndex--;
		if( _m->ImageIndex < 0 )
			_m->ImageIndex = _m->ImagesList.size() - 1;
	}

	return _m->ImagesList[ _m->ImageIndex ];
}

String ImageKeeper :: GetCurrentImage()
{
	if( _m->ImagesList.empty() )
		return "";

	if( _m->ImageIndex < 0  && _m->ImageIndex >= (int)_m->ImagesList.size() )
		return "";

	return _m->ImagesList[ _m->ImageIndex ];
}

void ImageKeeper :: RemoveCurrentImage()
{
	_m->ImagesList.erase( _m->ImagesList.begin() + _m->ImageIndex );

	if( _m->ImagesList.empty() || _m->ImageIndex >= (int)_m->ImagesList.size() - 1 )
		_m->ImageIndex = 0;
}

void ImageKeeper :: AddImage( String file )
{
	_AddImage( file );
}


///////////////////////////////////////////////////////////////////////////////
//
// S i n g l e I m a g e K e e p e r
//
///////////////////////////////////////////////////////////////////////////////

SingleImageKeeper :: SingleImageKeeper() : ImageKeeper()
{	
}

SingleImageKeeper :: SingleImageKeeper( String image ) : ImageKeeper()
{	
	AddImage( image );
}

SingleImageKeeper :: ~SingleImageKeeper()
{
}

void SingleImageKeeper :: AddImage( String image )
{
	_SetImageStartIndex( -1 );

	_FixCompletePath( image );

	// Add all files (including none-images)
	Path c( image );
	String path = image.substr( 0, image.size() - c.GetLeaf().size() );

	Directory dir( path );
	dir.Rewind();

	String tmp;
	int i = 0;
	while( dir.GetNextEntry( &tmp ) )	
	{
		if( tmp == "." || tmp == ".." )
			continue;

		_AddImage( path + tmp );
		if( path + tmp == image )
		{
			_SetImageStartIndex( i );
		}
		i++;
	}

	// If the index has not been set the file couldnt be found
	if( _GetImageStartIndex() == -1 )
		throw ID_FILE_NOT_FOUND;
}

///////////////////////////////////////////////////////////////////////////////
//
// S e v e r a l I m a g e K e e p e r
//
///////////////////////////////////////////////////////////////////////////////
SeveralImageKeeper :: SeveralImageKeeper() : ImageKeeper()
{
}

SeveralImageKeeper :: SeveralImageKeeper( int no, char* path[] ) : ImageKeeper()
{	
	for( int i = 0 ; i < no ; i++ )
		_AddImage( path[ i ] );
}

SeveralImageKeeper :: ~SeveralImageKeeper()
{
}

