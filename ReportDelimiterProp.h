//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#include "colorbutton.h"
#include "afxcmn.h"
#if !defined(AFX_REPORTDELIMITERPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_REPORTDELIMITERPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ReportDelimiterProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReportDelimiterProp dialog

class CReportDelimiterProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CReportDelimiterProp)

// Construction
public:
	CReportDelimiterProp();
	~CReportDelimiterProp();
	CReportOptionsSet * reportSet;
	void SetReportSet(CReportOptionsSet *set);
	
	void ReadData();
	
	void SaveData();

// Dialog Data
	//{{AFX_DATA(CReportDelimiterProp)
	enum { IDD = IDD_PROPPAGE_DELIMITER };
	CButton	UseSpace;
	CButton	UseTabs;
	CButton	UseComma;
	//CButton	UseColon;
	//CButton UseSemicolon;
	CButton UsePipe;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CReportDelimiterProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CReportDelimiterProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTDelimiterPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_)
