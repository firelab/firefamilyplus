// BatchOutputFrame.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "BatchOutputFrame.h"
#include "BatchOutputTabView.h"
#include "BatchOutputDoc.h"
//#include "FFPTabView.h"
#include <afxrich.h>
#include "richview.h"
//#include "NullView.h"
#include ".\batchoutputframe.h"
#include "FireDay.h"
#include "wxSet.h"
#include "perioddoc.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include "RichCtrlView.h"
#include "WorkingSetDialog.h"
#include "graphview.h"
#include "PercentileView.h"
#include "graphset.h"
#include "UserVarSet.h"
#include "ForecastDialog.h"
#include "PocketCardView.h"
#include "PocketCardSet.h"
#include "StationInSIGSet.h"
#include "QuickBatchGraphSheet.h"
#include "OverlayDialog.h"
#include "firestats.h"
#include "CumulativeBars.h"
#include "CumulativePcnt.h"
#include "FireStatGraph.h"
#include "YearsPage.h"
#include "FireQuerySheet.h"
#include "QuickFireOptionsDlg.h"
#include<direct.h>
#include "PocketCardSet.h"
#include "PocketCardDialog.h"
#include "ListingDialog.h"
#include "FFPOverlaysSet.h"
#include "CandidateSet.h"
#include "EditStationDialog.h"
#include "sigset.h"
#include "SIGEditDialog.h"
#include "FireSumView.h"
#include "CandidatesDoc.h"
#include "CandidatesFrame.h"
#include "CandidatesView.h"

extern CFireplusApp theApp;
// CBatchOutputFrame
static UINT statusindicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_XPROMPT,
	ID_INDICATOR_X,
	ID_INDICATOR_YPROMPT,
	ID_INDICATOR_Y,
//	ID_INDICATOR_CAPS,
};

IMPLEMENT_DYNCREATE(CBatchOutputFrame, CMDIChildWnd)

CBatchOutputFrame::CBatchOutputFrame()
{
}

CBatchOutputFrame::~CBatchOutputFrame()
{
}

