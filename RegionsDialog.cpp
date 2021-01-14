// RegionsDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "FireRegionSet.h"
#include "RegionsDialog.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "UnitsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionsDialog dialog


CRegionsDialog::CRegionsDialog(CWnd* pParent /*=NULL*/, CDatabase *_pDB, CString _agencyName,
		long _agencyID)
	: CDialog(CRegionsDialog::IDD, pParent), regSet(_pDB)
{
	//{{AFX_DATA_INIT(CRegionsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pDB = _pDB;
	agencyName = _agencyName;
	agencyID = _agencyID;
	regSet.m_strSort.Format("[RegionID]");
	regSet.Open();
	regSet.MoveLast();
	nextID = regSet.m_RegionID + 1;
}


void CRegionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionsDialog)
	DDX_Control(pDX, IDC_REGIONS, regions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegionsDialog, CDialog)
	//{{AFX_MSG_MAP(CRegionsDialog)
	ON_BN_CLICKED(IDC_DELBUTTON, OnDelbutton)
	ON_BN_CLICKED(IDC_EDITBUTTON, OnEditbutton)
	ON_BN_CLICKED(IDC_NEWBUTTON, OnNewbutton)
	ON_BN_CLICKED(IDC_UNITSBUTTON, OnUnitsbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionsDialog message handlers

BOOL CRegionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	wp.rcNormalPosition.left += 10; 
	wp.rcNormalPosition.top += 6; 
	wp.rcNormalPosition.right += 10; 
	wp.rcNormalPosition.bottom += 6; 
	wp.showCmd = SW_SHOWNA;
	SetWindowPlacement(&wp);
	// TODO: Add extra initialization here
	CString str;
	str.Format("Create/Edit/Delete Regions for %s", agencyName);
	SetWindowText(str);
	FillRegions();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegionsDialog::OnDelbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = regions.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No region selected");
		return;
	}
	CString str, nameStr;
	regions.GetText(loc, nameStr);
	str.Format("Delete region '%s' and all of its\nunits and subunits?", nameStr);
	if(AfxMessageBox(str, MB_YESNO) == IDYES)
	{
		//DELETE agency, regions, units, and subunits
		CWaitCursor wait;
		CFireUnitSet units(pDB);
		units.m_strFilter.Format("[RegionID] = %ld", regions.GetItemData(loc));
		units.Open();
		while(!units.IsEOF())
		{
			CFireSubunitSet subunits(pDB);
			subunits.m_strFilter.Format("[UnitID] = %ld", units.m_UnitID);
			subunits.Open();
			while(!subunits.IsEOF())
			{
				subunits.Delete();
				subunits.MoveNext();
			}
			subunits.Close();
			units.Delete();
			units.MoveNext();
		}
		units.Close();
		regSet.m_strFilter.Format("[RegionID] = %ld", regions.GetItemData(loc));
		regSet.Requery();
		regSet.Delete();
		FillRegions();
	}
	
}

void CRegionsDialog::OnEditbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = regions.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No region selected");
		return;
	}
	CNewRegDialog nrd(this, &regSet, false, regions.GetItemData(loc), agencyID);
	if(nrd.DoModal() == IDOK)
		FillRegions();
	
}

void CRegionsDialog::OnNewbutton() 
{
	// TODO: Add your control notification handler code here
	CNewRegDialog nrd(this, &regSet, true, nextID, agencyID);
	if(nrd.DoModal() == IDOK)
	{
		nextID++;
		FillRegions();
	}
}

void CRegionsDialog::FillRegions()
{
	regions.ResetContent();
	regSet.m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet.Requery();
	if(!regSet.IsBOF() && !regSet.IsEOF())
	{
		regSet.MoveFirst();
		while(!regSet.IsEOF())
		{
			int loc = regions.AddString(regSet.m_Name);
			if(loc >= 0)
				regions.SetItemData(loc, regSet.m_RegionID);
			regSet.MoveNext();
		}
	}
}

int CRegionsDialog::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	int ret =  CDialog::DoModal();
	regSet.Close();
	return ret;
}

void CRegionsDialog::OnUnitsbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = regions.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No region selected");
		return;
	}
	CString str;
	regions.GetText(loc, str);
	CUnitsDialog ud(this, pDB, str, regions.GetItemData(loc));
	ud.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
// CNewRegDialog dialog


CNewRegDialog::CNewRegDialog(CWnd* pParent /*=NULL*/, CFireRegionSet *_regSet, bool _isNew, long id, long _agencyID)
	: CDialog(CNewRegDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewRegDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	regSet = _regSet;
	isNew = _isNew;
	nextID = id;
	agencyID = _agencyID;
}


void CNewRegDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewRegDialog)
	DDX_Control(pDX, IDC_NAME, name);
	DDX_Control(pDX, IDC_CODE, code);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewRegDialog, CDialog)
	//{{AFX_MSG_MAP(CNewRegDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRegDialog message handlers

BOOL CNewRegDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!isNew)
	{
		CString str1, str2, str3;
		regSet->m_strFilter.Format("[RegionID] = %ld", nextID);
		regSet->Requery();
		str3 = regSet->m_Name;
		str1 = str3.SpanExcluding(" ");
		code.SetWindowText(str1);
		str2 = regSet->m_Name.Right(regSet->m_Name.GetLength() - str1.GetLength() - 1);
		name.SetWindowText(str2);
		regSet->m_strFilter.Format("[AgencyID] = %ld", agencyID);
		regSet->Requery();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewRegDialog::OnOK() 
{
	// TODO: Add extra validation here
	CString cd, nm;
	code.GetWindowText(cd);
	name.GetWindowText(nm);
	cd.TrimLeft();
	cd.TrimRight();
	nm.TrimLeft();
	nm.TrimRight();
	if(cd.GetLength() == 0)
	{
		AfxMessageBox("ERROR: The code field can not be blank!");
		return;
	}
	if(nm.GetLength() == 0)
	{
		AfxMessageBox("ERROR: The name field can not be blank!");
		return;
	}
	//have valid strings
	if(isNew)
	{
		regSet->AddNew();
		regSet->m_AgencyID = agencyID;
		regSet->m_RegionID = nextID;
	}
	else
	{
		regSet->m_strFilter.Format("[RegionID] = %ld", nextID);
		regSet->Requery();
		regSet->Edit();
	}
	regSet->m_Name = cd + " " + nm;
	regSet->Update();
	//record appended
	regSet->m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet->Requery();
	CDialog::OnOK();
}
