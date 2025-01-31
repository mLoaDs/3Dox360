// 3dox.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#ifdef _XBOX
#include <xgraphics.h>
#include "XFileCache.h"
#else
HWND hWnd;
 
#endif
#include "EmulationHelper.h"
#include "ScalingEffect.h"

#include "IGameSource.h"
#include "GameSourceBase.h"
#include "GameConsole.h"
#include "BiosOnlyGameSource.h"
#include "FileGameSource.h"
#include "DiscGameSource.h"

#include "DxSound.h"

#include "SimpleIni.h"
 

namespace GameSource
{
	enum Type
	{
		None = 0,
		File,
		Disc,
		BinaryFile,
	};
}
typedef GameSource::Type GameSourceType;

//=============================================================================
// Global constants
//=============================================================================
const int emuWidth = 320;
const int emuHeight = 240;
const int backbufferWidth = 1280;
const int backbufferHeight = 720;
const int textureWidth = 320;
const int textureHeight = 240;

// Projection matrices
D3DXMATRIX			m_matProj;
D3DXMATRIX			m_matWorld;
D3DXMATRIX			m_matView;

D3DXMATRIX			m_matPreProj;
D3DXMATRIX			m_matPreView;
D3DXMATRIX			m_matPreWorld;

// Pixel shader
char						pshader[30];
ScalingEffect*				psEffect;
LPDIRECT3DTEXTURE9			lpWorkTexture1;
LPDIRECT3DTEXTURE9			lpWorkTexture2;
LPDIRECT3DVOLUMETEXTURE9	lpHq2xLookupTexture;

//=============================================================================
// Global variables
//=============================================================================
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices
LPDIRECT3DTEXTURE9		g_pTexture	 = NULL; // Texture to render to.
LPDIRECT3DSURFACE9		g_pSurface	 = NULL; // Temp surface.

#ifdef _XBOX
D3DVertexDeclaration*	g_pGradientVertexDecl = NULL;
#else
IDirect3DVertexDeclaration9 *g_pGradientVertexDecl = NULL;
#endif

int* g_DispBuffer = 0; // Global buffer to store pixel data

GameConsole* g_GameConsole = GameConsole::GetInstance();
IGameSource* g_GameSource = NULL;

DxSound g_DxSound;

int running = 0;
const char* g_GameName = 0;
const char* g_GameID = 0;
const char* g_GameMD5 = 0;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// A structure for our custom vertex type

#ifndef _XBOX
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position.
    D3DCOLOR    color;    // The color.
    FLOAT       tu, tv;   // The texture coordinates.
};
#else
struct CUSTOMVERTEX {
	float x, y, z, rhw;
	float u, v;         //texture coords
};

static CUSTOMVERTEX vertex[4];

#endif

FLOAT fSecsPerTick;
LARGE_INTEGER qwTime, qwLastTime, qwElapsedTime, qwAppTime, qwElapsedAppTime;
FLOAT fTime, fElapsedTime, fAppTime, fElapsedAppTime;

//=============================================================================
// Utility functions
//=============================================================================
void DisplayText(IDirect3DSurface9* surface, char* str, int x, int y, DWORD colour);
void DisplayDebug(IDirect3DSurface9* surface);

//=============================================================================
// Console Control
//=============================================================================
void DoConsoleStart(bool alsoAllowLoadState);
void DoConsoleReset(bool alsoAllowLoadState) {};
void DoConsoleTogglePause() {};
void DoConsoleAdvanceFrame() {};
void DoConsoleStop() {};
void DoCloseGame() {};
void DoShowRomNag() {};
void DoHideRomNag() {};
void DoChooseBiosRom() {};
void DoChooseGameRom() {};
void DoOpenDiscDrive() {};
char* GetLastRomDirectory() { return NULL; };
void DoSaveState() {};
void DoLoadState() {};
void DoAdvanceSaveSlot() {};
void DoUpdateFPS() {};
void DoShowSettings() {};
void DoShowConfigureInput() {};


void Console_FrameDone();
IGameSource* CreateGameSource();

