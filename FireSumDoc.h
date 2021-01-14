#if !defined(AFX_FIRESUMDOC_H__53A132C6_6E7B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIRESUMDOC_H__53A132C6_6E7B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireSumDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireSumDoc document

class CFireSumDoc : public CDocument
{
protected:
	CFireSumDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFireSumDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireSumDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	CFireSummary * summary;
	virtual ~CFireSumDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFireSumDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileExportgraphdata();
	afx_msg void OnOptionsViewclassdefinitions();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRESUMDOC_H__53A132C6_6E7B_11D2_B8E5_000000000000__INCLUDED_)
