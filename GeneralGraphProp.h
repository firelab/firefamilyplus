//{{AFX_INCLUDES()
#include "colorbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_GENERALGRAPHPROP_H__D7711A61_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_GENERALGRAPHPROP_H__D7711A61_675B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "graphview.h"

// GeneralGraphProp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CGeneralGraphProp dialog

class CGeneralGraphProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CGeneralGraphProp)

// Construction
public:
	void WriteData(CGraphView *pView = NULL);
	void ReadData(CGraphView *pView = NULL);
	void SaveData();
	CGeneralGraphProp();
	~CGeneralGraphProp();
	CGraphSet * graphSet;
	void SetGraphSet(CGraphSet *set);
	bool isQuick;
// Dialog Data
	//{{AFX_DATA(CGeneralGraphProp)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	CComboBox	vLinestyle;
	CComboBox	hLinestyle;
	CButton	vLines;
	CButton	hLines;
	CButton	statBars;
	CButton	lineGraph;
	BOOL	overallCP1;
	BOOL	overallCP2;
	CColorButton	overallCP1Color;
	CColorButton	overallCP2Color;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralGraphProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGeneralGraphProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALGRAPHPROP_H__D7711A61_675B_11D2_B8E5_000000000000__INCLUDED_)
