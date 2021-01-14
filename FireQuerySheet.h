#if !defined(AFX_FIREQUERYSHEET_H__D7711A67_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIREQUERYSHEET_H__D7711A67_675B_11D2_B8E5_000000000000__INCLUDED_

#include "FireAgencySet.h"	// Added by ClassView
#include "FireRegionSet.h"	// Added by ClassView
#include "FireUnitSet.h"	// Added by ClassView
#include "FireSubunitSet.h"	// Added by ClassView
#include "FireAssocSet.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireQuerySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireQuerySheet

class CFireQuerySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFireQuerySheet)

// Construction
public:
	CFireQuerySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		CDatabase *_pDB = NULL, CString _staStr = "", int *start = NULL, 
		int *end = NULL, CFireplusSet *_fpSet = NULL, CFireplusSet *_fpSet2 = NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFireQuerySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	afx_msg void OnEditButton( );
	afx_msg void OnViewButton( );


// Implementation
public:
	CFireplusSet * fpSet;
	CFireplusSet * fpSet2;

	CYearsPage * yearsPage;
	int agencyPages;
	CButton editButton;
	CButton viewButton;
	CButton DOIbutton;
    
	
	

	CFireAssocSet associations;
	CString staStr;
	CFireSubunitSet subUnits;
	CFireUnitSet units;
	CFireRegionSet regions;
	CFireAgencySet agencies;
	virtual ~CFireQuerySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFireQuerySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREQUERYSHEET_H__D7711A67_675B_11D2_B8E5_000000000000__INCLUDED_)
