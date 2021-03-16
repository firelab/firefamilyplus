// RunSave.cpp: implementation of the CRunSave class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//#include "dibapi.h"





#include "fireplus.h"
#include "fireplusset.h"
#include "RunSave.h"

#include "RunsSet.h"
#include "RunDescriptionSet.h"
#include "RunFireAssocSet.h"
#include "RunClimateOptionsSet.h"
#include "RunWxStationSet.h"
#include "RunFireOptionsSet.h"
#include "RunOptionsSet.h"

#include "FireAssocSet.h"
#include "ClimateSet.h"
#include "SIGStationSet.h"
#include "FireOptionsSet.h"

#include "BatchOptionsSet.h"
#include "FFPOptsSet.h"
#include "StationInSIGSet.h"
#include "FFPOverlaysSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern int curr_runID;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRunSave::CRunSave(CFireplusSet *fpSet)
{
	m_saveIsOn = false;
	//check database to see if saving is on...
	CFFPOptsSet optsSet(fpSet->m_pDatabase);
	optsSet.Open();
	if(!optsSet.IsEOF())
	{
		m_saveIsOn = optsSet.m_RunSaveOn;
	}
	optsSet.Close();
}

CRunSave::~CRunSave()
{

}


int CRunSave::SaveGeneral(int runID, CString Description, CFireplusSet *fpSet)
{
	if(m_saveIsOn)
		return SaveGeneral(runID,Description,fpSet,NULL);
	return 0;
}

