#pragma once
#include "afxdialogex.h"
#include "updialog2.h"


// CFW21ImportDlg dialog
class CFireplusDoc;

class CFW21ImportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFW21ImportDlg)

public:
	CFW21ImportDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFW21ImportDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FW21_IMPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CFireplusDoc* pDoc;
	CComboBox m_comboStation;
	CEdit m_editFW21File;
	CString m_strFW21File;
	afx_msg void OnBnClickedButtonFw21File();
	afx_msg void OnBnClickedImportFw21();
	virtual BOOL OnInitDialog();
	//bool ImportFW21RunProc(const CUPDUPDATA* pCUPDUPData);
};
