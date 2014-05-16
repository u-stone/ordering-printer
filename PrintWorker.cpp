#include "stdafx.h"
#include "PrintWorker.h"
#include "ComonFunc.h"
#include "httpclient.h"
#include "LogMsgDef.h"
#include "AutoCS.h"
#include "dtIO.h"


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
INT PrintWorker::sock_worker(void* pParam)
{
	if (pParam == NULL)
		return -1;
	Init();

	s_pPrintInfo = (PrintInfo *)pParam;

	while (1){
		try {
			SOCK_Lsh::dtIO dtio(s_pPrintInfo->strPrintServerIP, s_pPrintInfo->strPrintServerPort, 
				s_pPrintInfo->strPrintPW, s_pPrintInfo->strPrintIMEI);
			dtio.setRdEvent(s_pPrintInfo->hEventCallPrint);
			dtio.setDataBuf(&s_pPrintInfo->printBuf);
			dtio.EventDispatch();
		} catch (char* e){
			s_pPrintInfo->bPrintThreadWorking = FALSE;
			AddLog(e);
		} catch (...) {
			s_pPrintInfo->bPrintThreadWorking = FALSE;
			AddLog(_T("未知的网络错误..."));
		}
		Sleep(60000);//等待一分钟，之后重新连接
	}
	//StopTimer();
	AfxMessageBox(_T("网络连接线程崩溃，请查看网络！"));
	return 0;
}

INT PrintWorker::doprint(void* pParam){

	if (pParam == NULL)
		return -1;

	PrintItem prtitem;
	std::string strData;
	s_pPrintInfo = (PrintInfo*)pParam;
	s_pPrintInfo->bPrintThreadWorking = TRUE;
	s_pPrintInfo->bEnablePrint = TRUE;

	packdt pd;
	while (1){
		DWORD ret = WaitForSingleObject(s_pPrintInfo->hEventCallPrint, 5000);
		if (ret == WAIT_FAILED){
			AddLog(_T("[DoPrint] 等待失败"));
		} else if (ret == WAIT_TIMEOUT){
			AddLog(_T("[DoPrint] 等待超时"));
		} else if (ret == WAIT_OBJECT_0){//有新数据
			AddLog(_T("[DoPrint] 等待事件被激活"));
		}

		ResetEvent(s_pPrintInfo->hEventCallPrint);

		while (s_pPrintInfo->printBuf.readBuf(prtitem)){
			if (s_pPrintInfo->pPrintFunc != NULL){

				if(s_pPrintInfo->IsPrinter)  //如果选择用驱动程序打印
					s_pPrintInfo->pStartPrinterFunc();

				strData = pd.parseprtdt(prtitem.strRawData.c_str(), prtitem.strRawData.length());

				if (s_pPrintInfo->pPrintFunc((char *)strData.c_str())) {
					prtitem.bSuc = true;
					AddLog(LOG_PRINT_SUC);
				} else {
					prtitem.bSuc = false;
					AddLog(LOG_PRINT_FAILED);
				}
				prtitem.bPrinted = true;
				s_pPrintInfo->printBuf.writeBuf(prtitem);

				if(s_pPrintInfo->IsPrinter) //如果选择用驱动程序打印
					s_pPrintInfo->pEndPrinterPrintFunc();
			}
			AddLog(LOG_PRINTEND);
		}
	}
	s_pPrintInfo->bPrintThreadWorking = FALSE;
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