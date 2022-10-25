// PAnalysisDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "SeasonBinSet.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "PAnalysisDialog.h"
#include "wxSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "CFiresSet.h"
#include "climanalysis.h"
#include "FFPOptsSet.h"
#include "RunSave.h"
#include "MainFrm.h"

extern int curr_runID;
extern CFireplusApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPAnalysisDialog dialog

CPAnalysisDialog::CPAnalysisDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*=NULL*/)
	: CDialog(CPAnalysisDialog::IDD, pParent), climSet(_fpSet->m_pDatabase), binSet(_fpSet->m_pDatabase)
{
	//{{AFX_DATA_INIT(CPAnalysisDialog)
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
	varID1 = varID2 = 0;
	//hasData1 = hasData2 = false;
}

void CPAnalysisDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPAnalysisDialog)
	DDX_Control(pDX, IDC_COMBO2, index2List);
	DDX_Control(pDX, IDC_COMBO1, index1List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPAnalysisDialog, CDialog)
	//{{AFX_MSG_MAP(CPAnalysisDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeIndex1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeIndex2)
	ON_COMMAND(ID_VARSORT, OnVarSort)
	ON_COMMAND(ID_VARSORT2, OnVarSort2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPAnalysisDialog message handlers

void CPAnalysisDialog::ConfigureTable1()
{
	CUGCell cell;
	m_grid1.GetCell(1, 0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin1Min);
	m_grid1.SetCell(1, 0, &cell);
	m_grid1.GetCell(2, 0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin1Max);
	m_grid1.SetCell(2, 0, &cell);
	m_grid1.QuickSetText(0, 1, "2");
	m_grid1.GetCell(1, 1, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin2Min);
	m_grid1.SetCell(1, 1, &cell);
	m_grid1.GetCell(2, 1, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin2Max);
	m_grid1.SetCell(2, 1, &cell);
	m_grid1.QuickSetText(0, 2, "3");
	m_grid1.GetCell(1, 2, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin3Min);
	m_grid1.SetCell(1, 2, &cell);
	m_grid1.GetCell(2, 2, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin3Max);
	m_grid1.SetCell(2, 2, &cell);
	m_grid1.QuickSetText(0, 3, "4");
	m_grid1.GetCell(1, 3, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin4Min);
	m_grid1.SetCell(1, 3, &cell);
	m_grid1.GetCell(2, 3, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin4Max);
	m_grid1.SetCell(2, 3, &cell);
	m_grid1.QuickSetText(0, 4, "5");
	m_grid1.GetCell(1, 4, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin5Min);
	m_grid1.SetCell(1, 4, &cell);
	m_grid1.GetCell(2, 4, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin5Max);
	m_grid1.SetCell(2, 4, &cell);
	m_grid1.QuickSetText(0, 5, "6");
	m_grid1.GetCell(1, 5, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin6Min);
	m_grid1.SetCell(1, 5, &cell);
	m_grid1.GetCell(2, 5, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin6Max);
	m_grid1.SetCell(2, 5, &cell);
}

void CPAnalysisDialog::ConfigureTable2()
{
	CUGCell cell;
	m_grid2.GetCell(1, 0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin1Min);
	m_grid2.SetCell(1, 0, &cell);
	m_grid2.GetCell(2, 0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin1Max);
	m_grid2.SetCell(2, 0, &cell);
	m_grid2.QuickSetText(0, 1, "2");
	m_grid2.GetCell(1, 1, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin2Min);
	m_grid2.SetCell(1, 1, &cell);
	m_grid2.GetCell(2, 1, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin2Max);
	m_grid2.SetCell(2, 1, &cell);
	m_grid2.QuickSetText(0, 2, "3");
	m_grid2.GetCell(1, 2, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin3Min);
	m_grid2.SetCell(1, 2, &cell);
	m_grid2.GetCell(2, 2, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin3Max);
	m_grid2.SetCell(2, 2, &cell);
	m_grid2.QuickSetText(0, 3, "4");
	m_grid2.GetCell(1, 3, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin4Min);
	m_grid2.SetCell(1, 3, &cell);
	m_grid2.GetCell(2, 3, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin4Max);
	m_grid2.SetCell(2, 3, &cell);
	m_grid2.QuickSetText(0, 4, "5");
	m_grid2.GetCell(1, 4, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin5Min);
	m_grid2.SetCell(1, 4, &cell);
	m_grid2.GetCell(2, 4, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin5Max);
	m_grid2.SetCell(2, 4, &cell);
	m_grid2.QuickSetText(0, 5, "6");
	m_grid2.GetCell(1, 5, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin6Min);
	m_grid2.SetCell(1, 5, &cell);
	m_grid2.GetCell(2, 5, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin6Max);
	m_grid2.SetCell(2, 5, &cell);
}

bool CPAnalysisDialog::TablesOK()
{
	CUGCell cell;
	double mx, mn, nmn;
	int bad = 0;
	for(int i = 0; i < 6; i++)
	{
		m_grid1.GetCell(1, i, &cell);
		mn = cell.GetNumber();
		m_grid1.GetCell(2, i, &cell);
		mx = cell.GetNumber();
		if(mn >= mx)
			bad = 1;
		if(i < 5)
		{
			m_grid1.GetCell(1, i + 1, &cell);
			nmn = cell.GetNumber();
			if(nmn != mx)
				bad = 1;
		}
	}
	for(int i = 0; i < 6; i++)
	{
		m_grid2.GetCell(1, i, &cell);
		mn = cell.GetNumber();
		m_grid2.GetCell(2, i, &cell);
		mx = cell.GetNumber();
		if(mn >= mx)
			bad = 2;
		if(i < 5)
		{
			m_grid2.GetCell(1, i + 1, &cell);
			nmn = cell.GetNumber();
			if(nmn != mx)
				bad = 2;
		}
	}
	if(bad)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: One or more incorrect table entries\n"
			"in table for Index %d.\n"
			"The rules are:\n"
			"Max must be greater than Min.\n"
			"Max must be equal to next row's Min.\n"
			"All rows must be filled in.\n", bad);
		AfxMessageBox(strMsg);
		return false;
	}
	return true;
}

