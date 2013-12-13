#pragma once
#include <string>
#include <vector>
#include "PrintSamples.h"

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
			strUrl = "";//Ĭ��ʹ�������ַ��������
			hTimer = INVALID_HANDLE_VALUE;
			pPrintFunc = PrintInStandardMode56;//Ĭ��ʹ�����ִ�ӡ
			IsPrinter = FALSE;
			pStartPrinterFunc = startPrinterPOS;
			pEndPrinterPrintFunc = endPrinterPOS;
			bEnablePrint = TRUE;
		}

		BOOL   bPrintThreadWorking;         //��¼��ӡ�߳��Ƿ��ڹ���
		BOOL IsPrinter;
		BOOL bEnablePrint;					//�Ƿ�����ӡ
		HWND hWndMsg;                       //ָ��������־�Ĵ��ھ��
		//volatile int   iPrinting;         //��¼��ǰ��ӡ�߳��Ƿ����ڴ�ӡ, Ϊ0��ʾû���ڴ�ӡ��Ϊ1��ʾ���ڴ�ӡ
		std::string strUrl;                 //��¼�õ����ݵ�URL
		HANDLE hTimer;                      //�ȴ���ʱ�����
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
		bool bSuc;                   //��¼��ӡ�Ƿ�ɹ�
		size_t index;         //��¼��ӡ�ı��
		std::string  strPrintText;   //��¼��ӡ���ı�

		static size_t s_index;
	};


	class PrintWorker
	{
	public:
		PrintWorker(void);
		~PrintWorker(void);

		//�����̵߳�������
		static INT worker(void* pParam);

	private:
		//��ʼ��
		static void Init();

		//Group: �ɵȴ���ʱ��
		static HANDLE CreateTimer();
		static void StartTimer();
		static void StopTimer();
		//static void NextLoopTimer();

		//��ӡ��ʵ��ִ�к���
		static void DoPrint();

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
		static void AddLog(CString strlog);//�����־

	private:
		static PrintInfo * s_pPrintInfo;
		static CRITICAL_SECTION s_csUrl;      //����URL
		static CRITICAL_SECTION s_csPrintFunc;//��֤�����滻��ӡ����
		static CRITICAL_SECTION s_csLog;      //������־����
		static std::vector<CString> s_Log; //��¼��ӡ����־
	};

}

