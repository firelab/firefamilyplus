#pragma once

#include "UGCtrl.h"
#include "SIGStationSet.h"
// CSelSIGStationDialog dialog

class CSelSIGStationDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelSIGStationDialog)
	CSIGStationSet * stations;

public:
	CSelSIGStationDialog(CWnd* pParent = NULL, CSIGStationSet *_stations = NULL);   // standard constructor
	virtual ~CSelSIGStationDialog();
	void RedisplayTable();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELSIGSTATION };
    CString selectedID;

protected:
	CUGCtrl m_grid;
    bool showall;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedShowallstations();
protected:
	virtual void OnOK();
};
