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

HANDLE g_hComm	= INVALID_HANDLE_VALUE;//�˿ھ��
bool IsPrinter	= false;//�ж��Ƿ��������������ӡ
int  m_iMode	= 0;//ҳģʽ���׼ģʽ
bool bSaveToTxt = false;//�Ƿ�ѡ�����ݱ��浽�ļ�Test.txt������˿��·�

extern HMODULE				g_hPosdll;//��̬����

extern POS_Open				VC_POS_Open;//�򿪶˿�
extern POS_Close			VC_POS_Close;//�رն˿�
extern POS_RTQueryStatus	VC_POS_RTQueryStatus;//ʵʱ״̬��ѯ
extern POS_NETQueryStatus	VC_POS_NETQueryStatus;//����ӿڵ�״̬��ѯ
extern POS_StartDoc			VC_POS_StartDoc;//����һ���ĵ�
extern POS_EndDoc			VC_POS_EndDoc;//����һ���ĵ�
extern POS_BeginSaveFile	VC_POS_BeginSaveFile;//�����������ݵ��ļ�
extern POS_EndSaveFile		VC_POS_EndSaveFile;//�����������ݵ��ļ�
extern POS_GetVersionInfo	VC_POS_GetVersionInfo;//��ȡ��ǰ��̬��汾


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
		// �����̬�����ʧ��, ���˳�
		AfxMessageBox("���ض�̬��ʧ�ܣ�");
		CDialog::OnCancel();
		return false;
	}
	/*��ʼ��������ʾ*/
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
	str = "һ���������汾��V";
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

void CPosdllDemoDlg::OnPortCom() //�˿�ѡ��Ϊ����
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

	SetDlgItemText(IDC_EDIT_STATUS,"�˿�ѡ��Ϊ����");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortLpt() //�˿�ѡ��Ϊ����
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

	SetDlgItemText(IDC_EDIT_STATUS,"�˿�ѡ��Ϊ����");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortUsb() //�˿�ѡ��ΪUSB��
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

	SetDlgItemText(IDC_EDIT_STATUS,"�˿�ѡ��ΪUSB��");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortNet() //�˿�ѡ��Ϊ����ӿ�
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

	SetDlgItemText(IDC_EDIT_STATUS,"�˿�ѡ��Ϊ����ӿ�");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnPortDrv() //�˿�ѡ��Ϊ��������
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

	SetDlgItemText(IDC_EDIT_STATUS,"�˿�ѡ��Ϊ��������");
	VC_POS_Close();
	m_ctrlQueryStatus.EnableWindow(false);
	m_ctrlPrint.EnableWindow(false);
	m_ctrlClosePort.EnableWindow(false);
	GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
}

