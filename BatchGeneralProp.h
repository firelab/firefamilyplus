//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_BatchGeneralPROP_)
#define AFX_BatchGeneralPROP_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BatchGeneralProp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CBatchGeneralProp dialog


class CBatchOptionsSet;

class CBatchGeneralProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CBatchGeneralProp)

// Construction
public:
	void WriteData();
	void ReadData();
	void SaveData();
	CBatchGeneralProp();
	~CBatchGeneralProp();
	CBatchOptionsSet * boptSet;
	void SetBatchOptionsSet(CBatchOptionsSet *set);
	

// Dialog Data
	//{{AFX_DATA(CBatchGeneralProp)
	enum { IDD = IDD_PROPPAGE_BATCH_GENERAL };
	
	CEdit	runDesc;
	BOOL	promptDesc;
	
	
	//}}AFX_DATA

    afx_msg void OnBnClickedPromptDesc();
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBatchGeneralProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBatchGeneralProp)
	virtual BOOL OnInitDialog();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BatchGeneralPROP_)
