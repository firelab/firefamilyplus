#pragma once

#include "fireplusview.h"
#include "afxwin.h"
class CRawClim;

// CWorkingSetDialog dialog

class CWorkingSetDialog : public CDialog
{
	DECLARE_DYNAMIC(CWorkingSetDialog)

public:
	CWorkingSetDialog(CWnd* pParent, CRawClim *_m_pClim);   // standard constructor
	virtual ~CWorkingSetDialog();

// Dialog Data
	enum { IDD = IDD_WORKINGSETDIALOG };
	CButton	dataYearsBtn;
	CEdit	m_StartYear;
	CComboBox	m_StartMonth;
	CEdit	m_StartDay;
	CEdit	m_EndYear;
	CEdit	m_EndDay;
	CComboBox	m_EndMonth;
	CComboBox	m_PeriodLenControl;
	CSpinButtonCtrl	m_spnEday;
	CSpinButtonCtrl	m_spnSday;
	CSpinButtonCtrl	m_spnEyear;
	CSpinButtonCtrl	m_spnSyear;
	CRawClim* m_pClim;
	FFPViewCUG m_grid;
	int tableRightMin;
	int tableBottomMin;
	bool fxParamChange;
	void PopulateMetaTable();
	void SaveMetaTable();
	bool SetYears(CString fmStr = "");
	void ConfigCheckBoxes();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetDayNumControlRange(CSpinButtonCtrl *pSpinCtrl, int month);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	//CString m_Start;
	int m_sYear;
	int m_eYear;
	int m_sDay;
	int m_eDay;
	CString m_sMonth;
	CString m_eMonth;
	CString m_period;
	afx_msg void OnCbnSelchangeSmonth();
	afx_msg void OnCbnSelchangeEmonth();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStatic m_staticTableLabel;
	CButton m_chkDailyExtremes;
	CButton m_chkForceRecompute;
	afx_msg void OnBnClickedCheckDailyExtremes();
};
