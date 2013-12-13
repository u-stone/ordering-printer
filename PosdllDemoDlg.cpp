// PosdllDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PosdllDemo.h"
#include "PosdllDemoDlg.h"
#include "LoadDll.h"
#include "PrintSamples.h"
#include "PrintWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE g_hComm	= INVALID_HANDLE_VALUE;//端口句柄
bool IsPrinter	= false;//判断是否是用驱动程序打印
int  m_iMode	= 0;//页模式或标准模式
bool bSaveToTxt = false;//是否选择数据保存到文件Test.txt，不向端口下发

extern HMODULE				g_hPosdll;//动态库句柄

extern POS_Open				VC_POS_Open;//打开端口
extern POS_Close			VC_POS_Close;//关闭端口
extern POS_RTQueryStatus	VC_POS_RTQueryStatus;//实时状态查询
extern POS_NETQueryStatus	VC_POS_NETQueryStatus;//网络接口的状态查询
extern POS_StartDoc			VC_POS_StartDoc;//启动一个文档
extern POS_EndDoc			VC_POS_EndDoc;//结束一个文档
extern POS_BeginSaveFile	VC_POS_BeginSaveFile;//启动保存数据到文件
extern POS_EndSaveFile		VC_POS_EndSaveFile;//结束保存数据到文件
extern POS_GetVersionInfo	VC_POS_GetVersionInfo;//获取当前动态库版本


extern Printer_Lsh::PrintInfo g_pi;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosdllDemoDlg dialog

