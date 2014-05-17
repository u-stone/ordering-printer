#pragma once

#include <list>
#include <string>

#include "AutoCS.h"


struct PrintItem 
{
	PrintItem(){
		bPrinted = bSuc = bReplied = false;
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
		bReplied = obj.bReplied;
	}
	std::string strRawData;
	std::string msgno;         //唯一标示符
	bool        bPrinted;
	bool        bSuc;
	bool        bReplied;
};


//这是一个存储POD数据，或者满足std::list要求的数据类型的数据缓存，支持多线程安全。
class RWList_ms{
public:
	RWList_ms(){
		InitializeCriticalSection(&mCS);
	}
	~RWList_ms(){
		DeleteCriticalSection(&mCS);
	}
public:
	bool getData2Print(PrintItem& strData){
		return readBuf(strData, false, false);
	}
	bool getData2Reply(PrintItem& strData){
		return readBuf(strData, true, false);
	}
	bool pushData(PrintItem& strData){
		return writeBuf(strData, false);
	}
	bool updateData(PrintItem& strData){
		return writeBuf(strData, true);
	}
private:
	bool readBuf(PrintItem& strData, bool bPrinted, bool bReplied){
		AutoCS acs(&mCS);
		if (mBuf.empty())
			return false;
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (iter->bPrinted == bPrinted && iter->bReplied == bReplied){
				strData = *iter;
				return true;
			}
		}
		if (mBuf.size() > 1000)
			cleandt();
		return false;
	}
	bool writeBuf(PrintItem& strData, bool bWriteBack){
		AutoCS acs(&mCS);
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (strData.msgno == iter->msgno){//只能让未打印的数据被替换为已打印的数据
				if (bWriteBack){
					iter->bPrinted = strData.bPrinted;
					iter->bSuc = strData.bSuc;
					iter->bReplied = strData.bReplied;
				}
				return true;
			}
		}
		mBuf.push_back(strData);
		if (mBuf.size() > 1000)
			cleandt();
		return true;
	}
// 	void setReplied(std::string& msgno){
// 		AutoCS acs(&mCS);
// 		std::list<PrintItem>::iterator iter = mBuf.begin();
// 		for (; iter != mBuf.end(); ++iter){
// 			if (msgno == iter->msgno){//只能让未打印的数据被替换为已打印的数据
// 				//mBuf.erase(iter);
// 				iter->bReplied = true;
// 				return ;
// 			}
// 		}
// 	}
private:
	bool cleandt(){
		AutoCS acs(&mCS);
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (iter->bReplied){//已回应的数据(一定是已经打印的数据)，要删除掉
				iter = mBuf.erase(iter);
			}
		}
		return true;
	}
	//如果数据量很大的话需要支持数据的本地化缓存
	//bool serialization();
private:
	CRITICAL_SECTION       mCS;
	std::list<PrintItem> mBuf;
};

