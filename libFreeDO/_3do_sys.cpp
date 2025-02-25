/*
	3DOplay sources v1.7.3 based on FreeDOcore
	3doplay.do.am
	Developer: Viktor Ivanov
	Any uses of the 3DOplay sources or any other material published by Viktor Ivanov have to be accompanied with full credits.
All rights reserved.
*/
/*
  www.freedo.org
The first and only working 3DO multiplayer emulator.

The FreeDO licensed under modified GNU LGPL, with following notes:

*   The owners and original authors of the FreeDO have full right to develop closed source derivative work.
*   Any non-commercial uses of the FreeDO sources or any knowledge obtained by studying or reverse engineering
    of the sources, or any other material published by FreeDO have to be accompanied with full credits.
*   Any commercial uses of FreeDO sources or any knowledge obtained by studying or reverse engineering of the sources,
    or any other material published by FreeDO is strictly forbidden without owners approval.

The above notes are taking precedence over GNU LGPL in conflicting situations.

Project authors:

Alexander Troosh
Maxim Grishin
Allen Wright
John Sammons
Felix Lazarev
*/


#include "3doplay.h"

#include "arm.h"
#include "vdlp.h"
#include "dsp.h"
#include "clio.h"
#include "madam.h"
#include "sport.h"
#include "xbus.h"
#include "DiagPort.h"
#include "quarz.h"

#ifdef _XBOX
#include <xtl.h>
#include <process.h>
#else
#include <windows.h>
#endif

_ext_Interface  io_interface;

extern void* Getp_NVRAM();
extern void* Getp_ROMS();
extern void* Getp_RAMS();
extern UINT8 bAudOkay;

__inline uint32 _bswap(uint32 x)
{
	return (x>>24) | ((x>>8)&0x0000FF00L) | ((x&0x0000FF00L)<<8) | (x<<24);
}

 
#if defined (THREADING)
HANDLE dsploopHandle;

unsigned int __stdcall dsp_thread_func(LPVOID eventHandle)
{
	HANDLE evHandle = (LPVOID *)eventHandle;

	XSetThreadProcessor( GetCurrentThread(), 3 );

	//SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);

	while(1)
	{
		WaitForSingleObject(evHandle,INFINITE);

		if (bAudOkay)
		{
			unsigned int dspVal = _dsp_Loop();		 
			io_interface(EXT_PUSH_SAMPLE,(void*)dspVal);    
		}
	}

}

 
#endif

extern void* _xbplug_MainDevice(int proc, void* data);
int _3do_Init()
{
 unsigned char *Memory;
 unsigned char *rom;

 
	Memory=_arm_Init();

        io_interface(EXT_READ_ROMS,Getp_ROMS());
        rom=(unsigned char*)Getp_ROMS();

#if !defined (BIG_ENDIAN)
	for (int i = (1024*1024*2) - 4; i >= 0; i -= 4)
	{
		int val = *(int *)(rom+i);
		*(int *)(rom+i) = _bswap(val);
	}
#endif
 
	_vdl_Init(Memory+0x200000);   // Visible only VRAM to it
	_sport_Init(Memory+0x200000);  // Visible only VRAM to it
	_madam_Init(Memory);

    _xbus_Init(_xbplug_MainDevice);
    _clio_Init(0x40); // 0x40 for start from  3D0-CD, 0x01/0x02 from PhotoCD ?? (NO use 0x40/0x02 for BIOS test)

 
	_dsp_Init();
	_diag_Init(-1);  // Select test, use -1 -- if d'nt need tests
/*
00	DIAGNOSTICS TEST	(run of test: 1F, 24, 25, 32, 50, 51, 60, 61, 62, 68, 71, 75, 80, 81, 90)
01	AUTO-DIAG TEST		(run of test: 1F, 24, 25, 32, 50, 51, 60, 61, 62, 68,         80, 81, 90)
12	DRAM1 DATA TEST
1A	DRAM2 DATA TEST
1E	EARLY RAM TEST
1F	RAM DATA TEST
22	VRAM1 DATA TEST
24	VRAM1 FLASH TEST
25	VRAM1 SPORT TEST
32	SRAM DATA TEST
50	MADAM TEST
51	CLIO TEST
60	CD-ROM POLL TEST
61	CD-ROM PATH TEST
62	CD-ROM READ TEST	???
63	CD-ROM AutoAdjustValue TEST
67	CD-ROM#2 AutoAdjustValue TEST
68  DEV#15 POLL TEST
71	JOYPAD1 PRESS TEST
75	JOYPAD1 AUDIO TEST
80	SIN WAVE TEST
81	MUTING TEST
90	COLORBAR
F0	CHECK TESTTOOL  ???
F1	REVISION TEST
FF	TEST END (halt)
*/
        _xbus_DevLoad(0,NULL);

        _qrz_Init();


#if defined (THREADING)
	  dsploopHandle = CreateEvent(NULL,0,0,NULL);
	  _beginthreadex(NULL,0,dsp_thread_func,(void *)dsploopHandle,0,NULL);
#endif
 
        return 0;
}


