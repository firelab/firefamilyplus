// BatchOutputTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "BatchOutputTreeView.h"
#include ".\batchoutputtreeview.h"
#include "BatchOutputFrame.h"
#include "WorkingSetDialog.h"


CTreeItem::CTreeItem(ItemType _type, HTREEITEM _treeItem, CRawClim *_pClim /*= NULL*/, int _varID/* = -1*/)
{
	type = _type;
	pClim = _pClim;
	varID = _varID;
	treeItem = _treeItem;
}


// CBatchOutputTreeView

IMPLEMENT_DYNCREATE(CBatchOutputTreeView, CTreeView)

CBatchOutputTreeView::CBatchOutputTreeView()
{
}

CBatchOutputTreeView::~CBatchOutputTreeView()
{
}

BEGIN_MESSAGE_MAP(CBatchOutputTreeView, CTreeView)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnTvnDeleteitem)
	//ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
//	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
ON_WM_LBUTTONDOWN()
//ON_WM_CONTEXTMENU()
ON_WM_RBUTTONDOWN()
ON_COMMAND(ID_TREE_FIREASSOCIATIONS, &CBatchOutputTreeView::OnTreeFireassociations)
ON_COMMAND(ID_TREE_WORKINGSET, &CBatchOutputTreeView::OnTreeWorkingSet)
ON_COMMAND(ID_TREE_EDIT, &CBatchOutputTreeView::OnTreeEditSIGStation)
END_MESSAGE_MAP()


// CBatchOutputTreeView diagnostics

#ifdef _DEBUG
void CBatchOutputTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CBatchOutputTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG


// CBatchOutputTreeView message handlers

void CBatchOutputTreeView::OnInitialUpdate()
{
	CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
	m_image.Create( IDB_BITMAPTREE, 13, 1, RGB(255,255,255) );
    GetTreeCtrl().SetImageList( &m_image, TVSIL_NORMAL );
	m_imageState.Create( IDB_BITMAPTREESELECT, 13, 1, RGB(255,255,255) );
    GetTreeCtrl().SetImageList( &m_imageState, TVSIL_STATE);
	CTreeView::OnInitialUpdate();
	root = GetTreeCtrl().InsertItem("SIG/Station");
	CTreeItem *item = new CTreeItem(DUMMY, root);
	GetTreeCtrl().SetItemData(root, (DWORD) item);
	GetTreeCtrl().SetItemState(root, TVIS_BOLD, TVIS_BOLD);
	POSITION pos = pDoc->rawClimList.GetHeadPosition();
	bool firstTime = true;
	HTREEITEM limbWx, limbNFDRS, limbCNFDRS;
	CString staStr;
	while(pos)
	{

		CRawClim *pClim = pDoc->rawClimList.GetNext(pos);
		if(pClim->m_sigStaID.GetLength() <= 6)//single station
			staStr = pClim->m_sigStaID + " - " + pClim->m_StnName;
		else
			staStr = pClim->m_sigStaID;
		HTREEITEM staItem = GetTreeCtrl().InsertItem(staStr, 0, 0, root);
		CTreeItem *item = new CTreeItem(SIGSTATION, staItem, pClim);
		GetTreeCtrl().SetItemData(staItem, (DWORD)item);
		GetTreeCtrl().SetItemState(staItem, TVIS_BOLD, TVIS_BOLD);
		//create/insert limbs
		limbWx = GetTreeCtrl().InsertItem("Weather", 0, 0, staItem);
		item = new CTreeItem(DUMMY, limbWx);
		GetTreeCtrl().SetItemData(limbWx, (DWORD) item);
		GetTreeCtrl().SetItemState(limbWx, TVIS_BOLD, TVIS_BOLD);
		limbNFDRS = GetTreeCtrl().InsertItem("NFDRS", 0, 0, staItem);
		item = new CTreeItem(DUMMY, limbNFDRS);
		GetTreeCtrl().SetItemData(limbNFDRS, (DWORD) item);
		GetTreeCtrl().SetItemState(limbNFDRS, TVIS_BOLD, TVIS_BOLD);
		limbCNFDRS = GetTreeCtrl().InsertItem("CFFDRS", 0, 0, staItem);
		item = new CTreeItem(DUMMY, limbCNFDRS);
		GetTreeCtrl().SetItemData(limbCNFDRS, (DWORD) item);
		GetTreeCtrl().SetItemState(limbCNFDRS, TVIS_BOLD, TVIS_BOLD);
		for(int v = 0; v < pClim->m_nVars; v++)
		{
			HTREEITEM varItem;
			switch(pClim->m_wxData[v]->m_optionType)
			{
			case 0:
			case 2:
				varItem = GetTreeCtrl().InsertItem(pClim->m_wxData[v]->varName, 1, 5, limbWx);
				break;
			case 1:
				varItem = GetTreeCtrl().InsertItem(pClim->m_wxData[v]->varName, 1, 5, limbNFDRS);
				break;
			case 3:
				varItem = GetTreeCtrl().InsertItem(pClim->m_wxData[v]->varName, 1, 5, limbCNFDRS);
				break;
			default:
				varItem = GetTreeCtrl().InsertItem(pClim->m_wxData[v]->varName, 1, 5, limbWx);
			}
			//HTREEITEM varItem = GetTreeCtrl().InsertItem(pClim->m_wxData[v]->varName, 1, 5, staItem);
			CTreeItem *item = new CTreeItem(VARIABLE, varItem, pClim, pClim->m_wxData[v]->varID - 1);
			GetTreeCtrl().SetItemData(varItem, (DWORD)item);
			if(firstTime && v == pDoc->activeVar)
			{
				GetTreeCtrl().Expand(root, TVE_EXPAND);				
				GetTreeCtrl().Expand(staItem, TVE_EXPAND);				
				//GetTreeCtrl().SetItemState(varItem, TVIS_SELECTED, TVIS_SELECTED);
				firstTime = false;
			}
		}
	}
	pDoc->SwitchTreeView(pDoc->activeClim, pDoc->activeVar);
	
}

