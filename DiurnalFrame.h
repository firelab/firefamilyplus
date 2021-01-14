#if !defined(AFX_DIURNALFRAME_H__64D3C9E4_E459_11D3_AB9F_005004BF64C9__INCLUDED_)
#define AFX_DIURNALFRAME_H__64D3C9E4_E459_11D3_AB9F_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiurnalFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiurnalFrame frame

class CDiurnalFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDiurnalFrame)
protected:
	CDiurnalFrame();           // protected constructor used by dynamic creation
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

// Attributes
public:
	CSplitterWnd m_wndSplitter;
	CString varName;
	int varID;

// Operations
public:
	void MaximizePane(CView *view);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiurnalFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDiurnalFrame();

	// Generated message map functions
	//{{AFX_MSG(CDiurnalFrame)
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintGraph();
	afx_msg void OnFileSaveGraph();
	afx_msg void OnFileSaveDiurnalReport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIURNALFRAME_H__64D3C9E4_E459_11D3_AB9F_005004BF64C9__INCLUDED_)
