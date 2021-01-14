//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#include "colorbutton.h"

#if !defined(AFX_FORECASTOPTIONSDIALOG_H__18B4BEC1_9EA4_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_FORECASTOPTIONSDIALOG_H__18B4BEC1_9EA4_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ForecastOptionsDialog.h : header file
//
#include "ForecastDialog.h"
#include "afxcmn.h"
//class CForecastDialog;
/////////////////////////////////////////////////////////////////////////////
// CForecastOptionsDialog dialog

class CForecastOptionsDialog : public CDialog
{
// Construction
//class ForecastOptions;
public:
	ForecastOptions * trg;
	CDatabase * pDB;
	void ConfigureControls();
	CForecastOptionsDialog(CWnd* pParent = NULL, ForecastOptions *_options = NULL, CDatabase *_pDB = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CForecastOptionsDialog)
	enum { IDD = IDD_FORECASTOPTIONS };
	CComboBox	lineStyleCtl;
	CListBox	outputs;
	long	lineWidth;
	OLE_COLOR	lineColor;
	CColorButton	lineColorCtl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CForecastOptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ForecastOptions options;
	// Generated message map functions
	//{{AFX_MSG(CForecastOptionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSame();
	afx_msg void OnPercentiles();
	afx_msg void OnBlank();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinWidth;
	CSpinButtonCtrl m_spinWind;
	CSpinButtonCtrl m_spinRH;
	CSpinButtonCtrl m_spinTemp;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORECASTOPTIONSDIALOG_H__18B4BEC1_9EA4_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
