#pragma once
#include <string>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "dtprotocol.h"

#define SIZE_RECVBUF   1024
#define SIZE_SENDBUF   50
#define SIZE_MAXRECV   3

namespace SOCK_Lsh{

	typedef bool (*rdFinished)(std::string& strData);
	typedef bool (*wrFinished)();

	class dtIO
	{
	public:
		dtIO(std::string pw, std::string imei);
		dtIO(std::string strSvrIP, std::string strSvrPort, std::string pw, std::string imei);
		~dtIO(void);

	public:
		void addOnRDFunc(rdFinished pRDF);
		void addOnWRFunc(wrFinished pWRF);
		int EventDispatch();   //����socket�¼�����Ӧ

	private:
		bool read();
		bool write(bool bFirst = true);//������Ϊtrue����ô��Ҫ�ȴ���ӡ����Ϣ
		bool heartbeat();
	private:
		//inner classes

		class AutoWSADATA{
		public:
			AutoWSADATA();
			~AutoWSADATA();
		};

		class AutoAddrInfo{
		public:
			AutoAddrInfo();
			~AutoAddrInfo();

			void set(std::string ip, std::string port);
			struct addrinfo* get();
		private:
			struct addrinfo* mpAddrs;
		};

		class lcsock{
		public:
			lcsock();
			lcsock(std::string ip, std::string port);
			~lcsock();

			int reconn();  //ʹ��lcsock�����������socket����ʧ�ܵĻ�������ʹ�������������������
		//private:
			SOCKET      mSock;
			std::string mSvrIP;
			std::string mSvrPort;	
			AutoWSADATA  mSockIniter;  //���Ҫ����mAutoAIǰ�棬�Ա�֤��ʼ��˳��
			AutoAddrInfo mAutoAI; 
			WSAEVENT    mEvent;
		};
	private:
		char        mBufRecv[SIZE_RECVBUF];  //���ջ�����
		char        mBufSend[SIZE_SENDBUF];  //���ͻ�����
		packdt      mPack;                   //���ݰ�
		lcsock      mlcsock;   				  //�׽���ά��
		int         mOOT;    				  //��ʱʱ����λms
		rdFinished  rdFunc;  				  //��ȡ���ݽ���֮��Ļص�����
		wrFinished  wrFunc;  				  //д�����ݽ���֮��Ļص�����
	};

}