#pragma once
//#include "uiInfo.h"
#include "basefile.h"
class packdt
{
public:
	packdt(std::string pw, std::string imei){
		memset(fpt.hd.hdcon, ' ', sizeof(fpt.hd));
		fpt.hd.efID[0] = '1';
		memcpy(fpt.hd.pw, pw.c_str(), min(pw.length(), sizeof fpt.hd.pw));
		memset(imeiid, ' ', sizeof(imeiid));
		memcpy(imeiid, imei.c_str(), min(imei.length(), sizeof imeiid));
	}
	~packdt(){}

public:
	void set(char* devid, char* pw){
		memcpy(imeiid, devid, sizeof(imeiid));
		memcpy(fpt.hd.pw, pw, sizeof(fpt.hd.pw));
	}

	///构建第一次连接的时候需要报的信息：
	bool sayname(char* pbuf){
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
		//pbuf[sizeof(packdt)] = '#';
		return true;
	}
	//构建心跳数据包
	void alivedt(char* pbuf){
		memcpy(pbuf, "00", 2);
	}
	//

	bool input(char* pbuf, int nlen){
		if (nlen < sizeof(fpart))
			return true;
		memcpy(&fpt, pbuf, sizeof(fpart));
		//应该吃进的是811数据
		int ilen = atoi(fpt.len);//目前来说ilen似乎没什么用
		if (ilen > 0){
			vpart.resize(nlen - sizeof fpt,0);
			//char buf[26];
			//memcpy(buf, (void)&pbuf[23], 26);
			memcpy((void*)vpart.c_str(), (void*)&pbuf[sizeof(fpt)], nlen - sizeof fpt);
			return true;
		} else 
			return false;
	}
	std::string getprintdt(){
		int npos = vpart.find("###");
		if (npos == std::string::npos)
			return "";
		std::string str = vpart.substr(npos+3);
		return str;
	}
	bool output(char* pbuf, int len){
		if (memcmp(fpt.exc, "810", 3) == 0){//自己刚刚构建了介绍自己的数据给服务器
			
		} else if (memcmp(fpt.exc, "811", 3) == 0){//服务器返回了打印数据，要构建反馈数据
			memset(fpt.len, ' ', sizeof(fpt.len));
			memcpy(fpt.len, "17 ", 2);
			memcpy(fpt.exc, "815", 3);
			memcpy(pbuf, this, sizeof(fpart));
			int index = sizeof(fpart);
			pbuf[index++] = '1';
			memcpy(&pbuf[index], imeiid, sizeof(imeiid));
			index += sizeof(imeiid);
			pbuf[index] = '0';//表示打印成功
			if (vpart.find("###") == std::string::npos)
				return false;//this time, pbuf is dirty
			std::string msgno = vpart.substr(6, vpart.find("###") - 6 - 1);//得到消息号
			memcpy(&pbuf[++index], msgno.c_str(), 4);
			memcpy(&pbuf[index+4], "#", 1);
		} else if (memcmp(fpt.exc, "815", 3) == 0) {
			
		}
		return true;
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
	std::string vpart;  //可变部分
	char imeiid[15];
};