CPosdllDemoDlg::CPosdllDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPosdllDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPosdllDemoDlg)
	m_PortType = 0;
	m_ModeSelect = 0;
	m_strMsg = _T("");
	m_strDrvName = _T("");
	m_iTphWidth = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPosdllDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPosdllDemoDlg)
	DDX_Control(pDX, IDC_PAGE_WIDTH, m_ctrlPageWidth);
	DDX_Control(pDX, IDC_DRV_NAME, m_ctrlDrvName);
	DDX_Control(pDX, IDC_IPADDRESS, m_ctrlIPAddr);
	DDX_Control(pDX, IDC_CLOSE_PORT, m_ctrlClosePort);
	DDX_Control(pDX, IDC_PRINT, m_ctrlPrint);
	DDX_Control(pDX, IDC_QUERY_STATUS, m_ctrlQueryStatus);
	DDX_Control(pDX, IDC_LPT_NAME, m_ctrlLPTName);
	DDX_Control(pDX, IDC_PARITY, m_ctrlParity);
	DDX_Control(pDX, IDC_FLOW_CONTROL, m_ctrlFlowControl);
	DDX_Control(pDX, IDC_STOPBITS, m_ctrlStopBits);
	DDX_Control(pDX, IDC_BAUDRATE, m_ctrlBaudrate);
	DDX_Control(pDX, IDC_DATABITS, m_ctrlDataBits);
	DDX_Control(pDX, IDC_COM_NAME, m_ctrlComName);
	DDX_Radio(pDX, IDC_PORT_COM, m_PortType);
	DDX_Radio(pDX, IDC_MODE_STANDARD, m_ModeSelect);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_strMsg);
	DDX_Text(pDX, IDC_DRV_NAME, m_strDrvName);
	DDX_CBIndex(pDX, IDC_PAGE_WIDTH, m_iTphWidth);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPosdllDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CPosdllDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_PORT, OnOpenPort)
	ON_BN_CLICKED(IDC_PORT_COM, OnPortCom)
	ON_BN_CLICKED(IDC_PORT_LPT, OnPortLpt)
	ON_BN_CLICKED(IDC_PORT_USB, OnPortUsb)
	ON_BN_CLICKED(IDC_PORT_NET, OnPortNet)
	ON_BN_CLICKED(IDC_PORT_DRV, OnPortDrv)
	ON_BN_CLICKED(IDC_QUERY_STATUS, OnQueryStatus)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_BN_CLICKED(IDC_CLOSE_PORT, OnClosePort)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MODE_STANDARD, OnModeStandard)
	ON_BN_CLICKED(IDC_MODE_PAGE, OnModePage)
	ON_BN_CLICKED(IDC_SAVE_TO_TXT, OnSaveToTxt)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BtnSetUrl, &CPosdllDemoDlg::OnBnClickedBtnseturl)
	ON_BN_CLICKED(IDC_BtnLog, &CPosdllDemoDlg::OnBnClickedBtnlog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosdllDemoDlg message handlers

BOOL CPosdllDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	if ( false == LoadPosdll() )
	{
		// 如果动态库加载失败, 则退出
		AfxMessageBox("加载动态库失败！");
		CDialog::OnCancel();
		return false;
	}
	/*初始化界面显示*/
	m_ctrlIPAddr.SetAddress(192,168,10,251);
	m_ctrlIPAddr.EnableWindow(false);
	
	m_ctrlComName.SetCurSel(0);
	m_ctrlDataBits.SetCurSel(1);
	m_ctrlBaudrate.SetCurSel(2); 
	m_ctrlStopBits.SetCurSel(0);
	m_ctrlParity.SetCurSel(2);
	m_ctrlFlowControl.SetCurSel(1);

	m_ctrlLPTName.SetCurSel(0);

	m_ctrlPageWidth.SetCurSel(1);

	SetDlgItemText(IDC_DRV_NAME,"BTP-2002CP(S)");

	int iMajor,iMinor;
	CString str,tmp;
	VC_POS_GetVersionInfo(&iMajor,&iMinor);
	str = "一切正常，版本号V";
	tmp.Format("%d.%d",iMajor,iMinor);
	str += tmp;
	SetDlgItemText(IDC_EDIT_STATUS,str);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPosdllDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPosdllDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPosdllDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPosdllDemoDlg::OnPortCom() //端口选择为串口
{
	// TODO: Add your control notification handler code here
	m_ctrlComName.EnableWindow(true);
	m_ctrlDataBits.EnableWindow(true);
	m_ctrlBaudrate.EnableWindow(true);
	m_ctrlStopBits.EnableWindow(true);
	m_ctrlParity.EnableWindow(true);
	m_ctrlFlowControl.EnableWindow(true);

	m_ctrlLPTName.EnableWindow(false);

	m_ctrlIPAddr.EnableWindow(false);

	m_ctrlDrvName.EnableWindow(false);

	SetDlgItemText(IDC_EDIT_STATUS,"端口选择为串口");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortLpt() //端口选择为并口
{
	// TODO: Add your control notification handler code here
	m_ctrlLPTName.EnableWindow(true);
	
	m_ctrlComName.EnableWindow(false);
	m_ctrlDataBits.EnableWindow(false);
	m_ctrlBaudrate.EnableWindow(false);
	m_ctrlStopBits.EnableWindow(false);
	m_ctrlParity.EnableWindow(false);
	m_ctrlFlowControl.EnableWindow(false);	

	m_ctrlIPAddr.EnableWindow(false);

	m_ctrlDrvName.EnableWindow(false);

	SetDlgItemText(IDC_EDIT_STATUS,"端口选择为并口");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortUsb() //端口选择为USB口
{
	// TODO: Add your control notification handler code here
	m_ctrlComName.EnableWindow(false);
	m_ctrlDataBits.EnableWindow(false);
	m_ctrlBaudrate.EnableWindow(false);
	m_ctrlStopBits.EnableWindow(false);
	m_ctrlParity.EnableWindow(false);
	m_ctrlFlowControl.EnableWindow(false);	

	m_ctrlLPTName.EnableWindow(false);

	m_ctrlIPAddr.EnableWindow(false);

	m_ctrlDrvName.EnableWindow(false);

	SetDlgItemText(IDC_EDIT_STATUS,"端口选择为USB口");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortNet() //端口选择为网络接口
{
	// TODO: Add your control notification handler code here
	m_ctrlIPAddr.EnableWindow(true);
	
	m_ctrlComName.EnableWindow(false);
	m_ctrlDataBits.EnableWindow(false);
	m_ctrlBaudrate.EnableWindow(false);
	m_ctrlStopBits.EnableWindow(false);
	m_ctrlParity.EnableWindow(false);
	m_ctrlFlowControl.EnableWindow(false);	

	m_ctrlLPTName.EnableWindow(false);

	m_ctrlDrvName.EnableWindow(false);

	SetDlgItemText(IDC_EDIT_STATUS,"端口选择为网络接口");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortDrv() //端口选择为驱动程序
{
	// TODO: Add your control notification handler code here
	m_ctrlDrvName.EnableWindow(true);
	
	m_ctrlComName.EnableWindow(false);
	m_ctrlDataBits.EnableWindow(false);
	m_ctrlBaudrate.EnableWindow(false);
	m_ctrlStopBits.EnableWindow(false);
	m_ctrlParity.EnableWindow(false);
	m_ctrlFlowControl.EnableWindow(false);	

	m_ctrlLPTName.EnableWindow(false);

	m_ctrlIPAddr.EnableWindow(false);	

	SetDlgItemText(IDC_EDIT_STATUS,"端口选择为驱动程序");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnOpenPort() //打开端口
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	if(0 == m_PortType) //要打开的端口为串口
	{	
		CString strPortName;

		int iBaudrate;
		int iDataBits;
		int iStopBits;
		int iParity;
		int iFlowControl;

		m_ctrlComName.GetLBText(m_ctrlComName.GetCurSel(),strPortName);
		iDataBits = m_ctrlDataBits.GetCurSel()+7;//索引为0表示数据位为7，为1表示8，详见帮助文档
		int tmp = m_ctrlStopBits.GetCurSel();//详见帮助文档
		if(0 == tmp)//停止位为1
		{
			iStopBits = 0;
		}
		if(1 == tmp)//停止位为2
		{
			iStopBits = 2;
		}
		int temp = m_ctrlParity.GetCurSel();//详见帮助文档
		if(0 == temp)//偶校验
		{
			iParity = 2;
		}
		else if(1 == temp)//奇校验
		{
			iParity = 1;
		}
		else//无校验
		{
			iParity = 0;
		}
		int iRet = m_ctrlFlowControl.GetCurSel();//详见帮助文档
		if(0 == iRet)//Xon/Xoff
		{
			iFlowControl = 2;
		}else if(1 == iRet)//POS_COM_RTS_CTS
		{
			iFlowControl = 1;
		}
		else//POS_COM_NO_HANDSHAKE
		{
			iFlowControl = 3;
		}

		switch(m_ctrlBaudrate.GetCurSel())
		{		
		case 0:
			iBaudrate = 2400;
			break;

		case 1:
			iBaudrate = 4800;
			break;

		case 2:
			iBaudrate = 9600;
			break;

		case 3:
			iBaudrate = 19200;
			break;

		case 4:
			iBaudrate = 38400;
			break;

		case 5:
			iBaudrate = 57600;
			break;

		case 6:
			iBaudrate = 115200;
			break;

		default:
			iBaudrate = 9600;
			break;
		}

		g_hComm = VC_POS_Open(strPortName,iBaudrate,iDataBits,iStopBits,iParity,iFlowControl);
	}
	
	if(1 == m_PortType) //要打开的端口为并口
	{
		IsPrinter = false;

		CString strPortName;
		m_ctrlLPTName.GetLBText(m_ctrlLPTName.GetCurSel(),strPortName);
		g_hComm = VC_POS_Open(strPortName,0,0,0,0,POS_OPEN_PARALLEL_PORT);
	}

	if(2 == m_PortType) //要打开的端口为USB口
	{
		IsPrinter = false;

		g_hComm = VC_POS_Open("BYUSB-0",0,0,0,0,POS_OPEN_BYUSB_PORT);
	}

	if(3 == m_PortType) //要打开的端口为网络接口
	{
		if(m_ctrlIPAddr.IsBlank())
		{
			m_strMsg = "IP地址不能为空！";
			m_ctrlIPAddr.SetFocus();
			UpdateData(false);
			return;
		}
		else
		{
			IsPrinter = false;

			CString tmp;
			BYTE  nField1, nField2, nField3, nField4;

			m_ctrlIPAddr.GetAddress(nField1, nField2, nField3, nField4);
			
			tmp.Format("%d",nField1);
			strNetPortName = tmp;
			strNetPortName += ".";

			tmp.Format("%d",nField2);
			strNetPortName += tmp;
			strNetPortName += ".";

			tmp.Format("%d",nField3);
			strNetPortName += tmp;
			strNetPortName += ".";

			tmp.Format("%d",nField4);
			strNetPortName += tmp;

			g_hComm = VC_POS_Open(strNetPortName,0,0,0,0,POS_OPEN_NETPORT);
		}
	}

	if(4 == m_PortType) //要打开的端口为驱动程序
	{
		if("" == m_strDrvName)
		{
			m_strMsg = "驱动程序名不能为空！";
			m_ctrlDrvName.SetFocus();
			UpdateData(false);
			return;
		}
		else
		{
			IsPrinter = true;
			g_hComm = VC_POS_Open(m_strDrvName,0,0,0,0,POS_OPEN_PRINTNAME);
		}
	}

	if (g_hComm != INVALID_HANDLE_VALUE) //判断打开端口函数返回值
	{
		GetDlgItem(IDC_OPEN_PORT)->EnableWindow(false);
		m_strMsg = "端口打开成功！";
		m_ctrlPrint.EnableWindow(true);
		m_ctrlClosePort.EnableWindow(true);
		if(m_PortType != 1 && m_PortType != 4)//并口和驱动程序接口不支持状态查询
		{
			m_ctrlQueryStatus.EnableWindow(true);
		}
	}
	else
	{
		m_strMsg = "端口打开失败！";
		m_ctrlQueryStatus.EnableWindow(false);
		m_ctrlPrint.EnableWindow(false);
		m_ctrlClosePort.EnableWindow(false);
	}

	UpdateData(false);
	m_strMsg = "";
}



void CPosdllDemoDlg::OnQueryStatus() //查询状态
{
	// TODO: Add your control notification handler code here
	char Status = 0;
	int nRet;
	if(3 == m_PortType)
	{
		nRet = VC_POS_NETQueryStatus((LPSTR)(LPCTSTR)strNetPortName,&Status);
	}
	else
	{
		nRet = VC_POS_RTQueryStatus(&Status);
	}
	if(POS_FAIL == nRet)
	{
		m_strMsg = "查询状态失败！";
		GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
		GetDlgItem(IDC_QUERY_STATUS)->EnableWindow(false);
		GetDlgItem(IDC_PRINT)->EnableWindow(false);
		GetDlgItem(IDC_CLOSE_PORT)->EnableWindow(false);
	}
	else
	{	
		int iBits[8];
		for (int i = 0; i < 8; i++)
		{
			iBits[i] = (Status >> i) & 0x01;
		}
		if(Status == 1)
		{
			m_strMsg = "一切正常！";
		}
		else
		{
			m_strMsg = "";
			if (iBits[0] == 0)
			{
				m_strMsg += "有钱箱打开！";
			}
			
			if (iBits[1] == 1)
			{
				m_strMsg += "打印机脱机！";
			}
			
			if (iBits[2] == 1)
			{
				m_strMsg += "上盖打开！";
			}

			if (iBits[3] == 1)
			{
				m_strMsg += "正在进纸！";
			}
			
			if (iBits[4] == 1)
			{
				m_strMsg += "打印机出错！";
			}
			
			if (iBits[5] == 1)
			{
				m_strMsg += "切刀出错！";
			}
			
			if (iBits[6] == 1)
			{
				m_strMsg += "纸将尽！";
			}
			
			if (iBits[7] == 1)
			{
				m_strMsg += "缺纸！";
			}
		}
	}

	UpdateData(false); 
}

void CPosdllDemoDlg::OnPrint() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(IsPrinter)  //如果选择用驱动程序打印
	{
		VC_POS_StartDoc();
	}

	if (g_pi.bPrintThreadWorking == FALSE){//保证只能开一个打印线程
		AfxBeginThread((AFX_THREADPROC)Printer_Lsh::PrintWorker::worker, &g_pi);
	}

	if(IsPrinter) //如果选择用驱动程序打印
	{
		VC_POS_EndDoc();
	}

	UpdateData(false);
	return;
}

void CPosdllDemoDlg::OnClosePort() //关闭端口
{
	// TODO: Add your control notification handler code here
	int nRet;
	nRet = VC_POS_Close();
	if(POS_FAIL == nRet)
	{
		m_strMsg = "端口关闭失败！";
	}
	else if(POS_ERROR_INVALID_HANDLE == nRet)
	{
		m_strMsg = "端口关闭失败！";
	}
	else
	{
		GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
		m_strMsg = "端口关闭成功！";
		m_ctrlQueryStatus.EnableWindow(false);
		m_ctrlPrint.EnableWindow(false);
		m_ctrlClosePort.EnableWindow(false);		
	}

	UpdateData(false);
}

void CPosdllDemoDlg::OnDestroy() //窗口关闭
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(INVALID_HANDLE_VALUE != g_hComm)
	{
		VC_POS_Close();
	}
	if(g_hPosdll)
	{
		UnloadPosdll();
	}
}

void CPosdllDemoDlg::OnModeStandard() //标准模式
{
	// TODO: Add your control notification handler code here
	m_iMode = 0;
}

void CPosdllDemoDlg::OnModePage() //页模式
{
	// TODO: Add your control notification handler code here
	m_iMode = 1;
}

void CPosdllDemoDlg::OnSaveToTxt() //数据保存到文件Test.txt，不向端口下发
{
	// TODO: Add your control notification handler code here
	if(bSaveToTxt)
	{
		bSaveToTxt = false;
	}
	else
	{
		bSaveToTxt = true;
	}
}

BOOL CPosdllDemoDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	//屏蔽F1键自动寻找帮助文件
	return 0;
}

void CPosdllDemoDlg::OnOK() 
{
	// TODO: Add extra validation here
	//CDialog::OnOK();
	//屏蔽回车关闭程序功能
}

void CPosdllDemoDlg::OnBnClickedBtnseturl()
{
	// TODO: 在此添加控件通知处理程序代码
	//在这里写代码，设置打印线程中使用的URL
	//比如说写个对话框之类的，在对话框里面输入新的URL
	CString strUrl = Printer_Lsh::PrintWorker::GetCurURL();//得到现在使用的URL

	//TODO: Printer_Lsh::PrintWorker::UpdateURL(_T("在这里替换旧的URL"));
}

void CPosdllDemoDlg::OnBnClickedBtnlog()
{
	//打开日志对话框
	//size_t count = Printer_Lsh::PrintWorker::GetLogCount();
	//for (int i = 0; i < count; ++i){
	//	//循环得到日志
	//	Printer_Lsh::PrintRecorder recorder = Printer_Lsh::PrintWorker::GetLog(i);
	//	//AddString(recorder.);这个可以添加在列表控件中
	//}
}
