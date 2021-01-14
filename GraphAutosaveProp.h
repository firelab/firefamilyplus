//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_GRAPHAUTOSAVEPROP_)
#define AFX_GRAPHAUTOSAVEPROP_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GraphAutoSaveProp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CGraphAutoSaveProp dialog

class CGraphAutoSaveProp : public CPropertyPage
{
	DECLARE_DYNCREATE(CGraphAutoSaveProp)

// Construction
public:
	void WriteData();
	void ReadData();
	void SaveData();
	CGraphAutoSaveProp();
	~CGraphAutoSaveProp();
	CGraphSet * graphSet;
	void SetGraphSet(CGraphSet *set);
	

// Dialog Data
	//{{AFX_DATA(CGraphAutoSaveProp)
	enum { IDD = IDD_PROPPAGE_GRAPH_AUTOSAVE };
	CComboBox	fileType;
	//BOOL	autoSave;
	CEdit	Width;
	CEdit	Height;
	CEdit	Location;
	
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGraphAutoSaveProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGraphAutoSaveProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//afx_msg void OnBnClickedAutosaveGraph();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHAUTOSAVEPROP_)
