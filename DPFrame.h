#if !defined(AFX_DPFRAME_H__CED90D03_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_DPFRAME_H__CED90D03_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DPFrame.h : header file
//
#include "DPDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CDPFrame frame

class CDPFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDPFrame)
protected:
	CDPFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter2;
	CSplitterWnd m_wndSplitter3;

// Operations
public:
	CDPDialog * dpDialog;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPFrame)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDPFrame();

	// Generated message map functions
	//{{AFX_MSG(CDPFrame)
	afx_msg void OnFilePrintGraphClasspercentages();
	afx_msg void OnFilePrintGraphPercentiles();
	afx_msg void OnFilePrintGraphProbabilities();
	afx_msg void OnFileSaveGraphClasspercentages();
	afx_msg void OnFileSaveGraphPercentiles();
	afx_msg void OnFileSaveGraphProbabilities();
	afx_msg void OnToolsClassdefinitions();
	afx_msg void OnFileSaveReport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPFRAME_H__CED90D03_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