int CRunSave::SaveGeneral(int runID, CString Description, CFireplusSet *fpSet,CString *newDesc )
{
	if(!m_saveIsOn)
		return 0;
	int runExists = false;

	CString runDesc,fullDesc;


	// get next run ID
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();

	// if new record, get new runID
	if (! runID)
	{
		if (! runSet.IsEOF())
		{
			runSet.MoveLast();
			curr_runID = runSet.m_runID + 1;
		} 
		else
			curr_runID = 1;
	}
	// look for optional saved run description - from batch options
	CBatchOptionsSet bopt(fpSet->m_pDatabase);
	bopt.Open();
	if (bopt.IsOpen()){
		if (! bopt.IsEOF()){
             bopt.MoveFirst();
             runDesc.Format("%s",bopt.m_runDesc);
			 runDesc.Trim();
		}

	}

	/* new description prompt if necessary */
	if (bopt.m_PromptDesc){
       CRunSavePromptDialog rsp(NULL, fpSet, &runDesc);

	if (rsp.DoModal()== IDOK) 
         runDesc.Trim();
	}

	bopt.Close();
    if (runDesc.IsEmpty())
		  fullDesc.Format("%s",Description);
	else
		  fullDesc.Format("%s - %s",runDesc.Trim(),Description);
    fullDesc.Trim();

	if (fullDesc.GetLength() > 119){
		fullDesc.Truncate(116);
		fullDesc.Append("...");
	}


	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%Y/%m/%d-%H:%M:%S");

	// new method 2/2006 ljs:
	// if passed a runID, attempt to UPDATE the table if possible before INSERTing

	if (! runID)
	   runSet.AddNew();
	else { // look for existing
	   runSet.m_strFilter.Format("[runID] = %d", runID);
	   runSet.Requery();
	   if (runSet.IsEOF())  // still not found? add new
		   runSet.AddNew();
	   else {
		   runExists = true;
		   runSet.Edit();
	   }
	}

	runSet.m_runID = (runID > 0 ? runID : curr_runID);
	runSet.m_CreatedOn = dStr;
	runSet.m_LastRun = dStr;
	runSet.m_Description = fullDesc;
    runSet.Update();
	runSet.Close();


	if (newDesc)
		newDesc->Format("%s",fullDesc);


	// save copy of fireplusSet (ffpDescription) into ffpRunDescription
	CRunDescriptionSet runDescSet(fpSet->m_pDatabase);
	runDescSet.Open();
	if (runExists){ // update existing
		runDescSet.m_strFilter.Format("[runID] = %d", runID);
		runDescSet.Requery();
		if (runDescSet.IsEOF()){ // this should not happen
           CString strMsg;
		   strMsg.Format("ERROR: runID %d found in ffpRuns, but not in ffpRunDescription.\n",runID);
		   AfxMessageBox(strMsg);
		   return false;
	    } else
           runDescSet.Edit();

	} else
	   runDescSet.AddNew();
	runDescSet.m_runID = (runID > 0 ? runID : curr_runID);

	runDescSet.m_Description = fpSet->m_Description;
	runDescSet.m_EndDay = fpSet->m_EndDay;
	runDescSet.m_EndMonth = fpSet->m_EndMonth;
	runDescSet.m_EndYear = fpSet->m_EndYear;
	runDescSet.m_SIG_Station = fpSet->m_SIG_Station;
	runDescSet.m_StartDay = fpSet->m_StartDay;
	runDescSet.m_StartMonth = fpSet->m_StartMonth;
	runDescSet.m_StartYear = fpSet->m_StartYear;
	runDescSet.m_Use78 = fpSet->m_Use78;
	runDescSet.m_Use88 = fpSet->m_Use88;
	runDescSet.m_UseCanadian = fpSet->m_UseCanadian;
	runDescSet.m_PeriodLength = fpSet->m_PeriodLength;
	runDescSet.Update();
	runDescSet.Close();

	// save copy of fireAssocSet into ffpRunFireAssociations
	CRunFireAssocSet runAssocSet(fpSet->m_pDatabase);
	runAssocSet.Open();
	
	CFireAssocSet assocSet(fpSet->m_pDatabase);
	
	assocSet.Open();
	
	// delete existing if necessary
	if (runExists){
    	runAssocSet.m_strFilter.Format("[runID] = %d",runID);
        runAssocSet.Requery();
		while (!runAssocSet.IsEOF()){
                runAssocSet.Delete();
				runAssocSet.MoveNext();

		}

	} 

	while(!assocSet.IsEOF())
	{
		runAssocSet.AddNew();
		runAssocSet.m_runID = (runID > 0 ? runID : curr_runID);
		runAssocSet.m_SIG_Station = assocSet.m_SIG_Station;
		runAssocSet.m_AgencyID = assocSet.m_AgencyID;
		runAssocSet.m_RegionID = assocSet.m_RegionID;
		runAssocSet.m_UnitID = assocSet.m_UnitID;
		runAssocSet.m_SubunitID = assocSet.m_SubunitID;
		runAssocSet.Update();
		assocSet.MoveNext();
	}
	
	runAssocSet.Close();
	assocSet.Close();
	//deal with Overlays....
	CFFPOverlaysSet oSet(fpSet->m_pDatabase);
	oSet.m_strFilter.Format("[runID] = 0");
	oSet.Open();
	if(!oSet.IsEOF())//got some overlays...
	{
		CFFPOverlaysSet o2Set(fpSet->m_pDatabase);
		o2Set.m_strFilter.Format("[runID] = %d", (runID > 0 ? runID : curr_runID));
		o2Set.Open();
		if(!o2Set.IsEOF())
			o2Set.MoveFirst();
		while(!o2Set.IsEOF())
		{
			o2Set.Delete();
			o2Set.MoveNext();
		}
		oSet.MoveFirst();
		while(!oSet.IsEOF())
		{
			o2Set.AddNew();
			o2Set.m_runID = (runID > 0 ? runID : curr_runID);
			o2Set.m_Color = oSet.m_Color;
			o2Set.m_Year = oSet.m_Year;
			o2Set.m_Width = oSet.m_Width;
			o2Set.m_Style = oSet.m_Style;
			o2Set.Update();
			oSet.MoveNext();
		}
		o2Set.Close();
	}
	oSet.Close();
   return 1;
}




