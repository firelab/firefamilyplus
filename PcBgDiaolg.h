#if !defined(AFX_PCBGDIAOLG_H__62B5ABE0_EF24_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
#define AFX_PCBGDIAOLG_H__62B5ABE0_EF24_11D2_BD5A_00A0CC24A3EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PcBgDiaolg.h : header file
//

class CPocketCardSet;
/////////////////////////////////////////////////////////////////////////////
// CPcBgDiaolg dialog

class CPcBgDiaolg : public CDialog
{
// Construction
public:
	CPocketCardSet * pSet;
	CPcBgDiaolg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPcBgDiaolg)
	enum { IDD = IDD_PCBKGNDDIALOG };
	int		gyVal;
	int		ryVal;
	int		yVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPcBgDiaolg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPcBgDiaolg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCBGDIAOLG_H__62B5ABE0_EF24_11D2_BD5A_00A0CC24A3EB__INCLUDED_)
