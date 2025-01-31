/*  Copyright 2006 Theo Berkau

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef _DX_SOUND_H_
#define _DX_SOUND_H_
#define SNDCORE_DIRECTX 2
 
#include <math.h>

#ifdef _XBOX
#include <xaudio2.h>
#else
#include <windows.h>
#endif

typedef unsigned char u8;
typedef unsigned short u16;

typedef signed char s8;
typedef signed short s16;

typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef unsigned __int32 pointer;

typedef signed __int32 s32;
typedef signed __int64 s64;

class DxSound
{
public:
int SNDDXInit();
void SNDDXDeInit();
int SNDDXReset();
int SNDDXChangeVideoFormat(int vertfreq);
void DxSound::SNDDXUpdateAudio(u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples);
u32 SNDDXGetAudioSpace();
void SNDDXMuteAudio();
void SNDDXUnMuteAudio();
void SNDDXSetVolume(int volume);
 
};
//extern SoundInterface_struct SNDDIRECTX;
#endif