int CRunSave::SaveClimateOptions(int runID,  CFireplusSet *fpSet )
{
	if(!m_saveIsOn)
		return 0;
	
	int runExists = false;

	// get next run ID
	CRunsSet runSet(fpSet->m_pDatabase);
	
	// if new record, get new runID
	if (! runID){
		runSet.Open();
		if (! runSet.IsEOF()){
			runSet.MoveLast();
			curr_runID = runSet.m_runID + 1;
		} else
			curr_runID = 1;
		runSet.Close();
	} else { // we were passed a runID. see if it exists.
       runSet.m_strFilter.Format("[runID] = %d", runID);
	   runSet.Open();
	   if (!runSet.IsEOF())  // still not found? add new
		   runExists = true;
       runSet.Close();

	}
	
	


	// save copy of climateoptions  ffpClimateOptions
	CRunClimateOptionsSet runClimSet(fpSet->m_pDatabase);
	runClimSet.Open();

	// delete existing if necessary
	if (runExists){
    	runClimSet.m_strFilter.Format("[runID] = %d",runID);
        runClimSet.Requery();
		while (!runClimSet.IsEOF()){
                runClimSet.Delete();
				runClimSet.MoveNext();

		}

	} 

	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

	climSet.Open();
	
	while(!climSet.IsEOF())
	{
		runClimSet.AddNew();
		runClimSet.m_runID = (runID > 0 ? runID : curr_runID);

		runClimSet.m_Variable_Name = climSet.m_Variable_Name;
		runClimSet.m_ShortName = climSet.m_ShortName;
		
		runClimSet.m_OptionType = climSet.m_OptionType;
		runClimSet.m_Stats_Table = climSet.m_Stats_Table;
		runClimSet.m_Stats_Graph = climSet.m_Stats_Graph;
		runClimSet.m_Daily_Freqs = climSet.m_Daily_Freqs;
		runClimSet.m_Period_Mins = climSet.m_Period_Mins;
		runClimSet.m_Period_Maxs = climSet.m_Period_Maxs;
		runClimSet.m_VarID = climSet.m_VarID;
		runClimSet.m_cp2 = climSet.m_cp2;
		runClimSet.m_CriticalPercentile = climSet.m_CriticalPercentile;
		runClimSet.m_Data_Count = climSet.m_Data_Count;
		
		runClimSet.m_BinSize = climSet.m_BinSize;
		runClimSet.m_Daily_List = climSet.m_Daily_List;
		runClimSet.m_FilterValue = climSet.m_FilterValue;



		runClimSet.Update();
		climSet.MoveNext();
	}
	
	runClimSet.Close();
	climSet.Close();

	
	

	

	
   return 1;
}

