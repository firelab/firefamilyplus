#pragma once


// CFW21ExportDlg dialog

class CFW21ExportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFW21ExportDlg)
public:
	CFW21ExportDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFW21ExportDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FW21_EXPORT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editDestFile;
	CButton m_btnWindGusts;
	afx_msg void OnBnClickedButtonDestFile();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CString m_strDestFileName;
	CWxSet *records;
	CComboBox m_comboTimeFormat;
};
