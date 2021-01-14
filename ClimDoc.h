#if !defined(AFX_CLIMDOC_H__87F03BC4_DDD7_11D1_B8B9_000000000000__INCLUDED_)
#define AFX_CLIMDOC_H__87F03BC4_DDD7_11D1_B8B9_000000000000__INCLUDED_

#include "fireplusSet.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClimDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClimDoc document

class CClimDoc : public CDocument
{
//protected:
public:
	CClimDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CClimDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClimDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	CFireplusSet *m_pSet;
	virtual ~CClimDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CClimDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIMDOC_H__87F03BC4_DDD7_11D1_B8B9_000000000000__INCLUDED_)
