//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_MERGEDIALOG_H__0A52A9A3_9449_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_MERGEDIALOG_H__0A52A9A3_9449_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeDialog.h : header file
//
#include "GraphView.h"
#include "ugctrl.h"
#include "ugctbutn.h"

class CMergedView;
class CMergeDialog;

class MergeCUG : public CUGCtrl
{
public:
	MergeCUG();
	~MergeCUG();
	CMergeDialog *pDlg;
protected:
	CUGButtonType		m_button;
	int					m_nButtonIndex;
public:
	//***** Over-ridable Notify Functions *****
	virtual void OnSetup();
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
};

/////////////////////////////////////////////////////////////////////////////
// CMergeDialog dialog

class CMergeDialog : public CDialog
{
// Construction

public:
	void FillTable();
	//CGraphView * trgView;
	CWnd* parent;
	CPeriodDoc * pDoc;
	CMergeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMergeDialog)
	enum { IDD = IDD_MERGEDIALOG };
	CListBox	yrsList;
	//}}AFX_DATA
	MergeCUG m_grid;
	CString m_varsString;
	//CStringArray m_widthsList;
	//CStringArray m_lineStyleList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMergeDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDIALOG_H__0A52A9A3_9449_11D4_ABA1_005004BF64C9__INCLUDED_)