void CPAnalysisDialog::SaveTable1()
{
	CUGCell cell;
	if(binSet.IsOpen() && m_grid1.GetNumberRows() > 0)
	{
		if(varID1 == 0)
			return;
		try
		{    // requery recordset
			binSet.m_strFilter.Format("[VarID] = %d", varID1);
			binSet.Requery();
			binSet.Edit();
			//put data in appropriate slots
			m_grid1.GetCell(1, 0, &cell);
			binSet.m_Bin1Min = cell.GetNumber();
			m_grid1.GetCell(2, 0, &cell);
			binSet.m_Bin1Max = cell.GetNumber();
			m_grid1.GetCell(1, 1, &cell);
			binSet.m_Bin2Min = cell.GetNumber();
			m_grid1.GetCell(2, 1, &cell);
			binSet.m_Bin2Max = cell.GetNumber();
			m_grid1.GetCell(1, 2, &cell);
			binSet.m_Bin3Min = cell.GetNumber();
			m_grid1.GetCell(2, 2, &cell);
			binSet.m_Bin3Max = cell.GetNumber();
			m_grid1.GetCell(1, 3, &cell);
			binSet.m_Bin4Min = cell.GetNumber();
			m_grid1.GetCell(2, 3, &cell);
			binSet.m_Bin4Max = cell.GetNumber();
			m_grid1.GetCell(1, 4, &cell);
			binSet.m_Bin5Min = cell.GetNumber();
			m_grid1.GetCell(2, 4, &cell);
			binSet.m_Bin5Max = cell.GetNumber();
			m_grid1.GetCell(1, 5, &cell);
			binSet.m_Bin6Min = cell.GetNumber();
			m_grid1.GetCell(2, 5, &cell);
			binSet.m_Bin6Max = cell.GetNumber();
			//write the data
			binSet.Update();
			// added for batch
			CString value1;

			// save variable
			value1.Format("%2d", varID1);
			CRunSave saveRun(fpSet);
			saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_PROBABILITY_ANAL,0,"varID1",value1,fpSet);

			// save bin values

			value1.Format("%6.2f", binSet.m_Bin1Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin1Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin1Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin1Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin2Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin2Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin2Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin2Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin3Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin3Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin3Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin3Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin4Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin4Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin4Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin4Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin5Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin5Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin5Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin5Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin6Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin6Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin6Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table1-Bin6Max", value1, fpSet);
			//end added for batch
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
}

void CPAnalysisDialog::SaveTable2()
{
	CUGCell cell;
	if(binSet.IsOpen() && m_grid2.GetNumberRows() > 0)//index2Table.GetRowSet().GetCount() > 0)
	{
		if(varID2 == 0)
			return;
		try
		{    // requery recordset
			binSet.m_strFilter.Format("[VarID] = %d", varID2);
			binSet.Requery();
			binSet.Edit();
			//put data in appropriate slots
			m_grid2.GetCell(1, 0, &cell);
			binSet.m_Bin1Min = cell.GetNumber();
			m_grid2.GetCell(2, 0, &cell);
			binSet.m_Bin1Max = cell.GetNumber();
			m_grid2.GetCell(1, 1, &cell);
			binSet.m_Bin2Min = cell.GetNumber();
			m_grid2.GetCell(2, 1, &cell);
			binSet.m_Bin2Max = cell.GetNumber();
			m_grid2.GetCell(1, 2, &cell);
			binSet.m_Bin3Min = cell.GetNumber();
			m_grid2.GetCell(2, 2, &cell);
			binSet.m_Bin3Max = cell.GetNumber();
			m_grid2.GetCell(1, 3, &cell);
			binSet.m_Bin4Min = cell.GetNumber();
			m_grid2.GetCell(2, 3, &cell);
			binSet.m_Bin4Max = cell.GetNumber();
			m_grid2.GetCell(1, 4, &cell);
			binSet.m_Bin5Min = cell.GetNumber();
			m_grid2.GetCell(2, 4, &cell);
			binSet.m_Bin5Max = cell.GetNumber();
			m_grid2.GetCell(1, 5, &cell);
			binSet.m_Bin6Min = cell.GetNumber();
			m_grid2.GetCell(2, 5, &cell);
			binSet.m_Bin6Max = cell.GetNumber();

			//write the data
			binSet.Update();
			// added for batch
			CString value1;
			// save variable
			CRunSave saveRun(fpSet);

			value1.Format("%2d", varID2);
			saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_PROBABILITY_ANAL,0,"varID2",value1,fpSet);

			// save bin values

			value1.Format("%6.2f", binSet.m_Bin1Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin1Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin1Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin1Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin2Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin2Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin2Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin2Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin3Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin3Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin3Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin3Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin4Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin4Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin4Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin4Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin5Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin5Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin5Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin5Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin6Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin6Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin6Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PROBABILITY_ANAL, 0, "table2-Bin6Max", value1, fpSet);
			//end added for batch
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
}

void CPAnalysisDialog::OnOK()
{
	if(index1List.GetCurSel() == CB_ERR || index2List.GetCurSel() == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: You must select two different indices and\n"
			"complete the display ranges table for each index." );
		AfxMessageBox(strMsg);
		return;
	}
	if(TablesOK())
	{
		// added for batch
		// single run mode

		CString runDesc;
		runDesc.Format("Probability Analysis: ");
        CClimateSet cSet(fpSet->m_pDatabase);
	    cSet.m_strFilter.Format("[VarID]=%d",varID1);

	    cSet.Open();
	    if (cSet.IsOpen() && !cSet.IsEOF())
	    {
		    runDesc.Append(cSet.m_ShortName);
	    }
		cSet.m_strFilter.Format("[VarID]=%d",varID2);
        cSet.Requery();
        if (cSet.IsOpen() && !cSet.IsEOF())
	    {
			runDesc.Append(", ");
		    runDesc.Append(cSet.m_ShortName);
	    }
	    cSet.Close();

		CRunSave saveRun(fpSet);
		saveRun.SaveGeneral(0, runDesc, fpSet);
		saveRun.SaveClimateOptions(curr_runID, fpSet);
		// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
		saveRun.SaveFireOptions(curr_runID, fpSet);

		// end single run mode
		// end added for batch
		SaveTable1();
		SaveTable2();
		CClimAnalysis analysis(fpSet->m_pDatabase);
		int *varIDs = new int[2];
		varIDs[0] = varID1 - 1;
		varIDs[1] = varID2 - 1;
		//EnableWindow(FALSE);
		int ret = analysis.RunAnalyze(varIDs, 2, fpSet, false);
			//Analyze(this, varIDs, 2, fpSet, false);
		//EnableWindow(TRUE);
		if(ret == IDOK)
		{
			CSeasonBinSet set1(fpSet->m_pDatabase), set2(fpSet->m_pDatabase);
			set1.m_strFilter.Format("[VarID] = %d", varID1);
			set2.m_strFilter.Format("[VarID] = %d", varID2);
			set1.Open();
			set2.Open();
			analysis.ProbabilityAnalysis(&set1, &set2);
			set1.Close();
			set2.Close();
			CDialog::OnOK();
		}
	}
}

void CPAnalysisDialog::OnSelchangeIndex1()
{
	if(varID1)
		SaveTable1();
	int varID = index1List.GetItemData(index1List.GetCurSel());
	if(varID != CB_ERR)
	{
		try
		{
			varID++;
			varID1 = varID;
			binSet.m_strFilter.Format("[VarID] = %d", varID);
			binSet.Requery();
			ConfigureTable1();
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	m_grid1.RedrawAll();
}

void CPAnalysisDialog::OnSelchangeIndex2()
{
	if(varID2)
		SaveTable2();
	int varID = index2List.GetItemData(index2List.GetCurSel());
	if(varID != CB_ERR)
	{
		try
		{
			varID++;
			varID2 = varID;
			binSet.m_strFilter.Format("[VarID] = %d", varID);
			binSet.Requery();
			ConfigureTable2();
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	m_grid2.RedrawAll();
}

/*void CPAnalysisDialog::OnUpdateCellDatatblctrl1(long Row, long Column, long FAR* Status)
{
	// TODO: Add your control notification handler code here
	switch(Column)
	{
	case 0: //leaving a Min cell
	{
		double mx, mn, ev;
		CDTData RowData = index1Table.GetRowSet().GetItem(Row).GetValue(),
			cellData = index1Table.GetCurCell().GetEditValue();
		cellData.Get(&ev);
		RowData.GetItem(&mn, 0);
		RowData.GetItem(&mx, 1);
		if(mx <= mn)
		{//set max value if necessary
			ev += 10;
			CDTData nVal(ev);
			CPVDTCell nCell = index1Table.GetCellSet().GetItem(Row, 1);
			nCell.SetValue(nVal);
		}
		//set previous row's Min value automatically...
		if(Row > 0)
		{
			//ev += step;
			CDTData nVal(ev);
			CPVDTCell nCell = index1Table.GetCellSet().GetItem(Row - 1, 1);
			nCell.SetValue(nVal);
		}
	}
		break;
	case 1: //leaving a Max cell
	{
		double mx, mn, ev;
		CDTData RowData = index1Table.GetRowSet().GetItem(Row).GetValue(),
			cellData = index1Table.GetCurCell().GetEditValue();
		cellData.Get(&ev);
		RowData.GetItem(&mn, 0);
		RowData.GetItem(&mx, 1);
		//CDTCurCell curCell = indexTable.GetCurCell();
		if(ev <= mn)
		{
			CString strMsg;
			strMsg.Format(
				"ERROR: Max must be greater than Min.\n");
			AfxMessageBox(strMsg);
			*Status = true;
			return;
		}
		//set next row's Min value automatically...
		if(Row < 5)
		{
			//ev += step;
			CDTData nVal(ev);
			CPVDTCell nCell = index1Table.GetCellSet().GetItem(Row + 1, 0);
			nCell.SetValue(nVal);
		}
	}
		break;
	}
}

void CPAnalysisDialog::OnUpdateCellDatatblctrl2(long Row, long Column, long FAR* Status)
{
	// TODO: Add your control notification handler code here
	switch(Column)
	{
	case 0: //leaving a Min cell
	{
		double mx, mn, ev;
		CDTData RowData = index2Table.GetRowSet().GetItem(Row).GetValue(),
			cellData = index2Table.GetCurCell().GetEditValue();
		cellData.Get(&ev);
		RowData.GetItem(&mn, 0);
		RowData.GetItem(&mx, 1);
		if(mx <= mn)
		{//set max value if necessary
			ev += 10;
			CDTData nVal(ev);
			CPVDTCell nCell = index2Table.GetCellSet().GetItem(Row, 1);
			nCell.SetValue(nVal);
		}
		//set previous row's Min value automatically...
		if(Row > 0)
		{
			//ev += step;
			CDTData nVal(ev);
			CPVDTCell nCell = index2Table.GetCellSet().GetItem(Row - 1, 1);
			nCell.SetValue(nVal);
		}
	}
		break;
	case 1: //leaving a Max cell
	{
		double mx, mn, ev;
		CDTData RowData = index2Table.GetRowSet().GetItem(Row).GetValue(),
			cellData = index2Table.GetCurCell().GetEditValue();
		cellData.Get(&ev);
		RowData.GetItem(&mn, 0);
		RowData.GetItem(&mx, 1);
		//CDTCurCell curCell = indexTable.GetCurCell();
		if(ev <= mn)
		{
			CString strMsg;
			strMsg.Format(
				"ERROR: Max must be greater than Min.\n");
			AfxMessageBox(strMsg);
			*Status = true;
			return;
		}
		//set next row's Min value automatically...
		if(Row < 5)
		{
			//ev += step;
			CDTData nVal(ev);
			CPVDTCell nCell = index2Table.GetCellSet().GetItem(Row + 1, 0);
			nCell.SetValue(nVal);
		}
	}
		break;
	}
}*/

BOOL CPAnalysisDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid1.AttachGrid(this, IDC_GRID1);
	m_grid2.AttachGrid(this, IDC_GRID2);

	if(fpSet->m_pDatabase)
	{
		try
		{    // requery recordset
			binSet.Open();
			climSet.m_strFilter = "[OptionType >= 0]";
			climSet.setVarSortOrder(theApp.varSortOrder);
			climSet.Open();
			if (climSet.IsOpen())
			{
				while (!climSet.IsEOF())
				{
					CString str = climSet.m_Variable_Name;
					
					int eLoc = index1List.AddString(str);
					index1List.SetItemData(eLoc, climSet.m_VarID - 1);
					eLoc = index2List.AddString(str);
					index2List.SetItemData(eLoc, climSet.m_VarID - 1);
					climSet.MoveNext();
				}
			}
			climSet.Close();
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPAnalysisDialog::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	binSet.Close();
}

void CPAnalysisDialog::OnVarSort()
{
	// change the app's varSortOrder (and save to ffpOptions)
	// then re-sort and re-display

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;
	theApp.varSortOrder += 1;

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			optsSet1.Edit();
			optsSet1.m_VarSortOrder = theApp.varSortOrder;
			optsSet1.Update();
	}
	optsSet1.Close();

	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	index1List.ResetContent();
	index2List.ResetContent();

	while (!climSet.IsEOF())
				{
					CString str = climSet.m_Variable_Name;
					int eLoc = index1List.AddString(str);
					index1List.SetItemData(eLoc, climSet.m_VarID - 1);
					eLoc = index2List.AddString(str);
					index2List.SetItemData(eLoc, climSet.m_VarID - 1);
					climSet.MoveNext();
				}

	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}

void CPAnalysisDialog::OnVarSort2()
{
	// change the app's varSortOrder (and save to ffpOptions)
	// then re-sort and re-display

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;
	theApp.varSortOrder += 1;

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if (!optsSet1.IsEOF())
	{
		optsSet1.Edit();
		optsSet1.m_VarSortOrder = theApp.varSortOrder;
		optsSet1.Update();
	}
	optsSet1.Close();

	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	index1List.ResetContent();
	index2List.ResetContent();

	while (!climSet.IsEOF())
	{
		CString str = climSet.m_Variable_Name;
		int eLoc = index1List.AddString(str);
		index1List.SetItemData(eLoc, climSet.m_VarID - 1);
		eLoc = index2List.AddString(str);
		index2List.SetItemData(eLoc, climSet.m_VarID - 1);
		climSet.MoveNext();
	}

	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *)AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}