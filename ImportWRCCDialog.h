#if !defined(AFX_ImportWRCCDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_ImportWRCCDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ImportWRCCDialog.h : header file
//

#include "afxcmn.h"


/////////////////////////////////////////////////////////////////////////////
// CImportWRCCDialog dialog

class CFireplusSet;


class CImportWRCCDialog : public CDialog
{
// Construction
public:
	
	
    CImportWRCCDialog(CWnd* pParent /*=NULL*/,   CString _message, CString _wimsStation, CString _wrccStation);
// Dialog Data
	//{{AFX_DATA(CImportWRCCDialog)
	enum { IDD = IDD_IMPORT_WRCC_DIALOG };
		// NOTE: the ClassWizard will add data members here

	CString	message;
	CString	wimsStation;
	CString	wrccStation;
	
void ConfigureControls();

	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportWRCCDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int importWRCC;
	CButton *buttonWIMS;
	CButton *buttonWRCC;
	CStatic *messageBox;


};

#endif // !defined(AFX_ImportWRCCDIALOG_H__E1F21140_09D8_11D3_BD5A_00A0CC24A3EB__INCLUDED_)
