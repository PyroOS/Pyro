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

#include <storage/registrar.h>
#include <iostream>
#include <util/settings.h>
#include "main.h"

using namespace os;
using namespace std;

int main( int argc, char* argv[] )
{
 	NView* app = new NView( argc, argv );
	app->Run();

	return( 0 );
}

AppWindow* NView :: m_AppWindow = NULL;

NView :: NView( int argc, char* argv[] ) : Application( "application/x-ImageViewer" )
{
	try {
		SetCatalog( "lang.catalog" );
	} catch( ... ) {
	}

	// Register some common image formats and make ourselves the default handler for them
	try
	{
		RegistrarManager *pcRegistrar = RegistrarManager::Get();

		/* PNG */
		pcRegistrar->RegisterType( "image/png", "PNG Image" );
		pcRegistrar->RegisterTypeExtension( "image/png", "png" );
		pcRegistrar->RegisterTypeIcon( "image/png", Path( "/system/icons/filetypes/image_png.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/png" );

		/* JPEG */
		pcRegistrar->RegisterType( "image/jpeg", "JPEG Image" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jpeg" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jpg" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jpe" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jif" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jfif" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jfi" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jff" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "pjpeg" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jls" );
		pcRegistrar->RegisterTypeExtension( "image/jpeg", "jmh" );
		pcRegistrar->RegisterTypeIcon( "image/jpeg", Path( "/system/icons/filetypes/image_jpeg.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/jpeg" );

		/* GIF */
		pcRegistrar->RegisterType( "image/gif", "GIF Image" );
		pcRegistrar->RegisterTypeExtension( "image/gif", "gif" );
		pcRegistrar->RegisterTypeIcon( "image/gif", Path( "/system/icons/filetypes/image_gif.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/gif" );

		/* BMP */
		pcRegistrar->RegisterType( "image/bmp", "BMP Image" );
		pcRegistrar->RegisterTypeExtension( "image/bmp", "bmp" );
		pcRegistrar->RegisterTypeIcon( "image/bmp", Path( "/system/icons/filetypes/image_bmp.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/bmp" );

		/* TIFF */
		pcRegistrar->RegisterType( "image/tiff", "TIFF Image" );
		pcRegistrar->RegisterTypeExtension( "image/tiff", "tif" );
		pcRegistrar->RegisterTypeExtension( "image/tiff", "tiff" );
		pcRegistrar->RegisterTypeIcon( "image/tiff", Path( "/system/icons/filetypes/image_tiff.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/tiff" );

		/* XBM */
		pcRegistrar->RegisterType( "image/x-bitmap", "XBM Image" );
		pcRegistrar->RegisterTypeExtension( "image/x-bitmap", "xbm" );
		pcRegistrar->RegisterTypeIcon( "image/x-bitmap", Path( "/system/icons/filetypes/image_xbm.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/x-bitmap" );

		/* XPM */
		pcRegistrar->RegisterType( "image/x-pixmap", "XPM Image" );
		pcRegistrar->RegisterTypeExtension( "image/x-pixmap", "xpm" );
		pcRegistrar->RegisterTypeIcon( "image/x-pixmap", Path( "/system/icons/filetypes/image_xpm.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/x-pixmap" );

		/* TGA */
		pcRegistrar->RegisterType( "image/x-tga", "TGA Image" );
		pcRegistrar->RegisterTypeExtension( "image/x-tga", "tga" );
		pcRegistrar->RegisterTypeExtension( "image/x-tga", "icb" );
		pcRegistrar->RegisterTypeExtension( "image/x-tga", "tpic" );
		pcRegistrar->RegisterTypeExtension( "image/x-tga", "vda" );
		pcRegistrar->RegisterTypeExtension( "image/x-tga", "vst" );
		pcRegistrar->RegisterTypeIcon( "image/x-tga", Path( "/system/icons/filetypes/image_tga.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/x-tga" );

		/* PCX */
		pcRegistrar->RegisterType( "image/x-pcx", "PCX Image" );
		pcRegistrar->RegisterTypeExtension( "image/x-pcx", "pcx" );
		pcRegistrar->RegisterTypeIcon( "image/x-pcx", Path( "/system/icons/filetypes/image_pcx.png" ) );
		pcRegistrar->RegisterAsTypeHandler( "image/x-pcx" );

		pcRegistrar->Put();
	}
	catch( std::exception e )
	{
		std::cerr << e.what() << std::endl;
	}

	try
	{
		// Load application settings
		Settings cSettings;
		cSettings.Load();

		Rect cBounds = cSettings.GetRect( "window_position", Rect( 100,100,400,275 ) );
		m_AppWindow = new AppWindow( argc, argv, cBounds );
		m_AppWindow->Show();
		m_AppWindow->MakeFocus();
	}
	catch( String& s )
	{
		printf( "%s\n", s.c_str() );
		Application::GetInstance()->PostMessage( M_QUIT );
	}
}



