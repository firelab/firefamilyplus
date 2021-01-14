//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#include "colorbutton.h"
#include "afxcmn.h"
#if !defined(AFX_LINEGRAPHPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_LINEGRAPHPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LineGraphProp.h : header file
//
#include "graphview.h"

/////////////////////////////////////////////////////////////////////////////
// CLineGraphProp dialog

class CLineGraphProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CLineGraphProp)

// Construction
public:
	CLineGraphProp();
	~CLineGraphProp();
	CGraphSet * graphSet;
	void SetGraphSet(CGraphSet *set);
	void WriteData(CGraphView *pView = NULL);
	void ReadData(CGraphView *pView = NULL);
	void SaveData();
	bool isQuick;

// Dialog Data
	//{{AFX_DATA(CLineGraphProp)
	enum { IDD = IDD_PROPPAGE_LGRAPH };
	CButton	mins;
	CButton	maxs;
	CButton	cp2;
	CButton	cp1;
	CColorButton	avgColor;
	CColorButton	cp1Color;
	CColorButton	cp2Color;
	CColorButton	maxsColor;
	CColorButton	minsColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLineGraphProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLineGraphProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSpinButtonCtrl m_spinAvg;
	CSpinButtonCtrl m_spinMins;
	CSpinButtonCtrl m_spinMaxs;
	CSpinButtonCtrl m_spinCP1;
	CSpinButtonCtrl m_spinCP2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEGRAPHPROP_H__8A6E2DE4_642C_11D2_B8E5_000000000000__INCLUDED_)
