#if !defined(AFX_WINDROSEVIEW_H__7F113BD3_476C_11D2_B8D7_000000000000__INCLUDED_)
#define AFX_WINDROSEVIEW_H__7F113BD3_476C_11D2_B8D7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WindRoseView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWindRoseView view

#define WRSPEEDS	10     // 10 speed bins for wind rose
#define WRDIRS		16	  // 16 direction bins for wind rose


class CWindRoseView : public CScrollView
{
protected:
	CWindRoseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWindRoseView)

// Attributes
public:
	//CBitmap pcardBitmap;
	//CBitmap stoplightBitmap;
	//CBitmap checkBitmap;

// Operations
public:
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindRoseView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL
	void DrawToDCRegion(CDC *pDC, CRect rect);
    float Round(const float &number,const int num_digits);


// Implementation
protected:
	virtual ~CWindRoseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CWindRoseView)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDROSEVIEW_H__7F113BD3_476C_11D2_B8D7_000000000000__INCLUDED_)
