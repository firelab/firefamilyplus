#include "stdafx.h"
#include "CFiresSet.h"
#include "FireAssocSet.h"
#include "fireplusSet.h"

IMPLEMENT_DYNAMIC(CFiresSet, CRecordset)


CFiresSet::CFiresSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_AgencyID = 0;
	m_RegionID = 0;
	m_UnitID = 0;
	m_SubunitID = 0;
	m_FireNumber = L"";
	m_TotalAcres = 0.0;
	m_FireName = L"";
	m_StatisticalCause = 0;
	m_latitude = 0.0;
	m_longitude = 0.0;
	m_FORID = L"";
	m_IRWINID = L"";

	m_nFields = 15;
	m_nDefaultType = dynaset;
}

CString CFiresSet::GetDefaultConnect()
{
	return _T("");
}

CString CFiresSet::GetDefaultSQL()
{
	return _T("[Fires]");
}

void CFiresSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);

	RFX_Long(pFX, _T("[AgencyID]"), m_AgencyID);
	RFX_Long(pFX, _T("[RegionID]"), m_RegionID);
	RFX_Long(pFX, _T("[UnitID]"), m_UnitID);
	RFX_Long(pFX, _T("[SubunitID]"), m_SubunitID);
	RFX_Text(pFX, _T("[FireNumber]"), m_FireNumber);
	RFX_Double(pFX, _T("[TotalAcres]"), m_TotalAcres);
	RFX_Text(pFX, _T("[FireName]"), m_FireName);
	RFX_Long(pFX, _T("[StatisticalCause]"), m_StatisticalCause);
	RFX_Double(pFX, _T("[Latitude]"), m_latitude);
	RFX_Double(pFX, _T("[Longitude]"), m_longitude);
	RFX_Date(pFX, _T("[Discovery]"), m_Discovery);
	RFX_Date(pFX, _T("[Contain]"), m_Contain);
	RFX_Date(pFX, _T("[StrategyMet]"), m_StrategyMet);
	RFX_Text(pFX, _T("[FORID]"), m_FORID);
	RFX_Text(pFX, _T("[IRWINID]"), m_IRWINID);
}

#ifdef _DEBUG
void CFiresSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFiresSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

bool CFiresSet::FilterToWorkingSet(CFireplusSet* fpSet, int causeType, int moreCauseFlag, bool inverted /*= false*/)
{
	CFireAssocSet assocSet(fpSet->m_pDatabase);
	char tStr[64];
	strcpy_s(tStr, fpSet->m_SIG_Station);
	assocSet.m_strFilter.Format("[SIG/Station] = '%s'",
		strlen(tStr) > 6 ? &tStr[6] : tStr);
	assocSet.Open();
	if (assocSet.IsEOF() || assocSet.IsBOF())
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
	while (!assocSet.IsEOF())
	{
		temp = "";
		if (!assocSet.IsFieldNull(&assocSet.m_SubunitID))
			temp.Format("[SubunitID] = %ld", assocSet.m_SubunitID);
		else if (!assocSet.IsFieldNull(&assocSet.m_UnitID))
			temp.Format("[UnitID] = %ld", assocSet.m_UnitID);
		else if (!assocSet.IsFieldNull(&assocSet.m_RegionID))
			temp.Format("[RegionID] = %ld", assocSet.m_RegionID);
		if (count > 0)
			query += " or ";
		query += temp;
		assocSet.MoveNext();
		count++;
	}
	assocSet.Close();
	fpSet->BuildBaseQuery(temp, "Discovery", false, inverted);
	if (count > 0)
		m_strFilter.Format("(%s) and (%s)", query, temp);
	else
		m_strFilter = temp;


	//
	if (causeType == 1)//lightning
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

		for (int i = 2; i <= 20; i++)
		{
			int testFlag = (moreCauseFlag >> (i - 1)) & 1;
			if (testFlag == 1)
			{

				temp.Format(" OR ");
				causeStr += temp;

				temp.Format("[StatisticalCause] = %d", i);
				causeStr += temp;



			}

		}
		// DOI clause
		temp.Format(")) OR ([AgencyID] > 1 AND ([StatisticalCause]=99");
		causeStr += temp;



		for (int i = 2; i <= 20; i++)
		{
			int testFlag = (moreCauseFlag >> (i - 1)) & 1;
			if (testFlag == 1)
			{

				temp.Format(" OR ");
				causeStr += temp;

				int doiCause = 0;
				switch (i)
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
	else if (causeType == 2)//human
	{
		temp.Format(" and [StatisticalCause] > 1");
		m_strFilter += temp;
	}

	m_strSort.Format("[Discovery]");
	//m_strFilter.Format("%s", query);
	Requery();
	if (IsEOF() || IsBOF())
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

bool CFiresSet::FilterToWorkingSet2(CFireplusSet* fpSet, int causeType, int moreCauseFlag)
{
	CFireAssocSet assocSet(fpSet->m_pDatabase);
	char tStr[64];
	strcpy_s(tStr, fpSet->m_SIG_Station);
	assocSet.m_strFilter.Format("[SIG/Station] = '%s'",
		strlen(tStr) > 6 ? &tStr[6] : tStr);
	assocSet.Open();
	if (assocSet.IsEOF() || assocSet.IsBOF())
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
	while (!assocSet.IsEOF())
	{
		temp = "";
		if (!assocSet.IsFieldNull(&assocSet.m_SubunitID))
			temp.Format("[SubunitID] = %ld", assocSet.m_SubunitID);
		else if (!assocSet.IsFieldNull(&assocSet.m_UnitID))
			temp.Format("[UnitID] = %ld", assocSet.m_UnitID);
		else if (!assocSet.IsFieldNull(&assocSet.m_RegionID))
			temp.Format("[RegionID] = %ld", assocSet.m_RegionID);
		if (count > 0)
			query += " or ";
		query += temp;
		assocSet.MoveNext();
		count++;
	}
	assocSet.Close();
	fpSet->BuildBaseQuery2(temp, "Discovery", false, 1900, 3000);
	if (count > 0)
		m_strFilter.Format("(%s) and (%s)", query, temp);
	else
		m_strFilter = temp;


	//
	if (causeType == 1)//lightning
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

		for (int i = 2; i <= 20; i++)
		{
			int testFlag = (moreCauseFlag >> (i - 1)) & 1;
			if (testFlag == 1)
			{

				temp.Format(" OR ");
				causeStr += temp;

				temp.Format("[StatisticalCause] = %d", i);
				causeStr += temp;



			}

		}
		// DOI clause
		temp.Format(")) OR ([AgencyID] > 1 AND ([StatisticalCause]=99");
		causeStr += temp;



		for (int i = 2; i <= 20; i++)
		{
			int testFlag = (moreCauseFlag >> (i - 1)) & 1;
			if (testFlag == 1)
			{

				temp.Format(" OR ");
				causeStr += temp;

				int doiCause = 0;
				switch (i)
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
	else if (causeType == 2)//human
	{
		temp.Format(" and [StatisticalCause] > 1");
		m_strFilter += temp;
	}

	m_strSort.Format("[Discovery]");
	//m_strFilter.Format("%s", query);
	Requery();
	if (IsEOF() || IsBOF())
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
