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

	///������һ�����ӵ�ʱ����Ҫ������Ϣ��
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
	//�����������ݰ�
	void alivedt(char* pbuf){
		memcpy(pbuf, "00", 2);
	}
	//

	bool input(char* pbuf, int nlen){
		if (nlen < sizeof(fpart))
			return true;
		memcpy(&fpt, pbuf, sizeof(fpart));
		//Ӧ�óԽ�����811����
		int ilen = atoi(fpt.len);//Ŀǰ��˵ilen�ƺ�ûʲô��
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
		if (memcmp(fpt.exc, "810", 3) == 0){//�Լ��ոչ����˽����Լ������ݸ�������
			
		} else if (memcmp(fpt.exc, "811", 3) == 0){//�����������˴�ӡ���ݣ�Ҫ������������
			memset(fpt.len, ' ', sizeof(fpt.len));
			memcpy(fpt.len, "17 ", 2);
			memcpy(fpt.exc, "815", 3);
			memcpy(pbuf, this, sizeof(fpart));
			int index = sizeof(fpart);
			pbuf[index++] = '1';
			memcpy(&pbuf[index], imeiid, sizeof(imeiid));
			index += sizeof(imeiid);
			pbuf[index] = '0';//��ʾ��ӡ�ɹ�
			if (vpart.find("###") == std::string::npos)
				return false;//this time, pbuf is dirty
			std::string msgno = vpart.substr(6, vpart.find("###") - 6 - 1);//�õ���Ϣ��
			memcpy(&pbuf[++index], msgno.c_str(), 4);
			memcpy(&pbuf[index+4], "#", 1);
		} else if (memcmp(fpt.exc, "815", 3) == 0) {
			
		}
		return true;
	}
private:
	typedef struct Type_fpart{
		union head{//��ͷ������10λ�ĺ󲹿ո�
			struct{
				char efID[1];//�ն˳��ұ�ʶ
				char pw[9];  //����
			};
			char hdcon[10]; //ͷ
		}hd;
		char len[10];    //�����ȣ�����10λ���󲹿ո�
		char exc[3];     //������
	} fpart;  //�̶�����
	fpart fpt;
	std::string vpart;  //�ɱ䲿��
	char imeiid[15];
};
