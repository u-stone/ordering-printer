#pragma once

#include <list>
#include <string>

#include "AutoCS.h"

template <typename T = std::string>
class RWList_ms{
public:
	RWList_ms(){
		InitializeCriticalSection(&mCS);
	}
	~RWList_ms(){
		DeleteCriticalSection(&mCS);
	}
public:
	typedef T _DataType;
	typedef _DataType& _DataType_Ref;
	bool readBuf(_DataType_Ref strData){
		AutoCS acs(mCS);
		if (mBuf.empty())
			return false;
		strData = mBuf.front();
		mBuf.pop_front();
		return true;
	}
	bool writeBuf(_DataType strData){
		AutoCS acs(mCS);
		mBuf.push_back(strData);
		return true;
	}

private:
	CRITICAL_SECTION       mCS;
	std::list<_DataType> mBuf;
};