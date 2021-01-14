#pragma once
#include "atlcomtime.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "fireplusSet.h"


// CFriskExportDialog dialog
const int MaxBinStatic = 0;
const int MaxBinObserved = 1;

class CFriskExportDialog : public CDialog
{
	DECLARE_DYNAMIC(CFriskExportDialog)

public:
	CFriskExportDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFriskExportDialog();
	CFireplusSet* m_fpSet;

// Dialog Data
	enum { IDD = IDD_DIALOGFRISK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	COleDateTime m_startTime;
	COleDateTime m_endTime;
	CEdit m_editCalm;
	CSpinButtonCtrl m_spinCalm;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	int m_calmVal;
	int windType;
	int m_maxBinType;
	int m_maxBinSpeed;
	int m_binSize;
	afx_msg void OnBnClickedGust();
	afx_msg void OnBnClickedAvg();
	afx_msg void OnBnClickedBoth();
	COleDateTime m_startMonthDay;
	COleDateTime m_endMonthDay;
	CEdit	m_StartYear;
	CEdit	m_EndYear;
	int	m_sYear;
	int	m_eYear;
	int m_iMaxBin;
	int m_iBinSize;
	CSpinButtonCtrl	m_spnEyear;
	CSpinButtonCtrl	m_spnSyear;
	BOOL m_monthlyWinds;
	BOOL m_generateWindrose;
	CButton m_chkWeibull;
	afx_msg void OnDeltaposSpinBinSize(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_editBinSize;
	CSpinButtonCtrl m_spinBinSize;
	CEdit m_editMaxBin;
	CSpinButtonCtrl m_spinMaxBin;
	CButton m_radioStaticBin;
	CButton m_radioUseMax;
	afx_msg void OnBnClickedRadioAbsoluteStatic();
	afx_msg void OnBnClickedRadioUseMax();
	afx_msg void OnDeltaposSpinMaxSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMaxSpeed();
	afx_msg void OnEnChangeEditBinSize();
	BOOL m_bUseWeibull;
};
