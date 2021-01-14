#pragma once
#include "afxwin.h"
#include "TermsSet.h"
#include "fireplusSet.h"
#include "ugctrl.h"

class CTermsDataSource :
	public CUGDataSource
{
public:
	CTermsDataSource(void);
	~CTermsDataSource(void);
	
	CTermsSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	void SetRecords(CTermsSet *_records);
	long totalRecs;
	long virRec;

};

class TermsCUG : public CUGCtrl
{
public:
	TermsCUG();
	~TermsCUG();
	virtual void OnSetup();
	//virtual void OnDrawFocusRect(CDC *dc,RECT *rect);
	virtual COLORREF OnGetDefBackColor(int section);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	//virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//menu notifications
	//virtual void OnMenuCommand(int col,long row,int section,int item);
	//virtual int  OnMenuStart(int col,long row,int section);
	CTermsDataSource m_data;
protected:
};

// CTermsDialog dialog

class CTermsDialog : public CDialog
{
	DECLARE_DYNAMIC(CTermsDialog)

public:
	CTermsDialog(CWnd* pParent = NULL, CTermsSet *pRecords = NULL, CString _StaSIG = "", bool _IsGeneral = FALSE, CFireplusSet *_fpSet = NULL);   // standard constructor
	//CTermsDialog(CWnd* pParent = NULL, CTermsSet *pRecords = NULL, CFireplusSet *fpSet = NULL, bool _IsGeneral = FALSE);   // standard constructor
	virtual ~CTermsDialog();
	long virRec;
	long totalRecs;
	TermsCUG m_grid;
	CTermsSet *records;
	CFireplusSet *m_fpSet;

// Dialog Data
	enum { IDD = IDD_TERM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//CFireplusSet *m_fpSet;
	CDatabase *pDB;
	DECLARE_MESSAGE_MAP()
	bool IsGeneral;
	CString m_staID;
public:
	afx_msg void OnBnClickedButtonNew();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	CButton m_EditButton;
	CButton m_DeleteButton;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonExport();
};
