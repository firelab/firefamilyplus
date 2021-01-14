#pragma once


// CGraphSaveDialog dialog

class CGraphSaveDialog : public CDialog
{
	DECLARE_DYNAMIC(CGraphSaveDialog)

public:
	CGraphSaveDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGraphSaveDialog();

// Dialog Data
	enum { IDD = IDD_GRAPHSAVEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//afx_msg void OnGsave();

	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinGWidth;
	CSpinButtonCtrl m_spinGHeight;
	int width;
	int height;
	virtual BOOL OnInitDialog();

protected:
	virtual void OnOK();
};
