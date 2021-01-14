// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F8C3425C_BF4C_11D1_B8AE_000000000000__INCLUDED_)
#define AFX_MAINFRM_H__F8C3425C_BF4C_11D1_B8AE_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "MDIClientWnd.h"

HRESULT CompactDatabase(LPCTSTR src, LPCTSTR dest);

class CGraphPropSheet;
class CReportPropSheet;
class CBatchPropSheet;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CGraphPropSheet *graphProperties;
	// added for batch
	CReportPropSheet *reportProperties;
	CBatchPropSheet *batchProperties;
	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	int RemoveWxDuplicates();
	void SaveItAs();
	void StatusBarMessage(CString msg);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CStatusBar  m_wndStatusBar;
protected:  // control bar embedded members
	CToolBar    m_wndToolBar;
	CMDIClientWnd m_wndMDIClient;
	CString m_sStatusBarString;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowCloseallReportsandgraphs();
	afx_msg void OnWindowCloseallWindows();
	afx_msg void OnOptionsDisplayoptons();
	// added for batch
	afx_msg void OnOptionsReportOptions();
	afx_msg void OnOptionsBatchOptions1();
	afx_msg void OnOptionsBatchOptions2();
	afx_msg void OnClose();
	afx_msg void OnDataCompact();
	afx_msg void OnFileSaveAsFfp();
	afx_msg void OnUpdateOptionsDisplayoptons(CCmdUI* pCmdUI);
	// added for batch
	afx_msg void OnUpdateOptionsReportOptions(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsBatchOptions(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUtilitiesSnowflag();
	afx_msg void OnUtilitiesFillSnowflag();
	afx_msg void OnUtilitiesSetDailyObs();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F8C3425C_BF4C_11D1_B8AE_000000000000__INCLUDED_)