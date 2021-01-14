#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CFireplusDoc;
// CSnowFlagDlg dialog

class CSnowFlagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSnowFlagDlg)

public:
	CSnowFlagDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSnowFlagDlg();
	//CDatabase *m_pDB;
	CFireplusDoc *m_pDoc;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SNOWFLAG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetStartDayRange();
	void SetEndDayRange();
	void SetYears(CString stastionStr);
	void BuildQuery(CString &query);
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboStation;
	CComboBox m_comboValue;
	CEdit m_editStartYear;
	CSpinButtonCtrl m_spinStartYear;
	CEdit m_editEndYear;
	CSpinButtonCtrl m_spinEndYear;
	CComboBox m_comboStartMonth;
	CComboBox m_comboEndMonth;
	CEdit m_editStartDay;
	CSpinButtonCtrl m_spinStartDay;
	CEdit m_editEndDay;
	CSpinButtonCtrl m_spinEndDay;
	afx_msg void OnCbnSelchangeComboStation();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnCbnSelchangeSmonth();
	afx_msg void OnCbnSelchangeEmonth();
	virtual BOOL OnInitDialog();
};
