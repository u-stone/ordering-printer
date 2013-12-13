#pragma once
#include <string>
#include <vector>
#include "PrintSamples.h"

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
			strUrl = "";//默认使用这个地址请求数据
			hTimer = INVALID_HANDLE_VALUE;
			pPrintFunc = PrintInStandardMode56;//默认使用这种打印
			IsPrinter = FALSE;
			pStartPrinterFunc = startPrinterPOS;
			pEndPrinterPrintFunc = endPrinterPOS;
			bEnablePrint = TRUE;
		}

		BOOL   bPrintThreadWorking;         //记录打印线程是否在工作
		BOOL IsPrinter;
		BOOL bEnablePrint;					//是否开启打印
		HWND hWndMsg;                       //指定接收日志的窗口句柄
		//volatile int   iPrinting;         //记录当前打印线程是否正在打印, 为0表示没有在打印，为1表示正在打印
		std::string strUrl;                 //记录得到数据的URL
		HANDLE hTimer;                      //等待计时器句柄
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
// 		PrintRecorder(PrintRecorder& rhp){
// 			swap(rhp);
// 		}
// 		PrintRecorder& operator = (PrintRecorder& rhp){
// 			swap(rhp);
// 			return *this;
// 		}
// 		void swap(PrintRecorder& rhs){
// 			std::swap(index, rhs.index);
// 			std::swap(strPrintText, rhs.strPrintText);
// 		}

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
		bool bSuc;                   //记录打印是否成功
		size_t index;         //记录打印的编号
		std::string  strPrintText;   //记录打印的文本

		static size_t s_index;
	};


	class PrintWorker
	{
	public:
		PrintWorker(void);
		~PrintWorker(void);

		//工作线程的主函数
		static INT worker(void* pParam);

	private:
		//初始化
		static void Init();

		//Group: 可等待计时器
		static HANDLE CreateTimer();
		static void StartTimer();
		static void StopTimer();
		//static void NextLoopTimer();

		//打印的实际执行函数
		static void DoPrint();

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
		static PrintInfo * s_pPrintInfo;
		static CRITICAL_SECTION s_csUrl;      //保护URL
		static CRITICAL_SECTION s_csPrintFunc;//保证可以替换打印函数
		static CRITICAL_SECTION s_csLog;      //保护日志数组
		static std::vector<CString> s_Log; //记录打印的日志
	};

}

