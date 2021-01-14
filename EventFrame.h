#pragma once
// RichFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRichFrame frame

class CEventFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CEventFrame)
protected:
	CEventFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEventFrame();

	// Generated message map functions
	//{{AFX_MSG(CRichFrame)
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