int CRunSave::SaveWxStation(int runID,  CFireplusSet *fpSet )
{
	if(!m_saveIsOn)
		return 0;

    int runExists = false;

	// get next run ID
	CRunsSet runSet(fpSet->m_pDatabase);
	
	// if new record, get new runID
	if (! runID){
		runSet.Open();
		if (! runSet.IsEOF()){
			runSet.MoveLast();
			curr_runID = runSet.m_runID + 1;
		} else
			curr_runID = 1;
		runSet.Close();
	} else { // we were passed a runID. see if it exists.
       runSet.m_strFilter.Format("[runID] = %d", runID);
	   runSet.Open();
	   if (!runSet.IsEOF())  // still not found? add new
		   runExists = true;
       runSet.Close();

	}
	
	


	
	CRunWxStationSet runWxSet(fpSet->m_pDatabase);
	runWxSet.Open();

	// delete existing if necessary
	if (runExists){
    	runWxSet.m_strFilter.Format("[runID] = %d",runID);
        runWxSet.Requery();
		while (!runWxSet.IsEOF()){
                runWxSet.Delete();
				runWxSet.MoveNext();

		}

	} 

	CSIGStationSet staSet(fpSet->m_pDatabase);
	if(fpSet->m_SIG_Station.GetLength() <= 6)
	{
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.Open();
		//staSet.MoveFirst();
		

		//while(!staSet.IsEOF())
		if(!staSet.IsEOF())
		{
			runWxSet.AddNew();
			runWxSet.m_runID = (runID > 0 ? runID : curr_runID);
			
			runWxSet.m_StationID = staSet.m_StationID;
			runWxSet.m_Name = staSet.m_Name;
			runWxSet.m_NFDRSFM = staSet.m_NFDRSFM;
			runWxSet.m_Use88 = staSet.m_Use88;
			runWxSet.m_LatDegrees = staSet.m_LatDegrees;
			runWxSet.m_Elevation = staSet.m_Elevation;
			runWxSet.m_ClimateCls = staSet.m_ClimateCls;
			runWxSet.m_SlopeCls = staSet.m_SlopeCls;
			runWxSet.m_HerbAnnual = staSet.m_HerbAnnual;
			runWxSet.m_StartKBDI = staSet.m_StartKBDI;
			runWxSet.m_Deciduous = staSet.m_Deciduous;
			runWxSet.m_AvgPrecip = staSet.m_AvgPrecip;
			runWxSet.m_Start1000 = staSet.m_Start1000;
			runWxSet.m_FM1Eq10 = staSet.m_FM1Eq10;
			runWxSet.m_Aspect = staSet.m_Aspect;
			runWxSet.m_PsnOnSlope = staSet.m_PsnOnSlope;
			runWxSet.m_FreezeJulian = staSet.m_FreezeJulian;
			runWxSet.m_GreenJulian = staSet.m_GreenJulian;
			runWxSet.m_ObsAgy = staSet.m_ObsAgy;
			runWxSet.m_ObsUnit = staSet.m_ObsUnit;
			runWxSet.m_Site = staSet.m_Site;
			runWxSet.m_State = staSet.m_State;
			runWxSet.m_StnType = staSet.m_StnType;
			runWxSet.m_FSRegion = staSet.m_FSRegion;
			runWxSet.m_LonDegrees = staSet.m_LonDegrees;
			runWxSet.m_County = staSet.m_County;
			runWxSet.m_WRCC_ID = staSet.m_WRCC_ID;



			runWxSet.Update();
			//staSet.MoveNext();
		}
	}
	else //SIG
	{
		staSet.Open();
		CStationInSIGSet sigSet(fpSet->m_pDatabase);
		char tmpStr[64];
		strcpy_s(tmpStr, fpSet->m_SIG_Station);
		sigSet.m_strFilter.Format("[SIG] = '%s'", &tmpStr[6]);
		sigSet.Open();
		while(!sigSet.IsEOF())
		{
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", sigSet.m_StationID);
			staSet.Requery();
			if(!staSet.IsEOF())
			{
				runWxSet.AddNew();
				runWxSet.m_runID = (runID > 0 ? runID : curr_runID);
				
				runWxSet.m_StationID = staSet.m_StationID;
				runWxSet.m_Name = staSet.m_Name;
				runWxSet.m_NFDRSFM = staSet.m_NFDRSFM;
				runWxSet.m_Use88 = staSet.m_Use88;
				runWxSet.m_LatDegrees = staSet.m_LatDegrees;
				runWxSet.m_Elevation = staSet.m_Elevation;
				runWxSet.m_ClimateCls = staSet.m_ClimateCls;
				runWxSet.m_SlopeCls = staSet.m_SlopeCls;
				runWxSet.m_HerbAnnual = staSet.m_HerbAnnual;
				runWxSet.m_StartKBDI = staSet.m_StartKBDI;
				runWxSet.m_Deciduous = staSet.m_Deciduous;
				runWxSet.m_AvgPrecip = staSet.m_AvgPrecip;
				runWxSet.m_Start1000 = staSet.m_Start1000;
				runWxSet.m_FM1Eq10 = staSet.m_FM1Eq10;
				runWxSet.m_Aspect = staSet.m_Aspect;
				runWxSet.m_PsnOnSlope = staSet.m_PsnOnSlope;
				runWxSet.m_FreezeJulian = staSet.m_FreezeJulian;
				runWxSet.m_GreenJulian = staSet.m_GreenJulian;
				runWxSet.m_ObsAgy = staSet.m_ObsAgy;
				runWxSet.m_ObsUnit = staSet.m_ObsUnit;
				runWxSet.m_Site = staSet.m_Site;
				runWxSet.m_State = staSet.m_State;
				runWxSet.m_StnType = staSet.m_StnType;
				runWxSet.m_FSRegion = staSet.m_FSRegion;
				runWxSet.m_LonDegrees = staSet.m_LonDegrees;
				runWxSet.m_County = staSet.m_County;
				runWxSet.m_WRCC_ID = staSet.m_WRCC_ID;

				runWxSet.Update();
			}
			sigSet.MoveNext();
		}
		sigSet.Close();
	}
	runWxSet.Close();
	staSet.Close();

	
	
   return 1;
}


int CRunSave::SaveFireOptions(int runID,  CFireplusSet *fpSet )
{
	if(!m_saveIsOn)
		return 0;

    int runExists = false;

	// get next run ID
	CRunsSet runSet(fpSet->m_pDatabase);
	
	// if new record, get new runID
	if (! runID){
		runSet.Open();
		if (! runSet.IsEOF()){
			runSet.MoveLast();
			curr_runID = runSet.m_runID + 1;
		} else
			curr_runID = 1;
		runSet.Close();
	} else { // we were passed a runID. see if it exists.
       runSet.m_strFilter.Format("[runID] = %d", runID);
	   runSet.Open();
	   if (!runSet.IsEOF())  // still not found? add new
		   runExists = true;
       runSet.Close();

	}
	
	
	


	
	CRunFireOptionsSet runFireSet(fpSet->m_pDatabase);
	runFireSet.Open();
	// delete existing if necessary
	if (runExists){
    	runFireSet.m_strFilter.Format("[runID] = %d",runID);
        runFireSet.Requery();
		while (!runFireSet.IsEOF()){
                runFireSet.Delete();
				runFireSet.MoveNext();

		}

	} 

	CFireOptionsSet fireSet(fpSet->m_pDatabase);
	
	fireSet.Open();
	
	while(!fireSet.IsEOF())
	{
		runFireSet.AddNew();
		runFireSet.m_runID = (runID > 0 ? runID : curr_runID);

		runFireSet.m_SIG_Station = fireSet.m_SIG_Station;
		runFireSet.m_Lightning = fireSet.m_Lightning;
		runFireSet.m_Human = fireSet.m_Human;
		runFireSet.m_All = fireSet.m_All;
		runFireSet.m_Cumulative = fireSet.m_Cumulative;
		runFireSet.m_Probability = fireSet.m_Probability;
		runFireSet.m_Both = fireSet.m_Both;
		runFireSet.m_LFAcres = fireSet.m_LFAcres;
		runFireSet.m_MFDFires = fireSet.m_MFDFires;
		runFireSet.m_VarID = fireSet.m_VarID;
		

		runFireSet.Update();
		fireSet.MoveNext();
	}
	
	runFireSet.Close();
	fireSet.Close();

	
	
   return 1;
}


