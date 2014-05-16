#pragma once

#include <list>
#include <string>

#include "AutoCS.h"


struct PrintItem 
{
	PrintItem(){
		bPrinted = bSuc = false;
	}
	PrintItem(const PrintItem& obj){
		cpy(obj);
	}
	const PrintItem operator = (const PrintItem& obj){
		if (this != &obj){
			cpy(obj);
		}
		return *this;
	}
	void cpy(const PrintItem& obj){
		strRawData = obj.strRawData;
		msgno = obj.msgno;
		bPrinted = obj.bPrinted;
		bSuc = obj.bSuc;
	}
	std::string strRawData;
	std::string msgno;
	bool        bPrinted;
	bool        bSuc;
};


//����һ���洢POD���ݣ���������std::listҪ����������͵����ݻ��棬֧�ֶ��̰߳�ȫ��
class RWList_ms{
public:
	RWList_ms(){
		InitializeCriticalSection(&mCS);
	}
	~RWList_ms(){
		DeleteCriticalSection(&mCS);
	}
public:
	bool readBuf(PrintItem& strData){
		AutoCS acs(&mCS);
		if (mBuf.empty())
			return false;
		strData = mBuf.front();
		//mBuf.pop_front();
		return true;
	}
	bool writeBuf(PrintItem& strData){
		AutoCS acs(&mCS);
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (strData.msgno == iter->msgno){
				//*iter = strData;
				iter->bPrinted = strData.bPrinted;
				iter->bSuc = strData.bSuc;
				return true;
			}
		}
		mBuf.push_back(strData);
		return true;
	}
	bool pop(){
		AutoCS acs(&mCS);
		mBuf.pop_front();
		return true;
	}
	//����������ܴ�Ļ���Ҫ֧�����ݵı��ػ�����
	//bool serialization();
private:
	CRITICAL_SECTION       mCS;
	std::list<PrintItem> mBuf;
};

