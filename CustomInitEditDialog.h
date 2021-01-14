//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined(AFX_CustomInitEditDIALOG_995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_CustomInitEditDIALOG_H__995B0AE0_FC8B_11D2_BD5A_00A0CC24A3EB__INCLUDED_
#endif

#include "ExtCheckBox.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include "numspinctrl.h"
#include "afxwin.h"

#include "fireplusset.h"
#include "CustomInitset.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomInitEditDialog.h : header file
//
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001



/////////////////////////////////////////////////////////////////////////////
// CCustomInitEditDialog dialog

class CCustomInitEditDialog : public CDialog
{
public:
	
	CCustomInitSet * records;
	
	CCustomInitEditDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL,CCustomInitSet *_records = NULL, int _editYear = 0);   // standard constructor
	
	int Year;

	CFireplusSet *fpSet;
	
	CDateTimeCtrl	greenupDate;
    
	CSpinButtonCtrl m_spinKBDI;

	CEdit			m_editFFMC;
	CNumSpinCtrl m_spinFFMC;

	CEdit		m_editDMC;
	CNumSpinCtrl m_spinDMC;

	CEdit		m_editDC;
    CNumSpinCtrl m_spinDC;

	CEdit			m_editFM1000;
	CNumSpinCtrl	m_numSpinFM1000;

    CSpinButtonCtrl	m_spinYear;

	

	CButton 
			*EditButton,
			*cancelButton;

	enum { IDD = IDD_CUSTOMINIT_EDIT_DIALOG };
	
  

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomInitEditDialog)
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
afx_msg void OnSize(UINT nType, int cx, int cy);
// Implementation
	protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	virtual void OnOK();
	
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CDateTimeCtrl m_startUpDay;
};
