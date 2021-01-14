// NFDRSCalculator2Dlg.h : header file
//

#pragma once
#include "NumSpinCtrl.h"
#include "CNFDRS.h"
//#include "afxwin.h"
#include "FMParamsDlg.h"
#include "afxwin.h"
#include "afxcmn.h"


// CNFDRSCalculator2Dlg dialog
class CNFDRSCalculator2Dlg : public CDialog
{
// Construction
public:
	CNFDRSCalculator2Dlg(CWnd* pParent = NULL);	// standard constructor
	~CNFDRSCalculator2Dlg();
// Dialog Data
	enum { IDD = IDD_NFDRSCALCULATOR2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	CFMParamsDlg *pFMParamsDlg;

// Implementation
protected:
	HICON m_hIcon;
	CString	sow;
	CString	season;
	CString	slopeClass;
	CString	fuelModel;
	BOOL	rainEvent;
	BOOL	use88;
	BOOL	deciduous;
	double	bi;
	double	erc;
	double	ic;
	double	sc;
	CNumSpinCtrl m_spinFM1;
	CNumSpinCtrl m_spinFM10;
	CNumSpinCtrl m_spinFM100;
	CNumSpinCtrl m_spinFM1000;
	CNumSpinCtrl m_spinHerb;
	CNumSpinCtrl m_spinWoody;
	CNumSpinCtrl m_spinTemp;
	CNumSpinCtrl m_spinWind;
	CNumSpinCtrl m_spinWoodyGreen;
	CNumSpinCtrl m_spinKBDI;
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
	CEdit m_editTemp;
	CEdit m_editWind;
	CEdit m_editWoodyGreen;
	CEdit m_editKBDI;
	CEdit m_editGSI;
	CEdit m_editKBDI2016;
	CEdit m_editMaxGSI;
	CEdit m_editGSIThreshold;
	CEdit m_editFuelTemp;
	CComboBox	seasonCtl;
	CButton	rainCtl;
	CButton	decidCtl;


	CButton m_btnUse88;

	long	woodyGreen;
	double	fm1;
	double	fm10;
	double	fm100;
	double	fm1000;
	double	fmHerb;
	double	fmWood;
	long	windSpeed;
	long	temperature;
	long	kbdi;
	double	gsi;
	long	kbdi2016;
	double gsiMax;
	double gsiThreshold;
	int fuelTemperature;

	void SetFMFields();
	void Set88Fields();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCbnSelchangeNfdrsfm();
	afx_msg void OnBnClickedUse88();
	afx_msg void OnBnClickedCalculate();
	DECLARE_MESSAGE_MAP()
public:
	NFDR2016Calc m_nfdrs2016;
	CNFDRS m_nfdrs;
	CStatic static1;
	CStatic static2;
	CStatic static3;
	CStatic static4;
	afx_msg void OnBnClickedButtonFmparams();
	afx_msg void OnBnClickedButtonKbdicalc();
	CComboBox m_comboFuelModel;
	CComboBox m_comboSlopeClass;
	CComboBox m_comboSOW;
	afx_msg void OnClose();
	//float m_fFM1;
	afx_msg void OnEnKillfocusTemp();
	afx_msg void OnEnChangeTemp();
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
	afx_msg void OnEnKillfocusWoodyGreen();
	afx_msg void OnEnChangeWoodyGreen();
	CSpinButtonCtrl m_spinSCM;
	CButton m_btnHumid;
	CEdit m_editSCM;
};