VDLFrame *curr_frame;

void _3do_InternalFrame(int cicles)
{
 
	// comment out scipfame = false calls since we will always use multitask mode
	// this should save us a bunch of cycles

	int line = 0;
	unsigned int cyc = cicles;
 
    _qrz_PushARMCycles(cyc);
    if (_qrz_QueueDSP())
	{
#if defined (THREADING)
		// Signal the DSP thread that it needs to do some work
		SetEvent(dsploopHandle);
#else
		unsigned int dspVal = _dsp_Loop();		 
		io_interface(EXT_PUSH_SAMPLE,(void*)dspVal);    
#endif
	}
	if(_qrz_QueueTimer())_clio_DoTimers();
                
	if(_qrz_QueueVDL())
    {
            line=_qrz_VDCurrLine();
            _clio_UpdateVCNT(line, _qrz_VDHalfFrame());
            if(line==1 ) io_interface(EXT_FRAMETRIGGER_MT,NULL);
            if(line==_clio_v0line())
            {
                _clio_GenerateFiq(1<<0,0);
            }
            if(line==_clio_v1line())
            {
				_clio_GenerateFiq(1<<1,0);
				_madam_KeyPressed((unsigned char*)io_interface(EXT_GETP_PBUSDATA,NULL),(int)io_interface(EXT_GET_PBUSLEN,NULL));

            }
			
    }

	
 
}



__inline void _3do_Frame(VDLFrame *frame, bool __scipframe=false)
{
		int i,cnt=0;

 
        for(i=0;i<(12500000/60);)
        {						    
	            cnt+=_arm_Execute();
				if(cnt>>4){_3do_InternalFrame(cnt);i+=cnt;cnt=0;}
        }
 
}

void _3do_Destroy()
{
        _arm_Destroy();
        _xbus_Destroy();
}

unsigned int _3do_SaveSize()
{
 unsigned int tmp;
        tmp=_arm_SaveSize();
        tmp+=_vdl_SaveSize();
        tmp+=_dsp_SaveSize();
        tmp+=_clio_SaveSize();
        tmp+=_qrz_SaveSize();
        tmp+=_sport_SaveSize();
        tmp+=_madam_SaveSize();
        tmp+=_xbus_SaveSize();
        tmp+=16*4;
        return tmp;
}
void _3do_Save(void *buff)
{
 unsigned char *data=(unsigned char*)buff;
 int *indexes=(int*)buff;

        indexes[0]=0x97970101;
        indexes[1]=16*4;
        indexes[2]=indexes[1]+_arm_SaveSize();
        indexes[3]=indexes[2]+_vdl_SaveSize();
        indexes[4]=indexes[3]+_dsp_SaveSize();
        indexes[5]=indexes[4]+_clio_SaveSize();
        indexes[6]=indexes[5]+_qrz_SaveSize();
        indexes[7]=indexes[6]+_sport_SaveSize();
        indexes[8]=indexes[7]+_madam_SaveSize();
        indexes[9]=indexes[8]+_xbus_SaveSize();

        _arm_Save(&data[indexes[1]]);
        _vdl_Save(&data[indexes[2]]);
        _dsp_Save(&data[indexes[3]]);
        _clio_Save(&data[indexes[4]]);
        _qrz_Save(&data[indexes[5]]);
        _sport_Save(&data[indexes[6]]);
        _madam_Save(&data[indexes[7]]);
        _xbus_Save(&data[indexes[8]]);

}

