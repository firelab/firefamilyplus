#if !defined(AFX_POCKETCARDVIEW_H__7F113BD3_476C_11D2_B8D7_000000000000__INCLUDED_)
#define AFX_POCKETCARDVIEW_H__7F113BD3_476C_11D2_B8D7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PocketCardView.h : header file
//
class CClimAnalysis;
/////////////////////////////////////////////////////////////////////////////
// CPocketCardView view

class CPocketCardView : public CScrollView
{
//protected:
public:
	CPocketCardView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPocketCardView)

// Attributes
public:
	//CBitmap pcardBitmap;
	//CBitmap stoplightBitmap;
	//CBitmap checkBitmap;

// Operations
public:
	CClimAnalysis *m_pAnalysis;
	int m_years;
	long m_nObs;
	CString string4;
	CString string3;
	CString string2;
	CString string1;
	bool extremeIsHigh;
	void GetLine(int lineNo, CString src, char * trg);
	int GetLineCount(CString str);
	void Star(CDC *pDC, int x, int y, int starSize);
	double pcntile97;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPocketCardView)
	//protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL
	void DrawToDCRegion(CDC *pDC, CRect rect);
	double IdealFromYMax(double ymax);

// Implementation
	afx_msg void OnFilePrint();
	afx_msg void OnFileSaveAs();
	virtual ~CPocketCardView();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPocketCardView)
	//afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	int SaveToFile(CString _saveNameStr, CRect *_outRect);
	void PreDrawCalcs();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POCKETCARDVIEW_H__7F113BD3_476C_11D2_B8D7_000000000000__INCLUDED_)
