#include "stdafx.h"
#include "PrintWorker.h"
#include "ComonFunc.h"
#include "json/json.h"
#include "httpclient.h"
#include "LogMsgDef.h"

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
		if (s_pPrintInfo->hTimer == INVALID_HANDLE_VALUE){//准备计时器
			s_pPrintInfo->hTimer = CreateTimer();
			StartTimer();
		}

		while (s_pPrintInfo->bPrintThreadWorking){
			DWORD res = WaitForSingleObject(s_pPrintInfo->hTimer, INFINITE);
			if (res != WAIT_OBJECT_0){//等待失败
				break;
			}
// 			if (s_pPrintInfo->iPrinting == 1){//如果没有完成打印就跳过此次循环，继续
// 				OutputDebugString(_T("跳过一次打印请求，因为有正在打印的任务"));
//   				continue ;
// 			}
			
			if (s_pPrintInfo->bEnablePrint) {
  				DoPrint();
			}
		}
	} catch (...) {
		StopTimer();
		s_pPrintInfo->bPrintThreadWorking = FALSE;
		return -1;
	}
	StopTimer();
	s_pPrintInfo->bPrintThreadWorking = FALSE;
	return 0;
}


void PrintWorker::DoPrint(){

	AddLog(LOG_PRINTBEGAIN);
	//InterlockedExchange(s_pPrintInfo->iPrinting, 1);//替换为表示没有在打印的标记 1

	CHttpClient   asker;
	Json::Reader reader;  
	Json::Value root;  

	EnterCriticalSection(&s_csUrl);
	CString strRes = asker.doGet(s_pPrintInfo->strUrl.c_str());
	std::string strToParse = strRes.GetBuffer();
	strRes.ReleaseBuffer();
	//不懂 怎么算是请求服务失败  或者是成功，这个日志你自己写吧
	//AddLog(LOG_SERVERCON_FAILED);
	//AddLog(LOG_SERVERCON_SUC);

	if (reader.parse(strToParse, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
	{
		std::string print_text = root["text"].asString();  // 访问节点，upload_id = "UP000000"   
		std::string pid_str = root["pid"].asString();    // 访问节点，code = 100   
		std::string out_gbk;
		UTF_8ToGB2312(out_gbk, (char *)print_text.c_str(), print_text.length());
		if (print_text.length() > 0 && out_gbk.length() > 0 ) {
			if (s_pPrintInfo->pPrintFunc != NULL){
				if(s_pPrintInfo->IsPrinter)  //如果选择用驱动程序打印
					s_pPrintInfo->pStartPrinterFunc();

				if (s_pPrintInfo->pPrintFunc((char *)out_gbk.c_str())) {
					AddLog(LOG_PRINT_SUC);
					pid_str = s_pPrintInfo->strUrl + _T("&pid=") + pid_str;
					asker.doGet(pid_str.c_str());
				} else {
					AddLog(LOG_PRINT_FAILED);
				}

				if(s_pPrintInfo->IsPrinter) //如果选择用驱动程序打印
					s_pPrintInfo->pEndPrinterPrintFunc();
			}
		} else {
			AddLog(LOG_PRINTTXT_EMPTY);
		}
	} 
	LeaveCriticalSection(&s_csUrl);

	//表示这一次打印执行完毕了，不要在没执行完打印之前又调用打印
	//InterlockedExchange(s_pPrintInfo->iPrinting, 0);//替换为表示没有在打印的标记 0

	AddLog(LOG_PRINTEND);
}


void PrintWorker::UpdateURL(CString strUrl)
{
	EnterCriticalSection(&s_csUrl);
	s_pPrintInfo->strUrl = strUrl.GetBuffer(0);
	LeaveCriticalSection(&s_csUrl);
}


HANDLE PrintWorker::CreateTimer(){
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;
	sa.lpSecurityDescriptor = NULL;
	return CreateWaitableTimer(&sa, FALSE, NULL);
}

void PrintWorker::StartTimer(){
	LARGE_INTEGER li;
	li.QuadPart = -(BEGIN * TIME_UNIT);
	SetWaitableTimer(s_pPrintInfo->hTimer, &li, PERIOD * 1000, NULL, NULL, FALSE);
}	

void PrintWorker::StopTimer(){
	CancelWaitableTimer(s_pPrintInfo->hTimer);
	CloseHandle(s_pPrintInfo->hTimer);
	s_pPrintInfo->hTimer = INVALID_HANDLE_VALUE;
}


void PrintWorker::UpdatePrintFunc(PPRINTFUNC pFunc)
{
	EnterCriticalSection(&s_csPrintFunc);
	s_pPrintInfo->pPrintFunc = pFunc;
	LeaveCriticalSection(&s_csPrintFunc);
}

CString PrintWorker::GetCurURL()
{
	CString strUrl;
	EnterCriticalSection(&s_csUrl);
	strUrl = s_pPrintInfo->strUrl.c_str();
	LeaveCriticalSection(&s_csUrl);
	return strUrl;
}

size_t PrintWorker::GetLogCount()
{
	size_t size;
	EnterCriticalSection(&s_csLog);
	size = s_Log.size();
	LeaveCriticalSection(&s_csLog);
	return size;
}

CString PrintWorker::GetLog(size_t index)
{
	CString tmp;
	EnterCriticalSection(&s_csLog);
	tmp = s_Log.at(index);
	LeaveCriticalSection(&s_csLog);
	return tmp;
}


void PrintWorker::AddLog(CString strlog)
{
	EnterCriticalSection(&s_csLog);

	//如果想要修改日志的格式，可以在这里操作strlog
	s_Log.push_back(strlog);

	LeaveCriticalSection(&s_csLog);

	//通知界面，准备取数据
	::PostMessage(s_pPrintInfo->hWndMsg, WM_UPDATELOG, 0, 0);
}