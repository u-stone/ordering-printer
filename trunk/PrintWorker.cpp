#include "stdafx.h"
#include "PrintWorker.h"
#include "ComonFunc.h"
#include "json/json.h"
#include "httpclient.h"
#include "LogMsgDef.h"
#include "AutoCS.h"
#include "dtIO.h"

#pragma comment(lib, "json_vc71_libmt.lib")

using namespace Printer_Lsh;

CRITICAL_SECTION PrintWorker::s_csUrl;
CRITICAL_SECTION PrintWorker::s_csLog;
CRITICAL_SECTION PrintWorker::s_csPrintFunc;
PrintInfo * PrintWorker::s_pPrintInfo = NULL;
std::vector<CString> PrintWorker::s_Log;

//静态变量的初始化
Printer_Lsh::PrintInfo g_pi;
size_t PrintRecorder::s_index = 0;


PrintWorker::PrintWorker(void)
{
}

PrintWorker::~PrintWorker(void)
{
}

void PrintWorker::Init()
{
	InitializeCriticalSection(&s_csUrl);
	InitializeCriticalSection(&s_csPrintFunc);
	InitializeCriticalSection(&s_csLog);
}

//打印线程
INT PrintWorker::worker(void* pParam)
{
	if (pParam == NULL)
		return -1;
	Init();

	s_pPrintInfo = (PrintInfo *)pParam;
	s_pPrintInfo->bPrintThreadWorking = TRUE;
	s_pPrintInfo->bEnablePrint = TRUE;

	try {
		SOCK_Lsh::dtIO dtio(s_pPrintInfo->strPrintServerIP, s_pPrintInfo->strPrintServerPort, 
			s_pPrintInfo->strPrintPW, s_pPrintInfo->strPrintIMEI);
		dtio.addOnRDFunc((SOCK_Lsh::rdFinished)PrintWorker::DoPrint);
		dtio.EventDispatch();
	} catch (char* e){
		s_pPrintInfo->bPrintThreadWorking = FALSE;
		AddLog(e);
		return -1;
	} catch (...) {
		//StopTimer();
		s_pPrintInfo->bPrintThreadWorking = FALSE;
		return -1;
	}
	//StopTimer();
	s_pPrintInfo->bPrintThreadWorking = FALSE;
	return 0;
}


bool PrintWorker::DoPrint(std::string& strData){

	AutoCS acs(&s_csUrl);
	if (s_pPrintInfo->pPrintFunc != NULL){
		if(s_pPrintInfo->IsPrinter)  //如果选择用驱动程序打印
			s_pPrintInfo->pStartPrinterFunc();

 		if (s_pPrintInfo->pPrintFunc((char *)strData.c_str())) {
 			AddLog(LOG_PRINT_SUC);
 		} else {
 			AddLog(LOG_PRINT_FAILED);
 		}

		if(s_pPrintInfo->IsPrinter) //如果选择用驱动程序打印
			s_pPrintInfo->pEndPrinterPrintFunc();
	}

	AddLog(LOG_PRINTEND);
	return true;
}


void PrintWorker::UpdateURL(CString strUrl)
{
	AutoCS acs(&s_csUrl);
	s_pPrintInfo->strUrl = strUrl.GetBuffer(0);
}


// HANDLE PrintWorker::CreateTimer(){
// 	SECURITY_ATTRIBUTES sa = {0};
// 	sa.nLength = sizeof(sa);
// 	sa.bInheritHandle = FALSE;
// 	sa.lpSecurityDescriptor = NULL;
// 	return CreateWaitableTimer(&sa, FALSE, NULL);
// }

//void PrintWorker::StartTimer(){
//	LARGE_INTEGER li;
//	li.QuadPart = -(BEGIN * TIME_UNIT);
//	SetWaitableTimer(s_pPrintInfo->hTimer, &li, PERIOD * 1000, NULL, NULL, FALSE);
//}	
//
//void PrintWorker::StopTimer(){
//	CancelWaitableTimer(s_pPrintInfo->hTimer);
//	CloseHandle(s_pPrintInfo->hTimer);
//	s_pPrintInfo->hTimer = INVALID_HANDLE_VALUE;
//}


void PrintWorker::UpdatePrintFunc(PPRINTFUNC pFunc)
{
	AutoCS acs(&s_csPrintFunc);
	s_pPrintInfo->pPrintFunc = pFunc;
}

CString PrintWorker::GetCurURL()
{
	CString strUrl;
	AutoCS acs(&s_csUrl);
	strUrl = s_pPrintInfo->strUrl.c_str();
	return strUrl;
}

size_t PrintWorker::GetLogCount()
{
	size_t size;
	AutoCS acs(&s_csLog);
	size = s_Log.size();
	return size;
}

CString PrintWorker::GetLog(size_t index)
{
	CString tmp;
	AutoCS acs(&s_csLog);
	tmp = s_Log.at(index);
	return tmp;
}


void PrintWorker::AddLog(CString strlog)
{
	AutoCS acs(&s_csLog);

	//如果想要修改日志的格式，可以在这里操作strlog
	s_Log.push_back(strlog);


	//通知界面，准备取数据
	::PostMessage(s_pPrintInfo->hWndMsg, WM_UPDATELOG, 0, 0);
}