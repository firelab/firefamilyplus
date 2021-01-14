#if !defined(AFX_RICHVIEW_H__565CB532_3783_11D2_B8D2_000000000000__INCLUDED_)
#define AFX_RICHVIEW_H__565CB532_3783_11D2_B8D2_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RichView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRichView view

class CRichView : public CRichEditView
{
protected: // create from serialization only
	CRichView();
	DECLARE_DYNCREATE(CRichView)

// Attributes
public:

// Operations
public:

// Overrides
	//void OnFilePrint();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnFilePrint();
	//}}AFX_VIRTUAL
	virtual void OnPrinterChanged( const CDC& dcPrinter );
	//int GetNumPages(CDC* pDC, CPrintInfo* pInfo);
	UINT m_nLastPage;//fix printing bug

// Implementation
public:
	void SaveTheDamnThing();
	void PrintTheDamnThing();
	virtual ~CRichView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
	//{{AFX_MSG(CRichView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHVIEW_H__565CB532_3783_11D2_B8D2_000000000000__INCLUDED_)
