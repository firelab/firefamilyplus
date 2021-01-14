#if !defined(AFX_FARSITEDIALOG_H__800A4A80_1434_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_FARSITEDIALOG_H__800A4A80_1434_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "fireplusSet.h"
#include "WxSet.h"
// FARSITEDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFARSITEDialog dialog

class CFARSITEDialog : public CDialog
{
// Construction
public:
	void GetLastPcp();
	void WriteWinds();
	void WriteWx4();
	void WriteWx3();
	void WriteWxs();

	void GenerateFiles();
	void EnableSelections();
	FILE * winds;
	FILE * wx4;
	FILE * wx3;
	FILE * wxs;

	CFireplusSet *fpSet;
	int OutputData();
	CFARSITEDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFARSITEDialog)
	enum { IDD = IDD_FARSITEDIALOG };
	CStatic	wndStatic;
	CStatic	wtr4Static;
	//CStatic	wtr3Static;
	CButton	wtr4Button;
	//CButton	wtr3Button;
	CButton	wndButton;
	CButton wxsButton;
	CButton windsAveButton;
	CButton windsGustButton;
	int windsType;
	CEdit	wtr4Edit;
	CEdit	wtr3Edit;
	CEdit	wndEdit;
	CEdit	wxsEdit;

	BOOL	bWnd;
	//BOOL	bWtr3;
	BOOL	bWtr4;
	BOOL    bWxs;

	CString	wndFileName;
	//CString	wtr3FileName;
	CString	wtr4FileName;
	//}}AFX_DATA
	CString	wxsFileName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFARSITEDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFARSITEDialog)
	virtual void OnOK();
	afx_msg void OnWtr4check();
	//afx_msg void OnWtr3check();
	afx_msg void OnWndcheck();
	afx_msg void OnWndfilebutton();
	//afx_msg void OnWtr3filebutton();
	afx_msg void OnWtr4filebutton();
	afx_msg void OnWxscheck();
	afx_msg void OnWxsAvecheck();
	afx_msg void OnWxsGustcheck();
	afx_msg void OnWxsfilebutton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	long recs3, recs4, recsWnd, recsWxs, added3, added4;
	COleDateTime curDay, pcpStart, pcpEnd;
	//CTime curDay, pcpStart, pcpEnd;
	CWxSet wxSet;
	int elev, slope,aspect,maxRH, minRH, maxT, minT, maxHr, minHr, cloud;
	double lat, lon;
	double pcp, lastPcp;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FARSITEDIALOG_H__800A4A80_1434_11D4_ABA1_005004BF64C9__INCLUDED_)