int CRunSave::SaveRunOption(int runID, short reportID, short row, CString Variable, CString Value, CFireplusSet *fpSet)
{
	if(!m_saveIsOn)
		return 0;

    int runExists = false;

	// get next run ID
	CRunsSet runSet(fpSet->m_pDatabase);
	
	// if new record, get new runID
	if (! runID){
		runSet.Open();
		if (! runSet.IsEOF()){
			runSet.MoveLast();
			curr_runID = runSet.m_runID + 1;
		} else
			curr_runID = 1;
		runSet.Close();
	} else { // we were passed a runID. see if it exists.
       runSet.m_strFilter.Format("[runID] = %d", runID);
	   runSet.Open();
	   if (!runSet.IsEOF())  // still not found? add new
		   runExists = true;
       runSet.Close();

	}
	


	
	CRunOptionsSet runOptSet(fpSet->m_pDatabase);
	runOptSet.Open();

    // delete existing if necessary
	if (runExists){
    	runOptSet.m_strFilter.Format("[runID] = %d and [reportID] = %d and [row] = %d and [Variable] = '%s'",runID,reportID,row,Variable);
        runOptSet.Requery();
		while (!runOptSet.IsEOF()){
                runOptSet.Delete();
				runOptSet.MoveNext();

		}

	} 


	runOptSet.AddNew();
	runOptSet.m_runID = (runID > 0 ? runID : curr_runID);

	runOptSet.m_reportID = reportID;
	runOptSet.m_row = row;
	runOptSet.m_Variable = (Variable ? Variable : " ");
	runOptSet.m_Value = (Value ? Value : " ");

	runOptSet.Update();
	runOptSet.Close();

	return 1;
} 







