#pragma once
#include "ugctrl.h"
#include "TermPercentilesSet.h"

class TermProbabilitiesCUG : public CUGCtrl
{
public:
	TermProbabilitiesCUG();
	~TermProbabilitiesCUG();
	virtual void OnSetup();
	virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	virtual int OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param);
	virtual COLORREF OnGetDefBackColor(int section);
	virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
};

// CTermProbabilitiesDialog dialog

class CTermProbabilitiesDialog : public CDialog
{
	DECLARE_DYNAMIC(CTermProbabilitiesDialog)

public:
	CTermProbabilitiesDialog(CWnd* pParent = NULL, int _termID = -1, CDatabase *pDB = NULL);   // standard constructor
	virtual ~CTermProbabilitiesDialog();

// Dialog Data
	enum { IDD = IDD_TERM_PROBABILIATIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_TermID;
	CTermPercentilesSet m_pcntSet;
	TermProbabilitiesCUG m_grid;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	bool ValidateProbabilities();
public:
	afx_msg void OnBnClickedButtonInsert();
	afx_msg void OnBnClickedButtonAppend();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAscending();
	afx_msg void OnBnClickedButtonDescending();
	afx_msg void OnBnClickedButtonLoadDefaults();
	afx_msg void OnBnClickedButtonSetDefaults();
};
