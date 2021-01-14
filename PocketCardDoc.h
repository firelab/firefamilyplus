#if !defined(AFX_POCKETCARDDOC_H__7F113BD5_476C_11D2_B8D7_000000000000__INCLUDED_)
#define AFX_POCKETCARDDOC_H__7F113BD5_476C_11D2_B8D7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PocketCardDoc.h : header file
//
class CClimAnalysis;
/////////////////////////////////////////////////////////////////////////////
// CPocketCardDoc document

class CPocketCardDoc : public CDocument
{
protected:
	CPocketCardDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPocketCardDoc)

// Operations
public:
	CClimAnalysis *m_pAnalysis;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPocketCardDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void DeleteContents();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPocketCardDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPocketCardDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POCKETCARDDOC_H__7F113BD5_476C_11D2_B8D7_000000000000__INCLUDED_)
