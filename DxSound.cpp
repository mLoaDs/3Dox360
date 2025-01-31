/*  Copyright (C) 2005-2007 Theo Berkau

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

#include "DxSound.h"

//SoundInterface_struct SNDDIRECTX = {
//SNDCORE_DIRECTX,
//"Direct Sound Interface",
//SNDDXInit,
//SNDDXDeInit,
//SNDDXReset,
//SNDDXChangeVideoFormat,
//SNDDXUpdateAudio,
//SNDDXGetAudioSpace,
//SNDDXMuteAudio,
//SNDDXUnMuteAudio,
//SNDDXSetVolume
//};




//----#define NUMSOUNDBLOCKS  4
#define NUMSOUNDBLOCKS  4 // set to 2 and no annoying sound at startup 
static u16 *stereodata16;
static u32 soundlen;
static u32 soundoffset=0;
static u32 soundbufsize;
static LONG soundvolume;
static int issoundmuted;

static IXAudio2* pXAudio2 = NULL;
static IXAudio2MasteringVoice* pMasterVoice = NULL;
static IXAudio2SourceVoice* pSourceVoice = NULL;
static XAUDIO2_BUFFER sAudioBuffer;
static XAUDIO2_VOICE_STATE vState;
 
INT32 nAudVolume = 10000;				// Sound volume (% * 100)
INT32 nAudSegCount = 2;					// Segs in the pdsbLoop buffer
INT32 nAudSegLen = 0;					// Seg length in samples (calculated from Rate/Fps)
INT32 nAudAllocSegLen = 0;
UINT8 bAudOkay = 0;						// True if DSound was initted okay
static UINT8 bAudPlaying = 0;					// True if the Loop buffer is playing
 
INT16* nAudNextSound = NULL;			// The next sound seg we will add to the sample loop

 
static UINT32 nAudActive = 0;


BYTE* pAudioBuffers = NULL;
int currentBuffer = 0;

static int cbLoopLen = 0;					// Loop length (in bytes) calculated
static int nXAudio2Fps = 0;					// Application fps * 100
static float nXAudio2Vol = 1.0f;

//////////////////////////////////////////////////////////////////////////////


struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
	HANDLE hBufferEndEvent;

	StreamingVoiceContext() {
		hBufferEndEvent = NULL;
		hBufferEndEvent = CreateEvent( NULL, FALSE, FALSE, NULL);
	}
	~StreamingVoiceContext() {
		CloseHandle(hBufferEndEvent);
		hBufferEndEvent = NULL;
	}

	STDMETHOD_(void, OnBufferEnd) (void *pBufferContext) {
		SetEvent(hBufferEndEvent);
	}

	// dummies:
	STDMETHOD_(void, OnVoiceProcessingPassStart) (UINT32 BytesRequired) {}
	STDMETHOD_(void, OnVoiceProcessingPassEnd) () {}
	STDMETHOD_(void, OnStreamEnd) () {}
	STDMETHOD_(void, OnBufferStart) (void *pBufferContext) {}
	STDMETHOD_(void, OnLoopEnd) (void *pBufferContext) {}
	STDMETHOD_(void, OnVoiceError) (void *pBufferContext, HRESULT Error) {};
};
StreamingVoiceContext voiceContext;

int DxSound::SNDDXInit()
{

	HRESULT hr;

	nXAudio2Fps = 6000;

	// Calculate the Seg Length and Loop length (round to nearest sample)
	nAudSegLen = (44100 * 100 + (nXAudio2Fps >> 1)) / nXAudio2Fps;
	nAudAllocSegLen = nAudSegLen  ;
	cbLoopLen = (nAudSegLen * nAudSegCount) << 2;

	if ( FAILED(hr = XAudio2Create( &pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
		return hr;


// Create a mastering voice
	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice, XAUDIO2_DEFAULT_CHANNELS, 44100, 0, 0, NULL))) {
		return 1;
	}

	// Make the format of the sound
	WAVEFORMATEX wfx;
	memset(&wfx, 0, sizeof(wfx));
	wfx.cbSize = sizeof(wfx);
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;											// stereo
	wfx.nSamplesPerSec = 44100;									// sample rate
	wfx.wBitsPerSample = 16;									// 16-bit
	wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;	// bytes per sample
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	// Create the source voice
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL))) {
		return 1;
	}

	 
	// manipulated while the voice plays from it
	pAudioBuffers = (BYTE *)malloc(8404);
	 
	currentBuffer = 0;

	memset(pAudioBuffers, 0, cbLoopLen);

	pSourceVoice->Start(0);

	bAudOkay = 1;

   return 0;
}

//////////////////////////////////////////////////////////////////////////////

void DxSound::SNDDXDeInit()
{
   DWORD status=0;

}

//////////////////////////////////////////////////////////////////////////////

int DxSound::SNDDXReset()
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

int DxSound::SNDDXChangeVideoFormat(int vertfreq)
{

   return 0;
}

//////////////////////////////////////////////////////////////////////////////
 
static int cnt = 0;
void DxSound::SNDDXUpdateAudio(u32 *leftchanbuffer, u32 *rightchanbuffer, u32 num_samples)
{
	if (!pSourceVoice) {
		return ;
	}
 
	 
	 u32 sample = (*leftchanbuffer) | (*rightchanbuffer);

	 
	// copy & protect the audio data in own memory area while playing it
	memcpy(&pAudioBuffers[cnt], &sample,4);

	cnt+=4;

	if (cnt == 8404)
	{
	sAudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
	sAudioBuffer.AudioBytes = 8404;
	sAudioBuffer.pAudioData = &pAudioBuffers[0];
	HRESULT hr = pSourceVoice->SubmitSourceBuffer(&sAudioBuffer); // send buffer to queue
	cnt = 0;

		currentBuffer++;
		currentBuffer %= (6);
	}
 

	
}

//////////////////////////////////////////////////////////////////////////////

u32 DxSound::SNDDXGetAudioSpace()
{
 
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void DxSound::SNDDXMuteAudio()
{
 
}

//////////////////////////////////////////////////////////////////////////////

void DxSound::SNDDXUnMuteAudio()
{
 
}

//////////////////////////////////////////////////////////////////////////////

void DxSound::SNDDXSetVolume(int volume)
{
 
}

//////////////////////////////////////////////////////////////////////////////

