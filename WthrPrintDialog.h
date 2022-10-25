#if !defined(AFX_WTHRPRINTDIALOG_H__F96F62C5_F666_11D3_ABA0_005004BF64C9__INCLUDED_)
#define AFX_WTHRPRINTDIALOG_H__F96F62C5_F666_11D3_ABA0_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WthrPrintDialog.h : header file
//
#include "wxset.h"
#include "CFiresSet.h"
//#include "BrowseDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CWthrPrintDialog dialog

class CWthrPrintDialog : public CDialog
{
// Construction
public:
	CSize sz;
	long recPos;
	CFont * oldFont;
	long retRec;
	CFont *pFont;
	void Print();
	CBitmapButton upButton;
	CBitmapButton downButton;
	CBitmapButton topButton;
	CBitmapButton bottomButton;
	CWthrPrintDialog(CWnd* pParent = NULL, CWxSet *_wxSet = NULL, CFiresSet *_fireSet = NULL);   // standard constructor
	CWxSet *wxSet;
	CFiresSet *fireSet;
	void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
// Dialog Data
	//{{AFX_DATA(CWthrPrintDialog)
	enum { IDD = IDD_WTHRPRINTDIALOG };
	CListBox	selectedList;
	CListBox	fieldsList;
	BOOL	headers;
	BOOL	pageNums;
	BOOL	recNums;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWthrPrintDialog)
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
	//{{AFX_MSG(CWthrPrintDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectall();
	afx_msg void OnRemoveall();
	afx_msg void OnMoveleft();
	afx_msg void OnMoveright();
	afx_msg void OnDblclkFieldslist();
	afx_msg void OnDblclkSelectedlist();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTHRPRINTDIALOG_H__F96F62C5_F666_11D3_ABA0_005004BF64C9__INCLUDED_)
