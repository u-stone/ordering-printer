// PosdllDemoDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_POSDLLDEMODLG_H__F2D1A6BE_D5B5_41EA_8BD1_A9EDB256EAE4__INCLUDED_)
#define AFX_POSDLLDEMODLG_H__F2D1A6BE_D5B5_41EA_8BD1_A9EDB256EAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPosdllDemoDlg dialog

class CPosdllDemoDlg : public CDialog
{
// Construction
public:
	CPosdllDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPosdllDemoDlg)
	enum { IDD = IDD_POSDLLDEMO_DIALOG };
	CComboBox	m_ctrlPageWidth;
	CEdit	m_ctrlDrvName;
	CIPAddressCtrl	m_ctrlIPAddr;
	CButton	m_ctrlClosePort;
	CButton	m_ctrlPrint;
	CButton	m_ctrlQueryStatus;
	CComboBox	m_ctrlLPTName;
	CComboBox	m_ctrlParity;
	CComboBox	m_ctrlFlowControl;
	CComboBox	m_ctrlStopBits;
	CComboBox	m_ctrlBaudrate;
	CComboBox	m_ctrlDataBits;
	CComboBox	m_ctrlComName;
	int		m_PortType;
	int		m_ModeSelect;
	CString	m_strMsg;
	CString	m_strDrvName;
	CString strNetPortName;
	int		m_iTphWidth;

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPosdllDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPosdllDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpenPort();
	afx_msg void OnPortCom();
	afx_msg void OnPortLpt();
	afx_msg void OnPortUsb();
	afx_msg void OnPortNet();
	afx_msg void OnPortDrv();
	afx_msg void OnQueryStatus();
	afx_msg void OnPrint();
	afx_msg void OnClosePort();
	afx_msg void OnDestroy();
	afx_msg void OnModeStandard();
	afx_msg void OnModePage();
	afx_msg void OnSaveToTxt();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnseturl();
	afx_msg void OnBnClickedBtnlog();
	BOOL CPosdllDemoDlg::JustOpenPort();
	afx_msg LRESULT OnUpdateLog(WPARAM wParam, LPARAM lParam);

	void ClickHelper(int index);
	//Remark: bToLocal为FALSE表示用本地信息更新界面， 为TRUE表示将界面信息存到本地
	void UpdateUIData(BOOL bToLocal = FALSE);
	// 接口地址
	CString m_editUrl;
	//打印机信息配置
	CString mPrintServerIP;
	CString mPrintServerPort;
	CString mPrintIMEI;
	CString mPrintPW;
	// 日志listbox
	CListBox m_loglistbox;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSDLLDEMODLG_H__F2D1A6BE_D5B5_41EA_8BD1_A9EDB256EAE4__INCLUDED_)

