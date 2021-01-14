// FireQueryPage.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "FireAssocSet.h"
#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "FireQueryPage.h"
#include "DOIUnitSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFireQueryPage property page

IMPLEMENT_DYNCREATE(CFireQueryPage, CPropertyPage)

CFireQueryPage::CFireQueryPage(CFireAssocSet *_assocSet, CFireAgencySet *_agencySet, 
							   CFireRegionSet *_regSet, CFireUnitSet *_unitSet, 
							   CFireSubunitSet *_subUnitSet, CString _staStr) 
		: CPropertyPage(CFireQueryPage::IDD), staStr(_staStr)
{
	//{{AFX_DATA_INIT(CFireQueryPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//NOTE:
	assocSet = _assocSet;
	agencySet = _agencySet;
	regSet = _regSet;
	unitSet = _unitSet;
	subUnitSet = _subUnitSet;
	agencyID = agencySet->m_AgencyID;
	agency = agencySet->m_Name;
	m_psp.dwFlags |= PSP_USETITLE; 
	m_psp.pszTitle = agency;
	

}

CFireQueryPage::~CFireQueryPage()
{
}

void CFireQueryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireQueryPage)
	DDX_Control(pDX, IDC_UNITSLIST, units);
	DDX_Control(pDX, IDC_SUBUNITSLIST, subUnits);
	DDX_Control(pDX, IDC_REGIONSLIST, regions);
	//DDX_Control(pDX, IDC_DOIBUTTON, DOIbutton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFireQueryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFireQueryPage)
	ON_LBN_SELCHANGE(IDC_REGIONSLIST, OnSelchangeRegionslist)
	ON_LBN_SELCHANGE(IDC_UNITSLIST, OnSelchangeUnitslist)
	//ON_BN_CLICKED(IDC_DOIBUTTON, OnDOIbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireQueryPage message handlers


BOOL CFireQueryPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	CWaitCursor wait;
	//populate the lists...
	regSet->m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet->Requery();
	while(!regSet->IsEOF())
	{
		int loc = regions.AddString(regSet->m_Name);
		regions.SetItemData(loc, regSet->m_RegionID);
		regSet->MoveNext();
	}
	//now open associations table and add and/or select necessary items
	assocSet->m_strFilter.Format("[SIG/Station] = '%s' and [AgencyID] = %ld", staStr, agencyID);
	assocSet->m_strSort.Format("[RegionID]");
	assocSet->Requery();
	long lastReg = -1;
	while(!assocSet->IsEOF())
	{
		//find the region and select it
		CString rFind;
		regSet->m_strFilter.Format("[AgencyID] = %ld and [RegionID] = %ld", agencyID, assocSet->m_RegionID);
		//regSet->FindFirst(rFind);
		regSet->Requery();
		if(!regSet->IsEOF())
		{
			int rloc;
			
            rloc = regions.FindString(-1, regSet->m_Name);
			

			/*while(rloc != LB_ERR && regSet->m_RegionID != (long)regions.GetItemData(rloc)) {
			    if (DOIcodes)
					 rloc = regions.FindString(rloc, regSet->m_Name);
				else
                     rloc = regions.FindStringExact(rloc, regSet->m_Name);
			}*/
			if(rloc != LB_ERR)
			{
				//add region's units if necessary
				if(lastReg != regSet->m_RegionID)
				{
					regions.SetSel(rloc);
					AddUnits(regSet->m_RegionID);
					lastReg = regSet->m_RegionID;
				}
				if(!assocSet->IsFieldNull(&assocSet->m_UnitID))
				{
					//an unit is selected, find it in list
					//CString uFind;
					unitSet->m_strFilter.Format("[UnitID] = %ld", assocSet->m_UnitID);
					unitSet->Requery();//FindFirst(uFind);
					if(!unitSet->IsEOF())
					{
						int uloc;
						
						uloc = units.FindString(-1, unitSet->m_Name);
						
						/*while(uloc != LB_ERR && unitSet->m_UnitID != (long)units.GetItemData(uloc)) {
							if (DOIcodes)
							   uloc = units.FindString(uloc, unitSet->m_Name);
							else
                               uloc = units.FindStringExact(uloc, unitSet->m_Name);
						}*/
						if(uloc != LB_ERR)
						{
							if(!units.GetSel(uloc))//wasn't selected
							{
								units.SetSel(uloc);
								AddSubunits(unitSet->m_UnitID);
							}
							if(!assocSet->IsFieldNull(&assocSet->m_SubunitID))
							{	//find subunit and select it
								//CString sFind;
								subUnitSet->m_strFilter.Format("[SubunitID] = %ld", assocSet->m_SubunitID);
								subUnitSet->Requery();//FindFirst(sFind);
								if(!subUnitSet->IsEOF())
								{
									int sloc;
									
									sloc = subUnits.FindString(-1, subUnitSet->m_Name);
									
									/*while(sloc != LB_ERR && subUnitSet->m_SubunitID != (long)subUnits.GetItemData(sloc)) {
										if (DOIcodes)
											 sloc = subUnits.FindString(sloc, subUnitSet->m_Name);
										else
                                             sloc = subUnits.FindStringExact(sloc, subUnitSet->m_Name);
									} */
									if(sloc != LB_ERR)
										subUnits.SetSel(sloc);
								}
							}
						}
					}
				}
			}
		}
		assocSet->MoveNext();
	}				
	regSet->m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet->Requery();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFireQueryPage::OnSelchangeRegionslist() 
{
	//need to add or remove items from Units and Subunits lists...
	CWaitCursor wait;
	//regions.EnableWindow(FALSE);
	int itemLoc = regions.GetCaretIndex();
	if(regions.GetSel(itemLoc))//item is selected, add units to unitslist
	{
		AddUnits(regions.GetItemData(itemLoc));
	}
	else //item is not selected, need to remove units AND subunits if unit is selected
	{
		unitSet->m_strFilter.Format("[RegionID] = %ld", regions.GetItemData(itemLoc));
		unitSet->Requery();
		while(!unitSet->IsEOF())
		{
			for(int i = 0; i < units.GetCount(); i++)
			{
				if(unitSet->m_UnitID == (long)units.GetItemData(i))
				{
					RemoveSubunits(units.GetItemData(i));		
					units.DeleteString(i);
					//break;
				}
			}
			/*int loc = units.FindStringExact(-1, unitSet->m_Name);
			while(loc != LB_ERR && unitSet->m_UnitID != (long)units.GetItemData(loc)) 
				loc = units.FindStringExact(loc, unitSet->m_Name);
			if(loc != LB_ERR)
			{
				RemoveSubunits(units.GetItemData(loc));		
				units.DeleteString(loc);
			}*/
			unitSet->MoveNext();
		}
	}
	//regions.EnableWindow(TRUE);
}

