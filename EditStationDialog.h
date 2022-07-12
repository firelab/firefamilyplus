//{{AFX_INCLUDES()
#include "afxdtctl.h"
#include "afxcmn.h"
#include "numspinctrl.h"
#include "afxwin.h"
//}}AFX_INCLUDES
#if !defined(AFX_EDITSTATIONDIALOG_H__537AA602_5E09_11D2_B8DE_000000000000__INCLUDED_)
#define AFX_EDITSTATIONDIALOG_H__537AA602_5E09_11D2_B8DE_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditStationDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditStationDialog dialog

class CEditStationDialog : public CDialog
{
// Construction
public:
	void AddTimeZones();
	void AddCounties();
	void SaveStation();
	bool FieldsOK();
	CEditStationDialog(CWnd* pParent = NULL, CSIGStationSet *_station = NULL, bool _isNew = false);   // standard constructor
	CSIGStationSet *station;
	bool isNew;

// Dialog Data
	//{{AFX_DATA(CEditStationDialog)
	enum { IDD = IDD_EDITSTATIONDIALOG };
	CComboBox	county;
	CEdit	stationIDControl;
	CComboBox	stnType;
	CComboBox	agency;
	CButton	use88;
	CComboBox	state;
	CComboBox	slopePos;
	CComboBox	slopeClass;
	CComboBox	region;
	CComboBox	NFDRSFM;
	CButton	herbAnnual;
	CButton	fm1eq10;
	CButton	deciduous;
	CComboBox	climClass;
	CComboBox	aspect;
	//CDateTimeCtrl	freezeDate;
	//CDateTimeCtrl	greenupDate;
	CString	name;
	CString	stationID;
	CString	obsUnit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditStationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditStationDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeState();
	afx_msg void OnKillfocusClimateclass();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl greenupDate;
	CDateTimeCtrl freezeDate;
	CNumSpinCtrl m_spinLat;
	CNumSpinCtrl m_spinLong;
	CSpinButtonCtrl m_spinElev;
	CSpinButtonCtrl m_spinKBDI;
	CNumSpinCtrl m_numSpinPrecip;
	CEdit m_editPrecip;
	CEdit m_editFM1000;
	CEdit m_editLat;
	CEdit m_editLong;

	CNumSpinCtrl m_numSpinFM1000;
	CDateTimeCtrl m_dormantDate;
	CButton m_useDormant;
	CButton m_useStick;
	afx_msg void OnBnClickedUsedormant();
	afx_msg void OnBnClickedUse88();
	//afx_msg void OnBnClickedButtonSowThresholds();
	//afx_msg void OnBnClickedButtonSowWetflagOptions();
	//afx_msg void OnBnClickedButtonRecalcSowWetflag();
	CComboBox m_comboRegSchedObs;
	CEdit m_editNESDID_ID;
	CString m_strNESDID_ID;
	CEdit m_editWRCC_ID;
	CString m_strWRCC_ID;
protected:
public:
	CSpinButtonCtrl m_spinMaxSC_V;
	CSpinButtonCtrl m_spinMaxSC_W;
	CSpinButtonCtrl m_spinMaxSC_X;
	CSpinButtonCtrl m_spinMaxSC_Y;
	CSpinButtonCtrl m_spinMaxSC_Z;
	CButton m_btnHumid_V;
	CButton m_btnHumid_W;
	CButton m_btnHumid_X;
	CButton m_btnHumid_Y;
	CButton m_btnHumid_Z;
	CComboBox m_comboTimeZone;
	CEdit m_editKBDIThreshold;
	CSpinButtonCtrl m_spinKBDIThreshold;
	CEdit m_editMXD;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSTATIONDIALOG_H__537AA602_5E09_11D2_B8DE_000000000000__INCLUDED_)
