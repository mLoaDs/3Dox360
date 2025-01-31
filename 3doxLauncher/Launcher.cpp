#include "Global.h"
#include "IniFile.h"
#include "Debug.h"
#include "FileGameSource.h"
#include "md5.h"
 

bool g_bExit = false;

const DWORD         INFO_COLOR = 0xffffff00;          // Yellow

// xbox utility functions

#define DEVICE_MEMORY_UNIT0 1
#define DEVICE_MEMORY_UNIT1 2
#define DEVICE_MEMORY_ONBOARD 3
#define DEVICE_CDROM0 4
#define DEVICE_HARDISK0_PART1 5
#define DEVICE_HARDISK0_SYSPART 6
#define DEVICE_USB0 7
#define DEVICE_USB1 8
#define DEVICE_USB2 9
#define DEVICE_TEST 10
#define DEVICE_CACHE 11

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING;

extern "C" int __stdcall ObCreateSymbolicLink( STRING*, STRING*);

int Mounted[20];
static char isofilename[256]="";
static char MD5Hash[256] = "";
static char GameID[256] = "";

class Launcher : public ATG::Application
{
	ATG::Timer m_Timer;
    ATG::Font m_Font;
    LPDIRECT3DTEXTURE9		m_pCurrentImage;
	LPDIRECT3DTEXTURE9		newtex;
	LPDIRECT3DSURFACE9 m_pdBkBuffer;
    DWORD m_dwCurrentImageIndex;

	typedef struct _filenamestruct {
		WCHAR name[MAX_PATH] ;
		unsigned char filename[MAX_PATH] ;
	} FILENAME ;

	UINT32 topIdx;
	UINT32 curr;
	int spos;

	unsigned long numfiles;
	int		m_nXOffset, m_nFontHeight ;
	int		m_namesPerPage ;
	int		m_state;

	int		Skins; 
	UINT32 theWidth;
	UINT32 theHeight;
	FILENAME *files ;

public:
	HRESULT         SegaInit();
	void			ExecEmu();
	HRESULT			InitializeWithScreen();
	void			MoveCursor();
	VOID			ScreenGrab(char *fileName);
	void			QuickSort( int lo, int hi );
	void			FindAvailRoms();
private:
    virtual HRESULT Initialize();
    virtual HRESULT Update();
    virtual HRESULT Render();
};

Launcher atgApp;

int DriveMounted(string path)
{
	WIN32_FIND_DATA findFileData;
	memset(&findFileData,0,sizeof(WIN32_FIND_DATA));
	string searchcmd = path + "\\*.*";
	//debugLog(searchcmd.c_str());
	HANDLE hFind = FindFirstFile(searchcmd.c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	FindClose(hFind);

	return 1;
}

HRESULT Mount( int Device, char* MountPoint )
{
	char MountConv[260];
	sprintf_s( MountConv,"\\??\\%s", MountPoint );

	char * SysPath = NULL;
	switch( Device )
	{
		case DEVICE_MEMORY_UNIT0:
			SysPath = "\\Device\\Mu0";
			break;
		case DEVICE_MEMORY_UNIT1:
			SysPath = "\\Device\\Mu1";
			break;
		case DEVICE_MEMORY_ONBOARD:
			SysPath = "\\Device\\BuiltInMuSfc";
			break;
		case DEVICE_CDROM0:
			SysPath = "\\Device\\Cdrom0";
			break;
		case DEVICE_HARDISK0_PART1:
			SysPath = "\\Device\\Harddisk0\\Partition1";
			break;
		case DEVICE_HARDISK0_SYSPART:
			SysPath = "\\Device\\Harddisk0\\SystemPartition";
			break;
		case DEVICE_USB0:
			SysPath = "\\Device\\Mass0";
			break;
		case DEVICE_USB1:
			SysPath = "\\Device\\Mass1";
			break;
		case DEVICE_USB2:
			SysPath = "\\Device\\Mass2";
			break;
		case DEVICE_CACHE:
			SysPath = "\\Device\\Harddisk0\\Cache0";
			break;
	}

	STRING sSysPath = { (USHORT)strlen( SysPath ), (USHORT)strlen( SysPath ) + 1, SysPath };
	STRING sMountConv = { (USHORT)strlen( MountConv ), (USHORT)strlen( MountConv ) + 1, MountConv };
	int res = ObCreateSymbolicLink( &sMountConv, &sSysPath );

	if (res != 0)
		return res;

	return DriveMounted(MountPoint);
}

void __cdecl main()
{
	g_debug.Log("Starting Launcher CE\n");

	// Set file cache size
	XSetFileCacheSize(8 * 1024 * 1024);

	memset(&Mounted,0,20);

 	Mounted[DEVICE_USB0] = Mount(DEVICE_USB0,"Usb0:");
 	Mounted[DEVICE_USB1] = Mount(DEVICE_USB1,"Usb1:");
 	Mounted[DEVICE_USB2] = Mount(DEVICE_USB2,"Usb2:");
 	Mounted[DEVICE_HARDISK0_PART1] = Mount(DEVICE_HARDISK0_PART1,"Hdd1:");
 	Mounted[DEVICE_HARDISK0_SYSPART] = Mount(DEVICE_HARDISK0_SYSPART,"HddX:");
 	Mounted[DEVICE_MEMORY_UNIT0] = Mount(DEVICE_MEMORY_UNIT0,"Memunit0:");
 	Mounted[DEVICE_MEMORY_UNIT1] = Mount(DEVICE_MEMORY_UNIT1,"Memunit1:");
	Mounted[DEVICE_MEMORY_ONBOARD] = Mount(DEVICE_MEMORY_ONBOARD,"OnBoardMU:"); 
	Mounted[DEVICE_CDROM0] = Mount(DEVICE_CDROM0,"Dvd:"); 
 
	// Parse ini file for settings
	g_iniFile.CheckForIniEntry();

 

    ATG::GetVideoSettings( &atgApp.m_d3dpp.BackBufferWidth, &atgApp.m_d3dpp.BackBufferHeight );

    atgApp.m_d3dpp.BackBufferFormat  = ( D3DFORMAT )MAKESRGBFMT( D3DFMT_X8R8G8B8 );
	atgApp.m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	atgApp.m_d3dpp.FullScreen_RefreshRateInHz = 60;

	atgApp.Run();
}


HRESULT Launcher::InitializeWithScreen()
{

	m_nXOffset = 0 ;
	m_nFontHeight = 16 ;

	m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
	m_Font.Begin();
	m_Font.DrawText(m_nXOffset+  32, 16*2, 0xffffffff, L"Reading roms directory...please wait." ) ;
	m_Font.End();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	if ( m_nFontHeight < 1 )
		m_nFontHeight = 16 ;


	m_namesPerPage =  (36*14 ) / ( m_nFontHeight+2) ;

	curr = 0 ;
	topIdx = 0 ;
	numfiles = 0 ;

	
	FindAvailRoms() ;

	if( FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,"game:\\media\\background.png",&newtex)))
	{
			Skins = 0;
		}else{
			Skins = 1;
	}
 
   return S_OK;
}


