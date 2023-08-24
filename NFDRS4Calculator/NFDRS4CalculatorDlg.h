
// NFDRS4CalculatorDlg.h : header file
//

#pragma once
#include "NumSpinCtrl.h"
#include "nfdrs4.h"
#include "FMParamsDlg.h"


// CNFDRS4CalculatorDlg dialog
class CNFDRS4CalculatorDlg : public CDialogEx
{
// Construction
public:
	CNFDRS4CalculatorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NFDRS4CALCULATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CString	m_slopeClass;
	CString	m_fuelModel;
	double	m_bi;
	double	m_erc;
	double	m_ic;
	double	m_sc;

	long	m_kbdi2016;
	double m_gsi;
	double m_gsiMax;
	double m_gsiThreshold;
	int m_fuelTemperature;
	double	m_fm1;
	double	m_fm10;
	double	m_fm100;
	double	m_fm1000;
	double	m_fmHerb;
	double	m_fmWood;
	long	m_windSpeed;

	CComboBox m_comboFuelModel;
	CComboBox m_comboSlopeClass;
	CNumSpinCtrl m_spinFM1;
	CNumSpinCtrl m_spinFM10;
	CNumSpinCtrl m_spinFM100;
	CNumSpinCtrl m_spinFM1000;
	CNumSpinCtrl m_spinHerb;
	CNumSpinCtrl m_spinWoody;
	CNumSpinCtrl m_spinWind;
	CNumSpinCtrl m_spinGSI;
	CNumSpinCtrl m_spinKBDI2016;
	CNumSpinCtrl m_spinMaxGSI;
	CNumSpinCtrl m_spinGSIThreshold;
	CNumSpinCtrl m_spinFuelTemp;
	CEdit m_editFM1;
	CEdit m_editFM10;
	CEdit m_editFM100;
	CEdit m_editFM1000;
	CEdit m_editHerb;
	CEdit m_editWoody;
	CEdit m_editWind;
	CEdit m_editGSI;
	CEdit m_editKBDI2016;
	CEdit m_editMaxGSI;
	CEdit m_editGSIThreshold;
	CEdit m_editFuelTemp;
	CSpinButtonCtrl m_spinSCM;
	CEdit m_editSCM;
	CButton m_btnHumid;
	CFMParamsDlg* pFMParamsDlg;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	NFDRS4 m_nfdrs2016;
public:
	afx_msg void OnClickedCalculate();
	afx_msg void OnBnClickedButtonFmparams();
	afx_msg void OnBnClickedButtonKbdicalc();
	afx_msg void OnClose();
	//float m_fFM1;
	afx_msg void OnEnKillfocusWind();
	afx_msg void OnEnChangeWind();
	afx_msg void OnEnKillfocus1hrfm();
	afx_msg void OnEnChange1hrfm();
	afx_msg void OnEnKillfocus10hrfm();
	afx_msg void OnEnChange10hrfm();
	afx_msg void OnEnKillfocus100hrfm();
	afx_msg void OnEnChange100hrfm();
	afx_msg void OnEnKillfocus1000hrfm();
	afx_msg void OnEnChange1000hrfm();
	afx_msg void OnEnKillfocusHerbfm();
	afx_msg void OnEnChangeHerbfm();
	afx_msg void OnEnKillfocusWoodyfm();
	afx_msg void OnEnChangeWoodyfm();
	afx_msg void OnChangeGsi();
	afx_msg void OnKillfocusGsi();
	afx_msg void OnChangeGsiThreshold();
	afx_msg void OnKillfocusGsiThreshold();
	afx_msg void OnChangeFuelTemp();
	afx_msg void OnKillfocusFuelTemp();
	afx_msg void OnChangeGsimax();
	afx_msg void OnKillfocusGsimax();
	afx_msg void OnChangeKbdi2016();
	afx_msg void OnKillfocusKbdi2016();
	afx_msg void OnChangeEditScm();
	afx_msg void OnKillfocusEditScm();
};