void CPosdllDemoDlg::OnOpenPort() //�򿪶˿�
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	if(0 == m_PortType) //Ҫ�򿪵Ķ˿�Ϊ����
	{	
		CString strPortName;

		int iBaudrate;
		int iDataBits;
		int iStopBits;
		int iParity;
		int iFlowControl;

		m_ctrlComName.GetLBText(m_ctrlComName.GetCurSel(),strPortName);
		iDataBits = m_ctrlDataBits.GetCurSel()+7;//����Ϊ0��ʾ����λΪ7��Ϊ1��ʾ8����������ĵ�
		int tmp = m_ctrlStopBits.GetCurSel();//��������ĵ�
		if(0 == tmp)//ֹͣλΪ1
		{
			iStopBits = 0;
		}
		if(1 == tmp)//ֹͣλΪ2
		{
			iStopBits = 2;
		}
		int temp = m_ctrlParity.GetCurSel();//��������ĵ�
		if(0 == temp)//żУ��
		{
			iParity = 2;
		}
		else if(1 == temp)//��У��
		{
			iParity = 1;
		}
		else//��У��
		{
			iParity = 0;
		}
		int iRet = m_ctrlFlowControl.GetCurSel();//��������ĵ�
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
	
	if(1 == m_PortType) //Ҫ�򿪵Ķ˿�Ϊ����
	{
		IsPrinter = false;

		CString strPortName;
		m_ctrlLPTName.GetLBText(m_ctrlLPTName.GetCurSel(),strPortName);
		g_hComm = VC_POS_Open(strPortName,0,0,0,0,POS_OPEN_PARALLEL_PORT);
	}

	if(2 == m_PortType) //Ҫ�򿪵Ķ˿�ΪUSB��
	{
		IsPrinter = false;

		g_hComm = VC_POS_Open("BYUSB-0",0,0,0,0,POS_OPEN_BYUSB_PORT);
	}

	if(3 == m_PortType) //Ҫ�򿪵Ķ˿�Ϊ����ӿ�
	{
		if(m_ctrlIPAddr.IsBlank())
		{
			m_strMsg = "IP��ַ����Ϊ�գ�";
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

	if(4 == m_PortType) //Ҫ�򿪵Ķ˿�Ϊ��������
	{
		if("" == m_strDrvName)
		{
			m_strMsg = "��������������Ϊ�գ�";
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

	if (g_hComm != INVALID_HANDLE_VALUE) //�жϴ򿪶˿ں�������ֵ
	{
		GetDlgItem(IDC_OPEN_PORT)->EnableWindow(false);
		m_strMsg = "�˿ڴ򿪳ɹ���";
		m_ctrlPrint.EnableWindow(true);
		m_ctrlClosePort.EnableWindow(true);
		if(m_PortType != 1 && m_PortType != 4)//���ں���������ӿڲ�֧��״̬��ѯ
		{
			m_ctrlQueryStatus.EnableWindow(true);
		}
	}
	else
	{
		m_strMsg = "�˿ڴ�ʧ�ܣ�";
		m_ctrlQueryStatus.EnableWindow(false);
		m_ctrlPrint.EnableWindow(false);
		m_ctrlClosePort.EnableWindow(false);
	}

	UpdateData(false);
	m_strMsg = "";
}



void CPosdllDemoDlg::OnQueryStatus() //��ѯ״̬
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
		m_strMsg = "��ѯ״̬ʧ�ܣ�";
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
			m_strMsg = "һ��������";
		}
		else
		{
			m_strMsg = "";
			if (iBits[0] == 0)
			{
				m_strMsg += "��Ǯ��򿪣�";
			}
			
			if (iBits[1] == 1)
			{
				m_strMsg += "��ӡ���ѻ���";
			}
			
			if (iBits[2] == 1)
			{
				m_strMsg += "�ϸǴ򿪣�";
			}

			if (iBits[3] == 1)
			{
				m_strMsg += "���ڽ�ֽ��";
			}
			
			if (iBits[4] == 1)
			{
				m_strMsg += "��ӡ������";
			}
			
			if (iBits[5] == 1)
			{
				m_strMsg += "�е�����";
			}
			
			if (iBits[6] == 1)
			{
				m_strMsg += "ֽ������";
			}
			
			if (iBits[7] == 1)
			{
				m_strMsg += "ȱֽ��";
			}
		}
	}

	UpdateData(false); 
}

void CPosdllDemoDlg::OnPrint() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(IsPrinter)  //���ѡ�������������ӡ
	{
		VC_POS_StartDoc();
	}

	if (g_pi.bPrintThreadWorking == FALSE){//��ֻ֤�ܿ�һ����ӡ�߳�
		AfxBeginThread((AFX_THREADPROC)Printer_Lsh::PrintWorker::worker, &g_pi);
	}

	if(IsPrinter) //���ѡ�������������ӡ
	{
		VC_POS_EndDoc();
	}

	UpdateData(false);
	return;
}

void CPosdllDemoDlg::OnClosePort() //�رն˿�
{
	// TODO: Add your control notification handler code here
	int nRet;
	nRet = VC_POS_Close();
	if(POS_FAIL == nRet)
	{
		m_strMsg = "�˿ڹر�ʧ�ܣ�";
	}
	else if(POS_ERROR_INVALID_HANDLE == nRet)
	{
		m_strMsg = "�˿ڹر�ʧ�ܣ�";
	}
	else
	{
		GetDlgItem(IDC_OPEN_PORT)->EnableWindow(true);
		m_strMsg = "�˿ڹرճɹ���";
		m_ctrlQueryStatus.EnableWindow(false);
		m_ctrlPrint.EnableWindow(false);
		m_ctrlClosePort.EnableWindow(false);		
	}

	UpdateData(false);
}

void CPosdllDemoDlg::OnDestroy() //���ڹر�
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

void CPosdllDemoDlg::OnModeStandard() //��׼ģʽ
{
	// TODO: Add your control notification handler code here
	m_iMode = 0;
}

void CPosdllDemoDlg::OnModePage() //ҳģʽ
{
	// TODO: Add your control notification handler code here
	m_iMode = 1;
}

void CPosdllDemoDlg::OnSaveToTxt() //���ݱ��浽�ļ�Test.txt������˿��·�
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
	//����F1���Զ�Ѱ�Ұ����ļ�
	return 0;
}

void CPosdllDemoDlg::OnOK() 
{
	// TODO: Add extra validation here
	//CDialog::OnOK();
	//���λس��رճ�����
}

void CPosdllDemoDlg::OnBnClickedBtnseturl()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//������д���룬���ô�ӡ�߳���ʹ�õ�URL
	//����˵д���Ի���֮��ģ��ڶԻ������������µ�URL
	CString strUrl = Printer_Lsh::PrintWorker::GetCurURL();//�õ�����ʹ�õ�URL

	//TODO: Printer_Lsh::PrintWorker::UpdateURL(_T("�������滻�ɵ�URL"));
}

void CPosdllDemoDlg::OnBnClickedBtnlog()
{
	//����־�Ի���
	//size_t count = Printer_Lsh::PrintWorker::GetLogCount();
	//for (int i = 0; i < count; ++i){
	//	//ѭ���õ���־
	//	Printer_Lsh::PrintRecorder recorder = Printer_Lsh::PrintWorker::GetLog(i);
	//	//AddString(recorder.);�������������б�ؼ���
	//}
}