void Cleanup()
{

    g_pd3dDevice->SetStreamSource(0,NULL,0,NULL);	
    g_pd3dDevice->SetVertexShader( NULL );
    g_pd3dDevice->SetVertexDeclaration( NULL );
 
	g_pTexture->Release();
	g_pVB->Release();
	g_pd3dDevice->Release();
	g_pD3D->Release();

	g_GameConsole->Destroy();

	if (g_GameSource)
	{
		delete g_GameSource;
		g_GameSource = 0;
	}

	if (g_GameConsole)
	{
		delete g_GameConsole;
		g_GameConsole = 0;
	}

	if (g_DispBuffer)
	{
		delete[] g_DispBuffer;
		g_DispBuffer = 0;

	}

	g_DxSound.SNDDXDeInit();

	
};

void DoConsoleStart(bool alsoAllowLoadState)
{
	// Check for existence of NVRAM file.
	HANDLE nvRamFile = CreateFile("d:\\NVRAM_SaveData.ram", 
		GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// No nvRamFile, create one.
	if (GetLastError() == 0)
	{
		unsigned char* nvramBytes = new unsigned char[g_GameConsole->GetNvRamSize()];

		void* nvramBytePointer = nvramBytes;
		EmulationHelper::InitializeNvram(nvramBytePointer);

		unsigned long numBytesWritten = NULL;

		BOOL success = WriteFile(nvRamFile, nvramBytes, 
			g_GameConsole->GetNvRamSize(), &numBytesWritten, NULL);
	} // End if (GetLastError() == 0)

	CloseHandle(nvRamFile);

	// Create a game source.
	g_GameSource = (IGameSource*)CreateGameSource();

	try
	{
		g_GameConsole->Start("d:\\bios\\bios.rom", g_GameSource, "d:\\NVRAM_SaveData.ram");
	}
	catch (BadBiosRomException)
	{
		//"The bios file ({0}) failed to load. Please choose another."
	}
	catch (BadGameRomException)
	{
		//string errorMessage = null;
		//if (gameSource is FileGameSource)
		//	errorMessage = string.Format("The game file ({0}) failed to load. Please choose another.", ((FileGameSource)gameSource).GameFilePath);
		//else
		//	errorMessage = "The game failed to load. Please choose another.";
		//FourDO.UI.Error.ShowError(errorMessage);

		//// Since it failed to load, we want to un-remember this as the last loaded game.
		//Properties.Settings.Default.GameRomSourceType = (int)GameSourceType.None;
		//Properties.Settings.Default.Save();
	}
	catch (BadNvRamFileException)
	{
		//FourDO.UI.Error.ShowError(string.Format("The nvram file ({0}) could not be loaded. Emulation cannot start.", nvramFile));
	}

	// Load a state.
	if (alsoAllowLoadState == true)
	{
		//if (Properties.Settings.Default.AutoLoadLastSave == true)
			//this.DoLoadState();
	}

	//this.UpdateUI();
}

IGameSource* CreateGameSource()
{

	//return new BiosOnlyGameSource();
	
	int	sourceType = GameSource::File;

	//XFileCachePreload(XFILECACHE_STARTUP_FILES,
	//                       "game:\\roms\\*");
	
 
	if (sourceType == GameSource::None)
		return new BiosOnlyGameSource();

	if (sourceType == GameSource::File)
	{
		if(g_GameName != "")
		{
			return new FileGameSource(g_GameName);
		}
	
	}

	if (sourceType == GameSource::Disc)
		return new DiscGameSource();

	// Must be a currently unsupported type.
	return new BiosOnlyGameSource();
}

VOID InitTime()
{
    // Get the frequency of the timer
	LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency( &qwTicksPerSec );
    fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

    // Save the start time
    QueryPerformanceCounter( &qwTime );
    qwLastTime.QuadPart = qwTime.QuadPart;

    qwAppTime.QuadPart        = 0;
    qwElapsedTime.QuadPart    = 0;
    qwElapsedAppTime.QuadPart = 0;
}


#ifdef _XBOX
//  Vertex format:
  //
  //  0----------1
  //  |         /|
  //  |       /  |
  //  |     /    |
  //  |   /      |
  //  | /        |
  //  2----------3
  //
  //  (x,y) screen coords, in pixels
  //  (u,v) texture coords, betweeen 0.0 (top, left) to 1.0 (bottom, right)
static inline int dx9SetVertex(
	unsigned int px, unsigned int py, unsigned int pw, unsigned int ph,
    unsigned int tw, unsigned int th,
    unsigned int x, unsigned int y, unsigned int w, unsigned int h
    )
{
	// configure triangles
	// -0.5f is necessary in order to match texture alignment to display pixels

	float diff = -0.5f;

	void *pLockedVertexBuffer;
 
	vertex[0].x = vertex[2].x = (double)(x    ) + diff;
	vertex[1].x = vertex[3].x = (double)(x + w) + diff;
	vertex[0].y = vertex[1].y = (double)(y    ) + diff;
	vertex[2].y = vertex[3].y = (double)(y + h) + diff;
 
	double rw = (double)w / (double)pw * (double)tw;
	double rh = (double)h / (double)ph * (double)th;

	vertex[0].u = vertex[2].u = (double)(px    ) / rw;
	vertex[1].u = vertex[3].u = (double)(px + w) / rw;
	vertex[0].v = vertex[1].v = (double)(py    ) / rh;
	vertex[2].v = vertex[3].v = (double)(py + h) / rh;
 
	// Z-buffer and RHW are unused for 2D blit, set to normal values
	vertex[0].z = vertex[1].z = vertex[2].z = vertex[3].z = 0.0f;	 
	vertex[0].rhw = vertex[1].rhw = vertex[2].rhw = vertex[3].rhw = 1.0f;
 
	g_pd3dDevice->SetStreamSource(0,NULL,0,NULL);	

	HRESULT hr = g_pVB->Lock(0,0,&pLockedVertexBuffer,NULL);
	memcpy(pLockedVertexBuffer,vertex,sizeof(vertex));
	g_pVB->Unlock();	

	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );


	g_pd3dDevice->SetRenderState(D3DRS_VIEWPORTENABLE, FALSE);
 
	return 0;
}

