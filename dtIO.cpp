#include "stdafx.h"
#include "dtIO.h"
#include <iostream>

using namespace SOCK_Lsh;

dtIO::AutoWSADATA::AutoWSADATA(){
	WSADATA wa;
	int ret = WSAStartup(MAKEWORD(2,2), &wa);
	if (ret != 0)
		throw "WSAStartup Error";
}

dtIO::AutoWSADATA::~AutoWSADATA(){
	WSACleanup();
}

dtIO::AutoAddrInfo::AutoAddrInfo():mpAddrs(0){}

void dtIO::AutoAddrInfo::set(std::string ip, std::string port){
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int ret = getaddrinfo(ip.c_str(), port.c_str(), &hints, &mpAddrs);
	if (ret == SOCKET_ERROR)
		throw "getaddrinfo failed";
}

struct addrinfo* dtIO::AutoAddrInfo::get(){
	return mpAddrs;
}

dtIO::AutoAddrInfo::~AutoAddrInfo(){
	if (mpAddrs) freeaddrinfo(mpAddrs);
	mpAddrs = 0;
}
dtIO::lcsock::lcsock():mSockIniter(){
	mEvent = INVALID_HANDLE_VALUE;
	mAutoAI.set("127.0.0.1", "6666");//default value
	if (!reconn())
		throw "connect to localhost failed";
}
dtIO::lcsock::lcsock(std::string ip, std::string port):mSockIniter(){//mSockIniter must come first to initialize
	mSock = INVALID_SOCKET;
	mEvent = INVALID_HANDLE_VALUE;
	mAutoAI.set(ip, port);
	if (reconn() != 0)
		throw "connect failed";
}

dtIO::lcsock::~lcsock(){
	if (mEvent != INVALID_HANDLE_VALUE){
		WSACloseEvent(mEvent);
		mEvent = INVALID_HANDLE_VALUE;
	}
	WSACleanup();
}

int dtIO::lcsock::reconn(){
	struct addrinfo *addrs, *ptr;
	addrs = mAutoAI.get();
	int ret = 0;
	
	for (ptr = addrs; ptr != NULL; ptr = ptr->ai_next){
		mSock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (mSock == INVALID_SOCKET){
			return 1;
		}

		ret = connect(mSock, ptr->ai_addr, ptr->ai_addrlen);
		if (ret == SOCKET_ERROR){
			ret = WSAGetLastError();
			closesocket(mSock);
			mSock = INVALID_SOCKET;
			continue;
		}

		break;
	}
	if (mSock == SOCKET_ERROR)
		return 2;

	if (mEvent != INVALID_HANDLE_VALUE){
		WSACloseEvent(mEvent);
		mEvent = INVALID_HANDLE_VALUE;
	}
	mEvent = WSACreateEvent();
	ret = WSAEventSelect(mSock, mEvent, FD_READ | FD_WRITE | FD_CLOSE);
	if (ret == SOCKET_ERROR){
		closesocket(mSock);
		return 3;
	}
	return 0;
}


dtIO::dtIO(std::string pw, std::string imei)
{
	mPack.set(imei.c_str(), pw.c_str());
	mPrintEvent = INVALID_HANDLE_VALUE;
	mOOT = 20000;//20s
}

dtIO::dtIO(std::string strSvrIP, std::string strSvrPort, std::string pw, std::string imei)
     :mlcsock(strSvrIP, strSvrPort)
{
	mPack.set(imei.c_str(), pw.c_str());
	mPrintEvent = INVALID_HANDLE_VALUE;
}

dtIO::~dtIO(void)
{
	
}


// void dtIO::addOnRDFunc(rdFinished pRDF)
// {
// 	rdFunc = pRDF;
// }
// void dtIO::addOnWRFunc(wrFinished pWRF)
// {
// 	wrFunc = pWRF;
// }

void dtIO::setRdEvent(HANDLE hEvent)
{
	mPrintEvent = hEvent;
}

void dtIO::setDataBuf(void* pbuf)
{
	mDataBuf = (RWList_ms*)pbuf;
}

