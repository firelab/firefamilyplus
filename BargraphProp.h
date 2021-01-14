#if !defined(AFX_BARGRAPHPROP_H__D7711A62_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_BARGRAPHPROP_H__D7711A62_675B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BargraphProp.h : header file
//
#include "graphview.h"

/////////////////////////////////////////////////////////////////////////////
// CBargraphProp dialog

class CBargraphProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CBargraphProp)

// Construction
public:
	void WriteData(CGraphView *pView = NULL);
	void ReadData(CGraphView *pView = NULL);
	void SaveData();
	void SetGraphSet(CGraphSet *set);
	CGraphSet * graphSet;
	CBargraphProp();
	~CBargraphProp();
	bool isQuick;

// Dialog Data
	//{{AFX_DATA(CBargraphProp)
	enum { IDD = IDD_PROPPAGE_BARGRAPH };
	CButton	rangeTics;
	CButton	cpTics;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBargraphProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBargraphProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARGRAPHPROP_H__D7711A62_675B_11D2_B8E5_000000000000__INCLUDED_)