#endif

VOID InitVB()
{
	// Create a 4-point vertex buffer

#ifdef _XBOX


	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX)
		,D3DUSAGE_CPU_CACHED_MEMORY,0,0,&g_pVB,NULL)))	
		return ;

	dx9SetVertex(0,0,emuWidth,emuHeight,textureWidth,textureHeight,160,0,960,720);

#else
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX)
		,D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&g_pVB,NULL)))	
		return ;	

	CUSTOMVERTEX* pVertices;
	
	if( FAILED( g_pVB->Lock( 0, 4*sizeof(CUSTOMVERTEX),
		(void**)&pVertices, 0 ) ) )
		return ;

	pVertices[0].position=D3DXVECTOR3( -0.5, -0.5f, 0.0f/*-0.80f,  0.80f, 0.5f*/ );
	pVertices[0].color=0xffffffff;
	pVertices[0].tu=0.0;
	pVertices[0].tv=0.0;
	pVertices[1].position=D3DXVECTOR3( 0.0f + backbufferWidth, 0.0f, 0.0f/*0.80f,  0.80f, 0.5f*/ );
	pVertices[1].color=0xffffffff;
	pVertices[1].tu=1.0;
	pVertices[1].tv=0.0;
	pVertices[2].position=D3DXVECTOR3( 0.0f, 0.0f + backbufferHeight, 0.0f/*-0.80f,  -0.80f, 0.5f*/ );
	pVertices[2].color=0xffffffff;
	pVertices[2].tu=0.0;
	pVertices[2].tv=1.0;
	pVertices[3].position=D3DXVECTOR3(  0.0f + backbufferWidth, 0.0f + backbufferHeight, 0.0f/*0.80f,  -0.80f, 0.5f*/ );
	pVertices[3].color=0xffffffff;
	pVertices[3].tu=1.0;
	pVertices[3].tv=1.0;	

	g_pVB->Unlock();

	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );

#endif

#ifdef _XBOX
	g_pd3dDevice->SetRenderState(D3DRS_VIEWPORTENABLE, FALSE);
#endif	 
 
} 

