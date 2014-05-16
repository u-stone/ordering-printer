#pragma once
#include <string>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "Advapi32.lib")


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include "dtprotocol.h"
#include "RWBuf.h"

#define SIZE_RECVBUF   1024
#define SIZE_SENDBUF   50
#define SIZE_MAXRECV   3

namespace SOCK_Lsh{

// 	typedef bool (*rdFinished)();
// 	typedef bool (*wrFinished)();

	class dtIO
	{
	public:
		dtIO(std::string pw, std::string imei);
		dtIO(std::string strSvrIP, std::string strSvrPort, std::string pw, std::string imei);
		~dtIO(void);

	public:
// 		void addOnRDFunc(rdFinished pRDF);
// 		void addOnWRFunc(wrFinished pWRF);
		//using namespace Printer_Lsh;
		void setRdEvent(HANDLE hEvent);
		void setDataBuf(void* pbuf);
		int EventDispatch();   //处理socket事件的响应

	private:
		bool read();
		bool write(bool bFirst = true, char* pdata = 0);//如果这个为true，那么就要先传打印机信息
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

			int reconn();  //使用lcsock对象，如果发现socket连接失败的话，可以使用这个来重新连接网络
		//private:
			SOCKET      mSock;
			std::string mSvrIP;
			std::string mSvrPort;	
			AutoWSADATA  mSockIniter;  //这个要放在mAutoAI前面，以保证初始化顺序
			AutoAddrInfo mAutoAI; 
			WSAEVENT    mEvent;
		};
	private:
		HANDLE      mPrintEvent;             //接收到数据，要通知打印线程激活的事件
		char        mBufRecv[SIZE_RECVBUF];  //接收缓存区
		char        mBufSend[SIZE_SENDBUF];  //发送缓存区
		packdt      mPack;                   //数据包
		lcsock      mlcsock;   				  //套接字维护
		int         mOOT;    				  //超时时长单位ms
		RWList_ms*  mDataBuf;                 //接收到的数据的缓存
// 		rdFinished  rdFunc;  				  //读取数据结束之后的回调函数
// 		wrFinished  wrFunc;  				  //写出数据结束之后的回调函数
	};

}