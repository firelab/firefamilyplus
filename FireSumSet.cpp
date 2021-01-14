// FireSumSet.h : Implementation of the CFireSumSet class

 

// CFireSumSet implementation

// code generated on Wednesday, September 28, 2005, 2:04 PM

#include "stdafx.h"
#include "FireSumSet.h"
#include "FireAssocSet.h"

IMPLEMENT_DYNAMIC(CFireSumSet, CRecordset)

CFireSumSet::CFireSumSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_FireID = 0;
	m_Year = L"";
	m_AgencyID = 0;
	m_RegionID = 0;
	m_UnitID = 0;
	m_SubunitID = 0;
	m_FireNumber = L"";
	m_State = L"";
	m_County = 0;
	m_TotalAcres = 0.0;
	m_SizeClass = L"";
	m_FireName = L"";
	m_StatisticalCause = 0;
	m_SpecificCause = 0;
	m_GeneralCause = 0;
	m_ClassPeople = 0;
	m_CauseNarr = L"";
	m_Township = L"";
	m_Range = L"";
	m_Section = 0;
	m_SubSection = L"";
	m_LatDD = 0;
	m_LatMM = 0;
	m_LatSS = 0;
	m_LonDD = 0;
	m_LonMM = 0;
	m_LonSS = 0;
	m_Other = L"";
	m_WildNum = 0;
	m_Notes = L"";
	m_Ignition;
	m_Discovery;
	m_FirstAttack;
	m_Reinforcement;
	m_DeclareWildlandFire;
	m_Contain;
	m_StrategyMet;
	m_FireOut;
	m_nFields = 38;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CFireSumSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CFireSumSet::GetDefaultSQL()
{
	return _T("[Fire]");
}

void CFireSumSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[FireID]"), m_FireID);
	RFX_Text(pFX, _T("[Year]"), m_Year);
	RFX_Long(pFX, _T("[AgencyID]"), m_AgencyID);
	RFX_Long(pFX, _T("[RegionID]"), m_RegionID);
	RFX_Long(pFX, _T("[UnitID]"), m_UnitID);
	RFX_Long(pFX, _T("[SubunitID]"), m_SubunitID);
	RFX_Text(pFX, _T("[FireNumber]"), m_FireNumber);
	RFX_Text(pFX, _T("[State]"), m_State);
	RFX_Long(pFX, _T("[County]"), m_County);
	RFX_Double(pFX, _T("[TotalAcres]"), m_TotalAcres);
	RFX_Text(pFX, _T("[SizeClass]"), m_SizeClass);
	RFX_Text(pFX, _T("[FireName]"), m_FireName);
	RFX_Long(pFX, _T("[StatisticalCause]"), m_StatisticalCause);
	RFX_Long(pFX, _T("[SpecificCause]"), m_SpecificCause);
	RFX_Long(pFX, _T("[GeneralCause]"), m_GeneralCause);
	RFX_Long(pFX, _T("[ClassPeople]"), m_ClassPeople);
	RFX_Text(pFX, _T("[CauseNarr]"), m_CauseNarr);
	RFX_Text(pFX, _T("[Township]"), m_Township);
	RFX_Text(pFX, _T("[Range]"), m_Range);
	RFX_Long(pFX, _T("[Section]"), m_Section);
	RFX_Text(pFX, _T("[SubSection]"), m_SubSection);
	RFX_Long(pFX, _T("[LatDD]"), m_LatDD);
	RFX_Long(pFX, _T("[LatMM]"), m_LatMM);
	RFX_Long(pFX, _T("[LatSS]"), m_LatSS);
	RFX_Long(pFX, _T("[LonDD]"), m_LonDD);
	RFX_Long(pFX, _T("[LonMM]"), m_LonMM);
	RFX_Long(pFX, _T("[LonSS]"), m_LonSS);
	RFX_Text(pFX, _T("[Other]"), m_Other);
	RFX_Long(pFX, _T("[WildNum]"), m_WildNum);
	RFX_Text(pFX, _T("[Notes]"), m_Notes);
	RFX_Date(pFX, _T("[Ignition]"), m_Ignition);
	RFX_Date(pFX, _T("[Discovery]"), m_Discovery);
	RFX_Date(pFX, _T("[FirstAttack]"), m_FirstAttack);
	RFX_Date(pFX, _T("[Reinforcement]"), m_Reinforcement);
	RFX_Date(pFX, _T("[DeclareWildlandFire]"), m_DeclareWildlandFire);
	RFX_Date(pFX, _T("[Contain]"), m_Contain);
	RFX_Date(pFX, _T("[StrategyMet]"), m_StrategyMet);
	RFX_Date(pFX, _T("[FireOut]"), m_FireOut);

}
/////////////////////////////////////////////////////////////////////////////
// CFireSumSet diagnostics