//=============================================================================
// Name: InitD3D()
// Desc: Initializes Direct3D
//=============================================================================
HRESULT InitD3D()
{

	// Get the user video settings.	

#ifdef _XBOX
	XVIDEO_MODE VideoMode; 
	XMemSet( &VideoMode, 0, sizeof(XVIDEO_MODE) ); 
	XGetVideoMode( &VideoMode );
#endif

	D3DPRESENT_PARAMETERS pd3dPP;

    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	 
    // Set up the presentation parameters.
    ZeroMemory( &pd3dPP, sizeof( D3DPRESENT_PARAMETERS ) );

#ifdef _XBOX
	if (!VideoMode.fIsWideScreen)
	{
		pd3dPP.Flags |=  D3DPRESENTFLAG_NO_LETTERBOX;
	}

    pd3dPP.BackBufferWidth        = 1280;
    pd3dPP.BackBufferHeight       = 720;
    pd3dPP.BackBufferFormat       = D3DFMT_X8R8G8B8; 
    pd3dPP.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;

    // Create the device.
    g_pD3D->CreateDevice(
                    0, 
                    D3DDEVTYPE_HAL,
                    NULL,
                    D3DCREATE_FPU_PRESERVE,
                    &pd3dPP,
                    &g_pd3dDevice );

#else
	pd3dPP.BackBufferWidth				= 1280;
	pd3dPP.BackBufferHeight				= 720;
	pd3dPP.BackBufferFormat				= D3DFMT_X8R8G8B8;
	pd3dPP.SwapEffect					= D3DSWAPEFFECT_FLIP;
	pd3dPP.BackBufferCount				= 1;
	pd3dPP.hDeviceWindow				= hWnd;
	pd3dPP.Windowed						= TRUE;
	pd3dPP.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;

    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &pd3dPP, &g_pd3dDevice);
#endif
 
 
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	// Create texture.
 
#ifdef _XBOX
	g_pd3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_CPU_CACHED_MEMORY, D3DFMT_LIN_A8R8G8B8, 0, &g_pTexture, NULL);
	//D3DXCreateTextureFromFile(g_pd3dDevice,"d:\\x3tools.png",&g_pTexture);
#else
	g_pd3dDevice->CreateTexture(textureWidth, textureHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture, NULL);
	
#endif
	// Set texture here. No need to set it every frame.
	g_pd3dDevice->SetTexture( 0, g_pTexture );

	psEffect = new ScalingEffect(g_pd3dDevice);
	psEffect->LoadEffect("d:\\Shaders\\default.fx");

	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

#ifndef _XBOX
    // Turn off D3D lighting
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
#endif
	
	psEffect->Validate();
 
	// Create vertex declaration
    if( NULL == g_pGradientVertexDecl )
    {
        static const D3DVERTEXELEMENT9 decl[] =
        {
			{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            D3DDECL_END()
        };

        if( FAILED( g_pd3dDevice->CreateVertexDeclaration( decl, &g_pGradientVertexDecl ) ) )
            return E_FAIL;
    }
 
	
	
	g_pd3dDevice->SetVertexDeclaration( g_pGradientVertexDecl ); 		     
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

#ifdef _XBOX
	g_pd3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE, FALSE );
#endif

#ifdef _XBOX
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

#else
	// use an orthogonal matrix for the projection matrix
	D3DXMATRIX mat;

	D3DXMatrixOrthoOffCenterLH(&mat, 0, (float)backbufferWidth, (float)backbufferHeight, 0, 0.0f, 1.0f);

	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);
	// use an identity matrix for the world and view matrices
	D3DXMatrixIdentity(&mat);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);

	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = backbufferWidth;
	viewport.Height = backbufferHeight;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	g_pd3dDevice->SetViewport(&viewport);
#endif

	return S_OK;
}

void InitSound()
{
	g_DxSound.SNDDXInit();
	g_GameConsole->GetInstance()->m_DxSound = &g_DxSound;
}