int CRunSave::RestoreGeneral(int runID,  CFireplusSet *fpSet)
{
	//if(!m_saveIsOn)
	//	return 0;
	if (! runID)
		return 0;


	

	// find record
	CString seek;
	

	CRunsSet runSet(fpSet->m_pDatabase);
	
    CString strSQL = "SELECT * FROM ffpRuns";
	
    
	runSet.m_strFilter.Format("[runID] = %d", runID);
	if (! runSet.IsOpen())
     	runSet.Open(CRecordset::dynaset, strSQL);
	else
		runSet.Requery();

	//seek.Format("[runID] = %d", runID);

	if(runSet.IsEOF()){
		runSet.Close();	
		return 0;
	}

	// update "LastRun" field
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%Y/%m/%d-%H:%M:%S");
	
	
	runSet.Edit();
	runSet.m_LastRun = dStr;
	
    runSet.Update();
	runSet.Close();


	
	CRunDescriptionSet runDescSet(fpSet->m_pDatabase);
	runDescSet.m_strFilter.Format("[runID] = %d", runID);
	runDescSet.Open();

	//seek.Format("[runID] = %d", runID);

	if(runDescSet.IsEOF()){
		runDescSet.Close();
		return 0;
	}



	// hereXXX
	fpSet->Edit();
    fpSet->m_Description = runDescSet.m_Description.TrimRight();
	fpSet->m_EndDay = runDescSet.m_EndDay;
	fpSet->m_EndMonth = runDescSet.m_EndMonth.TrimRight();
	fpSet->m_EndYear = runDescSet.m_EndYear;
    fpSet->m_SIG_Station = runDescSet.m_SIG_Station.TrimRight();
	fpSet->m_StartDay = runDescSet.m_StartDay;
    fpSet->m_StartMonth = runDescSet.m_StartMonth.TrimRight();
	fpSet->m_StartYear = runDescSet.m_StartYear;
	fpSet->m_Use78 = runDescSet.m_Use78;
	fpSet->m_Use88 = runDescSet.m_Use88;
	fpSet->m_UseCanadian = runDescSet.m_UseCanadian;
	fpSet->m_PeriodLength = runDescSet.m_PeriodLength.TrimRight();
	fpSet->Update();
	

	
	//fireSet.Close();
	runDescSet.Close();

	// restore fire associations



	CRunFireAssocSet runAssocSet(fpSet->m_pDatabase);
	runAssocSet.m_strFilter.Format("[runID] = %d", runID);
	runAssocSet.Open();
		

	if(runAssocSet.IsEOF())
	{
		runAssocSet.Close();
		return 0;
	}

	CFireAssocSet assocSet(fpSet->m_pDatabase);
	
	assocSet.Open();
	// delete existing associations
	while(!assocSet.IsEOF())
	{
		assocSet.Delete();
		assocSet.MoveNext();
	}

	// now copy from saved run
	while(!runAssocSet.IsEOF())
	{
		assocSet.AddNew();
		
		assocSet.m_SIG_Station = runAssocSet.m_SIG_Station.TrimRight();
		assocSet.m_AgencyID = runAssocSet.m_AgencyID;
		assocSet.m_RegionID = runAssocSet.m_RegionID;
		assocSet.m_UnitID = runAssocSet.m_UnitID;
		
		assocSet.m_SubunitID = runAssocSet.m_SubunitID;
		

		assocSet.Update();
		
		runAssocSet.MoveNext();
		//if (runAssocSet.Find(AFX_DAO_NEXT,seek)==0)
		//	break;
	}
	
	runAssocSet.Close();
	assocSet.Close();
	//deal with overlays here...
	CFFPOverlaysSet oSet(fpSet->m_pDatabase);
	oSet.m_strFilter.Format("[runID] = %d", runID);
	oSet.Open();
	if(!oSet.IsEOF())//got some overlays...
	{
		//need to kill runID == 0 overlays first....
		CFFPOverlaysSet o2Set(fpSet->m_pDatabase);
		o2Set.m_strFilter.Format("[runID] = 0");
		o2Set.Open();
		if(!o2Set.IsEOF())
			o2Set.MoveFirst();
		while(!o2Set.IsEOF())
		{
			o2Set.Delete();
			o2Set.MoveNext();
		}
		oSet.MoveFirst();

		while(!oSet.IsEOF())
		{
			o2Set.AddNew();
			o2Set.m_runID = 0;
			o2Set.m_Color = oSet.m_Color;
			o2Set.m_Year = oSet.m_Year;
			o2Set.m_Width = oSet.m_Width;
			o2Set.m_Style = oSet.m_Style;
			o2Set.Update();
			oSet.MoveNext();
		}
		o2Set.Close();
	}
	oSet.Close();
	
   return 1;
}



int CRunSave::RestoreClimateOptions(int runID, CFireplusSet *fpSet)
{
	//if(!m_saveIsOn)
	//	return 0;
	if (! runID)
		return 0;


	

	// find record
	CString seek;
	

	CRunsSet runSet(fpSet->m_pDatabase);

	 CString strSQL = "SELECT * FROM ffpRuns";
	
    
	runSet.m_strFilter.Format("[runID] = %d", runID);
	if (! runSet.IsOpen())
     	runSet.Open(CRecordset::dynaset, strSQL);
	else
		runSet.Requery();


	//seek.Format("[runID] = %d", runID);

	if(runSet.IsEOF())
	{
		runSet.Close();
		return 0;
	}
	runSet.Close();


	

	CRunClimateOptionsSet runClimSet(fpSet->m_pDatabase);
	runClimSet.m_strFilter.Format("[runID] = %d", runID);
	runClimSet.Open();
		

	if(runClimSet.IsEOF()){
		runClimSet.Close();
		return 0;
	}
	CClimateSet climSet(fpSet->m_pDatabase);
	
	climSet.Open();

	/*while(!climSet.IsEOF())
	{
		climSet.Delete();
		climSet.MoveNext();
	}*/

	// now copy from saved run
	while(!climSet.IsEOF())
	{
		//climSet.AddNew();
		runClimSet.m_strFilter.Format("[runID] = %d AND [VarID] = %d", runID, climSet.m_VarID);
		runClimSet.Requery();
		if (!runClimSet.IsEOF())
		{
			//climSet.m_Variable_Name = runClimSet.m_Variable_Name.TrimRight();
			//climSet.m_ShortName = runClimSet.m_ShortName.TrimRight();

			//climSet.m_OptionType = runClimSet.m_OptionType;
			climSet.Edit();

			climSet.m_Stats_Table = runClimSet.m_Stats_Table;
			climSet.m_Stats_Graph = runClimSet.m_Stats_Graph;
			climSet.m_Daily_Freqs = runClimSet.m_Daily_Freqs;
			climSet.m_Period_Mins = runClimSet.m_Period_Mins;
			climSet.m_Period_Maxs = runClimSet.m_Period_Maxs;
			climSet.m_cp2 = runClimSet.m_cp2;
			climSet.m_CriticalPercentile = runClimSet.m_CriticalPercentile;
			climSet.m_Data_Count = runClimSet.m_Data_Count;
			climSet.m_BinSize = runClimSet.m_BinSize;
			climSet.m_Daily_List = runClimSet.m_Daily_List;
			climSet.m_FilterValue = runClimSet.m_FilterValue;

			climSet.Update();
		}
		climSet.MoveNext();
		//if (runClimSet.Find(AFX_DAO_NEXT,seek)==0)
		//	break;
	}
	
	runClimSet.Close();
	climSet.Close();

	

	return 1;
}

