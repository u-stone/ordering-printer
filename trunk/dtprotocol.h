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

	///������һ�����ӵ�ʱ����Ҫ���߷������Լ���˭
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
	//�����������ݰ�
	void alivedt(char* pbuf){
		memcpy(pbuf, "00", 2);
	}
	/**
	pbuf : Ҫ���������ݣ�Ҳ�ǿͻ��˷�����810���������������֮�󷵻ص�811����
	nlen : pbuf�ĳ���
	����ֵ: �����õ�Ҫ��ӡ���ַ���
	*/
	std::string parseprtdt(const char* pbuf, int nlen){
		if (nlen < sizeof(fpart))
			return "";
		memcpy(&fpt, pbuf, sizeof(fpart));
		//Ӧ�óԽ�����811����
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
	msgno  : ��ӡ����Ϣ��� 
	suc    : ��ӡ�Ƿ�ɹ�
	����ֵ : ���ؽ�����Ϊ815������
	*/
	std::string echodt(const char* msgno, bool suc){
		//memcpy(&fpt, pbuf, sizeof(fpt.hd));//�õ���ӡ�������ͣ��Լ���ӡ����
		//memset(fpt.len, ' ', sizeof(fpt.len));
		//memcpy(fpt.len, "17", sizeof(fpt.len));
		////memset(imeiid, ' ', sizeof(imeiid));//imeiidһ����ʼ���趨֮�󣬲����޸�
		//memcpy(&fpt.exc, "815", 3);
		//�������������811֮�������
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
	pbuf: �ǽ��յ�Ҫ��ӡ����811������ʱ����Ӧ�ķ��������ص�����
	nlen : ��pbuf�ĳ���
	����ֵ : �õ�811���ݺ󣬷��ش�ӡ��Ϣ���
	*/
	std::string getmsgno(const char* pbuf, int nlen){
		if (nlen < sizeof(fpart))
			return "";
		memcpy(&fpt, pbuf, sizeof(fpart));
		//Ӧ�óԽ�����811����
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
	char imeiid[30];
};
