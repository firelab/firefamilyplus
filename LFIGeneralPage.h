#pragma once
#include "numspinctrl.h"
#include "lfiset.h"


// CLFIGeneralPage dialog

class CLFIGeneralPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLFIGeneralPage)

public:
	CLFIGeneralPage();
	virtual ~CLFIGeneralPage();
	CLFISet *pLfiSet;
// Dialog Data
	enum { IDD = IDD_PROPPAGE_LFI_GENERAL };
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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	double tminMin;
	double tminMax;
	double vpdMin;
	double vpdMax;
	double daylenMin;
	double daylenMax;
	int daysAvg;
	int daysPcp;
	bool m_UseVPDavg;
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
	afx_msg void OnBnClickedButtonDefaults();
	afx_msg void OnBnClickedButtonStoredDefaults();
	afx_msg void OnBnClickedButtonSaveDefaults();
	afx_msg void OnDeltaposSpinPcpDays(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPcpDays();

	CEdit m_editPcpDays;
	CSpinButtonCtrl m_spinPcpDays;
	CComboBox m_comboVPD;
	afx_msg void OnDeltaposSpinPrecipMin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPrecipMax(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditPrecipMin();
	afx_msg void OnEnChangeEditPrecipMax();
	afx_msg void OnCbnSelchangeComboVpdUsage();
	CEdit m_editPcpMin;
	CNumSpinCtrl m_spinPcpMin;
	CEdit m_editPcpMax;
	CNumSpinCtrl m_spinPcpMax;
};