#ifdef _DEBUG
void CFireSumSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFireSumSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

bool CFireSumSet::FilterToWorkingSet(CFireplusSet * fpSet, int causeType, int moreCauseFlag, bool inverted /*= false*/)
{
	CFireAssocSet assocSet(fpSet->m_pDatabase);
	char tStr[64];
	strcpy(tStr, fpSet->m_SIG_Station);
	assocSet.m_strFilter.Format("[SIG/Station] = '%s'", 
		strlen(tStr) > 6 ? &tStr[6] : tStr);
	assocSet.Open();
	if(assocSet.IsEOF() || assocSet.IsBOF())
	{
		//CString msg = "WARNING: No fire associations specified.\n "
		//	"Fire Associations must be set to analyze fire data.\n\n";
		assocSet.Close();
		//AfxMessageBox(msg, MB_OK);
		return false;
	}
	//build the query for FireSet
	CString query = "", temp, tQuery;
	CString causeStr = "";
	int count = 0;
	while(!assocSet.IsEOF())
	{
		temp = "";
		if(!assocSet.IsFieldNull(&assocSet.m_SubunitID))
			temp.Format("[SubunitID] = %ld", assocSet.m_SubunitID);
		else if(!assocSet.IsFieldNull(&assocSet.m_UnitID))
			temp.Format("[UnitID] = %ld", assocSet.m_UnitID);
		else if(!assocSet.IsFieldNull(&assocSet.m_RegionID))
			temp.Format("[RegionID] = %ld", assocSet.m_RegionID);
		if(count > 0)
			query += " or ";
		query += temp;
		assocSet.MoveNext();
		count++;
	}
	assocSet.Close();
	fpSet->BuildBaseQuery(temp, "Discovery", false, inverted);
	if(count > 0)
		m_strFilter.Format("(%s) and (%s)", query, temp);
	else
		m_strFilter = temp;

	
	//
	if(causeType == 1)//lightning
	{
		temp.Format(" and [StatisticalCause] = 1");
		m_strFilter += temp;
	}
	// new cause filter 2014
	else if (moreCauseFlag > 1 && causeType == 2)
	{

		// USFS clause
		temp.Format(" and (([AgencyID] = 1 AND ([StatisticalCause]=99");
		causeStr += temp;

		// cause codes 2-9: 'factory' codes
		// cause code 10: new 'user-defined' code
		// cause codes 11-20: reserved for future usage

		for (int i=2; i <= 20; i++)
		{
			int testFlag = (moreCauseFlag >> (i-1)) & 1;
			if (testFlag == 1)
			{
				
				temp.Format(" OR ");
				causeStr += temp;
				
				temp.Format("[StatisticalCause] = %d",i);
				causeStr += temp;
				


			}

		}
		// DOI clause
		temp.Format(")) OR ([AgencyID] > 1 AND ([StatisticalCause]=99");
		causeStr += temp;

		
		
		for (int i=2; i <= 20; i++)
		{
			int testFlag = (moreCauseFlag >> (i-1)) & 1;
			if (testFlag == 1)
			{
				
				temp.Format(" OR ");
				causeStr += temp;
				
				int doiCause = 0;
				switch(i)
				{
					case 2:
						doiCause = 6; break;   // equipment
					case 4:
						doiCause = 2; break;   // campfire
					case 5:
						doiCause = 4; break;    // debris
					case 6:
						doiCause = 7; break;    // railroad
					case 7:
						doiCause = 5; break;    // arson
					default:
						doiCause = 1; break;
				}
			

				temp.Format("[StatisticalCause] = %d", doiCause);
				causeStr += temp;
				


			}

		}

		temp.Format(")))");
		causeStr += temp;
		m_strFilter += causeStr;

	}
	else if(causeType == 2)//human
	{
		temp.Format(" and [StatisticalCause] > 1");
		m_strFilter += temp;
	}

	m_strSort.Format("[Discovery]");
	//m_strFilter.Format("%s", query);
	Requery();
	if(IsEOF() || IsBOF())
	{
		//if(!inverted)
		//{
			//CString msg = "WARNING: No fire records match query.\n Please set appropriate Fire Associations\n"
		//		"or import fire data.\n";
		//	AfxMessageBox(msg, MB_OK);
		//}
		return false;
	}

	return true;
}

