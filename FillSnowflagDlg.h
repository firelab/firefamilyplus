#pragma once
#include "afxwin.h"


class CFireplusDoc;
// CFillSnowflagDlg dialog

class CFillSnowflagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFillSnowflagDlg)

public:
	CFillSnowflagDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFillSnowflagDlg();
	CFireplusDoc *m_pDoc;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILL_SNOWFLAG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboStation;
	afx_msg void OnBnClickedApply();
	virtual BOOL OnInitDialog();
};
