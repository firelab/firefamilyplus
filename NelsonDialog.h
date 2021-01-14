//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_NelsonDialog_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_)
#define AFX_NelsonDialog_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_
#include "UGCtrl.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NelsonDialog.h : header file
//

#include "Fireplusset.h"

/////////////////////////////////////////////////////////////////////////////
// CNelsonDialog dialog

class CNelsonDialog : public CDialog
{
// Construction
public:
	CNelsonDialog();   // standard constructor
    int start1h;
	int start10h;
	int start100h;
	int start1000h;

// Dialog Data
	//{{AFX_DATA(CNelsonDialog)
	enum { IDD = IDD_NELSON_DIALOG };
	
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNelsonDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNelsonDialog)
	
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NelsonDialog_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_)
