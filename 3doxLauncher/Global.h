#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <list>
#include <vector>
#include <algorithm>
#ifdef _XBOX
	#include <xtl.h>
	#include <xgraphics.h>
	#include <xboxmath.h>
	#include <AtgApp.h>
	#include <AtgFont.h>
	#include <AtgHelp.h>
	#include <AtgInput.h>
	#include <AtgMesh.h>
	#include <AtgResource.h>
	#include <AtgUtil.h>
	#include <AtgSimpleShaders.h>
	#include <AtgDebugDraw.h>
 
#else
	#pragma comment(lib,"d3d8.lib")
	#pragma comment(lib,"d3dx8.lib")
	#pragma comment(lib,"DxErr8.lib")
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <d3d8.h>
	#include <d3dx8.h>
	#include <dxerr8.h>
#endif


using namespace std;

#define XBUILD "Launcher CE"

typedef unsigned __int8		byte;
typedef unsigned __int16	word;
typedef unsigned __int32	dword;
typedef unsigned __int64	qword;