void Launcher::MoveCursor()
{
	char c ;
	int  lcv ;

		 ATG::Input::GetMergedInput();

		 
		if ( ATG::Input::m_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN || ATG::Input::m_Gamepads[0].bRightTrigger > 128) 
		{
			if ( curr == topIdx + (m_namesPerPage-1) )
			{
				if ( curr < numfiles - 1 )
				{
					topIdx++ ;
					curr++ ;
				}
			}
			else
			{
				if ( curr < numfiles-1 )
					curr++ ;
			}
		}
		else if ( ATG::Input::m_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP || ATG::Input::m_Gamepads[0].bLeftTrigger > 128 ) 
		{
			if ( curr == topIdx )
			{
				if ( curr > 0 )
				{
					topIdx-- ;
					curr-- ;
				}
			}
			else
			{
				if ( curr > 0 )
					curr-- ;
			}
		}

}
 
VOID Launcher::ScreenGrab(char *fileName)
{
	// write the entire surface to the requested file
	D3DXSaveTextureToFile(fileName,D3DXIFF_BMP,m_pCurrentImage,NULL);
}

//--------------------------------------------------------------------------------------
// Name: Initialize
// Desc: This creates all device-dependent display objects.
//--------------------------------------------------------------------------------------
HRESULT Launcher::Initialize()
{
    m_pCurrentImage = NULL;


	// Create the font
    if( FAILED( m_Font.Create( "game:\\media\\fonts\\arial_12.xpr" ) ) )
        return ATGAPPERR_MEDIANOTFOUND;

    // Confine text drawing to the title safe area
    m_Font.SetWindow( ATG::GetTitleSafeArea() );

    // Initialize the simple shaders
    ATG::SimpleShaders::Initialize( NULL, NULL );

    if( m_pCurrentImage )
    {
        m_pd3dDevice->SetTexture( 0, NULL );
        m_pCurrentImage->Release();

    }


	InitializeWithScreen();
	
	return S_OK;
}


