//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_STATIONDIALOG_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_)
#define AFX_STATIONDIALOG_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_
#include "UGCtrl.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Stationdialog.h : header file
//

#include "SIGStationSet.h"
#include "Fireplusset.h"

/////////////////////////////////////////////////////////////////////////////
// CStationdialog dialog

class CStationdialog : public CDialog
{
// Construction
public:
	void RedisplayTable();
	CSIGStationSet * stations;

	CStationdialog(CWnd* pParent = NULL, CSIGStationSet * _stations = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor
    CFireplusSet *fpSet;

// Dialog Data
	//{{AFX_DATA(CStationdialog)
	enum { IDD = IDD_STATIONDIALOG };
	CButton	editButton;
	CButton	deleteButton;
	//}}AFX_DATA
	CUGCtrl m_grid;
	bool showall;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationdialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStationdialog)
	afx_msg void OnStationdelete();
	afx_msg void OnStationedit();
    afx_msg void OnShowAllButton();
	afx_msg void OnStationnew();
	afx_msg void OnStationExport();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONDIALOG_H__693D7B82_548A_11D2_B8DD_000000000000__INCLUDED_)