int CRunSave::RestoreWxStation(int runID, CFireplusSet *fpSet){

	if (! runID)
		return 0;


	

	// find record
	CString seek;
	

	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.m_strFilter.Format("[runID] = %d", runID);
	runSet.Open();


	//seek.Format("[runID] = %d", runID);

	if(runSet.IsEOF()){
		runSet.Close();	
		return 0;
	}
	runSet.Close();


	

	CRunWxStationSet runWxSet(fpSet->m_pDatabase);
	runWxSet.m_strFilter.Format("[runID] = %d", runID);
	runWxSet.Open();
		

	if(runWxSet.IsEOF()){
		runWxSet.Close();
		return 0;
	}
	CSIGStationSet staSet(fpSet->m_pDatabase);
	
	//staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();

	//if(!staSet.IsEOF())
	//{

		// now copy from saved run
		while(!runWxSet.IsEOF())
		{
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", runWxSet.m_StationID);
			staSet.Requery();
			if(!staSet.IsEOF())
			{
				staSet.Edit();
				
				staSet.m_StationID = runWxSet.m_StationID.TrimRight();
				staSet.m_Name = runWxSet.m_Name.TrimRight();
				staSet.m_NFDRSFM = runWxSet.m_NFDRSFM.TrimRight();
				staSet.m_Use88 = runWxSet.m_Use88;
				staSet.m_LatDegrees = runWxSet.m_LatDegrees;
				staSet.m_Elevation = runWxSet.m_Elevation;
				staSet.m_ClimateCls = runWxSet.m_ClimateCls;
				staSet.m_SlopeCls = runWxSet.m_SlopeCls;
				staSet.m_HerbAnnual = runWxSet.m_HerbAnnual;
				staSet.m_StartKBDI = runWxSet.m_StartKBDI;
				staSet.m_Deciduous = runWxSet.m_Deciduous;
				staSet.m_AvgPrecip = runWxSet.m_AvgPrecip;
				staSet.m_Start1000 = runWxSet.m_Start1000;
				staSet.m_FM1Eq10 = runWxSet.m_FM1Eq10;
				staSet.m_Aspect = runWxSet.m_Aspect.TrimRight();
				staSet.m_PsnOnSlope = runWxSet.m_PsnOnSlope.TrimRight();
				staSet.m_FreezeJulian = runWxSet.m_FreezeJulian;
				staSet.m_GreenJulian = runWxSet.m_GreenJulian;
				staSet.m_ObsAgy = runWxSet.m_ObsAgy;
				staSet.m_ObsUnit = runWxSet.m_ObsUnit.TrimRight();
				staSet.m_Site = runWxSet.m_Site;
				staSet.m_State = runWxSet.m_State.TrimRight();
				staSet.m_StnType = runWxSet.m_StnType;
				staSet.m_FSRegion = runWxSet.m_FSRegion;
				staSet.m_LonDegrees = runWxSet.m_LonDegrees;
				staSet.m_County = runWxSet.m_County;
				staSet.m_WRCC_ID = runWxSet.m_WRCC_ID;
				staSet.Update();
			}
			runWxSet.MoveNext();
		
			//if (runWxSet.Find(AFX_DAO_NEXT,seek)==0)
			//	break;
			//runWxSet.MoveNext();
		}
	runWxSet.Close();
	staSet.Close();

	
	return 1;
}

