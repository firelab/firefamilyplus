#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "numspinctrl.h"
#include "SowThresholdsSet.h"


// CSowThresholdsDlg dialog

class CSowThresholdsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSowThresholdsDlg)

public:
	CSowThresholdsDlg(CWnd* pParent = NULL, CSowThresholdsSet *_pSowSet = NULL);   // standard constructor
	virtual ~CSowThresholdsDlg();

// Dialog Data
	enum { IDD = IDD_SOW_THRESHOLDS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editPcntClear;
	CSpinButtonCtrl m_spinPcntClear;
	CEdit m_editPcntScattered;
	CSpinButtonCtrl m_spinPcntScattered;
	CEdit m_editPcntBroken;
	CSpinButtonCtrl m_spinPcntBroken;
	CEdit m_edit3HrDur;
	CSpinButtonCtrl m_spin3HrDur;
	CEdit m_edit24HrDur;
	CSpinButtonCtrl m_spin24HrDur;
	CEdit m_edit1HrDrizzle;
	CNumSpinCtrl m_spin1HrDrizzle;
	CEdit m_edit1HrRain;
	CNumSpinCtrl m_spin1HrRain;
	CEdit m_edit1HrShowers;
	CNumSpinCtrl m_spin1HrShowers;
	CEdit m_edit3HrAmt;
	CNumSpinCtrl m_spin3HrAmt;
	CEdit m_edit24HrAmt;
	CNumSpinCtrl m_spin24HrAmt;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();

	CSowThresholdsSet *m_pSowSet;
	CString m_SIG_Station;
	int m_PCNT_Clear;
	int m_PCNT_Scattered;
	int m_PCNT_Broken;
	double	m_1HR_Drizzle;
	double	m_1HR_Rain;
	double	m_1HR_Showers;
	int m_3HR_DUR_WetFlag;
	double m_3HR_AMT_WetFlag;
	int m_24HR_DUR_WetFlag;
	double m_24HR_AMT_WetFlag;
};
