#pragma once
#include <string>
#include <vector>
#include <list>
#include "PrintSamples.h"
#include "RWBuf.h"
using namespace std;

//以下3个宏是关于打印时间的控制
//定义1秒
#define TIME_UNIT 10000000
//定义从现在开始起，在BEGIN秒内，开始请求第一次打印
#define BEGIN  3
//定义间隔时间，就是从BEGIN秒开始之后，每隔PERIOD秒开始，请求一次打印
#define PERIOD  30
namespace Printer_Lsh{
	//存储打印信息的结构体
	typedef BOOL (*PPRINTFUNC)(char* ); //定义打印的实际工作函数
	typedef BOOL (*START_PRINTER)(); //定义打印的实际工作函数
	typedef BOOL (*END_PRINTER)(); //定义打印的实际工作函数

	struct PrintInfo 
	{
		PrintInfo(){
			bPrintThreadWorking = FALSE;
			//iPrinting = 0;
			hWndMsg = NULL;
			strUrl = "";                     //默认使用这个地址请求数据
			//hTimer = INVALID_HANDLE_VALUE;
			IsPrinter = FALSE;
			pStartPrinterFunc = startPrinterPOS;
			pPrintFunc = PrintInStandardMode56;   //默认使用这种打印
			pEndPrinterPrintFunc = endPrinterPOS;
			bEnablePrint = TRUE;
			hEventCallPrint = INVALID_HANDLE_VALUE;
		}
		~PrintInfo(){
			if (hEventCallPrint != INVALID_HANDLE_VALUE){
				CloseHandle(hEventCallPrint);
				hEventCallPrint = INVALID_HANDLE_VALUE;
			}
		}

		BOOL   bPrintThreadWorking;         //记录打印线程是否在工作
		BOOL IsPrinter;
		BOOL bEnablePrint;					//是否开启打印
		HWND hWndMsg;                       //指定接收日志的窗口句柄
		//volatile int   iPrinting;         //记录当前打印线程是否正在打印, 为0表示没有在打印，为1表示正在打印
		std::string strUrl;                 //记录得到数据的URL
		std::string strPrintServerIP;       //打印机请求数据的服务器IP
		std::string strPrintServerPort;     //打印机请求数据的服务器打印服务端口
		std::string strPrintPW;             //打印机密码
		std::string strPrintIMEI;           //打印机IMEI设备号
		RWList_ms   printBuf;               //打印数据缓存
		//HANDLE      hTimer;                 //等待计时器句柄
		HANDLE      hEventCallPrint;        //提醒打印过程来取printBuf数据的事件的句柄
		PPRINTFUNC pPrintFunc;
		START_PRINTER pStartPrinterFunc;
		END_PRINTER pEndPrinterPrintFunc;
	};

	class PrintRecorder
	{
	public:
		PrintRecorder(){
			bSuc = false;
			index = -1;
		}

		PrintRecorder(const PrintRecorder& rhp){
			copy(rhp);
		}
		PrintRecorder& operator = (const PrintRecorder& rhp){
			if (this != &rhp)
				copy(rhp);
			return *this;
		}
		void copy(const PrintRecorder& rhs){
			index = rhs.index;
			strPrintText = rhs.strPrintText;
		}
	public:
		void SetPrintText(std::string& str){
			strPrintText = str;
		}
		std::string GetPrintText(){
			return strPrintText;
		}

		void IndexIncreament(){
			index = s_index++;
		}

		size_t GetIndex(){
			return index;
		}

		void SetPrintResult(bool b){
			bSuc = b;
		}
		bool GetPrintResult(){
			return bSuc;
		}

	private:
		bool         bSuc;           //记录打印是否成功
		size_t       index;          //记录打印的编号
		std::string  strPrintText;   //记录打印的文本

		static size_t s_index;
	};

	class PrintWorker
	{
	public:
		PrintWorker(void);
		~PrintWorker(void);

		//socket连接主线程
		static INT sock_worker(void* pParam);

		//打印线程函数
		static INT doprint(void* pParam);

	private:
		//初始化
		static void Init();

		//Group: 可等待计时器
 		//static HANDLE CreateTimer();
 		//static void StartTimer();
 		//static void StopTimer();
		//static void NextLoopTimer();

	public:
		//Group: 更新打印信息获取的 URL
		static void UpdateURL(CString strUrl);
		static void UpdatePrintFunc(PPRINTFUNC pFunc);
		static CString GetCurURL();

		/*
		Group: 日志操作
		下面这两个函数的用法是先得到目前日志的数量GetLogCount()，然后再使用一个循环得到日志的内容。
		但是要注意，日志的个数是会变的，可能在遍历的时候已经有新的了，所以应该定时检查，比较并得到新的日志
		*/
		static size_t GetLogCount();//得到日志大小
		static CString GetLog(size_t index);//得到日志
		static void AddLog(CString strlog);//添加日志

	private:
		static PrintInfo            * s_pPrintInfo; //保存打印数据
		static CRITICAL_SECTION       s_csUrl;      //保护URL
		static CRITICAL_SECTION       s_csPrintFunc;//保证可以替换打印函数
		static CRITICAL_SECTION       s_csLog;      //保护日志数组
		static std::vector<CString>   s_Log;    //记录打印的日志
		static std::list<std::string> s_PrintConBuf;  //打印内容的缓存
	};
}

