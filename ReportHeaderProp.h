//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#include "colorbutton.h"
#include "afxcmn.h"
#if !defined(AFX_REPORTHEADERPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_REPORTHEADERPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ReportHeaderProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportHeaderProp dialog

class CReportOptionsSet;

class CReportHeaderProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CReportHeaderProp)

// Construction
public:
	CReportHeaderProp();
	~CReportHeaderProp();
	CReportOptionsSet * reportSet;
	void SetReportSet(CReportOptionsSet *set);
	
	void ReadData();
	
	void SaveData();

// Dialog Data
	//{{AFX_DATA(CReportHeaderProp)
	enum { IDD = IDD_PROPPAGE_RHEADER };
	CButton	WorkingSet;
	CButton	AnnualFilter;
	CButton	SIGStation;
	//CButton	Addtl;
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CReportHeaderProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CReportHeaderProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTHEADERPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_)
