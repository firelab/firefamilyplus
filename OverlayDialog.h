//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_OVERLAYDIALOG_H__BBF5A5F2_EC29_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_OVERLAYDIALOG_H__BBF5A5F2_EC29_11D1_B8BA_000000000000__INCLUDED_
#include "OverlayCUG.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OverlayDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COverlayDialog dialog

class COverlayDialog : public CDialog
{
	friend class 	CPeriodDoc;
// Construction
public:
	CWnd * m_Parent;
	bool Create();
	int m_nID;
	//CPeriodDoc * pDoc;
	CClimAnalysis *m_pAnalysis;
	CDocument *m_pDoc;
	int varID;
	CStringArray m_yearList;
	CStringArray m_widthsList;
	CStringArray m_lineStyleList;
	//COverlayDialog(CWnd* pParent = NULL, CPeriodDoc *_pDoc = NULL, int _varID = 0);   // standard constructor
	COverlayDialog(CWnd* pParent = NULL, CDocument *pDoc = NULL, CClimAnalysis *pAnalysis = NULL, int _varID = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COverlayDialog)
	enum { IDD = IDD_OVERLAYDIALOG };
	CButton	deleteButton;
	//}}AFX_DATA
	OverlayCUG m_grid;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COverlayDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
		virtual void OnCancel();

protected:

	// Generated message map functions
	//{{AFX_MSG(COverlayDialog)
	afx_msg void OnNewYear();
	afx_msg void OnDeleteYear();
	virtual void OnOK();
	afx_msg void OnApply();
	virtual BOOL OnInitDialog();
	//afx_msg void OnLfClickDatatblctrl1(short Pane, long Row, long Column, long ClickPos, long FAR* Status);
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OVERLAYDIALOG_H__BBF5A5F2_EC29_11D1_B8BA_000000000000__INCLUDED_)
