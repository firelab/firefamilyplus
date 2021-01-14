#pragma once
#include "SowOptionsSet.h"


// CSowWetFlagOptionsDlg dialog
//enum SOW_OPTS {SOW_MISSING, SR_SOW_ONLY, SR_SOW_NEVER};
//enum WF_OPTS {WF_MISSING, EST_WF_ONLY, EST_WF_NEVER};

class CSowWetFlagOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSowWetFlagOptionsDlg)
public:
	CSowWetFlagOptionsDlg(CWnd* pParent = NULL, CSowOptionsSet *pSet = NULL);   // standard constructor
	virtual ~CSowWetFlagOptionsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SOW_WETFLAG_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CSowOptionsSet *m_pSet;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
