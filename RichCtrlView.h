#pragma once
#include "afxcmn.h"

// CRichCtrlView form view

class CRichCtrlView : public CFormView
{
protected:
	CRichCtrlView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRichCtrlView)
	virtual ~CRichCtrlView();

public:
	enum { IDD = IDD_RICHCTRLVIEW };
	//virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_richCtrl;
	CString m_strText;
	CString m_fileName;
	void LoadFromFile(CString _fileName);
	void SaveToFile(CString _fileName);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Print();
	bool isTemp;

	BOOL IsEdit( CWnd* pWnd );
	void OnUpdateNeedSel(CCmdUI* pCmdUI);
    void OnUpdateNeedClip(CCmdUI* pCmdUI);
	void OnEditCopy();
	void OnEditCut();
	void OnEditPaste();
};