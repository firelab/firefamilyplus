#pragma once
#include "afxwin.h"
//#include "eventlocatordoc.h"


// CAddToTermDialog dialog
class CEventLocatorDoc;
class CAddToTermDialog : public CDialog
{
	DECLARE_DYNAMIC(CAddToTermDialog)

public:
	CAddToTermDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddToTermDialog();

// Dialog Data
	CEventLocatorDoc *pDoc;
	enum { IDD = IDD_ADD_TO_TERM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	void AddEventsToTerm(int tID);
public:
	afx_msg void OnBnClickedButtonAddToNew();
	afx_msg void OnBnClickedButtonAddExisting();
	CComboBox m_ExistingTermsCombo;
	virtual BOOL OnInitDialog();
	CButton m_addExistingButton;
};