int CRunSave::RestoreFireOptions(int runID, CFireplusSet *fpSet){



	// find record
	CString seek;
	

	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.m_strFilter.Format("[runID] = %d", runID);
	runSet.Open();


	//seek.Format("[runID] = %d", runID);

	if(runSet.IsEOF()){
		runSet.Close();
		return 0;
	}
	
	runSet.Close();


	

	CRunFireOptionsSet runFireSet(fpSet->m_pDatabase);
	runFireSet.m_strFilter.Format("[runID] = %d", runID);
	runFireSet.Open();
		

	if(runFireSet.IsEOF()){
		runFireSet.Close();
		return 0;
	}
	CFireOptionsSet fireSet(fpSet->m_pDatabase);
	
	fireSet.Open();

	while(!fireSet.IsEOF())
	{
		fireSet.Delete();
		fireSet.MoveNext();
	}

	// now copy from saved run
	while(!runFireSet.IsEOF())
	{
		fireSet.AddNew();
		

		fireSet.m_SIG_Station = runFireSet.m_SIG_Station.TrimRight();
		fireSet.m_Lightning = runFireSet.m_Lightning;
		fireSet.m_Human = runFireSet.m_Human;
		fireSet.m_All = runFireSet.m_All;
		fireSet.m_Cumulative = runFireSet.m_Cumulative;
		fireSet.m_Probability = runFireSet.m_Probability;
		fireSet.m_Both = runFireSet.m_Both;
		fireSet.m_LFAcres = runFireSet.m_LFAcres;
		fireSet.m_MFDFires = runFireSet.m_MFDFires;
		fireSet.m_VarID = runFireSet.m_VarID;


		fireSet.Update();
		
		runFireSet.MoveNext();
		//if (runFireSet.Find(AFX_DAO_NEXT,seek)==0)
		//	break;
	}
	
	runFireSet.Close();
	fireSet.Close();


	return 1;
}


int CRunSave::RestoreRunOption(int runID, short reportID, short row, CString Variable, CString *Value, CFireplusSet *fpSet){


	if (! Variable)
		return 0;

	
	
	if (runID){
	   CRunsSet runSet(fpSet->m_pDatabase);
	   runSet.m_strFilter.Format("[runID] = %d", runID);
	   runSet.Open();


	   //seek.Format("[runID] = %d", runID);

    	if(runSet.IsEOF()){
	 	   runSet.Close();
		   return 0;
	    }
	
	    runSet.Close();
	}

	
	CRunOptionsSet runOptSet(fpSet->m_pDatabase);
	if (runID)
	   runOptSet.m_strFilter.Format("[runID] = %d and [reportID] = %d and [row] = %d and [Variable] = '%s'",
		  runID, reportID, row, Variable);
	else {
	   runOptSet.m_strFilter.Format("[reportID] = %d and [row] = %d and [Variable] = '%s'",
		 reportID, row, Variable);
	   runOptSet.m_strSort.Format("[runID] DESC");
	}
	runOptSet.Open();

	if(runOptSet.IsEOF()){
		runOptSet.Close();
		return 0;
	}
	*Value = runOptSet.m_Value.TrimLeft();

	
	runOptSet.Close();

	return 1;
}







/////////////////////////////////////////////////////////////////////////////
// CRunSavePromptDialog dialog


CRunSavePromptDialog::CRunSavePromptDialog(CWnd* pParent , CFireplusSet *_fpSet , CString *_Description  )
	: CDialog(CRunSavePromptDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRunSavePromptDialog)
	//}}AFX_DATA_INIT
	Description = _Description;


}


void CRunSavePromptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

     //{{AFX_DATA_MAP(CRunSavePromptDialog)
	DDX_Text(pDX, IDC_RUNSAVE_DESCRIPTION, *Description);
	
	 //}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CRunSavePromptDialog, CDialog)
	//{{AFX_MSG_MAP(CRunSavePromptDialog)
	
	
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunSavePromptDialog message handlers





BOOL CRunSavePromptDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CRunSavePromptDialog::OnOK() 
{
	
    UpdateData(TRUE);
	
    


	if (!Description){
		CString strMsg;
		strMsg.Format("ERROR: A valid description must be entered.\n");
		AfxMessageBox(strMsg);
		return;
	}
  
	Description->Trim();
	CDialog::OnOK();
	
}

