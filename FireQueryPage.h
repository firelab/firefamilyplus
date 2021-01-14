#if !defined(AFX_FIREQUERYPAGE_H__D7711A66_675B_11D2_B8E5_000000000000__INCLUDED_)
#define AFX_FIREQUERYPAGE_H__D7711A66_675B_11D2_B8E5_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FireQueryPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFireQueryPage dialog

class CFireQueryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFireQueryPage)

// Construction
public:
	void SaveData();
	void AddSubunits(long unID);
	void AddUnits(long regID);
	CString staStr;
	void RemoveSubunits(long unitID);
	CString agency;
	long agencyID;
	CFireQueryPage(CFireAssocSet *_assocSet = NULL, CFireAgencySet *_agencySet = NULL, 
		CFireRegionSet *_regSet = NULL, CFireUnitSet *_unitSet = NULL, 
		CFireSubunitSet *_subUnitSet = NULL, CString _staStr = "");
	~CFireQueryPage();

	
	
	//CButton DOIbutton;

// Dialog Data
	//{{AFX_DATA(CFireQueryPage)
	enum { IDD = IDD_FIREPROPPAGE };
	CListBox	units;
	CListBox	subUnits;
	CListBox	regions;
	//}}AFX_DATA

	CFireAssocSet * assocSet;
	CFireAgencySet *agencySet;
	CFireRegionSet *regSet; 
	CFireUnitSet *unitSet; 
	CFireSubunitSet *subUnitSet;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFireQueryPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFireQueryPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeRegionslist();
	afx_msg void OnSelchangeUnitslist();
	afx_msg void OnDOIbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIREQUERYPAGE_H__D7711A66_675B_11D2_B8E5_000000000000__INCLUDED_)