VOID UpdateTime()
{    
    QueryPerformanceCounter( &qwTime );
    qwElapsedTime.QuadPart = qwTime.QuadPart - qwLastTime.QuadPart;
    qwLastTime.QuadPart    = qwTime.QuadPart;
    qwElapsedAppTime.QuadPart = qwElapsedTime.QuadPart;
    qwAppTime.QuadPart    += qwElapsedAppTime.QuadPart;

    // Store the current time values as floating point
    fTime           = fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
    fElapsedTime    = fSecsPerTick * ((FLOAT)(qwElapsedTime.QuadPart));
    fAppTime        = fSecsPerTick * ((FLOAT)(qwAppTime.QuadPart));
    fElapsedAppTime = fSecsPerTick * ((FLOAT)(qwElapsedAppTime.QuadPart));
}

//=============================================================================
// Name: Update()
// Desc: Updates the world for the next frame
//=============================================================================
VOID Update()
{	
	g_GameConsole->Update();	
 
	// Copy the game image onto a texture for rendering	
 
	// Render our frame buffer to texture
	D3DLOCKED_RECT lr;
	RECT rect = { 0, 0, emuWidth, emuHeight };

	unsigned char* data = (unsigned char*)g_DispBuffer;

#ifdef _XBOX
	g_pTexture->LockRect(0, &lr, NULL, D3DLOCK_NOOVERWRITE);
#else
	g_pTexture->LockRect(0, &lr, NULL, 0);
#endif
	unsigned char *pPict = (unsigned char*)lr.pBits;

	for (int line = 0; line < emuHeight; line++)
	{
		XMemCpy(pPict, data, emuWidth * 4);
		pPict += lr.Pitch;
		data += emuWidth * 4;
	}

	 
	g_pTexture->UnlockRect(0);
 
}

//=============================================================================
// Name: Render()
// Desc: Draws the scene
//=============================================================================
VOID Render()
{
    // Clear the backbuffer to a blue color

	UINT uPasses = 0;

	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 
		D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

#ifdef _XBOX
	psEffect->SetTextures(g_pTexture, lpWorkTexture1, lpWorkTexture2, lpHq2xLookupTexture); 
	psEffect->SetVideoInputs(textureWidth,textureHeight,backbufferWidth,backbufferHeight, 0); 
	psEffect->Begin(ScalingEffect::Combine, &uPasses);	 
	psEffect->BeginPass(0);
#endif
	
	g_pd3dDevice->BeginScene();

	//g_pd3dDevice->SetShaderGPRAllocation(0,26,102);
 
	// Render the vertex buffer contents
	//g_pd3dDevice->SetVertexDeclaration( g_pGradientVertexDecl );

#ifndef _XBOX
	g_pd3dDevice->SetTexture(0,g_pTexture);
	g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
#endif
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );
	
#ifdef _XBOX
	psEffect->EndPass();	
	psEffect->End();
#endif

	g_pd3dDevice->EndScene();

    // End the scene

}

