#if !defined(AFX_LISTINGDIALOG_H__2690E9B2_5160_11D2_B8DC_000000000000__INCLUDED_)
#define AFX_LISTINGDIALOG_H__2690E9B2_5160_11D2_B8DC_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ListingDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListingDialog dialog

class CListingDialog : public CDialog
{
// Construction
public:
	int fireCause;
	BOOL isDaily;
	CListingDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL, BOOL daily = TRUE);   // standard constructor
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	DateType dt;
	TimeType tt;
// Dialog Data
	//{{AFX_DATA(CListingDialog)
	enum { IDD = IDD_LISTINGDIALOG };
	CSpinButtonCtrl	m_spinLFD;
	CListBox	destList;
	CListBox	sourceList;
	BOOL	reportHeader;
	BOOL	columnHeader;
	BOOL	dateStamp;
	BOOL	m_numFires;
	BOOL	m_numLargeFires;
	int		m_LFD;
	BOOL	m_Acres;
	BOOL	m_NumPerClass;
	//}}AFX_DATA
	CFireplusSet *fpSet;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnTop();
	afx_msg void OnBottom();
	// Generated message map functions
	//{{AFX_MSG(CListingDialog)
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnRemoveall();
	afx_msg void OnSelectall();
	virtual void OnOK();
	afx_msg void OnDblclkList1();
	afx_msg void OnDblclkList2();
	virtual BOOL OnInitDialog();
	afx_msg void OnTime1();
	afx_msg void OnTime2();
	afx_msg void OnNotime();
	afx_msg void OnDate1();
	afx_msg void OnDate2();
	afx_msg void OnDate3();
	afx_msg void OnDate4();
	afx_msg void OnDate5();
	afx_msg void OnAllfires();
	afx_msg void OnHuman();
	afx_msg void OnLightning();
	afx_msg void OnDLExport();
	afx_msg void OnVarSort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_stnPerRecord;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTINGDIALOG_H__2690E9B2_5160_11D2_B8DC_000000000000__INCLUDED_)