void Launcher::ExecEmu()
{
 
}

  
//--------------------------------------------------------------------------------------
// Name: Update
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//--------------------------------------------------------------------------------------
HRESULT Launcher::Update()
{
	WCHAR szMsg[100];		
	BYTE checkSumSourceData[4096];
	MoveCursor();
	ATG::Input::GetMergedInput();

	if(ATG::Input::m_Gamepads[0].wButtons & XINPUT_GAMEPAD_A)
	{
				
				

		m_Font.Begin();
		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
	 			
				

		// before we launch lets get the hash of the sectors and determine if we need to apply hacks

		sprintf(isofilename,"game:\\roms\\%s",files[curr].filename);

		swprintf(szMsg,L"%S",isofilename);

		m_Font.DrawText(m_nXOffset+  32, 16*2, 0xffffffff, L"Loading game...please wait." ) ;
		m_Font.DrawText(m_nXOffset+  32, 24*2, 0xffffffff, szMsg  ) ;

		FileGameSource *fileGame = new FileGameSource(isofilename);
		fileGame->Open();
		// read the first 2 sectors
			
		for (int x = 0; x < 2; x++)
		{
			fileGame->ReadSector(&checkSumSourceData[x * 2048],x);
		}

		fileGame->Close();

		if (fileGame)
		{
			delete fileGame;
			fileGame = 0;
		}

		unsigned char finalMD5[16];
		MD5_CTX md5;
		MD5Init(&md5);
		MD5Update(&md5,(void *)&checkSumSourceData[0], 4096);
		MD5Final(&finalMD5[0], &md5);
			 
		sprintf(MD5Hash,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
			finalMD5[0],finalMD5[1],finalMD5[2],finalMD5[3],
			finalMD5[4],finalMD5[5],finalMD5[6],finalMD5[7],
			finalMD5[8],finalMD5[9],finalMD5[10],finalMD5[11],
			finalMD5[12],finalMD5[13],finalMD5[14],finalMD5[15]);

		sprintf(GameID,"%02x%02x%02x%02x", 
			finalMD5[0],finalMD5[1],finalMD5[2],finalMD5[3]);

				
		g_iniFile.SaveTempRomFileName(isofilename,MD5Hash,GameID);
		
		swprintf(szMsg,L"MD5 : %S",MD5Hash); 
		m_Font.DrawText(m_nXOffset+  32, 32*2, 0xffffffff, szMsg ) ;

		swprintf(szMsg,L"GAME ID : %S",GameID); 
		m_Font.DrawText(m_nXOffset+  32, 40*2, 0xffffffff, szMsg ) ;
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		Sleep(1000);
		XLaunchNewImage("D:\\3dox.xex", NULL);

	}
				
	return S_OK ;
			
  
}

//--------------------------------------------------------------------------------------
// Name: Render
// Desc: Sets up render states, clears the viewport, and renders the scene.
//--------------------------------------------------------------------------------------

static DWORD col = 0xffffffff;
static int scale = 400;
HRESULT Launcher::Render()
{

		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		

		if (scale == 0)
			scale = 0;
		else
			scale-=2;
		D3DRECT d3drect =
		{
        scale,                          // x1
        0,                          // y1
		Launcher::m_d3dpp.BackBufferWidth,    // x2
        Launcher::m_d3dpp.BackBufferHeight,   // y2
		};
 
	 
	 
		if(Skins){
		ATG::DebugDraw::DrawScreenSpaceTexturedRect( d3drect, newtex);
		}
		WCHAR str[2000];
		swprintf( str, L"3dox V0.03 - New Years 2013 Pre-Release !! : %u games Found", numfiles);

		
		m_Font.Begin(); 
		m_Font.DrawText(300, 32, col, str );

		

		if (col < 0x00222222)
			col ++;
		else
			col--;
			
		
		float fWinX = 32, fWinY = 64;
			for ( unsigned int idx = topIdx ; ( idx < numfiles ) && ( idx < topIdx+m_namesPerPage) ; idx++ )
		{
			if ( curr == idx )
			{
					m_Font.DrawText(m_nXOffset+fWinX  , fWinY, INFO_COLOR, files[idx].name );
			}
			else
			{
					m_Font.DrawText(m_nXOffset+fWinX  , fWinY, 0xffffffff, files[idx].name );
			}
			fWinY += (m_nFontHeight + 2);
		}
	
			
		// end font drawing
		m_Font.End();

		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
 
 
    return S_OK;
}

void Launcher::FindAvailRoms()
{
	HANDLE hFind;	
	WIN32_FIND_DATA oFindData;

	files = new FILENAME[MAX_PATH];

	hFind = FindFirstFile( "game:\\roms\\*", &oFindData);
	
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	
	do 
	{

		files[numfiles].name[0] = 0 ;
		strcpy( (char*)files[numfiles].filename,(char*)oFindData.cFileName ) ;
		strlwr( (char*)( files[numfiles].filename )) ;
        swprintf( files[numfiles].name, L"%S", files[numfiles].filename );
		numfiles++ ;
	} while (FindNextFile(hFind, &oFindData));

	FindClose( hFind );
	QuickSort( 0, numfiles-1 ) ;
}


void Launcher::QuickSort( int lo, int hi )
{
	int i = lo;
	int j = hi;
	char partitionStr[200];
	FILENAME tmpf;

	strcpy( partitionStr, (const char*)files[(i+j)/2].filename ) ;
	do
	{
		while ( strcmp( (const char*)files[i].filename, partitionStr ) < 0 ) i++ ;
		while ( strcmp( partitionStr, (const char*)files[j].filename ) < 0 ) j-- ;

		if (i <= j)
		{
			memcpy( &tmpf, &(files[i]), sizeof(tmpf) ) ;
			memcpy( &(files[i]), &(files[j]), sizeof(tmpf) ) ;
			memcpy( &(files[j]), &tmpf, sizeof(tmpf) ) ;
			i++;
			j--;
		}
	} while (i <= j);

	if (lo < j) QuickSort(lo, j);
	if (i < hi) QuickSort(i, hi);

	
}

