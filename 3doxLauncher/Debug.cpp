#include "Debug.h"

CDebug g_debug;

CDebug::CDebug(void)
{
}

CDebug::~CDebug(void)
{
}

void CDebug::Log(const string &msg)
{
	OutputDebugString(msg.c_str());
	OutputDebugString("\n");
}

void CDebug::Log(int &value)
{

ostringstream ss;
ss << value;

OutputDebugString(ss.str().c_str());
OutputDebugString("\n");

}