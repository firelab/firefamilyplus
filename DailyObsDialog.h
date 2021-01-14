#pragma once
#include "afxwin.h"

class CFireplusDoc;

// CDailyObsDialog dialog

class CDailyObsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDailyObsDialog)

public:
	CDailyObsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDailyObsDialog();
	CFireplusDoc *m_pDoc;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_DAILY_OBS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboStation;
	CComboBox m_comboObsHour;
	afx_msg void OnCbnSelchangeComboStation();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();
};
