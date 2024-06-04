#pragma once
#include <afxwin.h>

class CGraphOptions;
class CClimAnalysis;
class CFireplusSet;

class CPercentilesView :
    public CView
{
public:
    CPercentilesView();
    ~CPercentilesView();
    DECLARE_DYNCREATE(CPercentilesView)

    CClimAnalysis* m_pAnalysis;
    CGraphOptions* m_pGraphOpts;

    double IdealFromYMax(double yMax);
    double overallCP2;
    double overallCP1;
    void PrintGraph();
    double* cps;
    double* cp2s;
    double* mins;
    double* maxs;
    int optionType;
    int varID;
    int gap;
    int barwidth;
    double ideal;
    double ymax;
    double ymin;
    virtual void OnInitialUpdate();
protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    CFireplusSet* fpSet;
protected:
    //{{AFX_MSG(CPercentilesView)
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnOptionsGraphtypeLineataverage();
    afx_msg void OnOptionsGraphtypeStatisticalbars();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnFileSaveAs();
    afx_msg void OnFileSave();
    afx_msg void	OnViewHeader();
    afx_msg void OnExportData();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    void SaveGraph(void);
    void SaveGraph(CString _outName, CRect* _outRect);
    void PreDrawCalcs();
};

