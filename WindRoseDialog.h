#if !defined(AFX_WINDROSEDIALOG_H__47C691A0_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_)
#define AFX_WINDROSEYDIALOG_H__47C691A0_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_

#include "afxwin.h"
#include "runsave.h"
#include "fireplusset.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindRoseDialog.h : header file
//

#define WRSPEEDS	10     // 10 speed bins for wind rose
#define WRDIRS		16	  // 16 direction bins for wind rose
#define WRHOURS		6	// 6 hourly period bins
/////////////////////////////////////////////////////////////////////////////
// CWindRoseDialog dialog

class CWindRoseDialog : public CDialog
{
// Construction
public:
	
	CWindRoseDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWindRoseDialog)
	enum { IDD = IDD_WINDROSEDIALOG };
	

	//}}AFX_DATA

    CFireplusSet *fpSet;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindRoseDialog)
	public:
	BOOL incGraph;
	BOOL incTable;
	BOOL incWinds;

	BOOL	wind_0_4;
	BOOL	wind_4_8;
	BOOL	wind_8_12;
	BOOL	wind_12_16;
	BOOL	wind_16_20;
	BOOL	wind_20_0;

	BOOL    splitGraphs;
	
	int     windsType;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDatabase *pDB;
	
	// Generated message map functions
	//{{AFX_MSG(CWindRoseDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedWinds48();
	afx_msg void OnBnClickedWinds04();

	afx_msg void OnBnClickedGust();
	afx_msg void OnBnClickedAvg();
	afx_msg void OnBnClickedBoth();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDROSEDIALOG_H__47C691A0_882C_11D2_BD5A_8F1833BA4F3D__INCLUDED_)
