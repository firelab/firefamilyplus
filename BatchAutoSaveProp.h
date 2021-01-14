//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_BATCHAUTOSAVEPROP_)
#define AFX_BATCHAUTOSAVEPROP_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BatchAutoSaveProp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CBatchAutoSaveProp dialog

class CBatchAutoSaveProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CBatchAutoSaveProp)

// Construction
public:
	void WriteData();
	void ReadData();
	void SaveData();
	CBatchAutoSaveProp();
	~CBatchAutoSaveProp();
	CGraphSet * graphSet;
	void SetGraphSet(CGraphSet *set);
	

// Dialog Data
	//{{AFX_DATA(CBatchAutoSaveProp)
	enum { IDD = IDD_PROPPAGE_BATCH_AUTOSAVE };
	CComboBox	fileType;
	//BOOL	autoSave;
	CEdit	Width;
	CEdit	Height;
	CEdit	Location;
	
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBatchAutoSaveProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBatchAutoSaveProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//afx_msg void OnBnClickedAutosaveBatch();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHAUTOSAVEPROP_)