void CFireQueryPage::OnSelchangeUnitslist() 
{
	// TODO: Add your control notification handler code here
	//need to add or remove items from Subunits lists...
	CWaitCursor wait;
	//units.EnableWindow(FALSE);
	int itemLoc = units.GetCaretIndex();
	if(units.GetSel(itemLoc))//item is selected, add subunits to subunitslist
		AddSubunits(units.GetItemData(itemLoc));
	else //item is not selected, need to remove subunits
		RemoveSubunits(units.GetItemData(itemLoc));		
	//units.EnableWindow(TRUE);
}

void CFireQueryPage::RemoveSubunits(long unitID)
{
	subUnitSet->m_strFilter.Format("[UnitID] = %ld", unitID);
	subUnitSet->Requery();
	while(!subUnitSet->IsEOF())
	{
		for(int i = 0; i < subUnits.GetCount(); i++)
		{
			if(subUnitSet->m_SubunitID == (long)subUnits.GetItemData(i))
			{
				subUnits.DeleteString(i);
				//break;
			}

		}
		/*int loc = subUnits.FindStringExact(-1, subUnitSet->m_Name);
		while(loc != LB_ERR && subUnitSet->m_SubunitID != (long)subUnits.GetItemData(loc)) 
			loc = subUnits.FindStringExact(loc, subUnitSet->m_Name);
		if(loc != LB_ERR)
			subUnits.DeleteString(loc);
		else
			AfxMessageBox("loc == LB_ERR in RemoveSubunits()");*/
		subUnitSet->MoveNext();
	}

}

void CFireQueryPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	SaveData();
	CPropertyPage::OnOK();
}

void CFireQueryPage::AddUnits(long regID)
{
	unitSet->m_strFilter.Format("[RegionID] = %ld", regID);
	unitSet->Requery();

    CDOIUnitSet doiSet(unitSet->m_pDatabase);
   doiSet.Open();
	while(!unitSet->IsEOF())
	{   
		int loc;
		
		  
           doiSet.m_strFilter.Format("[UnitID]=%d",unitSet->m_UnitID);
		   doiSet.Requery();
		   if (doiSet.IsEOF()){ // not found
              loc = units.AddString(unitSet->m_Name);
		   } else {
			  CString codestr;
			  codestr.Format("%s",doiSet.m_WFMIcode);
              CString tempstr;
		      tempstr.Format("%s (%s)",unitSet->m_Name,codestr);
             loc = units.AddString(tempstr);
			 
		   }
		
		//int loc = units.InsertString(-1, unitSet->m_Name);
		units.SetItemData(loc, unitSet->m_UnitID);
		unitSet->MoveNext();
	}
	doiSet.Close();

}

