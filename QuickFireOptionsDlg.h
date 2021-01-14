#pragma once
#include "FireOptionsSet.h"
#include "fireplusSet.h"


// CQuickFireOptionsDlg dialog

class CQuickFireOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuickFireOptionsDlg)

public:
	CQuickFireOptionsDlg(CWnd* pParent, CFireplusSet *_fpSet);   // standard constructor
	virtual ~CQuickFireOptionsDlg();
	CFireOptionsSet optionsSet;
	CFireplusSet * fpSet;

// Dialog Data
	enum { IDD = IDD_QUICK_FIRE_OPTIONS };
	//CButton	cumulative;
	CButton	lightning;
	BOOL	conditional;
	CSpinButtonCtrl m_spinFires;
	CSpinButtonCtrl m_spinAcres;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnConditional();

	DECLARE_MESSAGE_MAP()
};
