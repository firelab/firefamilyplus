#if !defined(AFX_DPDOC_H__CED90D08_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_DPDOC_H__CED90D08_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DPDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDPDoc document
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"

#define MAXDPS 9

class CDPDoc : public CDocument
{
protected:
	CDPDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDPDoc)
	
// Attributes
public:
	double lowers[MAXDPS];
	int nDP;
	long nd;
	long nfd;
	long nlfd;
	long nmfd;
	long cd[MAXDPS];
	long cfd[MAXDPS];
	long clfd[MAXDPS];
	long cmfd[MAXDPS];
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDPDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool goUp;
	void SetDefaults();
	void CalcClasses();
	CFireStats * fires;
	virtual ~CDPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDPDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DPDOC_H__CED90D08_86DA_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
