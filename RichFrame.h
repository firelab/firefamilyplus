#if !defined(AFX_RICHFRAME_H__565CB533_3783_11D2_B8D2_000000000000__INCLUDED_)
#define AFX_RICHFRAME_H__565CB533_3783_11D2_B8D2_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RichFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRichFrame frame

class CRichFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CRichFrame)
protected:
	CRichFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRichFrame();

	// Generated message map functions
	//{{AFX_MSG(CRichFrame)
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHFRAME_H__565CB533_3783_11D2_B8D2_000000000000__INCLUDED_)
