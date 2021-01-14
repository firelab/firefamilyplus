#if !defined(AFX_WINDROSEDOC_H__7F113BD5_476C_11D2_B8D7_000000000000__INCLUDED_)
#define AFX_WINDROSEDOC_H__7F113BD5_476C_11D2_B8D7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WindRoseDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWindRoseDoc document


#define WRSPEEDS	10     // 10 speed bins for wind rose
#define WRDIRS		16	  // 16 direction bins for wind rose


class CWindRoseDoc : public CDocument
{
protected:
	CWindRoseDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWindRoseDoc)

// Attributes
public:
	// members

	double percs[WRDIRS+1][WRSPEEDS+1];
	int recs,days,days16;
	double maxPerc;

	CString staStr;
	CString dateStr;
	CString perStr;
	CString hourStr;
	CString extraStr;   // gusts/average/both, etc.
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindRoseDoc)
	public:
	
	virtual void DeleteContents();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CWindRoseDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWindRoseDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDROSEDOC_H__7F113BD5_476C_11D2_B8D7_000000000000__INCLUDED_)
