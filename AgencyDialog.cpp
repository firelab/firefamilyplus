// AgencyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FireAgencySet.h"
#include "AgencyDialog.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "RegionsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgencyDialog dialog


CAgencyDialog::CAgencyDialog(CWnd* pParent /*=NULL*/, CDatabase *_pDB /*=NULL*/)
	: CDialog(CAgencyDialog::IDD, pParent), agencySet(_pDB)
{
	pDB = _pDB;
	agencySet.m_strSort.Format("[AgencyID]");
	agencySet.Open();
	agencySet.MoveLast();
	nextID = agencySet.m_AgencyID + 1;
	agencySet.m_strFilter.Format("[Permanent] = 0");
	agencySet.Requery();
	//{{AFX_DATA_INIT(CAgencyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

INT_PTR CAgencyDialog::DoModal() 
{
	INT_PTR ret = CDialog::DoModal();
	agencySet.Close();
	return ret;
}

void CAgencyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgencyDialog)
	DDX_Control(pDX, IDC_REGBUTTON, regButton);
	DDX_Control(pDX, IDC_NEWBUTTON, newButton);
	DDX_Control(pDX, IDC_EDITBUTTON, editButton);
	DDX_Control(pDX, IDC_DELBUTTON, delButton);
	DDX_Control(pDX, IDC_AGENCIES, agencies);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAgencyDialog, CDialog)
	//{{AFX_MSG_MAP(CAgencyDialog)
	ON_BN_CLICKED(IDC_DELBUTTON, OnDelbutton)
	ON_BN_CLICKED(IDC_EDITBUTTON, OnEditbutton)
	ON_BN_CLICKED(IDC_NEWBUTTON, OnNewbutton)
	ON_BN_CLICKED(IDC_REGBUTTON, OnRegbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgencyDialog message handlers

BOOL CAgencyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(pDB)
	{
		FillAgencies();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAgencyDialog::FillAgencies()
{
	agencies.ResetContent();
	if(!agencySet.IsBOF() && !agencySet.IsEOF())
	{
		agencySet.MoveFirst();
		while(!agencySet.IsEOF())
		{
			int loc = agencies.AddString(agencySet.m_Name);
			if(loc >= 0)
				agencies.SetItemData(loc, agencySet.m_AgencyID);
			agencySet.MoveNext();
		}
	}
}

void CAgencyDialog::OnDelbutton() 
{
	int loc = agencies.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No agency selected");
		return;
	}
	CString str, nameStr;
	agencies.GetText(loc, nameStr);
	str.Format("Delete agency '%s' and all of its regions,\nunits, and subunits?", nameStr);
	if(AfxMessageBox(str, MB_YESNO) == IDYES)
	{
		//DELETE agency, regions, units, and subunits
		CWaitCursor wait;
		CFireRegionSet regs(pDB);
		regs.m_strFilter.Format("[AgencyID] = %ld", agencies.GetItemData(loc));
		regs.Open();
		while(!regs.IsEOF())
		{
			CFireUnitSet units(pDB);
			units.m_strFilter.Format("[RegionID] = %ld", regs.m_RegionID);
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
			regs.Delete();
			regs.MoveNext();
		}
		regs.Close();
		agencySet.m_strFilter.Format("[AgencyID] = %ld", agencies.GetItemData(loc));
		agencySet.Requery();
		agencySet.Delete();
		agencySet.m_strFilter.Format("[Permanent] = 0");
		agencySet.Requery();
		FillAgencies();
	}
}

void CAgencyDialog::OnEditbutton() 
{
	int loc = agencies.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No agency selected");
		return;
	}

	CNewAgencyDialog nad(this, &agencySet, false, agencies.GetItemData(loc));
	if(nad.DoModal() == IDOK)
		FillAgencies();
}

void CAgencyDialog::OnNewbutton() 
{
	CNewAgencyDialog nad(this, &agencySet, true, nextID);
	if(nad.DoModal() == IDOK)
	{
		nextID++;
		FillAgencies();
	}
	
}

void CAgencyDialog::OnRegbutton() 
{
	int loc = agencies.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No agency selected");
		return;
	}
	CString str;
	agencies.GetText(loc, str);
	CRegionsDialog rd(this, pDB, str, agencies.GetItemData(loc));
	rd.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
// CNewAgencyDialog dialog


CNewAgencyDialog::CNewAgencyDialog(CWnd* pParent /*=NULL*/, CFireAgencySet *_agencySet /*=NULL*/, bool _isNew, long id)
	: CDialog(CNewAgencyDialog::IDD, pParent)
{
	agencySet = _agencySet;
	isNew = _isNew;
	nextID = id;
	//{{AFX_DATA_INIT(CNewAgencyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewAgencyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewAgencyDialog)
	DDX_Control(pDX, IDC_NAME, name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewAgencyDialog, CDialog)
	//{{AFX_MSG_MAP(CNewAgencyDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewAgencyDialog message handlers


void CNewAgencyDialog::OnOK() 
{
	CString str;
	name.GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	if(str.GetLength() <= 0)
	{
		str.Format("ERROR: Agency Name can not be blank!\n");
		AfxMessageBox(str);
		return;
	}
	agencySet->m_strFilter.Format("[Name] = '%s'", str);
	agencySet->Requery();
	if(!agencySet->IsBOF())
	{
		if(isNew || (!isNew && nextID != agencySet->m_AgencyID))
		{
			CString msg;
			msg.Format("ERROR: Agency Name '%s' already exists!\n", str);
			AfxMessageBox(msg);
			agencySet->m_strFilter.Format("[Permanent] = 0");
			agencySet->Requery();
			return;
		}
	}

	if(isNew)
	{
		agencySet->AddNew();
		agencySet->m_AgencyID = nextID;
		agencySet->m_Permanent = false;
		agencySet->m_Name = str;
	}
	else
	{
		agencySet->m_strFilter.Format("[AgencyID] = %ld", nextID);
		agencySet->Requery();
		agencySet->Edit();
		agencySet->m_Name = str;
	}
	agencySet->Update();
	agencySet->m_strFilter.Format("[Permanent] = 0");
	agencySet->Requery();
	CDialog::OnOK();
}


BOOL CNewAgencyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(!isNew)
	{
		agencySet->m_strFilter.Format("[AgencyID] = %ld", nextID);
		agencySet->Requery();
		name.SetWindowText(agencySet->m_Name);
		agencySet->m_strFilter.Format("[Permanent] = 0");
		agencySet->Requery();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

