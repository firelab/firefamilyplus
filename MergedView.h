#if !defined(AFX_MERGEDVIEW_H__E101BB80_A5D2_11D5_ABA1_005004BF64C9__INCLUDED_)
#define AFX_MERGEDVIEW_H__E101BB80_A5D2_11D5_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergedView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMergedView view

class CMergedView : public CView
{
protected:
	CMergedView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMergedView)

// Attributes
public:

// Operations
public:
	void BuildData();
	int optionType;
	long recsUsed;
	virtual void DrawToDCRegion(CDC *destDC, CRect rect);
	double IdealFromYMax(double ymax);
	//void DrawPathOutline(const COLORREF& c, CDC& dc, bool Round, int PenSize);
	CString yrsTitle;
	CString staTitle;
	int year;
	int styles[MAXMERGES];
	int widths[MAXMERGES];
	int vars[MAXMERGES];
	CString varNames[MAXMERGES];
	COLORREF colors[MAXMERGES];
	double yMax[MAXMERGES];
	double yMin[MAXMERGES];
	double *vals[MAXMERGES];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergedView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergedView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergedView)
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnFilePrint();
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDVIEW_H__E101BB80_A5D2_11D5_ABA1_005004BF64C9__INCLUDED_)
