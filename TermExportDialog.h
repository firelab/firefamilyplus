#if !defined(AFX_TERMEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)
#define AFX_TERMEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TermExportDialog.h : header file
//

#include "TermsDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CTermExportDialog dialog

class CFireplusSet;

class CTermExportDialog : public CDialog
{
// Construction
public:
	
	CTermExportDialog(CTermsDialog* pParent = NULL, CFireplusSet *_fpSet = NULL,int _tID = 0);   // standard constructor
	

// Dialog Data
	enum { IDD = IDD_TERM_EXPORT_DIALOG };
	

   CFireplusSet *fpSet;
   int tID;
  

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TermEXPORTDIALOG_H__3FE68F40_1911_11D4_ABA1_005004BF64C9__INCLUDED_)