BOOL CBatchOutputTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES
		| TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS;// | TVS_EDITLABELS;
	return CTreeView::PreCreateWindow(cs);
}

void CBatchOutputTreeView::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if (pNMTreeView->itemOld.lParam)
		delete ((CTreeItem *) pNMTreeView->itemOld.lParam);	
	*pResult = 0;
}

void CBatchOutputTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CTreeItem *itemNew = (CTreeItem *)GetTreeCtrl().GetItemData(pNMTreeView->itemNew.hItem); 
	if(itemNew->type != DUMMY )
	{
		CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
		//if(pDoc->activeClim != itemNew->pClim)
		//{
			//pDoc->activeClim == itemNew->pClim;
			//pDoc->GetAnalysis(pDoc->activeClim);
		//}
		int varNum = pDoc->activeVar;
		if(itemNew->type == VARIABLE)
		{
			varNum = itemNew->varID;
		}
		pDoc->SwitchTreeView(itemNew->pClim, varNum);
	}
	*pResult = 0;
}

//void CBatchOutputTreeView::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CBatchOutputTreeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CTreeView::OnLButtonDown(nFlags, point);
	UINT uFlags;
	HTREEITEM item = GetTreeCtrl().HitTest(point, &uFlags);
	if(item && (TVHT_ONITEM & uFlags))
	{
		CTreeItem * itemData = (CTreeItem *)GetTreeCtrl().GetItemData(item);
		switch(itemData->type)
		{
		case SIGSTATION:
			{
				CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
				if(pDoc->activeClim != itemData->pClim)
					pDoc->SwitchTreeView(itemData->pClim, pDoc->activeVar);
			}
			break;
		case VARIABLE:
			{
				CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
				int varNum = itemData->varID;
				pDoc->SwitchTreeView(itemData->pClim, varNum);
			}
			break;
		default:
			break;
		}
	}
}

/*void CBatchOutputTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//CView::OnRButtonDown(nFlags, point);
	UINT uFlags;
	//CPoint wPoint = point;
	//ScreenToClient(&wPoint);
	//HTREEITEM item = GetTreeCtrl().HitTest(wPoint, &uFlags);
	HTREEITEM item = GetTreeCtrl().HitTest(point, &uFlags);
	if(item)
	{
		CTreeItem *itemData = (CTreeItem *)GetTreeCtrl().GetItemData(item);
		if(itemData->type == SIGSTATION)
		{
			CMenu tMenu;
			tMenu.LoadMenuA(IDR_MENU2);
			CMenu *pPopup = tMenu.GetSubMenu(1);
			if(pPopup)
			{
				GetTreeCtrl().SelectItem(item);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
			}
		}
	}
}*/

void CBatchOutputTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CTreeView::OnRButtonDown(nFlags, point);
	UINT uFlags;
	HTREEITEM item = GetTreeCtrl().HitTest(point, &uFlags);
	if(item && (TVHT_ONITEM & uFlags))
	{
		CTreeItem * itemData = (CTreeItem *)GetTreeCtrl().GetItemData(item);
		switch(itemData->type)
		{
		case SIGSTATION:
			{
				CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
				if(pDoc->activeClim != itemData->pClim)
					pDoc->SwitchTreeView(itemData->pClim, pDoc->activeVar);
				//CTreeItem *itemData = (CTreeItem *)GetTreeCtrl().GetItemData(item);
				//if(itemData->type == SIGSTATION)
				//{
					CMenu tMenu;
					tMenu.LoadMenuA(IDR_MENU2);
					CMenu *pPopup = tMenu.GetSubMenu(1);
					if(pPopup)
					{
						GetTreeCtrl().SelectItem(item);
						CPoint wPoint = point;
						ClientToScreen(&wPoint);
						//pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, wPoint.x, wPoint.y, this);
						pPopup->TrackPopupMenuEx(TPM_LEFTALIGN | TPM_RIGHTBUTTON, wPoint.x, wPoint.y, GetParentFrame(), NULL);
					}
				//}
			}
		}
	}

//	CTreeView::OnRButtonDown(nFlags, point);
}

void CBatchOutputTreeView::OnTreeFireassociations()
{
	/*CBatchOutputFrame *pFrame = (CBatchOutputFrame *)GetParentFrame();
	pFrame->FireAssociations();*/
}
void CBatchOutputTreeView::OnTreeWorkingSet()
{
	/*CBatchOutputFrame *pFrame = (CBatchOutputFrame *)GetParentFrame();
	EnableWindow(FALSE);
	pFrame->WorkingSet();
	EnableWindow(TRUE);*/
	//CQuickBatch *pDoc = (CQuickBatch *)GetDocument();
	//CWorkingSetDialog wsd(this, pDoc->activeClim);
	//if(IDOK == wsd.DoModal())
	//{
	//	pDoc->GetAnalysis(pDoc->activeClim);
	//	pDoc->SwitchTreeView(pDoc->activeClim, pDoc->activeVar);
	//}
}
void CBatchOutputTreeView::OnTreeEditSIGStation()
{
	//CBatchOutputFrame *pFrame = (CBatchOutputFrame *)GetParentFrame();
	//pFrame->SetFocus();
	//::PostMessage(pFrame->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_DATA_EDITSIG, 0), NULL);
	//EnableWindow(FALSE);
	//pFrame->OnDataEditsig();
	//EnableWindow(TRUE);
}
