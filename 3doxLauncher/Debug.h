#pragma once

#include "Global.h"

class CDebug
{
public:
CDebug();
~CDebug();

void Log(const string &msg);
void Log(int &value);

private:

};

extern CDebug g_debug;
