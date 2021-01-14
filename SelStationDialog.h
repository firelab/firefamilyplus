//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_SelStationDialog_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_)
#define AFX_SelStationDialog_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_
#include "UGCtrl.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelStationDialog.h : header file
//

#include "SIGStationSet.h"
#include "Fireplusset.h"

/////////////////////////////////////////////////////////////////////////////
// CSelStationDialog dialog

class CSelStationDialog : public CDialog
{
// Construction
public:
	void RedisplayTable();
	CSIGStationSet * stations;

	CSelStationDialog(CWnd* pParent = NULL, CSIGStationSet * _stations = NULL);   // standard constructor
    CString selectedID;

// Dialog Data
	//{{AFX_DATA(CSelStationDialog)
	enum { IDD = IDD_SEL_STATION_DIALOG };
	
	

	//}}AFX_DATA
	CUGCtrl m_grid;

    bool showall;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelStationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelStationDialog)
	
	virtual BOOL OnInitDialog();
	virtual void OnOK();
    afx_msg void OnShowAllButton();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SelStationDialog_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_)