int dtIO::EventDispatch()
{
	SOCKET sock_con = mlcsock.mSock;
	WSAEVENT *event = &mlcsock.mEvent;
	char* recvbuf = mBufRecv;
	char* sendbuf = mBufSend;
	int ret = 0;
	int outtimecount = 0;
	int MAX_OUTTIME = 5;
	int MAX_SEC = 3;
	//15s为心跳包发送的时间间隔
	//但是每3s需要检查一下打印数据的结果，反馈给服务器
	while (1){
		DWORD index = WSAWaitForMultipleEvents(1, event, FALSE, MAX_SEC * 1000, FALSE);
		if (index == WSA_WAIT_FAILED){
			closesocket(mlcsock.mSock);
			return -6;
		}

		if (index == WSA_WAIT_TIMEOUT){
			outtimecount++;
			write(false);
			if (outtimecount >= MAX_OUTTIME){
				heartbeat();
				outtimecount = 0;
			}
			continue;
		}

		index -= WSA_WAIT_EVENT_0;
		WSANETWORKEVENTS myNWE;
		ret = WSAEnumNetworkEvents(sock_con, event[index], &myNWE);
		if (ret == SOCKET_ERROR){
			ret = WSAGetLastError();
			closesocket(sock_con);
			return -7;
		}

		if (myNWE.lNetworkEvents & FD_READ){//recv data
			if (myNWE.iErrorCode[FD_READ_BIT] != 0)
				continue;
			++outtimecount;
			read();
		}

 		if (myNWE.lNetworkEvents & FD_WRITE){//send data
 			if (myNWE.iErrorCode[FD_WRITE_BIT] != 0)
 				continue;
 			//static bool bfirst = true;
 			write(true);
 			//if (bfirst) bfirst = false;
 		}

		if (myNWE.lNetworkEvents & FD_CLOSE){
			if (myNWE.iErrorCode[FD_CLOSE_BIT] != 0)
				continue;
			shutdown(sock_con, SD_SEND);
			closesocket(sock_con);
		}
	}
	return 0;
}

bool dtIO::read()
{
	int ret = 0;
	int count = 0;
	memset(mBufRecv, 0, sizeof(mBufRecv));
	packdt pd;
	PrintItem pi;
	do{
		++count;
		ret = recv(mlcsock.mSock, mBufRecv, sizeof(mBufRecv), 0);
		if (ret == 0){
			OutputDebugString(_T("\n[read] 连接断开\n"));
			return false;
		} else if (ret > 0){
			OutputDebugString(_T("\n接收到服务器数据：\n"));
			OutputDebugString(mBufRecv);
			OutputDebugString(_T("\n================\n"));
			break;
		}
	} while (ret != 0 || (count > SIZE_MAXRECV));
	if (count == SIZE_MAXRECV)
		return false;

	pi.strRawData = mBufRecv;
	pi.msgno = pd.getmsgno(pi.strRawData.c_str(), pi.strRawData.length());
	if (!pi.msgno.empty()){
		mDataBuf->pushData(pi);//把接收到的数据mBufRecv写入打印缓存中
		SetEvent(mPrintEvent);
	}
	return true;
}

bool dtIO::write(bool bFirst, char* pdata)
{
	//如果是第一次写数据，还需要加上打印机的信息
	if (bFirst){
		memset(mBufSend, 0, sizeof(mBufSend));
		mPack.selfintr(mBufSend);
		OutputDebugString(_T("\n发送810数据 \n"));
	} else {
		PrintItem pi;
		//已经打印，但是没有回复服务器的数据
		if (!mDataBuf->getData2Reply(pi))
			return false;

		std::string strmsgno = mPack.getmsgno(pi.strRawData.c_str(), pi.strRawData.length());
		pi.bReplied = true;
		mDataBuf->updateData(pi);//把已打印的数据踢出去
		strmsgno = mPack.echodt(strmsgno.c_str(), pi.bSuc);   //向服务器回馈
		memset(mBufSend, 0, sizeof(mBufSend));
		memcpy(mBufSend, strmsgno.c_str(), strmsgno.length()+1);
		OutputDebugString(_T("\n发送815数据 \n"));
	}

	int ret = send(mlcsock.mSock, mBufSend, strlen(mBufSend), 0);
	if (ret == SOCKET_ERROR){
		if (WSAGetLastError() == WSAEWOULDBLOCK){
			return false;
		} else {
			std::cout << "error :" << WSAGetLastError() << std::endl;
		}
	}
	OutputDebugString(mBufSend);
	OutputDebugString(_T("\n================\n"));
	return true;
}

bool dtIO::heartbeat()
{
	char buf[3] = "00";
	//mPack.alivedt(mBufSend);
	send(mlcsock.mSock, buf, strlen(buf), 0);
	OutputDebugString(_T("Keep alive"));
	OutputDebugString(_T("\n发送心跳包：\n"));
	OutputDebugString(buf);
	OutputDebugString(_T("\n================\n"));
	return true;
}

