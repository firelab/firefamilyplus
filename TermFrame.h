//#if !defined(AFX_FIRESTATFRAME_H__AA2A8F36_7704_11D2_B8E5_000000000000__INCLUDED_)
//#define AFX_FIRESTATFRAME_H__AA2A8F36_7704_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TermFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireStatFrame frame



class CTermFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTermFrame)
protected:
	CTermFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	void MaximizePane(CView *view);
	CSplitterWnd m_wndSplitter;
	CStatusBar m_wndStatusBar;
	//CFireplusSet *fpSet;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireStatFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTermFrame();

	// Generated message map functions
	//{{AFX_MSG(CFireStatFrame)
	afx_msg void OnFilePrintPrintgraph();
	afx_msg void OnFileSaveGraph();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFilePrintReport();
	//afx_msg void OnViewHeader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

//#endif // !defined(AFX_FIRESTATFRAME_H__AA2A8F36_7704_11D2_B8E5_000000000000__INCLUDED_)
