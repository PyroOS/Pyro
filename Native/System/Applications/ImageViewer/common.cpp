// SBasic (C)opyright 2006 Jonas Jarvoll
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

#include <storage/file.h>
#include <util/resources.h>

#include "common.h"

using namespace os;
using namespace std;

BitmapImage* LoadImage( const String& zResource )
{
	File cSelf( open_image_file( get_image_id() ) );
	Resources cCol( &cSelf );		
    ResStream* pcStream = cCol.GetResourceStream(zResource);
    BitmapImage* vImage = new BitmapImage(pcStream);
    delete( pcStream );
    return(vImage);
}