BOOL CBatchOutputFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	//return m_wndSplitter.Create(this,
	//	1, 2,       // TODO: adjust the number of rows, columns
	//	CSize(10, 10),  // TODO: adjust the minimum pane size
	//	pContext);
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(statusindicators,
		  sizeof(statusindicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	//CONFIGURE STATUS BAR
	UINT id, style;
	int wid;
	m_wndStatusBar.GetPaneInfo(1, id, style , wid);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_XPROMPT, SBPS_NOBORDERS , wid);
	m_wndStatusBar.GetPaneInfo(2, id, style , wid);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_X, SBPS_NORMAL , wid + wid);
	m_wndStatusBar.GetPaneInfo(3, id, style , wid);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_YPROMPT, SBPS_NOBORDERS , wid);
	m_wndStatusBar.GetPaneInfo(4, id, style , wid);
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_Y, SBPS_NORMAL , wid + wid);
	m_wndStatusBar.SetPaneText(0, "Left click to retrieve values");
	m_wndStatusBar.SetPaneText(2, "");
	m_wndStatusBar.SetPaneText(4, "");

   if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_BATCHOUTPUT))
   {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
   }

   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);

	m_pDoc = (CQuickBatch *)pContext->m_pCurrentDoc;
	m_pDoc->m_pFrame = this;
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 1)
			// new splitter is in the first row, 1st column of 2nd splitter
	   ))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}
	if (!m_wndSplitter.CreateView(0, 0,
		pContext->m_pNewViewClass, CSize(200, 50), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
	if (!m_wndSplitter2.CreateView(0, 0,
		RUNTIME_CLASS(CBatchOutputTabView), CSize(0, 21), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	if (!m_wndSplitter2.CreateView(1, 0,
		RUNTIME_CLASS(CEditView), CSize(0, 0), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	//m_wndSplitter2.SendMessage(WM_GETMINMAXINFO
	//m_wndSplitter2.EnableWindow(FALSE);
	//m_wndSplitter2.EnableR
	return TRUE;
}

BEGIN_MESSAGE_MAP(CBatchOutputFrame, CMDIChildWnd)
	ON_COMMAND(32911, WorkingSet)
	ON_COMMAND(ID_BUTTON_FIRE_ASSOC, FireAssociations)
	ON_COMMAND(ID_BUTTON_FIRE_OPTIONS, FireOptions)
	ON_COMMAND(ID_QB_GRAPH_OPTIONS, GraphOptions)
	ON_COMMAND(ID_QUICK_OVERLAYS, OnOverlays)
	ON_COMMAND(ID_QUICK_SAVE, OnFileSave)
	ON_COMMAND(ID_QUICK_PRINT, OnFilePrint)
	ON_COMMAND(ID_QUICK_PROPERTIES, OnFileProperties)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_QB_CLOSE, OnClose)
	ON_COMMAND(ID_DATA_ADDTOCANDIDATESLIST, &CBatchOutputFrame::OnDataAddtocandidateslist)
	ON_COMMAND(ID_WEATHER_FIREBUSINESSCANDIDATES, &CBatchOutputFrame::OnDataViewCandidateslist)
	ON_COMMAND(ID_DATA_EDITSIG, &CBatchOutputFrame::OnDataEditsig)
	ON_UPDATE_COMMAND_UI(ID_DATA_ADDTOCANDIDATESLIST, &CBatchOutputFrame::OnUpdateDataAddtocandidateslist)
	ON_UPDATE_COMMAND_UI(ID_QUICK_OVERLAYS, &CBatchOutputFrame::OnUpdateOverlays)
	ON_UPDATE_COMMAND_UI(ID_QB_GRAPH_OPTIONS, &CBatchOutputFrame::OnUpdateGraphOptions)
END_MESSAGE_MAP()


// CBatchOutputFrame message handlers

int CBatchOutputFrame::SwitchView(int viewNum)
{
	CWaitCursor wait;
	CCreateContext pContext;
	pContext.m_pCurrentDoc = (CDocument *)m_pDoc;
	pContext.m_pCurrentFrame = this;
	pContext.m_pLastView = NULL;
	pContext.m_pNewDocTemplate = NULL;
	pContext.m_pNewViewClass = NULL;
	m_wndSplitter2.DeleteView(1, 0);
	switch(viewNum)
	{
	case 0://stats report
		{
			m_pDoc->m_pAnalysis->climSet->m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			m_pDoc->m_pAnalysis->climSet->Requery();
			CString tFileName = "";
			if(m_pDoc->activeVar == 8)//precip table
				tFileName = m_pDoc->m_pAnalysis->PrecipTable(m_pDoc->activeVar, m_pDoc->activeClim->m_climSet->m_CriticalPercentile / 100.0, false);
			else
				tFileName = m_pDoc->m_pAnalysis->StatsTable(m_pDoc->activeVar, m_pDoc->activeClim->m_climSet->m_CriticalPercentile / 100.0, false);
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 2://stats listing
		{
			m_pDoc->m_pAnalysis->climSet->m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			m_pDoc->m_pAnalysis->climSet->Requery();
			CString tFileName = "";
			tFileName = m_pDoc->m_pAnalysis->StatsGraphDataListing(m_pDoc->activeVar, false);
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 4://daily frequencies
		{
			m_pDoc->m_pAnalysis->climSet->m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			m_pDoc->m_pAnalysis->climSet->Requery();
			CString tFileName = "";
			if(m_pDoc->activeVar == 8)//precip table
				tFileName = m_pDoc->m_pAnalysis->PrecipFrequencyTable(m_pDoc->activeVar, false);
			else
				tFileName = m_pDoc->m_pAnalysis->FrequencyTable(m_pDoc->activeVar, false);
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 5://data count
		{
			m_pDoc->m_pAnalysis->climSet->m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			m_pDoc->m_pAnalysis->climSet->Requery();
			CString tFileName = "";
			tFileName = m_pDoc->m_pAnalysis->DataCount(m_pDoc->activeVar, false);
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 7://Daily Listing
		{
			//if(
			CString tFileName = "";
			//insure activevar is selected in daily listing output...
			m_pDoc->m_pAnalysis->climSet->m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			m_pDoc->m_pAnalysis->climSet->Requery();
			if(m_pDoc->m_pAnalysis->climSet->m_Daily_List <= 0)
			{
				m_pDoc->m_pAnalysis->climSet->m_strFilter = "";
				m_pDoc->m_pAnalysis->climSet->m_strSort = "[Daily List]";
				m_pDoc->m_pAnalysis->climSet->Requery();
				m_pDoc->m_pAnalysis->climSet->MoveLast();
				int lastVar = m_pDoc->m_pAnalysis->climSet->m_Daily_List;
				m_pDoc->m_pAnalysis->climSet->m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
				m_pDoc->m_pAnalysis->climSet->Requery();
				m_pDoc->m_pAnalysis->climSet->Edit();
				m_pDoc->m_pAnalysis->climSet->m_Daily_List = lastVar + 1;
				m_pDoc->m_pAnalysis->climSet->Update();
			}

			


			tFileName = m_pDoc->m_pAnalysis->DailyListing(&theApp.lo, false,false);
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 1://stats graph
		{
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CGraphView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CGraphView *pNewView = (CGraphView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			CGraphSet gSet(m_pDoc->m_pDB);
			gSet.Open();//CRecordset::dynaset, "SELECT * FROM ffpGraphOptions");
					//pDoc->m_pAnalysis = this;
			m_pDoc->m_pAnalysis->m_opts.dateTime = COleDateTime::GetCurrentTime();
			m_pDoc->m_pAnalysis->m_opts.nVars = m_pDoc->m_pAnalysis->nVarIDs;
			m_pDoc->m_pAnalysis->m_opts.pDB = m_pDoc->m_pDB;
			m_pDoc->m_pAnalysis->killPeriods = true;//false;
			m_pDoc->m_pAnalysis->m_opts.fires = m_pDoc->m_pAnalysis->fires;
			m_pDoc->m_pAnalysis->m_opts.numPeriods = m_pDoc->m_pAnalysis->numPeriods;
			m_pDoc->m_pAnalysis->m_opts.periodLength = atoi(m_pDoc->m_pAnalysis->fpSet->m_PeriodLength);
					//GRAPH OPTIONS
			m_pDoc->m_pAnalysis->m_opts.nWxObs = m_pDoc->m_pAnalysis->count;
			if(strncmp(m_pDoc->m_pAnalysis->fpSet->m_SIG_Station, "SIG - ", 6) == 0)//is a SIG
			{
				m_pDoc->m_pAnalysis->m_opts.stationName = m_pDoc->m_pAnalysis->fpSet->m_SIG_Station;
			}
			else
			{
				m_pDoc->m_pAnalysis->m_opts.stationName = m_pDoc->m_pAnalysis->fpSet->m_SIG_Station + "-" + m_pDoc->m_pAnalysis->m_strStationName;
			}
			m_pDoc->m_pAnalysis->m_opts.stationName.TrimRight();
			m_pDoc->m_pAnalysis->m_opts.model = m_pDoc->m_pAnalysis->m_strModel;
			//m_pDoc->m_pAnalysis->m_opts.numtPeriods = 0;//m_pDoc->m_pAnalysis->m_optstnumPeriods;
			//m_pDoc->m_pAnalysis->m_opts.tPeriods = NULL;//m_pDoc->m_pAnalysis->tperiods;
			//m_pDoc->m_pAnalysis->m_opts.tFires = NULL;//m_pDoc->m_pAnalysis->tFires;
					//check overlays
			CFFPOverlaysSet olSet(m_pDoc->m_pDB);
			olSet.m_strFilter = "[runID] = 0";
			//olSet.m_strFilter.Format("[SIG_Station] = '%s'", m_pDoc->m_pAnalysis->fpSet->m_SIG_Station);
			olSet.Open();
			int oCnt = 0;
			//if(olSet.IsEOF())
			//{
				//no previously saved record for this SIG_Station
				//olSet.m_strFilter = "[runID] = 0";
				//olSet.Requery();
			//}
			while(!olSet.IsEOF() && oCnt < OVERLAYS)
			{
				if(olSet.m_Year >= m_pDoc->m_pAnalysis->periods[m_pDoc->activeVar][0]->baseYear
					&& olSet.m_Year < m_pDoc->m_pAnalysis->periods[m_pDoc->activeVar][0]->baseYear 
					+ m_pDoc->m_pAnalysis->periods[m_pDoc->activeVar][0]->years)
				{
					m_pDoc->m_pAnalysis->m_opts.overLayYears[oCnt] = olSet.m_Year;
					m_pDoc->m_pAnalysis->m_opts.overLayColors[oCnt] = olSet.m_Color;
					m_pDoc->m_pAnalysis->m_opts.lineStyles[oCnt] = olSet.m_Style;
					m_pDoc->m_pAnalysis->m_opts.lineWidths[oCnt] = olSet.m_Width;
					oCnt++;
				}
				/*else if(tnumPeriods > 0)
				{
					if(olSet.m_Year >= tperiods[varID][0]->baseYear 
						&& olSet.m_Year < tperiods[varID][0]->baseYear + tperiods[varID][0]->years)
					{*/
				olSet.MoveNext();
			}
			olSet.Close();
			if(oCnt == 0)
			{
				int loc = 0;
				for(int i = 0; i < OVERLAYS; i++)
				{
					if(theApp.overLayYears[i] >= m_pDoc->m_pAnalysis->periods[m_pDoc->activeVar][0]->baseYear 
						&& theApp.overLayYears[i] < m_pDoc->m_pAnalysis->periods[m_pDoc->activeVar][0]->baseYear 
						+ m_pDoc->m_pAnalysis->periods[m_pDoc->activeVar][0]->years)
					{
						m_pDoc->m_pAnalysis->m_opts.overLayYears[loc] = theApp.overLayYears[i];
						m_pDoc->m_pAnalysis->m_opts.overLayColors[loc] = theApp.overLayColors[i];
						m_pDoc->m_pAnalysis->m_opts.lineStyles[loc] = theApp.lineStyles[i];
						m_pDoc->m_pAnalysis->m_opts.lineWidths[loc] = theApp.lineWidths[i];
						loc++;
					}
							/*else if(m_pDoc->m_pAnalysis->m_opts.numtPeriods > 0)
							{
								if(theApp.overLayYears[i] >= tperiods[m_pDoc->activeVar][0]->baseYear 
								&& theApp.overLayYears[i] < tperiods[m_pDoc->activeVar][0]->baseYear + tperiods[m_pDoc->activeVar][0]->years)
								{
									m_pDoc->m_pAnalysis->m_opts.overLayYears[loc] = theApp.overLayYears[i];
									m_pDoc->m_pAnalysis->m_opts.overLayColors[loc] = theApp.overLayColors[i];
									m_pDoc->m_pAnalysis->m_opts.lineStyles[loc] = theApp.lineStyles[i];
									m_pDoc->m_pAnalysis->m_opts.lineWidths[loc] = theApp.lineWidths[i];
									loc++;
								}
							}*/
				}
			}
			CClimateSet climSet(m_pDoc->m_pDB);
			climSet.m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			climSet.Open();
		//}
				//CSplitterFrame* pFrame = (CSplitterFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
				//CGraphView *pView = (CGraphView *) pFrame->m_wndSplitter.GetPane(0, 0);
			pNewView->m_graphOpts.varID = m_pDoc->activeVar;
			pNewView->m_graphOpts.optionType = climSet.m_OptionType;
			pNewView->m_graphOpts.varName = climSet.m_Variable_Name;
			pNewView->m_graphOpts.isBargraph = (gSet.m_InitBargraph == 0) ? false : true;
			pNewView->m_graphOpts.criticalPercentile = climSet.m_CriticalPercentile / 100.0;
			pNewView->m_graphOpts.criticalPercentile2 = climSet.m_cp2 / 100.0;
			pNewView->m_graphOpts.m_Horizontal_Lines = gSet.m_Horizontal_Lines;
			pNewView->m_graphOpts.m_Vertical_Lines = gSet.m_Vertical_Lines;
			pNewView->m_graphOpts.m_Range_Tics = gSet.m_Range_Tics;
			pNewView->m_graphOpts.m_CP_Tics = gSet.m_CP_Tics;
			pNewView->m_graphOpts.m_Line_Width = gSet.m_Line_Width;
			pNewView->m_graphOpts.m_H_Line_Style = gSet.m_H_Line_Style;
			pNewView->m_graphOpts.m_V_Line_Style = gSet.m_V_Line_Style;
			pNewView->m_graphOpts.m_Line_Color = (COLORREF)gSet.m_Line_Color;
			pNewView->m_graphOpts.m_cp1s = gSet.m_CP1;
			pNewView->m_graphOpts.m_cp2s = gSet.m_CP2;
			pNewView->m_graphOpts.m_maxs = gSet.m_Maxs;
			pNewView->m_graphOpts.m_mins = gSet.m_Mins;
			pNewView->m_graphOpts.m_cp1Width = gSet.m_CP1Width;
			pNewView->m_graphOpts.m_cp2Width = gSet.m_CP2Width;
			pNewView->m_graphOpts.m_maxsWidth = gSet.m_MaxsWidth;
			pNewView->m_graphOpts.m_minsWidth = gSet.m_MinsWidth;
			pNewView->m_graphOpts.m_cp1Color = gSet.m_CP1Color;
			pNewView->m_graphOpts.m_cp2Color = gSet.m_CP2Color;
			pNewView->m_graphOpts.m_maxsColor = gSet.m_MaxsColor;
			pNewView->m_graphOpts.m_minsColor = gSet.m_MinsColor;
			pNewView->m_graphOpts.m_Cause = gSet.m_Cause;
			pNewView->m_graphOpts.m_FD = gSet.m_FD;
			pNewView->m_graphOpts.m_LFD = gSet.m_LFD;
			pNewView->m_graphOpts.m_MFD = gSet.m_MFD;
			pNewView->m_graphOpts.m_LFDAcres = gSet.m_LFDAcres;
			pNewView->m_graphOpts.m_MFDFires = gSet.m_MFDFires;
			pNewView->m_graphOpts.m_OverallCP1 = theApp.m_OverallCP1;
			pNewView->m_graphOpts.m_OverallCP2 = theApp.m_OverallCP2;
			pNewView->m_graphOpts.m_OverallCP1Color = theApp.m_OverallCP1Color;
			pNewView->m_graphOpts.m_OverallCP2Color = theApp.m_OverallCP2Color;
				// added for batch2
			pNewView->m_graphOpts.fpSet = m_pDoc->m_pAnalysis->fpSet;
			pNewView->m_pAnalysis = m_pDoc->m_pAnalysis;
				/*CPercentileView *pctView = (CPercentileView *) pFrame->m_wndSplitter.GetPane(0, 1);
				//pctView->m_graphOpts = &pView->m_graphOpts;
				pctView->optionType = optType;
				pctView->varID = varID;
				pctView->varName = climSet->m_Variable_Name;
				pctView->criticalPercentile = climSet->m_CriticalPercentile / 100.0;
				pctView->criticalPercentile2 = climSet->m_cp2 / 100.0;
				pctView->m_pAnalysis = this;*/

			gSet.Close();
			ForecastOptions fo;
			fo.Read(m_pDoc->m_pDB);
			pNewView->m_graphOpts.m_fxColor = fo.lineColor;
			pNewView->m_graphOpts.m_fxWidth = fo.lineWidth;
			pNewView->m_graphOpts.m_fxStyle = fo.lineStyle;
			climSet.Close();
			pNewView->OnInitialUpdate();
		}
		break;
	case 3://percentiles graph
		{
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CPercentileView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CClimateSet climSet(m_pDoc->m_pDB);
			climSet.m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
			climSet.Open();
			CPercentileView *pctView = (CPercentileView *)m_wndSplitter2.GetPane(1, 0);
			pctView->optionType = climSet.m_OptionType;
			pctView->varID = m_pDoc->activeVar;;
			pctView->varName = climSet.m_Variable_Name;
			pctView->criticalPercentile = climSet.m_CriticalPercentile / 100.0;
			pctView->criticalPercentile2 = climSet.m_cp2 / 100.0;
			m_pDoc->m_pAnalysis->m_opts.dateTime = COleDateTime::GetCurrentTime();
			pctView->m_pAnalysis = m_pDoc->m_pAnalysis;
			if (!climSet.IsFieldNull(&climSet.m_FilterValue))
				pctView->filterValue = climSet.m_FilterValue;
			if (climSet.m_cp2 < climSet.m_CriticalPercentile)
				pctView->filterDirection = FILTER_GREATERTHAN_EQUAL;
			climSet.Close();
			pctView->OnInitialUpdate();
			
		}
		break;
	case 6: //pocket card
		{
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CPocketCardView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CPocketCardView *pctView = (CPocketCardView *)m_wndSplitter2.GetPane(1, 0);
			pctView->m_pAnalysis = m_pDoc->m_pAnalysis;
			COleDateTime today;
			today = COleDateTime::GetCurrentTime();
			CPocketCardSet *pocketSet = new CPocketCardSet(m_pDoc->m_pDB);
			pocketSet->OpenEnsureSIGStationRec(m_pDoc->activeClim->m_sigStaID);
			m_pDoc->m_pAnalysis->LoadPocketOptions(pocketSet, m_pDoc->activeVar);
			pocketSet->Close();
			delete pocketSet;
			pctView->OnInitialUpdate();
		}
		break;
	case 8://Fire Occurrence
		{
			if(!m_pDoc->m_pFiresSummary)
				m_pDoc->GetFireSummary();
			if(!m_pDoc->m_pFiresSummary)
			{
				//AfxMessageBox("No Fires, deal with it...");
				m_wndSplitter2.CreateView(1, 0,
					RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext);
				break;
			}
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CFireSumView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CFireSumView *fsView = (CFireSumView *)m_wndSplitter2.GetPane(1, 0);
			fsView->m_pFiresSummary = m_pDoc->m_pFiresSummary;
			fsView->OnInitialUpdate();

		}
		break;
	case 9://Fires Percentiles
		{
			if(!m_pDoc->m_pFireStats)
				m_pDoc->GetFireStats();
			if(!m_pDoc->m_pFireStats)
			{
				AfxMessageBox("No Fires, deal with it...");
				m_wndSplitter2.CreateView(1, 0,
					RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext);
				break;
			}
			CString tFileName = m_pDoc->m_pFireStats->ExportPercentiles();
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 10://Fires Percentile Graph
		{
			if(!m_pDoc->m_pFireStats)
				m_pDoc->GetFireStats();
			if(!m_pDoc->m_pFireStats)
			{
				AfxMessageBox("No Fires, deal with it...");
				m_wndSplitter2.CreateView(1, 0,
					RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext);
				break;
			}
			//CCumulativePcnt
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CCumulativePcnt), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CCumulativePcnt *pctView = (CCumulativePcnt *)m_wndSplitter2.GetPane(1, 0);
			pctView->m_fires = m_pDoc->m_pFireStats;
			pctView->OnInitialUpdate();
		}
		break;
	case 11://Cummulative Fires
		{
			if(!m_pDoc->m_pFireStats)
				m_pDoc->GetFireStats();
			if(!m_pDoc->m_pFireStats)
			{
				AfxMessageBox("No Fires, deal with it...");
				m_wndSplitter2.CreateView(1, 0,
					RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext);
				break;
			}
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CCumulativeBars), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CCumulativeBars *pctView = (CCumulativeBars *)m_wndSplitter2.GetPane(1, 0);
			pctView->m_fires = m_pDoc->m_pFireStats;
			pctView->OnInitialUpdate();
		}
		break;
	case 12://Fires Analysis Report
		{
			if(!m_pDoc->m_pFireStats)
				m_pDoc->GetFireStats();
			if(!m_pDoc->m_pFireStats)
			{
				AfxMessageBox("No Fires, deal with it...");
				m_wndSplitter2.CreateView(1, 0,
					RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext);
				break;
			}
			CString tFileName = m_pDoc->m_pFireStats->LogRegReport();
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CRichCtrlView), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CRichCtrlView *pNewView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
			pNewView->LoadFromFile(tFileName);
		}
		break;
	case 13://Fires Analysis Graph
		{
			if(!m_pDoc->m_pFireStats)
				m_pDoc->GetFireStats();
			if(!m_pDoc->m_pFireStats)
			{
				AfxMessageBox("No Fires, deal with it...");
				m_wndSplitter2.CreateView(1, 0,
					RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext);
				break;
			}
			//CFireStatGraph
			if (!m_wndSplitter2.CreateView(1, 0,
				RUNTIME_CLASS(CFireStatGraph), CSize(0, 0), &pContext))
			{
				TRACE0("Failed to create second pane\n");
				return FALSE;
			}
			CFireStatGraph *pctView = (CFireStatGraph *)m_wndSplitter2.GetPane(1, 0);
			pctView->fires = m_pDoc->m_pFireStats;
			pctView->OnInitialUpdate();
		}
		break;
	default:
		if (!m_wndSplitter2.CreateView(1, 0,
			RUNTIME_CLASS(CEditView), CSize(0, 0), &pContext))
		{
			TRACE0("Failed to create second pane\n");
			return FALSE;
		}
	}
	CView *pNewView = (CView *)m_wndSplitter2.GetPane(1, 0);//->ShowWindow(SW_SHOW);//A->Invalidate();
	pNewView->ShowWindow(SW_SHOW);
	SetActiveView(pNewView);
	m_wndSplitter2.RecalcLayout();
	pNewView->Invalidate();
	//m_wndSplitter2.SetActivePane(1, 0);
	//SetActiveView(((CView *)m_wndSplitter2.GetPane(1, 0)));
	return 0;
}

void CBatchOutputFrame::GraphOptions()
{
	CQuickBatchGraphSheet gs("Graph Options", this, 0, m_pDoc->m_pDB);
	if(IDOK == gs.DoModal())
	{
		//graphSet.Edit();
		//gs.ApplyToGraph();
		SwitchView(m_pDoc->activeView);
		//graphSet.Up
	}
}

bool RawClimRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CRawClim* pClim = (CRawClim*)pCUPDUPData->GetAppData();
	pClim->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->SetProgress(_T("Initializing.."), 0);
	pClim->AllocData();

	return true;
}

void CBatchOutputFrame::WorkingSet()
{
	CWorkingSetDialog wsd(this, m_pDoc->activeClim);
	if(IDOK == wsd.DoModal())
	{
		CWaitCursor wait;
//		if(wsd.fxParamChange == true)//need to recalculate everything
//		{
			int StartDay = m_pDoc->activeClim->m_fpSet->m_StartDay;
			CString StartMonth = m_pDoc->activeClim->m_fpSet->m_StartMonth;
			int StartYear = m_pDoc->activeClim->m_fpSet->m_StartYear;
			int EndDay = m_pDoc->activeClim->m_fpSet->m_EndDay;
			CString EndMonth = m_pDoc->activeClim->m_fpSet->m_EndMonth;
			int EndYear = m_pDoc->activeClim->m_fpSet->m_EndYear;
			// 2014 added
			CString PeriodLength = m_pDoc->activeClim->m_fpSet->m_PeriodLength;

			//if (m_pDoc->pCUPDUPData)
			//{
				CUPDialog cupDlg(m_hWnd, RawClimRunProc, m_pDoc->activeClim);
				INT_PTR ret = cupDlg.DoModal();

			//}
			//	m_pDoc->pCUPDUPData->SetTerminate(false);
			//CWaitCursor wait;
			//m_pDoc->activeClim->AllocData();
			if (ret == IDOK)//CANCEL)
			{
				m_pDoc->activeClim->m_fpSet->Edit();
				m_pDoc->activeClim->m_fpSet->m_StartDay = StartDay;
				m_pDoc->activeClim->m_fpSet->m_StartMonth = StartMonth;
				m_pDoc->activeClim->m_fpSet->m_StartYear = StartYear;
				m_pDoc->activeClim->m_fpSet->m_EndDay = EndDay;
				m_pDoc->activeClim->m_fpSet->m_EndMonth = EndMonth;
				m_pDoc->activeClim->m_fpSet->m_EndYear = EndYear;
				// 2014 added
				m_pDoc->activeClim->m_fpSet->m_PeriodLength = PeriodLength;
				m_pDoc->activeClim->m_fpSet->Update();
				m_pDoc->activeClim->m_fpSet->MoveLast();
			}
	//	}
		m_pDoc->GetAnalysis(m_pDoc->activeClim);
		m_pDoc->SwitchTreeView(m_pDoc->activeClim, m_pDoc->activeVar);
	}
}

void CBatchOutputFrame::OnFileSave()
{
	switch(m_pDoc->activeView)
	{
	case 0:
	case 2:
	case 4:
	case 5:
	case 7:
	case 9:
	case 12:
		{
			CRichCtrlView *pView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);
			chdir(theApp.userDir);
			CFileDialog fd(false, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
			if(fd.DoModal() == IDOK)
			{
				pView->SaveToFile(fd.GetPathName());
				theApp.SetUserDir(fd.GetPathName());
			}
			chdir(theApp.dbDir);//theApp.workDir);
			//chdir(m_pDoc->activeClim->);

		}
		break;
	case 1://graph view
		{
			CGraphView *pView = (CGraphView *)m_wndSplitter2.GetPane(1, 0);	
			if(pView)
				pView->SaveGraph();
		}
		break;
	case 3://percentile view
		{
			CPercentileView *pView = (CPercentileView *)m_wndSplitter2.GetPane(1, 0);	
			if(pView)
				pView->SaveGraph();
		}
		break;
	case 6://pocket card view
		{
			
			CPocketCardView *pView = (CPocketCardView *)m_wndSplitter2.GetPane(1, 0);	
			if(pView)
				pView->OnFileSaveAs();
		}
		break;
	case 8://fire occurrence
		{
		}
		break;
	case 10:
		{
			CCumulativePcnt *pView = (CCumulativePcnt *)m_wndSplitter2.GetPane(1, 0);
			if(pView)
			{
			chdir(theApp.userDir);
			CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				theApp.GetGraphicsFileStr(), this);
			if(fd.DoModal() == IDOK)
			{
				CDC *whdc = GetDC();
				CDC memhdc;
				memhdc.CreateCompatibleDC(NULL);
				HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, 640, 480);
				HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
				//CRect bnd(0, 0, pWidth.GetValueInteger(), pHeight.GetValueInteger());
				CRect rect(0, 0, 640, 480);
				pView->DrawToDCRegion(&memhdc, rect);
				char fName[256];
				strcpy_s(fName, fd.GetPathName());
				SaveBitmapFile(memhdc.m_hDC, picture, fName); 
				SelectObject(memhdc.m_hDC, Old);
				ReleaseDC(whdc);
				memhdc.DeleteDC();
				DeleteObject(picture);
				theApp.SetUserDir(fd.GetPathName());
				theApp.SetGraphicsExt(fd.GetFileExt());
			}
			chdir(theApp.dbDir);//theApp.workDir);
			}
		}
		break;
	case 11:
		{
			CCumulativeBars *pView = (CCumulativeBars *)m_wndSplitter2.GetPane(1, 0);
			if(pView)
			{
			chdir(theApp.userDir);
			CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				theApp.GetGraphicsFileStr(), this);
			if(fd.DoModal() == IDOK)
			{
				CDC *whdc = GetDC();
				CDC memhdc;
				memhdc.CreateCompatibleDC(NULL);
				HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, 640, 480);
				HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
				//CRect bnd(0, 0, pWidth.GetValueInteger(), pHeight.GetValueInteger());
				CRect rect(0, 0, 640, 480);
				pView->DrawToDCRegion(&memhdc, rect);
				char fName[256];
				strcpy_s(fName, fd.GetPathName());
				SaveBitmapFile(memhdc.m_hDC, picture, fName); 
				SelectObject(memhdc.m_hDC, Old);
				ReleaseDC(whdc);
				memhdc.DeleteDC();
				DeleteObject(picture);
				theApp.SetUserDir(fd.GetPathName());
				theApp.SetGraphicsExt(fd.GetFileExt());
			}
			chdir(theApp.dbDir);//theApp.workDir);
			}
		}
		break;
	case 13://Fires Analysis Graph
		{
			CFireStatGraph *pView = (CFireStatGraph *)m_wndSplitter2.GetPane(1, 0);
			chdir(theApp.userDir);
			CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				theApp.GetGraphicsFileStr(), this);
			if(fd.DoModal() == IDOK)
			{
				CDC *whdc = GetDC();
				CDC memhdc;
				memhdc.CreateCompatibleDC(NULL);
				HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, 640, 480);
				HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
				//CRect bnd(0, 0, pWidth.GetValueInteger(), pHeight.GetValueInteger());
				CRect rect(0, 0, 640, 480);
				pView->DrawToDCRegion(&memhdc, rect);
				char fName[256];
				strcpy_s(fName, fd.GetPathName());
				SaveBitmapFile(memhdc.m_hDC, picture, fName); 
				SelectObject(memhdc.m_hDC, Old);
				ReleaseDC(whdc);
				memhdc.DeleteDC();
				DeleteObject(picture);
				theApp.SetUserDir(fd.GetPathName());
				theApp.SetGraphicsExt(fd.GetFileExt());
			}
			chdir(theApp.dbDir);//theApp.workDir);
		}
		break;
	default:
		break;
	}
}

void CBatchOutputFrame::OnFilePrint()
{
	switch(m_pDoc->activeView)
	{
	case 0:
	case 2:
	case 4:
	case 5:
	case 7:
	case 9:
	case 12:
		{
			CRichCtrlView *pView = (CRichCtrlView *)m_wndSplitter2.GetPane(1, 0);
			if(pView)
			{
				pView->Print();
			}

		}
		break;
	case 1://graph view
		{
			CGraphView *pView = (CGraphView *)m_wndSplitter2.GetPane(1, 0);	
			if(pView)
				pView->PrintGraph();
		}
		break;
	case 3://percentile view
		{
			CPercentileView *pView = (CPercentileView *)m_wndSplitter2.GetPane(1, 0);	
			if(pView)
				pView->PrintGraph();
		}
		break;
	case 6://pocket card view
		{
			
			CPocketCardView *pView = (CPocketCardView *)m_wndSplitter2.GetPane(1, 0);	
			if(pView)
				pView->OnFilePrint();//>SendMessage(ID_FILE_PRINT);
		}
		break;
	case 8://fire occurrence
		{
		}
		break;
	case 10:
		{
			CCumulativePcnt *pView = (CCumulativePcnt *)m_wndSplitter2.GetPane(1, 0);
			if(pView)
				pView->PrintGraph();
		}
		break;
	case 11:
		{
			CCumulativeBars *pView = (CCumulativeBars *)m_wndSplitter2.GetPane(1, 0);
			if(pView)
				pView->PrintGraph();
		}
		break;
	case 13://Fires Analysis Graph
		{
			CFireStatGraph *pView = (CFireStatGraph *)m_wndSplitter2.GetPane(1, 0);
			if(pView)
				pView->PrintGraph();
		}
		break;
	default:
		break;
	}
}


void CBatchOutputFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//AfxMessageBox("CBatchOutputFrame::OnContextMenu");
	CRect rect;
	m_wndSplitter2.GetPane(1,0)->GetWindowRect(&rect);
	if(rect.PtInRect(point))
	{
		UINT uFlags;
		CPoint wPoint = point;
		ScreenToClient(&wPoint);
		CMenu tMenu;
		tMenu.LoadMenu(IDR_MENU2);
		CMenu *pPopup = tMenu.GetSubMenu(0);
		if(pPopup)
			pPopup->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CBatchOutputFrame::OnOverlays()
{
	//CPeriodDoc* pDoc = (CPeriodDoc*)GetActiveDocument();
	if(m_pDoc->m_pAnalysis->m_opts.m_OverlayDialog)
	{
		m_pDoc->m_pAnalysis->m_opts.m_OverlayDialog->SetActiveWindow();
		return;
	}
	//POSITION vPos = pDoc->GetFirstViewPosition();
	//pDoc->m_pAnalysis->m_opts.m_OverlayDialog = new COverlayDialog(this, pDoc, ((CGraphView *)pDoc->GetNextView(vPos))->varID);
	m_pDoc->m_pAnalysis->m_opts.m_OverlayDialog = new COverlayDialog(this, m_pDoc, m_pDoc->m_pAnalysis, m_pDoc->activeVar);
	m_pDoc->m_pAnalysis->m_opts.m_OverlayDialog->Create();
}

void CBatchOutputFrame::FireAssociations()
{
	CString str, staStr;
	//m_ctlSIG_Station.GetLBText(itemLoc, staStr);
	staStr = m_pDoc->activeClim->m_sigStaID;
	staStr.TrimRight();
	str.Format("Set Fire Associations for %s", staStr);
	CFireQuerySheet sheet(str, this, 0, m_pDoc->m_pDB, staStr,
		NULL, NULL, m_pDoc->activeClim->m_fpSet,m_pDoc->activeClim->m_fpSet);
	if(IDOK == sheet.DoModal())
	{
		CWaitCursor wait;
		m_pDoc->activeClim->GetFires();
		m_pDoc->GetAnalysis(m_pDoc->activeClim);
		m_pDoc->SwitchTreeView(m_pDoc->activeClim, m_pDoc->activeVar);
	}
}

void CBatchOutputFrame::FireOptions()
{
	CQuickFireOptionsDlg fod(this, m_pDoc->activeClim->m_fpSet);
	if(IDOK == fod.DoModal())
	{
		CWaitCursor wait;
		m_pDoc->activeClim->GetFires();
		m_pDoc->GetAnalysis(m_pDoc->activeClim);
		m_pDoc->SwitchTreeView(m_pDoc->activeClim, m_pDoc->activeVar);
	}
}

void CBatchOutputFrame::OnFileProperties()
{
	switch(m_pDoc->activeView)
	{
	case 0:
	case 2:
	case 3:
	case 4:
	case 5:
		{
			WorkingSet();
		}
		break;
	case 1:
		{
			GraphOptions();
		}
		break;
	case 6:
		{
			CPocketCardSet pocketSet(m_pDoc->m_pDB);
			pocketSet.OpenEnsureSIGStationRec(m_pDoc->activeClim->m_sigStaID);
			CPocketCardDialog dlg(this, m_pDoc->activeClim->m_fpSet, &pocketSet);
			if(IDOK == dlg.DoModal())
			{
				pocketSet.Close();
				SwitchView(m_pDoc->activeView);
			}
			else
				pocketSet.Close();

		}
		break;
	case 7:
		{
			CListingDialog cld(this, m_pDoc->activeClim->m_fpSet, TRUE);
			if(cld.DoModal() == IDOK)
				SwitchView(m_pDoc->activeView);
		}
		break;
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		{
			FireOptions();
		}
		break;
	default:
		//do nothing
		break;
	}
}

void CBatchOutputFrame::OnDataViewCandidateslist()
{
	theApp.UpdateCandidatesFrame(m_pDoc->m_pDB);
	/*CCandidateSet *pCandSet = new CCandidateSet(m_pDoc->m_pDB);
	pCandSet->Open();
	//create a document to view the results....
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("CandidatesView"))
		{
			CCandidatesDoc *pCandDoc = (CCandidatesDoc *)curTemplate->CreateNewDocument();
			pCandDoc->records = pCandSet;
			CCandidatesFrame* pCandFrame = (CCandidatesFrame*)curTemplate->CreateNewFrame(pCandDoc, NULL);
			curTemplate->InitialUpdateFrame(pCandFrame, pCandDoc);
			POSITION pos = pCandDoc->GetFirstViewPosition();
			CCandidatesView *pView = (CCandidatesView *)pCandDoc->GetNextView(pos);
			pView->SetRecords(pCandDoc->records);
			CString tTitle;
			tTitle = "Fire Business Candidates";
			pCandDoc->SetTitle(tTitle);

		}
	}*/
	//if(pCandSet->IsEOF())
	//	AfxMessageBox("No Fire Business Candidates saved");
	//else
	//{
		//CCandidatesDlg cDlg(this, &candSet);
		//cDlg.DoModal();
	//}
	//candSet.Close();
}

void CBatchOutputFrame::OnDataAddtocandidateslist()
{
	//if(AfxMessageBox("Add current Station/Variable to Fire Business Candidates list?", MB_YESNO) == IDYES)
	//{
		CCandidateSet cSet(m_pDoc->m_pDB);
		cSet.Open();
		cSet.AddNew();
		cSet.m_SIG_Station = m_pDoc->activeClim->m_sigStaID;
		cSet.m_StartYear = m_pDoc->activeClim->m_fpSet->m_StartYear;
		cSet.m_EndYear = m_pDoc->activeClim->m_fpSet->m_EndYear;
		cSet.m_StartMonth = m_pDoc->activeClim->m_fpSet->m_StartMonth;
		cSet.m_EndMonth = m_pDoc->activeClim->m_fpSet->m_EndMonth;
		cSet.m_StartDay = m_pDoc->activeClim->m_fpSet->m_StartDay;
		cSet.m_EndDay = m_pDoc->activeClim->m_fpSet->m_EndDay;
		CClimateSet climSet(m_pDoc->m_pDB);
		climSet.m_strFilter.Format("[VarID] = %d", m_pDoc->activeVar + 1);
		climSet.Open();
		if(!climSet.IsEOF())
			cSet.m_VarName = climSet.m_ShortName;
		else
			cSet.m_VarName = "Unknown";
		climSet.Close();
		cSet.m_MODEL = m_pDoc->m_pAnalysis->m_strModel;
		cSet.m_Comment = "";
		CSIGStationSet staSet(m_pDoc->m_pDB);
		staSet.Open();
		if(m_pDoc->activeClim->m_sigStaID.GetLength() <= 6)//single station
		{
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", m_pDoc->activeClim->m_sigStaID);
			staSet.Requery();
		}
		else
		{//SIG...just pick first station
			CStationInSIGSet staSigSet(m_pDoc->m_pDB);
			char sig[64];
			strcpy_s(sig, m_pDoc->activeClim->m_sigStaID);
			staSigSet.m_strFilter.Format("[SIG] = '%-20.20s'", &sig[6]);
			staSigSet.Open();
			if(!staSigSet.IsEOF())
			{
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", staSigSet.m_StationID);
				staSet.Requery();
			}
			staSigSet.Close();
		}
		cSet.m_GreenJulian = staSet.m_GreenJulian;
		cSet.m_FreezeJulian = staSet.m_FreezeJulian;
		staSet.Close();
		if(m_pDoc->m_pFireStats)
		{
			CString strCause = "All";
			switch(m_pDoc->m_pFireStats->cause)
			{
			case 0:
				strCause = "All";
				break;
			case 1:
				strCause = "Lightning";
				break;
			case 2:
				strCause = "Human";
				break;
			}
			cSet.m_MODEL = m_pDoc->m_pFireStats->graphModelString;
			cSet.m_FD_Type = strCause;
			//fire days
			cSet.m_FD_Beta0 = m_pDoc->m_pFireStats->b0fd;
			cSet.m_FD_Beta1 = m_pDoc->m_pFireStats->b1fd;
			cSet.m_FD_Chi2 = m_pDoc->m_pFireStats->fd_Chi2;
			cSet.m_FD_PMax = m_pDoc->m_pFireStats->fd_pMax;
			cSet.m_FD_PMin = m_pDoc->m_pFireStats->fd_pMin;
			cSet.m_FD_PVal = m_pDoc->m_pFireStats->fd_Chi2PVal;
			cSet.m_FD_R2 = m_pDoc->m_pFireStats->fd_R2;
			//large fire days
			CString strAcres;
			strAcres.Format("%d", m_pDoc->m_pFireStats->lfAcres);
			if(m_pDoc->m_pFireStats->m_conditional == true)
				strAcres += " (C)";
			cSet.m_LFD_Acres = strAcres;
			cSet.m_LFD_Beta0 = m_pDoc->m_pFireStats->b0lfd;
			cSet.m_LFD_Beta1 = m_pDoc->m_pFireStats->b1lfd;
			cSet.m_LFD_Chi2 = m_pDoc->m_pFireStats->lfd_Chi2;
			cSet.m_LFD_PMax = m_pDoc->m_pFireStats->lfd_pMax;
			cSet.m_LFD_PMin = m_pDoc->m_pFireStats->lfd_pMin;
			cSet.m_LFD_PVal = m_pDoc->m_pFireStats->lfd_Chi2PVal;
			cSet.m_LFD_R2 = m_pDoc->m_pFireStats->lfd_R2;
			//multiple fire days
			CString strMFDfires;
			strMFDfires.Format("%d", m_pDoc->m_pFireStats->mfdFires);
			if(m_pDoc->m_pFireStats->m_conditional == true)
				strMFDfires += " (C)";
			cSet.m_MFD_NumFires = strMFDfires;
			cSet.m_MFD_Beta0 = m_pDoc->m_pFireStats->b0mfd;
			cSet.m_MFD_Beta1 = m_pDoc->m_pFireStats->b1mfd;
			cSet.m_MFD_Chi2 = m_pDoc->m_pFireStats->mfd_Chi2;
			cSet.m_MFD_PMax = m_pDoc->m_pFireStats->mfd_pMax;
			cSet.m_MFD_PMin = m_pDoc->m_pFireStats->mfd_pMin;
			cSet.m_MFD_PVal = m_pDoc->m_pFireStats->mfd_Chi2PVal;
			cSet.m_MFD_R2 = m_pDoc->m_pFireStats->mfd_R2;
		}
		cSet.Update();
		cSet.Close();
	if(theApp.GetCandidatesFrame(m_pDoc->m_pDB))
		theApp.UpdateCandidatesFrame(m_pDoc->m_pDB);
	//}
}

void CBatchOutputFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);
	//((CGraphView*)m_wndSplitter.GetPane(0,0))->varID = varID;
	//((CPercentileView*)m_wndSplitter.GetPane(0,1))->varID = varID;

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	/*CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
	{
		TCHAR szText[256+_MAX_PATH];
		lstrcpy(szText, pDocument->GetTitle());
		if (m_nWindow > 0)
			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);

		// set title if changed, but don't remove completely
		AfxSetWindowText(m_hWnd, szText);
	}*/
}

