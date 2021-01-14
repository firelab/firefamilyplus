#pragma once
#include "CNFDRS.h"
#include "nfdrs2016.h"
#include "UGCtrl.h"
#include "ExtCheckBox.h"

class FMPViewCUG : public CUGCtrl
{
public:
	FMPViewCUG();
	~FMPViewCUG();
	virtual void OnSetup();
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual COLORREF OnGetDefBackColor(int section);
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	//CUTNumericEdit	m_cutNumeric;
	CExtCheckBox m_checkBoxCT;
	int m_iCheckBoxIndex;
};


// CFMParamsDlg dialog

class CFMParamsDlg : public CDialog
{
	DECLARE_DYNAMIC(CFMParamsDlg)

public:
	CFMParamsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFMParamsDlg();

// Dialog Data
	enum { IDD = IDD_FMPARAMS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	FMPViewCUG m_grid;
	void AddRow(char _model, int _use88);
	DECLARE_MESSAGE_MAP()
	CRect origRect;
	CRect origWRect;
	CRect origWinRect;
public:
	CComboBox m_FMCombo;
	BOOL m_use88;
	CNFDRS m_nfdrs;
	NFDR2016Calc m_nfdrs2016;
	afx_msg void OnBnClickedAddToTable();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnClose();
	void UpdateConfig();
	afx_msg void OnDestroy();
};
