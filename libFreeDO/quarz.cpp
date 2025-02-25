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
#include "quarz.h"
#include "types.h"
#include "clio.h"
#include "vdlp.h"
#include "madam.h"


int ARM_CLOCK=10000000;
#define SND_CLOCK       44100
//#define NTSC_CLOCK      12270000        //818*500(�����)  //15 ���
//#define PAL_CLOCK       14750000        //944*625(�����)  //15625 ��

 
struct QDatum
{
        uint32 qrz_AccARM;
        uint32 qrz_AccDSP;
        uint32 qrz_AccVDL;
        uint32 qrz_TCount;
        uint32 VDL_CLOCK, qrz_vdlline, VDL_HS,VDL_FS;
};
 

static QDatum quarz;

#include <memory.h>
unsigned int _qrz_SaveSize()
{
        return sizeof(QDatum);
}
void _qrz_Save(void *buff)
{
        memcpy(buff,&quarz,sizeof(QDatum));
}
void _qrz_Load(void *buff)
{
        memcpy(&quarz,buff,sizeof(QDatum));
}

#define qrz_AccARM quarz.qrz_AccARM
#define qrz_AccDSP quarz.qrz_AccDSP
#define qrz_AccVDL quarz.qrz_AccVDL
#define qrz_TCount quarz.qrz_TCount
#define VDL_CLOCK quarz.VDL_CLOCK
#define qrz_vdlline quarz.qrz_vdlline
#define VDL_HS quarz.VDL_HS
#define VDL_FS quarz.VDL_FS

void  _qrz_Init()
{
        qrz_AccVDL=qrz_AccDSP=0;
        qrz_AccARM=0;

        VDL_FS=526;
        VDL_CLOCK=VDL_FS*30;
        VDL_HS=VDL_FS/2;

        qrz_TCount=0;
        qrz_vdlline=0;
 
}

int  _qrz_VDCurrLine()
{
        return qrz_vdlline%(VDL_HS/*+(VDL_HS/2)*/);

}
int  _qrz_VDHalfFrame()
{
        return qrz_vdlline/(VDL_HS);
}

int  _qrz_VDCurrOverline()
{
        return qrz_vdlline;
}

int  _qrz_QueueVDL()
{
	if(qrz_AccVDL>>24)
	{
			qrz_AccVDL-=0x1000000;
			qrz_vdlline++;
			qrz_vdlline%=VDL_FS;
			return 1;
	}
	return 0;
}
int  _qrz_QueueDSP()
{
	if(qrz_AccDSP>>24)
	{
	//if(HightResMode!=0) qrz_AccDSP-=0x1000000/1.3;
	//else
	qrz_AccDSP-=0x1000000;
		return 1;
	}
	return 0;

}

int  _qrz_QueueTimer()
{
	//uint32 cnt=_clio_GetTimerDelay();
	if(qrz_TCount>>24)//=cnt)
	{
			qrz_TCount-=0x1000000;//cnt;
			return 1;
	}
	return 0;
}

void  _qrz_PushARMCycles(unsigned int clks)
{

// dont use timing hacks for now. We lose 5-7 fps due to all the conditional code
// theres gotta be a better way to do this

#if 1
	uint32 arm,cnt;
	int timers=21000000; //��������

	arm=(clks<<24)/(ARM_CLOCK);
	qrz_AccARM+=arm*(ARM_CLOCK);
	if( (qrz_AccARM>>24) != clks )
	{
			arm++;
			qrz_AccARM+=ARM_CLOCK;
			qrz_AccARM&=0xffffff;
	}
       
	qrz_AccVDL+=arm*(VDL_CLOCK);
	qrz_AccDSP+=arm*SND_CLOCK;

	if(_clio_GetTimerDelay())qrz_TCount+=arm*((timers)/(_clio_GetTimerDelay()));//clks<<1;

#else


 uint32 arm,cnt;
  int timers=21000000; //default
 int sp=0;
if(sdf>0) sdf--;
if(sf>0) sf--;
if(unknownflag11>0)unknownflag11--;
 if(speedfixes>0&&speedfixes<0x186A1) {/*sp=0x2DC6C0;*/ speedfixes--;}
 else if(speedfixes>0x186A1&&speedfixes<0x30D41) {/*if(sdf==0)sp=0x4C4B40; */speedfixes--;}
 else if(speedfixes<0) {sp=0x3D0900; speedfixes++;}
 else if(speedfixes>0x30D41) {/*sp=0x249F00;*/ speedfixes--;}
 else if(speedfixes==0x30D41||speedfixes==0x186A1) speedfixes=0;
      if((fixmode&FIX_BIT_TIMING_2)&&sf<=2500000) {sp=0; timers=21000000; if(sf==0)sp=-(0x1C9C380-ARM_CLOCK);}
      if((fixmode&FIX_BIT_TIMING_1)/*&&jw>0*/&&sf<=1500000){/*jw--;*/timers=1000000;sp=-1000000;}
	  if((fixmode&FIX_BIT_TIMING_4)/*&&jw>0*/){/*jw--;*/timers=1000000;sp=-1000000;}
	  if((fixmode&FIX_BIT_TIMING_3)&&(sf>0&&sf<=100000)/*&&jw>0*/){/*jw--;*/timers=900000;}
	  if((fixmode&FIX_BIT_TIMING_5)&&sf==0/*&&jw>0*/){/*jw--;*/timers=1000000;}
	  if((fixmode&FIX_BIT_TIMING_6)/*&&jw>0*/){/*jw--;*/timers=1000000; if(sf<=80000)sp=-23000000;}
	  if(fixmode&FIX_BIT_TIMING_7){sp=-3000000; timers=21000000;}
	  if((sf>0x186A0&&!(fixmode&FIX_BIT_TIMING_2))||((fixmode&FIX_BIT_TIMING_2)&&sf>2500000))sp=-(12200000-ARM_CLOCK);
  if((ARM_CLOCK-sp)<0x2DC6C0)sp=-(0x2DC6C0-ARM_CLOCK);
 
        arm=(clks<<24)/(ARM_CLOCK-sp);
        qrz_AccARM+=arm*(ARM_CLOCK-sp);
        if( (qrz_AccARM>>24) != clks )
        {
                arm++;
                qrz_AccARM+=ARM_CLOCK;
                qrz_AccARM&=0xffffff;
        }
        qrz_AccDSP+=arm*SND_CLOCK;
        qrz_AccVDL+=arm*(VDL_CLOCK);

        //if(Get_madam_FSM()!=FSM_INPROCESS)
        if(_clio_GetTimerDelay())qrz_TCount+=arm*((timers)/(_clio_GetTimerDelay()));//clks<<1;
#endif
}
