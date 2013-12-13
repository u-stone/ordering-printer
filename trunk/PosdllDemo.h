// PosdllDemo.h : main header file for the POSDLLDEMO application
//

#if !defined(AFX_POSDLLDEMO_H__B84F13D3_4BD8_45C3_AACB_9B3A3F0993CA__INCLUDED_)
#define AFX_POSDLLDEMO_H__B84F13D3_4BD8_45C3_AACB_9B3A3F0993CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPosdllDemoApp:
// See PosdllDemo.cpp for the implementation of this class
//

class CPosdllDemoApp : public CWinApp
{
public:
	CPosdllDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPosdllDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPosdllDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSDLLDEMO_H__B84F13D3_4BD8_45C3_AACB_9B3A3F0993CA__INCLUDED_)
