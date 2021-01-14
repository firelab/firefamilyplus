//{{AFX_INCLUDES()
//#include "pvnumeric.h"
//}}AFX_INCLUDES
#include "afxcmn.h"
#if !defined(AFX_GRAPHFIRESPAGE_H__14822700_7F9E_11D2_BD5A_CFF0185C283D__INCLUDED_)
#define AFX_GRAPHFIRESPAGE_H__14822700_7F9E_11D2_BD5A_CFF0185C283D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphFiresPage.h : header file
//
#include "graphview.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphFiresPage dialog

class CGraphFiresPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CGraphFiresPage)

// Construction
public:
	CGraphFiresPage();
	~CGraphFiresPage();
	int cause;
	CGraphSet * graphSet;
	void SetGraphSet(CGraphSet *set);
	void WriteData(CGraphView *pView = NULL);
	void ReadData(CGraphView *pView = NULL);
	void SaveData();
	bool isQuick;
// Dialog Data
	//{{AFX_DATA(CGraphFiresPage)
	enum { IDD = IDD_PROPPAGE_FIRES };
	CButton	mfd;
	CButton	lfd;
	CButton	fd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGraphFiresPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGraphFiresPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioAll();
	afx_msg void OnRadioHuman();
	afx_msg void OnRadioLightning();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSpinButtonCtrl m_spinFires;
	CSpinButtonCtrl m_spinAcres;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHFIRESPAGE_H__14822700_7F9E_11D2_BD5A_CFF0185C283D__INCLUDED_)
