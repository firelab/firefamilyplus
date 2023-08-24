#pragma once
#include "afxwin.h"
#include "NumSpinCtrl.h"
#include "nfdrs4.h"


// CKBDIDIalog dialog

class CKBDIDIalog : public CDialog
{
	DECLARE_DYNAMIC(CKBDIDIalog)

public:
	CKBDIDIalog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKBDIDIalog();

// Dialog Data
	enum { IDD = IDD_DIALOG_KBDI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_ApplyButton;
	afx_msg void OnBnClickedCalc();
	afx_msg void OnBnClickedApply();
	CEdit m_kbdiBox;
	long kbdi;
	double	avgPrecip;
	double	cumPrecip;
	long	maxTemp;
	double	precip;
	long	ykbdi;
	virtual BOOL OnInitDialog();
	NFDRS4 m_nfdrs4;
	CNumSpinCtrl m_spinPrevKBDI;
	CNumSpinCtrl m_spinRain;
	CNumSpinCtrl m_spinMaxTemp;
	CNumSpinCtrl m_spinCummPrecip;
	CNumSpinCtrl m_spinAvgPrecip;
	CEdit m_prevKBDI;
	CEdit m_rain;
	CEdit m_maxTemp;
	CEdit m_cummPrecip;
	CEdit m_avgPrecip;
	afx_msg void OnClose();
	void SaveFields();
};