void CFireQueryPage::AddSubunits(long unID)
{
	subUnitSet->m_strFilter.Format("[UnitID] = %ld", unID);
	subUnitSet->Requery();
	
	CDOIUnitSet doiSet(unitSet->m_pDatabase);
   doiSet.Open();
	while(!subUnitSet->IsEOF())
	{
        int loc;
		
           int regionID = 0;
		   CFireUnitSet cUnits(subUnitSet->m_pDatabase);
		   cUnits.m_strFilter.Format("[UnitID] = %ld",unID);
           cUnits.Open();
		   if (! cUnits.IsEOF()){
               regionID = cUnits.m_RegionID;
		   }
		   cUnits.Close();
           doiSet.m_strFilter.Format("[UnitID]=%d",unID);
		   doiSet.Requery();
		   if (doiSet.IsEOF()){ // not found
              loc = subUnits.AddString(subUnitSet->m_Name);
		   } else {
			  CString codestr;
			  codestr.Format("%s",doiSet.m_WFMIcode);
              CString tempstr;
		      tempstr.Format("%s (%s)",subUnitSet->m_Name,codestr);
             loc = subUnits.AddString(tempstr);
			 
		   }
		   
		

		//int loc = units.InsertString(-1, unitSet->m_Name);
		subUnits.SetItemData(loc, subUnitSet->m_SubunitID);
		subUnitSet->MoveNext();		


	}
	doiSet.Close();

}

/* 8/2006 no longer needed  - we now show WFMI/DOI codes all the time*/

void CFireQueryPage::OnDOIbutton(){

 



   // note: didn't use ResetContent() because it appeared to mess up selections

  regions.ResetContent();
  units.ResetContent();
  subUnits.ResetContent();
 

    // now redraw
 
    CWaitCursor wait;
	//populate the lists...
	regSet->m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet->Requery();
	while(!regSet->IsEOF())
	{
		int loc = regions.AddString(regSet->m_Name);
		regions.SetItemData(loc, regSet->m_RegionID);
		regSet->MoveNext();
	}
	//now open associations table and add and/or select necessary items
	assocSet->m_strFilter.Format("[SIG/Station] = '%s' and [AgencyID] = %ld", staStr, agencyID);
	assocSet->m_strSort.Format("[RegionID]");
	assocSet->Requery();
	long lastReg = -1;
	while(!assocSet->IsEOF())
	{
		//find the region and select it
		CString rFind;
		regSet->m_strFilter.Format("[AgencyID] = %ld and [RegionID] = %ld", agencyID, assocSet->m_RegionID);
		//regSet->FindFirst(rFind);
		regSet->Requery();
		if(!regSet->IsEOF())
		{
			int rloc;
			
            rloc = regions.FindString(-1, regSet->m_Name);
			

			/*while(rloc != LB_ERR && regSet->m_RegionID != (long)regions.GetItemData(rloc)) {
			    if (DOIcodes)
					 rloc = regions.FindString(rloc, regSet->m_Name);
				else
                     rloc = regions.FindStringExact(rloc, regSet->m_Name);
			}*/
			if(rloc != LB_ERR)
			{
				//add region's units if necessary
				if(lastReg != regSet->m_RegionID)
				{
					regions.SetSel(rloc);
					AddUnits(regSet->m_RegionID);
					lastReg = regSet->m_RegionID;
				}
				if(!assocSet->IsFieldNull(&assocSet->m_UnitID))
				{
					//an unit is selected, find it in list
					//CString uFind;
					unitSet->m_strFilter.Format("[UnitID] = %ld", assocSet->m_UnitID);
					unitSet->Requery();//FindFirst(uFind);
					if(!unitSet->IsEOF())
					{
						int uloc;
					
					    uloc = units.FindString(-1, unitSet->m_Name);
						
						/*while(uloc != LB_ERR && unitSet->m_UnitID != (long)units.GetItemData(uloc)) {
							if (DOIcodes)
							   uloc = units.FindString(uloc, unitSet->m_Name);
							else
                               uloc = units.FindStringExact(uloc, unitSet->m_Name);
						}*/
						if(uloc != LB_ERR)
						{
							if(!units.GetSel(uloc))//wasn't selected
							{
								units.SetSel(uloc);
								AddSubunits(unitSet->m_UnitID);
							}
							if(!assocSet->IsFieldNull(&assocSet->m_SubunitID))
							{	//find subunit and select it
								//CString sFind;
								subUnitSet->m_strFilter.Format("[SubunitID] = %ld", assocSet->m_SubunitID);
								subUnitSet->Requery();//FindFirst(sFind);
								if(!subUnitSet->IsEOF())
								{
									int sloc;
									
								    sloc = subUnits.FindString(-1, subUnitSet->m_Name);
									
									/*while(sloc != LB_ERR && subUnitSet->m_SubunitID != (long)subUnits.GetItemData(sloc)) {
										if (DOIcodes)
											 sloc = subUnits.FindString(sloc, subUnitSet->m_Name);
										else
                                             sloc = subUnits.FindStringExact(sloc, subUnitSet->m_Name);
									} */
									if(sloc != LB_ERR)
										subUnits.SetSel(sloc);
								}
							}
						}
					}
				}
			}
		}
		assocSet->MoveNext();
	}				
	regSet->m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet->Requery();
 

}

