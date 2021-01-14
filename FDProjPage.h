//{{AFX_INCLUDES()
//#include "pvnumeric.h"
//}}AFX_INCLUDES
#include "colorbutton.h"
#include "afxcmn.h"
//#include "SplitterFrame.h"
#if !defined(AFX_FDPROJPAGE_H__C89021A3_2599_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_FDPROJPAGE_H__C89021A3_2599_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FDProjPage.h : header file
//
#include "graphview.h"

/////////////////////////////////////////////////////////////////////////////
// CFDProjPage dialog

class CFDProjPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFDProjPage)

// Construction
public:
	CFDProjPage();
	~CFDProjPage();
	void WriteData(CGraphView *pView = NULL);
	void ReadData(CGraphView *pView = NULL);
	void SaveData();
	bool isQuick;
	CDatabase *m_pDB;
// Dialog Data
	//{{AFX_DATA(CFDProjPage)
	enum { IDD = IDD_PROPPAGE_FX };
	CComboBox	linestyle;
	CColorButton	lineColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFDProjPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFDProjPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSpinButtonCtrl m_spinWidth;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FDPROJPAGE_H__C89021A3_2599_11D4_ABA1_005004BF64C9__INCLUDED_)