void SetFixMode(const char* record)
{
 
	int fixMode = 0;

	if (record)
	{
		char *rec = strupr((char *)record);

		if ((strcmp(rec, "127BF39C") == 0)  // Tsuukai Gameshow - Twisted (JP)
		||  (strcmp(rec, "A7C4EE53") == 0)  // Twisted - The Game Show (US)
		||  (strcmp(rec, "813E41B1") == 0)  // Space Hulk - Vengeance of the Blood Angels (EU-US)
		||  (strcmp(rec, "638812DE") == 0)  // Blood Angels - Space Hulk (JP)
		||  (strcmp(rec, "F3AF1B13") == 0)  // Crash 'n Burn (JP)
		||  (strcmp(rec, "217344B0") == 0)  // Crash 'n Burn (US)
		) fixMode = fixMode | FIX_BIT_TIMING_1;

		if ( (strcmp(rec, "260DC12D") == 0)  // Twisted - The Game Show (EU)
		|| (strcmp(rec, "1757408B") == 0) // Seal of the pharaoh
		|| (strcmp(rec, "A4B2B740") == 0)// Seal of the pharaoh(jp)
		) fixMode = fixMode | FIX_BIT_TIMING_4;

		if ((strcmp(rec, "C39E4193") == 0) /* Phoenix 3*/
		|| (strcmp(rec, "6A4523F3") == 0) // Immercenary
		|| (strcmp(rec, "DBB419FA") == 0) // Street Fighter 2
		|| (strcmp(rec, "7340307E") == 0) // Street Fighter 2
		|| (strcmp(rec, "5282889F") == 0) // Street Fighter 2
		|| (strcmp(rec, "07C32F10") == 0) // Street Fighter 2
		|| (strcmp(rec, "870F95CD") == 0) // Olympic summer games
		) fixMode = fixMode | FIX_BIT_TIMING_5;

		if ((strcmp(rec, "6A7B3EAE") == 0)  // Wing Commander 3 disc 1
		|| (strcmp(rec, "A407D519") == 0) // Wing Commander 3 disc 1
		|| (strcmp(rec, "074BDE30") == 0) // Wing Commander 3 disc 2
		|| (strcmp(rec, "3EA4804D") == 0) // Wing Commander 3 disc 2
		|| (strcmp(rec, "7DDF4025") == 0) // Wing Commander 3 disc 3
		|| (strcmp(rec, "1A35B4B3") == 0) // Wing Commander 3 disc 3
		|| (strcmp(rec, "1E8D4F45") == 0) // Wing Commander 3 disc 4
		|| (strcmp(rec, "5DA4FF7F") == 0)  // Wing Commander 3 disc 4
		) fixMode = fixMode | FIX_BIT_TIMING_6;

		if ((strcmp(rec, "B347EE6D") == 0) // Scramble Cobra (demo) (JP)
		|| (strcmp(rec, "6A3AE6B5") == 0) // Scramble Cobra (EU)
		|| (strcmp(rec, "99670115") == 0) // Scramble Cobra (JP)
		|| (strcmp(rec, "9B87E5D7") == 0) // Scramble Cobra (US)
		) fixMode = fixMode | FIX_BIT_TIMING_3;

		if ((strcmp(rec, "BD2BC660") == 0) // Lost Eden (US)
		|| (strcmp(rec, "EBE0915C") == 0) // Novastorm (US)
		|| (strcmp(rec, "1F059B8F") == 0) // Nova-Storm (JP)
		|| (strcmp(rec, "1A370EBA") == 0) // Microcosm (JP)
		|| (strcmp(rec, "B35C911D") == 0) // Microcosm (US)
		) fixMode = fixMode | FIX_BIT_TIMING_2;

		if ((strcmp(rec, "ED705E42") == 0) // The Horde (US)
		|| (strcmp(rec, "8742A80C") == 0) // The Horde (JP)
		|| (strcmp(rec, "3D1B793D") == 0) // The Horde (EU-US)
		) fixMode = fixMode | FIX_BIT_TIMING_7;

		if ((strcmp(rec, "2AABA5B9") == 0) // Samurai Shodown (EU-US)
		|| (strcmp(rec, "BF61BB32") == 0)  // Samurai Shodown (JP)
		) fixMode = fixMode | FIX_BIT_GRAPHICS_STEP_Y;


		g_GameConsole->SetFixMode(fixMode);
	}


}

//=============================================================================
// Name: Console_FrameDone()
// Desc: Copies the fredooCore frame data to our frame buffer. Disabling gets
//		 us 1 more fps. sigh.
//=============================================================================
void Console_FrameDone()
{
	// TODO: Might get some more performance if we copy the frame data
	// directly to the texture. Right now the core seems to be the issue
	// performance wise.

	g_GameConsole->m_IsFrameDone = 1;

	VDLFrame* fPtr = (VDLFrame*)g_GameConsole->GetCurrentFrame();

	int* destPtr = g_DispBuffer;
	for (int line = 0; line < emuHeight; ++line)
	{
		VDLLine* linePtr = (VDLLine*)&(fPtr->lines[line]);
		short* srcPtr = (short*)linePtr;

		for (int pix = 0; pix < emuWidth; ++pix)
		{
			int blue = linePtr->xCLUTG[(*srcPtr) & 0x1F];
			int green = linePtr->xCLUTG[((*srcPtr) >> 5) & 0x1F];
			int red = linePtr->xCLUTR[(*srcPtr) >> 10 & 0x1F];

			*destPtr = ( red << 16 | green << 8 | blue );
			destPtr++;
			srcPtr++;
		}
	}
};
 
