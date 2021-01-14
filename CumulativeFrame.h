#if !defined(AFX_CUMULATIVEFRAME_H__168D89A0_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_)
#define AFX_CUMULATIVEFRAME_H__168D89A0_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CumulativeFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCumulativeFrame frame

class CCumulativeFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCumulativeFrame)
protected:
	CCumulativeFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	void MaximizePane(CView *view);
	CSplitterWnd m_wndSplitter;
	CStatusBar m_wndStatusBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCumulativeFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCumulativeFrame();

	// Generated message map functions
	//{{AFX_MSG(CCumulativeFrame)
	afx_msg void OnFilePrintPrintgraph();
	afx_msg void OnFilePrint1();
	afx_msg void OnFileSaveBargraph();
	afx_msg void OnFileSavePercentilesgraph();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUMULATIVEFRAME_H__168D89A0_7A07_11D2_BD5A_90F0AB3C243D__INCLUDED_)
