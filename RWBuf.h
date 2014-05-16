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
	std::string msgno;
	bool        bPrinted;
	bool        bSuc;
	bool        bReplied;
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
	bool readBuf(PrintItem& strData, bool handled){
		AutoCS acs(&mCS);
		if (mBuf.empty())
			return false;
		//�����ҵ���һ��δ��ӡ������
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (iter->bPrinted == handled){
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
			if (strData.msgno == iter->msgno){//ֻ����δ��ӡ�����ݱ��滻Ϊ�Ѵ�ӡ������
				if (bWriteBack){
					iter->bPrinted = strData.bPrinted;
					iter->bSuc = strData.bSuc;
				}
				return true;
			}
		}
		mBuf.push_back(strData);
		if (mBuf.size() > 1000)
			cleandt();
		return true;
	}
	void setReplied(std::string& msgno){
		AutoCS acs(&mCS);
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (msgno == iter->msgno){//ֻ����δ��ӡ�����ݱ��滻Ϊ�Ѵ�ӡ������
				//mBuf.erase(iter);
				iter->bReplied = true;
				return ;
			}
		}
	}
private:
	bool cleandt(){
		AutoCS acs(&mCS);
		std::list<PrintItem>::iterator iter = mBuf.begin();
		for (; iter != mBuf.end(); ++iter){
			if (iter->bPrinted){
				iter = mBuf.erase(iter);
			}
		}
		return true;
	}
	//����������ܴ�Ļ���Ҫ֧�����ݵı��ػ�����
	//bool serialization();
private:
	CRITICAL_SECTION       mCS;
	std::list<PrintItem> mBuf;
};

