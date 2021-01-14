#pragma once

class CQuickBatch;
// CBatchOutputFrame frame with splitter

class CBatchOutputFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CBatchOutputFrame)
public:
	CBatchOutputFrame();           // protected constructor used by dynamic creation
	virtual ~CBatchOutputFrame();

	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter2;
	CQuickBatch *m_pDoc;
	CStatusBar m_wndStatusBar;
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	CToolBar    m_wndToolBar;

	DECLARE_MESSAGE_MAP()
public:
	int SwitchView(int viewNum);
	afx_msg void FireOptions();
	afx_msg void FireAssociations();
	afx_msg void WorkingSet();
	afx_msg void GraphOptions();
	afx_msg void OnOverlays();
	afx_msg void OnFileSave();
	afx_msg void OnFilePrint();
	afx_msg void OnFileProperties();
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDataAddtocandidateslist();
	afx_msg void OnDataViewCandidateslist();
	afx_msg void OnDataEditsig();
	afx_msg void OnUpdateDataAddtocandidateslist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOverlays(CCmdUI *pCmdUI);
	afx_msg void OnUpdateGraphOptions(CCmdUI *pCmdUI);
};


