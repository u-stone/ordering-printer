#pragma once
#include <string>
#include <vector>
#include <list>
#include "PrintSamples.h"
#include "RWBuf.h"
using namespace std;

//����3�����ǹ��ڴ�ӡʱ��Ŀ���
//����1��
#define TIME_UNIT 10000000
//��������ڿ�ʼ����BEGIN���ڣ���ʼ�����һ�δ�ӡ
#define BEGIN  3
//������ʱ�䣬���Ǵ�BEGIN�뿪ʼ֮��ÿ��PERIOD�뿪ʼ������һ�δ�ӡ
#define PERIOD  30
namespace Printer_Lsh{
	//�洢��ӡ��Ϣ�Ľṹ��
	typedef BOOL (*PPRINTFUNC)(char* ); //�����ӡ��ʵ�ʹ�������
	typedef BOOL (*START_PRINTER)(); //�����ӡ��ʵ�ʹ�������
	typedef BOOL (*END_PRINTER)(); //�����ӡ��ʵ�ʹ�������

	struct PrintInfo 
	{
		PrintInfo(){
			bPrintThreadWorking = FALSE;
			//iPrinting = 0;
			hWndMsg = NULL;
			strUrl = "";                     //Ĭ��ʹ�������ַ��������
			//hTimer = INVALID_HANDLE_VALUE;
			IsPrinter = FALSE;
			pStartPrinterFunc = startPrinterPOS;
			pPrintFunc = PrintInStandardMode56;   //Ĭ��ʹ�����ִ�ӡ
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

		BOOL   bPrintThreadWorking;         //��¼��ӡ�߳��Ƿ��ڹ���
		BOOL IsPrinter;
		BOOL bEnablePrint;					//�Ƿ�����ӡ
		HWND hWndMsg;                       //ָ��������־�Ĵ��ھ��
		//volatile int   iPrinting;         //��¼��ǰ��ӡ�߳��Ƿ����ڴ�ӡ, Ϊ0��ʾû���ڴ�ӡ��Ϊ1��ʾ���ڴ�ӡ
		std::string strUrl;                 //��¼�õ����ݵ�URL
		std::string strPrintServerIP;       //��ӡ���������ݵķ�����IP
		std::string strPrintServerPort;     //��ӡ���������ݵķ�������ӡ����˿�
		std::string strPrintPW;             //��ӡ������
		std::string strPrintIMEI;           //��ӡ��IMEI�豸��
		RWList_ms   printBuf;               //��ӡ���ݻ���
		//HANDLE      hTimer;                 //�ȴ���ʱ�����
		HANDLE      hEventCallPrint;        //���Ѵ�ӡ������ȡprintBuf���ݵ��¼��ľ��
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
		bool         bSuc;           //��¼��ӡ�Ƿ�ɹ�
		size_t       index;          //��¼��ӡ�ı��
		std::string  strPrintText;   //��¼��ӡ���ı�

		static size_t s_index;
	};

	class PrintWorker
	{
	public:
		PrintWorker(void);
		~PrintWorker(void);

		//socket�������߳�
		static INT sock_worker(void* pParam);

		//��ӡ�̺߳���
		static INT doprint(void* pParam);

	private:
		//��ʼ��
		static void Init();

		//Group: �ɵȴ���ʱ��
 		//static HANDLE CreateTimer();
 		//static void StartTimer();
 		//static void StopTimer();
		//static void NextLoopTimer();

	public:
		//Group: ���´�ӡ��Ϣ��ȡ�� URL
		static void UpdateURL(CString strUrl);
		static void UpdatePrintFunc(PPRINTFUNC pFunc);
		static CString GetCurURL();

		/*
		Group: ��־����
		�����������������÷����ȵõ�Ŀǰ��־������GetLogCount()��Ȼ����ʹ��һ��ѭ���õ���־�����ݡ�
		����Ҫע�⣬��־�ĸ����ǻ��ģ������ڱ�����ʱ���Ѿ����µ��ˣ�����Ӧ�ö�ʱ��飬�Ƚϲ��õ��µ���־
		*/
		static size_t GetLogCount();//�õ���־��С
		static CString GetLog(size_t index);//�õ���־
		static void AddLog(CString strlog);//������־

	private:
		static PrintInfo            * s_pPrintInfo; //�����ӡ����
		static CRITICAL_SECTION       s_csUrl;      //����URL
		static CRITICAL_SECTION       s_csPrintFunc;//��֤�����滻��ӡ����
		static CRITICAL_SECTION       s_csLog;      //������־����
		static std::vector<CString>   s_Log;    //��¼��ӡ����־
		static std::list<std::string> s_PrintConBuf;  //��ӡ���ݵĻ���
	};
}
