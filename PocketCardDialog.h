//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#include "afxcmn.h"
#if !defined(AFX_POCKETCARDDIALOG_H__7F113BD2_476C_11D2_B8D7_000000000000__INCLUDED_)
#define AFX_POCKETCARDDIALOG_H__7F113BD2_476C_11D2_B8D7_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PocketCardDialog.h : header file
//
#include "ugctrl.h"
#include "ugctbutn.h"
class PocketCardCUG : public CUGCtrl
{
public:
	PocketCardCUG();
	~PocketCardCUG();
	//CMergeDialog *pDlg;
protected:
	CUGButtonType		m_button;
	int					m_nButtonIndex;
public:
	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
};

/////////////////////////////////////////////////////////////////////////////
// CPocketCardDialog dialog

class CPocketCardDialog : public CDialog
{
// Construction
public:
	int lastDataYear;
	double GetIndexValue(CClimAnalysis& analysis, COleDateTime iDate);
	CPocketCardSet * pocketSet;
	CPocketCardDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, CPocketCardSet *_pocketSet = NULL);   // standard constructor
	CFireplusSet *fpSet;
	int m_varID;
// Dialog Data
	//{{AFX_DATA(CPocketCardDialog)
	enum { IDD = IDD_POCKETCARDDIALOG };
	CButton	m_ctlNWCGStd;
	CComboBox	extraBox;
	CButton	ytdButton;
	CEdit	areaControl;
	CComboBox	m_IndexBox;
	CString	m_Area;
	CString	m_Bitmap;
	CString	m_Index;
	CString	m_PEText;
	CString	m_Line1;
	CString	m_Line2;
	CString	m_Line3;
	BOOL	yearToDate;
	CString	extra;
	CString	m_strResponsibleAgency;
	BOOL	m_bNWCGStd;
	//}}AFX_DATA
	PocketCardCUG m_grid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPocketCardDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnVarSort();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPocketCardDialog)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBkgrndbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinCP;
	CSpinButtonCtrl m_spinYear1;
	CSpinButtonCtrl m_spinYear2;
	CSpinButtonCtrl m_spinWind;
	CSpinButtonCtrl m_spinRH;
	CSpinButtonCtrl m_spinTemp;
	CSpinButtonCtrl m_spinLiveFM;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POCKETCARDDIALOG_H__7F113BD2_476C_11D2_B8D7_000000000000__INCLUDED_)