bool _3do_Load(void *buff)
{
 unsigned char *data=(unsigned char*)buff;
 int *indexes=(int*)buff;
        if(indexes[0]!=0x97970101)return false;

        _arm_Load(&data[indexes[1]]);
        _vdl_Load(&data[indexes[2]]);
        _dsp_Load(&data[indexes[3]]);
        _clio_Load(&data[indexes[4]]);
        _qrz_Load(&data[indexes[5]]);
        _sport_Load(&data[indexes[6]]);
        _madam_Load(&data[indexes[7]]);
        _xbus_Load(&data[indexes[8]]);

        return true;
}

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
extern uint32 *profiling;
extern uint32 *profiling3;
extern int ARM_CLOCK;

void _3do_OnSector(unsigned int sector)
{
        io_interface(EXT_ON_SECTOR,(void*)sector);
}

void _3do_Read2048(void *buff)
{
        io_interface(EXT_READ2048,(void*)buff);
}

unsigned int _3do_DiscSize()
{
        return (unsigned int)io_interface(EXT_GET_DISC_SIZE,NULL);
}

int __tex__scaler = 0;
int HightResMode=0;
int fixmode=0;
int speedfixes=0;
int sf=0;
int sdf=0;
int unknownflag11=0;
int jw=0;
int cnbfix=0;
bool __temporalfixes = false;
 
void* _freedo_Interface(int procedure, void *datum)
{
int line;
        switch(procedure)
        {
         case FDP_INIT:			
				sf=5000000;
				cnbfix=0;
                io_interface=(_ext_Interface)datum;
                return (void*)_3do_Init();
         case FDP_DESTROY:
                _3do_Destroy();
                break;
         case FDP_DO_EXECFRAME:
                _3do_Frame((VDLFrame*)datum);
                break;
         case FDP_DO_EXECFRAME_MT:
                _3do_Frame((VDLFrame*)datum, true);
                break;
         case FDP_DO_FRAME_MT:
                line=0;
                while(line<263)_vdl_DoLineNew(line++,(VDLFrame*)datum);
                ((VDLFrame*)datum)->srcw=320;
                ((VDLFrame*)datum)->srch=240;
                break;
         case FDP_GET_SAVE_SIZE:
                return (void*)_3do_SaveSize();
         case FDP_DO_SAVE:
                _3do_Save(datum);
                break;
         case FDP_DO_LOAD:
                return (void*)_3do_Load(datum);
         case FDP_GETP_NVRAM:
                return Getp_NVRAM();
         case FDP_GETP_RAMS:
                return Getp_RAMS();
         case FDP_GETP_ROMS:
                return Getp_ROMS();
         case FDP_GETP_PROFILE:
                return profiling;
         case FDP_FREEDOCORE_VERSION:
                return (void*)0x10703;
         case FDP_SET_ARMCLOCK:
                ARM_CLOCK=(int)datum;
                break;
		 case FDP_SET_FIX_MODE:
				fixmode=(int)datum;
				break;
         case FDP_SET_TEXQUALITY:
                if(datum)HightResMode=1;
                else HightResMode=0;
                break;
         case FDP_BUGTEMPORALFIX:
                __temporalfixes=(bool)datum;
                break;
		 case FDP_GETP_WRCOUNT:
				return profiling3;
        };

 return NULL;
}
