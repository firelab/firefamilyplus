#if !defined(AFX_MERGEDFRAME_H__E101BB81_A5D2_11D5_ABA1_005004BF64C9__INCLUDED_)
#define AFX_MERGEDFRAME_H__E101BB81_A5D2_11D5_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergedFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergedFrame frame

class CMergedFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMergedFrame)
protected:
	CMergedFrame();           // protected constructor used by dynamic creation
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

// Attributes
public:
	int year;
//	CString styles[4];
//	int widths[4];
	int vars[MAXMERGES];
	double yMax[MAXMERGES];
	double yMin[MAXMERGES];
//	CString varNames[4];
//	COLORREF colors[4];

// Operations
public:
	void ReconfigureView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergedFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergedFrame();

	// Generated message map functions
	//{{AFX_MSG(CMergedFrame)
	afx_msg void OnOptionsDisplayoptons();
	afx_msg void OnUpdateOptionsDisplayoptons(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDFRAME_H__E101BB81_A5D2_11D5_ABA1_005004BF64C9__INCLUDED_)
