//{{AFX_INCLUDES()
//#include "nfdrs.h"
#include "numspinctrl.h"
#include "afxwin.h"
//}}AFX_INCLUDES
#if !defined(AFX_RERAPDIALOG_H__7C900AA0_9FD9_11D2_BD5A_E44CCA8A1405__INCLUDED_)
#define AFX_RERAPDIALOG_H__7C900AA0_9FD9_11D2_BD5A_E44CCA8A1405__INCLUDED_

//#include "WxSet.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RerapDialog.h : header file
//
//#include "nfdr32.h"

class RerapObs
{
public:
	double var;
	double fm1;
	double fm10;
	double fm100;
	double fm1000;
	double herb;
	double woody;
	double wind;
	double kbdi;
};

class RerapStats
{
public:
	int start;
	int step;
	int nClasses;
	RerapStats();
	void BuildStats(CList<RerapObs, RerapObs&> *list, double vMin, double vMax);	
	Stats varStats[50];
	double fm1[50];
	double fm10[50];
	double fm100[50];
	double fm1000[50];
	double herb[50];
	double woody[50];
	double wind[50];
	double kbdi[50];
};

/////////////////////////////////////////////////////////////////////////////
// CRerapDialog dialog

class CRerapDialog : public CDialog
{
// Construction
public:
	void AddWDirStatsToFireDangerClass(int wDir, int c, int fdc);
	void AddClassToFireDangerClass(int c, int fdc);
	long windDays;
	long nRecs;
	int slopeClass;
	void AddClassLAvgs(int c);
	void AddClassMAvgs(int c);
	void AddClassHAvgs(int c);
	void AddClassEAvgs(int c);
	long ClassN(int c);
	CString varString;
	void BuildFrequencyText();
	Stats stats;
	void RecalcStats();
	void InsertRerapObs(CList<RerapObs, RerapObs&> *list, RerapObs& rObs);
	void AddStats(int wDir);
	void AddStatsToClasses(int wDir, double p1, double pm, double ph); 
	void ClearValues();
	BOOL ReadData();
	CRerapDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, int _varID = 0);   // standard constructor
	~CRerapDialog();
	CString tTitle;
// Dialog Data
	//{{AFX_DATA(CRerapDialog)
	enum { IDD = IDD_RERAPDIALOG };
	CEdit	freqControl;
	CButton	avgsApply;
	BOOL	wE;
	BOOL	wN;
	BOOL	wNE;
	BOOL	wNW;
	BOOL	wS;
	BOOL	wSE;
	BOOL	wSW;
	BOOL	wW;
	int		low;
	int		mod;
	int		high;
	CString	high1;
	CString	ext1;
	CString	mod1;
	int		lpcnt;
	long	hobs;
	int		hpcnt;
	long	lobs;
	long	mobs;
	int		mpcnt;
	int		epcnt;
	long	eobs;
	double	l1000;
	double	l100;
	double	l10;
	double	l1;
	double	l20;
	double	lherb;
	double	lwoody;
	double	Mod1000;
	double	Mod100;
	double	Mod10;
	double	Mod1;
	double	Mod20;
	double	Modherb;
	double	Modwoody;
	double	e1000;
	double	e100;
	double	e10;
	double	e1;
	double	e20;
	double	eherb;
	double	ewoody;
	double	h1000;
	double	h100;
	double	h10;
	double	h1;
	double	h20;
	double	hherb;
	double	hwoody;
	int		eerc;
	int		herc;
	int		lerc;
	int		Moderc;
	CString	freqs;
	int		em1;
	int		em2;
	int		hm1;
	int		hm2;
	int		lm1;
	int		lm2;
	int		mm1;
	int		mm2;
	int	lsc;
	int	Modsc;
	int	hsc;
	int	esc;
	//}}AFX_DATA
	CNumSpinCtrl m_spinL1;
	CNumSpinCtrl m_spinL10;
	CNumSpinCtrl m_spinL100;
	CNumSpinCtrl m_spinL1000;
	CNumSpinCtrl m_spinLHerb;
	CNumSpinCtrl m_spinLWoody;
	CNumSpinCtrl m_spinL20;
	CNumSpinCtrl m_spinM1;
	CNumSpinCtrl m_spinM10;
	CNumSpinCtrl m_spinM100;
	CNumSpinCtrl m_spinM1000;
	CNumSpinCtrl m_spinMHerb;
	CNumSpinCtrl m_spinMWoody;
	CNumSpinCtrl m_spinM20;
	CNumSpinCtrl m_spinH1;
	CNumSpinCtrl m_spinH10;
	CNumSpinCtrl m_spinH100;
	CNumSpinCtrl m_spinH1000;
	CNumSpinCtrl m_spinHHerb;
	CNumSpinCtrl m_spinHWoody;
	CNumSpinCtrl m_spinH20;
	CNumSpinCtrl m_spinE1;
	CNumSpinCtrl m_spinE10;
	CNumSpinCtrl m_spinE100;
	CNumSpinCtrl m_spinE1000;
	CNumSpinCtrl m_spinEHerb;
	CNumSpinCtrl m_spinEWoody;
	CNumSpinCtrl m_spinE20;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRerapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	CFireplusSet *fpSet;
//	NFDRCalc nfdrs;
	int varID;
	//bool ascending;
	RerapStats rStats[8];//rerapstats for each wind direction
	Stats lStats, mStats, hStats, eStats;
	double lkbdi, Modkbdi, hkbdi, ekbdi;
protected:

	// Generated message map functions
	//{{AFX_MSG(CRerapDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusLow();
	afx_msg void OnClassapply();
	afx_msg void OnAvgsapply();
	afx_msg void OnKillfocusMed2();
	afx_msg void OnKillfocusHigh2();
	afx_msg void OnE();
	afx_msg void OnN();
	afx_msg void OnNe();
	afx_msg void OnNw();
	afx_msg void OnS();
	afx_msg void OnSe();
	afx_msg void OnSw();
	afx_msg void OnW();
	afx_msg void OnKillfocusLm1();
	afx_msg void OnKillfocusLm2();
	afx_msg void OnKillfocusMm1();
	afx_msg void OnKillfocusMm2();
	afx_msg void OnKillfocusHm1();
	afx_msg void OnKillfocusHm2();
	afx_msg void OnKillfocusEm1();
	afx_msg void OnKillfocusEm2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editLH1;
	CEdit m_editLH10;
	CEdit m_editLH100;
	CEdit m_editLH1000;
	CEdit m_editLHerb;
	CEdit m_editLWoody;
	CEdit m_editL20;
	CEdit m_editMH1;
	CEdit m_editMH10;
	CEdit m_editMH100;
	CEdit m_editMH1000;
	CEdit m_editMHerb;
	CEdit m_editMWoody;
	CEdit m_editM20;
	CEdit m_editHH1;
	CEdit m_editHH10;
	CEdit m_editHH100;
	CEdit m_editHH1000;
	CEdit m_editHHerb;
	CEdit m_editHWoody;
	CEdit m_editH20;
	CEdit m_editEH1;
	CEdit m_editEH10;
	CEdit m_editEH100;
	CEdit m_editEH1000;
	CEdit m_editEHerb;
	CEdit m_editEWoody;
	CEdit m_editE20;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RERAPDIALOG_H__7C900AA0_9FD9_11D2_BD5A_E44CCA8A1405__INCLUDED_)
