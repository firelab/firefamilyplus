#pragma once
#include "numspinctrl.h"
#include "lfiset.h"


// CLFIWoodyPage dialog

class CLFIWoodyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLFIWoodyPage)

public:
	CLFIWoodyPage();
	virtual ~CLFIWoodyPage();
	CLFISet *pLfiSet;

// Dialog Data
	enum { IDD = IDD_PROPPAGE_LFI_WOODY };
	CNumSpinCtrl m_spinTminMin;
	CEdit m_editTminMin;
	CNumSpinCtrl m_spinTminMax;
	CEdit m_editTminMax;
	CNumSpinCtrl m_spinVPDMin;
	CEdit m_editVPDMin;
	CNumSpinCtrl m_spinVPDMax;
	CEdit m_editVPDMax;
	CNumSpinCtrl m_spinDaylenMin;
	CEdit m_editDaylenMin;
	CNumSpinCtrl m_spinDaylenMax;
	CEdit m_editDaylenMax;
	CSpinButtonCtrl m_spinDaysAvg;
	CEdit m_editDaysAvg;

	CNumSpinCtrl m_spinMaxGSI;
	CEdit m_editMaxGSI;
	CNumSpinCtrl m_spinGreenup;
	CEdit m_editGreenup;
	CNumSpinCtrl m_spinMaxWoody;
	CEdit m_editMaxWoody;
	CNumSpinCtrl m_spinMinWoody;
	CEdit m_editMinWoody;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	double tminMin;
	double tminMax;
	double vpdMin;
	double vpdMax;
	double daylenMin;
	double daylenMax;
	int daysAvg;
	bool m_UseVPDavg;
	double maxGSI;
	double greenup;
	double maxWoody;
	double minWoody;
	int daysPcp;
	double pcpMin;
	double pcpMax;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	afx_msg void OnDeltaposSpinTminmin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditTminmin();
	afx_msg void OnDeltaposSpinTminmax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditTminmax();
	afx_msg void OnDeltaposSpinVpdmin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditVpdmin();
	afx_msg void OnDeltaposSpinVpdmax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditVpdmax();
	afx_msg void OnDeltaposSpinDaylenmin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDaylenmin();
	afx_msg void OnDeltaposSpinDaylenmax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDaylenmax();
	//afx_msg void OnBnClickedRadioVpdMax();
	//afx_msg void OnBnClickedRadioVpdAvg();
	afx_msg void OnDeltaposSpinDaysAvg(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDaysAvg();
	afx_msg void OnDeltaposSpinMaxgsi(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMaxgsi();
	afx_msg void OnDeltaposSpinGreenup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditGreenup();
	afx_msg void OnDeltaposSpinMaxherbfm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMaxherbfm();
	afx_msg void OnDeltaposSpinMinherbfm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMinherbfm();
	afx_msg void OnBnClickedButtonDefaults();
	afx_msg void OnBnClickedButtonStoredDefaults();
	afx_msg void OnBnClickedButtonSaveDefaults();
	afx_msg void OnDeltaposSpinPcpDays(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPcpDays();
	afx_msg void OnDeltaposSpinPrecipMin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPrecipMax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPrecipMin();
	afx_msg void OnEnChangeEditPrecipMax();
	afx_msg void OnCbnSelchangeComboVpdUsage();

	CEdit m_editPcpDays;
	CSpinButtonCtrl m_spinPcpDays;
	CComboBox m_comboVPD;
	CEdit m_editPcpMin;
	CNumSpinCtrl m_spinPcpMin;
	CEdit m_editPcpMax;
	CNumSpinCtrl m_spinPcpMax;
};
