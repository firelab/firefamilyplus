#pragma once
#include "BatchOutputDoc.h"

typedef enum {
	DUMMY = 0, SIGSTATION = 1, VARIABLE = 2,
} ItemType;

class CTreeItem : public CObject
{
public:
	CTreeItem(ItemType _type, HTREEITEM _treeItem, CRawClim *_pClim = NULL, int _varID = -1);
	ItemType type;
	CRawClim *pClim;
	int varID;
	HTREEITEM treeItem;
};
// CBatchOutputTreeView view

class CBatchOutputTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CBatchOutputTreeView)

protected:
	CBatchOutputTreeView();           // protected constructor used by dynamic creation
	virtual ~CBatchOutputTreeView();
	CImageList m_image;
	CImageList m_imageState;
	HTREEITEM root;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTreeFireassociations();
	afx_msg void OnTreeWorkingSet();
	afx_msg void OnTreeEditSIGStation();
};