void CFireQueryPage::SaveData()
{
	CWaitCursor wait;
	//need to delete the entries for this Station/SIG and this Agency
	assocSet->m_strFilter.Format("[SIG/Station] = '%s' and [AgencyID] = %ld", staStr, agencyID);
	assocSet->Requery();
	while(!assocSet->IsEOF())
	{
		assocSet->Delete();
		assocSet->MoveNext();
	}
	//now write the records out
	bool hadSub = false, hadUnit = false;
	for(int i = 0; i < regions.GetCount(); i++)
	{
		if(regions.GetSel(i))//region is selected
		{
			//check if any units selected
			hadUnit = false;
			unitSet->m_strFilter.Format("[RegionID] = %ld", regions.GetItemData(i));
			unitSet->Requery();
			while(!unitSet->IsEOF())
			{
				//int loc = units.FindStringExact(-1, unitSet->m_Name);
				int loc = units.FindString(-1, unitSet->m_Name);
				while(loc != LB_ERR && unitSet->m_UnitID != (long)units.GetItemData(loc)) 
					//loc = units.FindStringExact(loc, unitSet->m_Name);
					loc = units.FindString(loc, unitSet->m_Name);
				if(loc != LB_ERR)
				{
					if(units.GetSel(loc))//unit is selected
					{
						//check if any subunits selected
						hadSub = false;
						subUnitSet->m_strFilter.Format("[UnitID] = %ld", units.GetItemData(loc));
						subUnitSet->Requery();
						while(!subUnitSet->IsEOF())
						{
							//int sloc = subUnits.FindStringExact(-1, subUnitSet->m_Name);
							int sloc = subUnits.FindString(-1, subUnitSet->m_Name);
							while(sloc != LB_ERR && subUnitSet->m_SubunitID != (long)subUnits.GetItemData(sloc)) 
								//sloc = subUnits.FindStringExact(sloc, subUnitSet->m_Name);
							    sloc = subUnits.FindString(sloc, subUnitSet->m_Name);
							if(sloc != LB_ERR)
							{
								if(subUnits.GetSel(sloc))
								{
								//add this leaf to query
									assocSet->AddNew();
									assocSet->m_SIG_Station = staStr;
									assocSet->m_AgencyID = agencyID;
									assocSet->m_RegionID = regions.GetItemData(i);
									assocSet->m_UnitID = unitSet->m_UnitID;
									assocSet->m_SubunitID = subUnitSet->m_SubunitID;
									assocSet->Update();
									hadSub = hadUnit = true;
								}
							}
							subUnitSet->MoveNext();
						}
						if(!hadSub) //no subunits selected, add unit to query
						{
							assocSet->AddNew();
							assocSet->m_SIG_Station = staStr;
							assocSet->m_AgencyID = agencyID;
							assocSet->m_RegionID = regions.GetItemData(i);
							assocSet->m_UnitID = unitSet->m_UnitID;
							assocSet->Update();
							hadUnit = true;
						}
					}
				}				
				unitSet->MoveNext();
			}
			if(!hadUnit) //add entire region to query
			{
				assocSet->AddNew();
				assocSet->m_SIG_Station = staStr;
				assocSet->m_AgencyID = agencyID;
				assocSet->m_RegionID = regions.GetItemData(i);
				assocSet->Update();
			}
		}
	}
}