void CBatchOutputFrame::OnDataEditsig()
{
	if(m_pDoc->activeClim->m_sigStaID.GetLength() > 6)//it's a SIG
	{
		char tmpStr[64];
		strcpy_s(tmpStr, m_pDoc->activeClim->m_sigStaID);
		CSIGSet sigSet(m_pDoc->activeClim->m_pDB);
		sigSet.m_strFilter.Format("[SIG] = '%s'", &tmpStr[6]);
		sigSet.Open();
		if(!sigSet.IsEOF())
		{
			CSIGEditDialog sed(&sigSet, this ,false);
			if(IDOK == sed.DoModal())
			{
				//m_pDoc->pCUPDUPData->SetTerminate(false);
				CWaitCursor wait;
				m_pDoc->activeClim->AllocData();
				m_pDoc->GetAnalysis(m_pDoc->activeClim);
				m_pDoc->SwitchTreeView(m_pDoc->activeClim, m_pDoc->activeVar);
			}
		}
		sigSet.Close();
	}
	else //single station
	{
		CSIGStationSet staSet(m_pDoc->activeClim->m_pDB);
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", m_pDoc->activeClim->m_sigStaID);
		staSet.Open();
		if(!staSet.IsEOF())
		{
			CEditStationDialog esd(this, &staSet, false);
			if(esd.DoModal() == IDOK)
			{
				//m_pDoc->pCUPDUPData->SetTerminate(false);
				CWaitCursor wait;
				m_pDoc->activeClim->AllocData();
				m_pDoc->GetAnalysis(m_pDoc->activeClim);
				m_pDoc->SwitchTreeView(m_pDoc->activeClim, m_pDoc->activeVar);
			}
		}
		staSet.Close();
	}
}

void CBatchOutputFrame::OnUpdateDataAddtocandidateslist(CCmdUI *pCmdUI)
{
	//Active view neeeds to be fires Analysis stats or Graph
	switch(m_pDoc->activeView)
	{
	case 12:
	case 13:
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
	}
}

void CBatchOutputFrame::OnUpdateOverlays(CCmdUI *pCmdUI)
{
	//Active view neeeds to be fires Graph
	switch(m_pDoc->activeView)
	{
	case 1:
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
	}
}

void CBatchOutputFrame::OnUpdateGraphOptions(CCmdUI *pCmdUI)
{
	//Active view neeeds to be fires Graph
	switch(m_pDoc->activeView)
	{
	case 1:
		pCmdUI->Enable(TRUE);
		break;
	default:
		pCmdUI->Enable(FALSE);
	}
}
