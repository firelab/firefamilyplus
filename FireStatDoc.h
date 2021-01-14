#if !defined(AFX_FIRESTATDOC_H__AA2A8F35_7704_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIRESTATDOC_H__AA2A8F35_7704_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireStatDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireStatDoc document

class CFireStatDoc : public CDocument
{
protected:
	CFireStatDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFireStatDoc)

// Attributes
public:
	//CDatabase * pDB;
	void SetFires(CFireStats *_fires);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireStatDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	CFireStats * fires;
	virtual ~CFireStatDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFireStatDoc)
	afx_msg void OnExportPercentiles();
	afx_msg void OnViewHeader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDataAddtocandidateslist();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIRESTATDOC_H__AA2A8F35_7704_11D2_B8E5_000000000000__INCLUDED_)
