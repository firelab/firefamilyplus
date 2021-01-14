//{{AFX_INCLUDES()
//#include "pvnumeric.h"
//}}AFX_INCLUDES
#include "afxcmn.h"
#if !defined(AFX_YEARSPAGE_H__53A132C5_6E7B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_YEARSPAGE_H__53A132C5_6E7B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// YearsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CYearsPage dialog

class CYearsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CYearsPage)

// Construction
public:
	void SaveData();
	CYearsPage(int *_start = NULL, int *_end = NULL);
	~CYearsPage();
	int *iStart;
	int *iEnd;
// Dialog Data
	//{{AFX_DATA(CYearsPage)
	enum { IDD = IDD_YEARSPAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CYearsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CYearsPage)
	//afx_msg void OnLostFocusEventEndYear();
	//afx_msg void OnLostFocusEventStartYear();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSpinButtonCtrl m_spinStart;
	CSpinButtonCtrl m_spinEnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YEARSPAGE_H__53A132C5_6E7B_11D2_B8E5_000000000000__INCLUDED_)
