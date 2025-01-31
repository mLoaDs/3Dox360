// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _XBOX
#include <xtl.h>
#include <process.h>
#include "xbdm.h"
#else
#include <windows.h>
#endif
#include <stdio.h>
#include <exception>

// TODO: reference additional headers your program requires here
#include "libFreeDO\freedoconfig.h"
#include "libFreeDO\3doplay.h"

#include "libFreeDO\arm.h"
#include "libFreeDO\vdlp.h"
#include "libFreeDO\DSP.h"
#include "libFreeDO\Clio.h"
#include "libFreeDO\Madam.h"
#include "libFreeDO\SPORT.h"
#include "libFreeDO\XBUS.h"
#include "libFreeDO\DiagPort.h"
#include "libFreeDO\quarz.h"

#include "3doxExceptions.h"
