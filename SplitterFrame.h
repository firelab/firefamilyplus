#if !defined(AFX_SPLITTERFRAME_H__D54ED147_E426_11D1_B8BA_000000000000__INCLUDED_)
#define AFX_SPLITTERFRAME_H__D54ED147_E426_11D1_B8BA_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SplitterFrame.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CSplitterFrame frame
class Period;
class CFireplusSet;


class CSplitterFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSplitterFrame)
protected:
	CSplitterFrame();           // protected constructor used by dynamic creation
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

// Attributes
public:

// Operations
public:
	CSplitterWnd m_wndSplitter;
	CStatusBar m_wndStatusBar;
	//void GraphViewBitmap(CDC *pDC, CRect rect);
	//void PercentileViewBitmap(CDC *pDC, CRect rect);
	//void SaveGraph();
	void MaximizePane(CView *view);
//	CString mergedStyles[3];
//	int mergedWidths[3];
//	int mergedVars[3];
//	CString mergedVarNames[3];
//	COLORREF mergedColors[3];
//	double mergedYMax[3];
//	void MergeVariables();
//	Period **pcpSums;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSplitterFrame();

	// Generated message map functions
	//{{AFX_MSG(CSplitterFrame)
	afx_msg void OnOptionsOverlays();
	afx_msg void OnUpdateOptionsGraphtypeLineataverage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsGraphtypeStatisticalbars(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPrintgraph();
	afx_msg void OnFilePrint1();
	afx_msg void OnOptionsMerge();
	afx_msg void OnSaveGraph();
	afx_msg void OnSavePercentileGraph();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERFRAME_H__D54ED147_E426_11D1_B8BA_000000000000__INCLUDED_)
