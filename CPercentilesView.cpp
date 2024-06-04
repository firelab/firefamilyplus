#include "stdafx.h"
#include "CPercentilesView.h"
#include <afxrich.h>
#include "richdoc.h"
//#include "dibapi.h"
#include "fireplus.h"
//#include "GraphView.h"
#include "wxSet.h"
#include "SplitterFrame.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include <math.h>
#include "OverlayDialog.h"
#include "Line.h"
#include "ReportOptionsSet.h"
#include "fireplusset.h"
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include ".\graphview.h"
#include "GraphSaveDialog.h"
#include <direct.h>
#include "BatchOutputFrame.h"

// added for batch2
extern int curr_runID;
extern CFireplusApp theApp;

IMPLEMENT_DYNCREATE(CPercentilesView, CView)

CPercentilesView::CPercentilesView()
{
	ymax = ymin = 0;
	cps = NULL;
	cp2s = NULL;
	mins = maxs = NULL;
	//pcpSums = NULL;
	//pcpAuxSums = NULL;
	m_pAnalysis = NULL;
}
