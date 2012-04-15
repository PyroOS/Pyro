#include "mainwindow.h"
#include "messages.h"

MainWindow::MainWindow() : os::Window( os::Rect( 0, 0, 300, 250 ), "main_wnd", "Format" )
{
	os::LayoutView* pcView = new os::LayoutView( GetBounds(), "layout_view" );
	#include "mainwindowLayout.cpp"
	pcView->SetRoot( m_pcRoot );
	AddChild( pcView );

	/* Set Icon */
	os::Resources cCol( get_image_id() );
	os::ResStream *pcStream = cCol.GetResourceStream( "icon48x48.png" );
	os::BitmapImage *pcIcon = new os::BitmapImage( pcStream );
	SetIcon( pcIcon->LockBitmap() );
	delete( pcIcon );


	/* Show Splash Screen */
	Splash* pcWindow = new Splash(LoadImageFromResource("logo.png"),"Format is scanning devices",false,0.0);
	pcWindow->Go();

	/* Clear All */
	m_pcDevice_Selection->Clear();
	m_pcFilesystem_Selection->Clear();
	m_pcVolumename_Text->Clear();
	m_pcArguments_Text->Clear();

	/* Load Devices & Filesystems */
	LoadDevices();
	LoadFilesystems();

	/*  Remove Splash Screen */
	pcWindow->Quit();
}

void MainWindow::HandleMessage( os::Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_FORMAT:
		{
			Format();
		}
		case M_APP_QUIT:
		{
			PostMessage( os::M_QUIT );
		}
		break;
	}
}

void MainWindow::LoadDevices()
{
	struct stat f__stat;
	char cDevice[24];

	/* Add File */
	m_pcDevice_Selection->AppendItem( "File..." );

	/* Scan for BIOS drives */
	for( int counter = 97 ; counter <= 122 ; counter = counter + 1 )
	{
		os::String cDev = os::String( "/dev/disk/bios/hd" ) + counter + os::String( "/" );
		os::String cDevRaw = cDev + os::String( "raw" );
		sprintf( cDevice, "%s", cDevRaw.c_str() );
		bool FileExist = (stat(cDevice,&f__stat) == 0);
		if( FileExist == true )
		{
			pDir = opendir(cDev.c_str());
			while( (psEntry = readdir( pDir )) != NULL)
			{
				if(os::String(psEntry->d_name) != os::String("raw"))
				{
					os::String cDevEntry = cDev+psEntry->d_name;
					m_pcDevice_Selection->AppendItem( cDevEntry );
				}
			}
		}
	}

	/* Scan for ATA drives */
	for( int counter = 97 ; counter <= 122 ; counter = counter + 1 )
	{
		os::String cDev = os::String( "/dev/disk/ata/hd" ) + counter + os::String( "/" );
		os::String cDevRaw = cDev + os::String( "raw" );
		sprintf( cDevice, "%s", cDevRaw.c_str() );
		bool FileExist = (stat(cDevice,&f__stat) == 0);
		if( FileExist == true )
		{
			pDir = opendir(cDev.c_str());
			while( (psEntry = readdir( pDir )) != NULL)
			{
				if(os::String(psEntry->d_name) != os::String("raw"))
				{
					os::String cDevEntry = cDev+psEntry->d_name;
					m_pcDevice_Selection->AppendItem( cDevEntry );
				}
			}
		}
	}

	/* Scan for USB/SCSI drives */
	for( int counter = 97 ; counter <= 122 ; counter = counter + 1 )
	{
		os::String cDev = os::String( "/dev/disk/scsi/hd" ) + counter + os::String( "/" );
		os::String cDevRaw = cDev + os::String( "raw" );
		sprintf( cDevice, "%s", cDevRaw.c_str() );
		bool FileExist = (stat(cDevice,&f__stat) == 0);
		if( FileExist == true )
		{
			pDir = opendir(cDev.c_str());
			while( (psEntry = readdir( pDir )) != NULL)
			{
				if(os::String(psEntry->d_name) != os::String("raw"))
				{
					os::String cDevEntry = cDev+psEntry->d_name;
					m_pcDevice_Selection->AppendItem( cDevEntry );
				}
			}
		}
	}

}

void MainWindow::LoadFilesystems()
{
	system( "dd if=/dev/random of=/tmp/TestDrive bs=1024 count=8192 && sync" );
	pDir = opendir("/system/drivers/fs/");
	while ( (psEntry = readdir( pDir )) != NULL)
	{
			if ( initialize_fs( "/tmp/TestDrive", psEntry->d_name, "TEST", NULL, 0 ) < 0 )
			{
				continue;
			} else {
				m_pcFilesystem_Selection->AppendItem( psEntry->d_name );
			}
	}
	m_pcFilesystem_Selection->SetSelection(0);
}

void MainWindow::Format()
{
			/* Show Splash Screen */
			Splash* pcWindow = new Splash(LoadImageFromResource("logo.png"),"Formatting...",false,0.0);
			pcWindow->Go();

			/* Get Device */
			char zDevice[24];
			int nDevice = m_pcDevice_Selection->GetSelection();
			if( nDevice == 0 )
			{
				m_pcFileRequester = new os::FileRequester( os::FileRequester::LOAD_REQ, new os::Messenger( this ), zDevice, os::FileRequester::NODE_FILE, true );
				m_pcFileRequester->Show();
				m_pcFileRequester->MakeFocus();
				printf(zDevice);
			} else {
				sprintf(zDevice, "%s", m_pcDevice_Selection->GetItem(nDevice).c_str());
			}

			/* Get Filesystem */
			int nFilesystem = m_pcFilesystem_Selection->GetSelection();
			char zFilesystem[16];
			sprintf(zFilesystem, "%s", m_pcFilesystem_Selection->GetItem(nFilesystem).c_str());

			/* Get Volumename */
			os::String cVolumename = m_pcVolumename_Text->GetValue();
			char zVolumename[32];
			sprintf(zVolumename, "%s", cVolumename.c_str());

			/* Get Arguments */
			os::String cArguments = m_pcArguments_Text->GetValue();
			char zArguments[64];
			sprintf(zArguments, "%s", cArguments.c_str());

			/* Format */
			if ( initialize_fs( zDevice, zFilesystem, zVolumename, NULL, 0 ) < 0 ) {
				char zError[256];
				sprintf( zError, "%s", strerror(errno));
				os::Alert* pcAlert = new os::Alert( "Format", zError, m_pcIcon->LockBitmap(),0, "_Ok",NULL);
				pcAlert->Go( new os::Invoker( 0 ) );
			}

			/*  Remove Splash Screen */
			pcWindow->Quit();
}

BitmapImage* MainWindow::LoadImageFromResource( String zResource )
{
	BitmapImage *pcImage = new BitmapImage();
	Resources cRes( get_image_id() );
	ResStream *pcStream = cRes.GetResourceStream( zResource );
	pcImage->Load( pcStream );
	delete ( pcStream );
	return pcImage;
}

bool MainWindow::OkToQuit()  // Obsolete?
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return( true );
}
