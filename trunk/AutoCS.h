#pragma once
//#include <Windows.h>

class AutoCS
{
public:
	AutoCS(CRITICAL_SECTION* cs){
		mpCS = cs;
		EnterCriticalSection(mpCS);
	}
	~AutoCS(){
		LeaveCriticalSection(mpCS);
	}
private:
	CRITICAL_SECTION *mpCS;
};