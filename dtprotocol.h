#pragma once
//#include "uiInfo.h"
#include "basefile.h"

class packdt
{
public:
	packdt(){
		fpt.hd.efID[0] = '1';
		
	}
	~packdt(){}

public:
	void set(const char* imei, const char* pw){
		memset(fpt.hd.hdcon, ' ', sizeof(fpt.hd));
		fpt.hd.efID[0] = '1';
		memcpy(fpt.hd.pw, pw, min(sizeof(fpt.hd.pw), strlen(pw)));
		memset(imeiid, ' ', sizeof(imeiid));
		memcpy(imeiid, imei, min(sizeof(imeiid), strlen(imei)));
	}

	///构建第一次连接的时候需要告诉服务器自己是谁
	bool selfintr(char* pbuf){
		memset(pbuf, ' ', sizeof(fpart));
		fpt.hd.efID[0] = '1';
		char* length = "15";
		memset(fpt.len, ' ', sizeof(fpt.len));
		memcpy(fpt.len, length, 2);
		memcpy(fpt.exc, "810", 3);
		memcpy(pbuf, this, sizeof(fpart));
		memset(&pbuf[sizeof(fpart)], ' ', sizeof(imeiid));
		memcpy(&pbuf[sizeof(fpart)], imeiid, sizeof(imeiid));
		pbuf[sizeof(fpart) + sizeof(imeiid)] = '#';
		return true;
	}
	//构建心跳数据包
	void alivedt(char* pbuf){
		memcpy(pbuf, "00", 2);
	}
	/**
	pbuf : 要解析的数据，也是客户端发出的810交易码请求服务器之后返回的811数据
	nlen : pbuf的长度
	返回值: 解析得到要打印的字符串
	*/
	std::string parseprtdt(const char* pbuf, int nlen){
		if (nlen < sizeof(fpart))
			return "";
		memcpy(&fpt, pbuf, sizeof(fpart));
		//应该吃进的是811数据
		std::string strtmp;
		strtmp.resize(nlen - sizeof fpt,0);
		memcpy((void*)strtmp.c_str(), (void*)&pbuf[sizeof(fpt)], nlen - sizeof(fpt));
		int npos = strtmp.find("###");
		if (npos == std::string::npos)
			return "";
		std::string str = strtmp.substr(npos+3);
		return str;
	}
	/**
	msgno  : 打印的消息编号 
	suc    : 打印是否成功
	返回值 : 返回交易码为815的数据
	*/
	std::string echodt(const char* msgno, bool suc){
		//memcpy(&fpt, pbuf, sizeof(fpt.hd));//得到打印机器类型，以及打印密码
		//memset(fpt.len, ' ', sizeof(fpt.len));
		//memcpy(fpt.len, "17", sizeof(fpt.len));
		////memset(imeiid, ' ', sizeof(imeiid));//imeiid一经初始化设定之后，不再修改
		//memcpy(&fpt.exc, "815", 3);
		//下面解析交易码811之后的数据
		std::string str;
		for (int i = 0; i < sizeof(fpt.hd.hdcon); ++i){
			str += fpt.hd.hdcon[i];
		}
		str += "17        ";
		str += "815";
		str += imeiid;

		if (suc)  str += "0";
		else      str += "1";

		for (int i = 0; i < sizeof(msgno); ++i){
			str += msgno[i];
		}
		str += "#";
		return str;
	}
	/**
	pbuf: 是接收到要打印数据811交易码时，对应的服务器返回的数据
	nlen : 是pbuf的长度
	返回值 : 得到811数据后，返回打印消息编号
	*/
	std::string getmsgno(const char* pbuf, int nlen){
		if (nlen < sizeof(fpart))
			return "";
		memcpy(&fpt, pbuf, sizeof(fpart));
		//应该吃进的是811数据
		std::string strtmp;
		strtmp.resize(nlen - sizeof fpt,0);
		memcpy((void*)strtmp.c_str(), (void*)&pbuf[sizeof(fpt)], nlen - sizeof(fpt));
		int pos = strtmp.find(":");
		if (pos == std::string::npos)
			return "";
		
		std::string str = strtmp.substr(6, pos - 6);
		return str;
	}

private:
	typedef struct Type_fpart{
		union head{//包头，不足10位的后补空格
			struct{
				char efID[1];//终端厂家标识
				char pw[9];  //密码
			};
			char hdcon[10]; //头
		}hd;
		char len[10];    //包长度，不足10位，后补空格
		char exc[3];     //交易码
	} fpart;  //固定部分

	fpart fpt;
	char imeiid[30];
};
