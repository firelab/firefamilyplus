#pragma once
#include "UGCtrl.h"
#include "ExtCheckBox.h"
#include "fireplusSet.h"

class BatchCUG : public CUGCtrl
{
public:
	BatchCUG();
	~BatchCUG();
	virtual void OnSetup();
	//virtual int OnEditStart(int col, long row,CWnd **edit);
	virtual void OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed);
	virtual void OnCharDown(UINT *vcKey,BOOL processed);
	virtual void OnKeyDown(UINT *vcKey,BOOL processed);
	//virtual int OnCellTypeNotify(long ID,int col,long row,long msg,long param);
	CExtCheckBox m_checkBoxCT;
	int m_iCheckBoxIndex;
};



// CQuickBatchDialog dialog

class CQuickBatchDialog : public CDialog
{
	DECLARE_DYNAMIC(CQuickBatchDialog)

public:
	CQuickBatchDialog(CWnd* pParent = NULL, CDatabase *pDB = NULL, CFireplusSet *pFpSet = NULL);   // standard constructor
	virtual ~CQuickBatchDialog();
	void ConfigureTable();

// Dialog Data
	enum { IDD = IDD_QUICKBATCHDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CDatabase *m_pDB;
	BatchCUG m_grid;
	CFireplusSet *m_pFpSet;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