//=============================================================================
// Name: main()
// Desc: The application's entry point
//=============================================================================
void __cdecl main()
{

	CSimpleIniA ini;
	SI_Error rc;
	ini.SetUnicode(true);
	ini.SetMultiKey(true);
	ini.SetMultiLine(true);

	rc = ini.LoadFile("d:\\tmp.lce");

	if (rc < 0)
	{
		OutputDebugString("Failed to load ");
		OutputDebugString("tmp.lce");
		OutputDebugString("\n");
		g_GameName = "";
	}
	else
	{
		g_GameName = ini.GetValue("LAUNCHER", "ROM", 0, false);
		g_GameMD5 = ini.GetValue("LAUNCHER", "MD5", 0, false);
		g_GameID = ini.GetValue("LAUNCHER", "ID", 0, false);
 
		bool ret = DeleteFile("d:\\tmp.lce");
	}
	
	//g_GameName = "game:\\roms\\The Need for Speed.iso";
	
	// Map console's FrameDone function to Console_FrameDone.
	g_GameConsole->FrameDone = &Console_FrameDone;

	// Create our frame buffer.
	g_DispBuffer = new int[320*240]; 

    // Initialize Direct3D.
    if( FAILED( InitD3D() ) )
        return;

	// Initialize the vertex buffer.
	InitVB();

	// Initialize timers.
	InitTime();

	// Initialize sound.
	InitSound();

	DoConsoleStart(false);

	if (g_GameID)
	{
		SetFixMode(g_GameID);
	}

	running = 1;

    while( running )
    {
		// What time is it?
		//UpdateTime();

		// Update the world
        Update();   

		//Console_FrameDone();
 
        // Render the scene
        Render();
		
        // Present the backbuffer contents to the display
        g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    }
	
	// Cleanup all created resources.
	Cleanup();

	XLaunchNewImage( "d:\\3doxLauncher.xex", NULL );
}


#ifndef _XBOX

LRESULT CALLBACK MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
 
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        "3dox", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    hWnd = CreateWindow( "3dox", "3dox",
                              WS_OVERLAPPEDWINDOW, 0, 0, 1280, 720,
                              NULL, NULL, wc.hInstance, NULL );


	CSimpleIniA ini;
	SI_Error rc;
	ini.SetUnicode(true);
	ini.SetMultiKey(true);
	ini.SetMultiLine(true);

	rc = ini.LoadFile("T:\\tmp.lce");

	if (rc < 0)
	{
		OutputDebugString("Failed to load ");
		OutputDebugString("tmp.lce");
		OutputDebugString("\n");
		g_GameName = "";
	}
	else
	{
		g_GameName = ini.GetValue("LAUNCHER", "ROM", 0, false);
		bool ret = DeleteFile("T:\\tmp.lce");
	}

	// Map console's FrameDone function to Console_FrameDone.
	g_GameConsole->FrameDone = &Console_FrameDone;

	// Create our frame buffer.
	g_DispBuffer = new int[320*240]; 

	// Initialize Direct3D
    if( SUCCEEDED( InitD3D() ) )
    {
        // Show the window
        ShowWindow( hWnd, SW_SHOWDEFAULT );
      

		// Initialize the vertex buffer.
		InitVB();

		// Initialize timers.
		InitTime();

		// Initialize sound.
		InitSound();

		DoConsoleStart(false);


		// Enter the message loop
        MSG msg;
        ZeroMemory( &msg, sizeof( msg ) );
        while( msg.message != WM_QUIT )
        {
            if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else
			{
                //UpdateTime();

				// Update the world
				Update();   

				// Render the scene
				Render();
		
				// Present the backbuffer contents to the display
				g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			}
        }

        
    }

 

	// Cleanup all created resources.
	Cleanup();
 
    UnregisterClass( "3dox", wc.hInstance );
    return 0;
}

 

#endif


void DisplayDebug(IDirect3DSurface9* surface)
{
 
}

void DisplayText( IDirect3DSurface9* surface, char* str, int x, int y, DWORD colour )
{
 
    WCHAR szbuff[200] = {0};
    swprintf(szbuff, L"%S", str);

	OutputDebugString(str);
}