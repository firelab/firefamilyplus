// UnitsDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "UnitsDialog.h"
#include "SubunitsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitsDialog dialog


CUnitsDialog::CUnitsDialog(CWnd* pParent, CDatabase *_pDB, CString _regName,
		long _regID)
	: CDialog(CUnitsDialog::IDD, pParent), unitSet(_pDB)
{
	//{{AFX_DATA_INIT(CUnitsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pDB = _pDB;
	regName = _regName;
	regID = _regID;
	unitSet.m_strSort.Format("[UnitID]");
	unitSet.Open();
	unitSet.MoveLast();
	nextID = unitSet.m_UnitID + 1;
}


void CUnitsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnitsDialog)
	DDX_Control(pDX, IDC_UNITS, units);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnitsDialog, CDialog)
	//{{AFX_MSG_MAP(CUnitsDialog)
	ON_BN_CLICKED(IDC_DELBUTTON, OnDelbutton)
	ON_BN_CLICKED(IDC_EDITBUTTON, OnEditbutton)
	ON_BN_CLICKED(IDC_NEWBUTTON, OnNewbutton)
	ON_BN_CLICKED(IDC_SUBUNITSBUTTON, OnSubunitsbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitsDialog message handlers

BOOL CUnitsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	wp.rcNormalPosition.left += 20; 
	wp.rcNormalPosition.top += 12; 
	wp.rcNormalPosition.right += 20; 
	wp.rcNormalPosition.bottom += 12; 
	wp.showCmd = SW_SHOWNA;
	SetWindowPlacement(&wp);
	
	// TODO: Add extra initialization here
	CString str;
	str.Format("Create/Edit/Delete Units for %s", regName);
	SetWindowText(str);
	FillUnits();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CUnitsDialog::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	int ret =  CDialog::DoModal();
	unitSet.Close();
	return ret;
}

void CUnitsDialog::FillUnits()
{
	units.ResetContent();
	unitSet.m_strFilter.Format("[RegionID] = %ld", regID);
	unitSet.Requery();
	if(!unitSet.IsBOF() && !unitSet.IsEOF())
	{
		unitSet.MoveFirst();
		while(!unitSet.IsEOF())
		{
			int loc = units.AddString(unitSet.m_Name);
			if(loc >= 0)
				units.SetItemData(loc, unitSet.m_UnitID);
			unitSet.MoveNext();
		}
	}
}

void CUnitsDialog::OnDelbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = units.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No unit selected");
		return;
	}
	CString str, nameStr;
	units.GetText(loc, nameStr);
	str.Format("Delete unit '%s' and all of its\nsubunits?", nameStr);
	if(AfxMessageBox(str, MB_YESNO) == IDYES)
	{
		//DELETE unit and subunits
		CWaitCursor wait;
		CFireSubunitSet subunits(pDB);
		subunits.m_strFilter.Format("[UnitID] = %ld", units.GetItemData(loc));
		subunits.Open();
		while(!subunits.IsEOF())
		{
			subunits.Delete();
			subunits.MoveNext();
		}
		subunits.Close();
		unitSet.m_strFilter.Format("[UnitID] = %ld", units.GetItemData(loc));
		unitSet.Requery();
		unitSet.Delete();
		FillUnits();
	}
}

void CUnitsDialog::OnEditbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = units.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No unit selected");
		return;
	}
	CNewUnitDialog nud(this, &unitSet, false, units.GetItemData(loc), regID);
	if(nud.DoModal() == IDOK)
		FillUnits();
	
}

void CUnitsDialog::OnNewbutton() 
{
	// TODO: Add your control notification handler code here
	CNewUnitDialog nud(this, &unitSet, true, nextID, regID);
	if(nud.DoModal() == IDOK)
	{
		nextID++;
		FillUnits();
	}
	
}

void CUnitsDialog::OnSubunitsbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = units.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No unit selected");
		return;
	}
	CString str;
	units.GetText(loc, str);
	CSubunitsDialog sd(this, pDB, str, units.GetItemData(loc));
	sd.DoModal();
	
}
/////////////////////////////////////////////////////////////////////////////
// CNewUnitDialog dialog


CNewUnitDialog::CNewUnitDialog(CWnd* pParent, CFireUnitSet *_unitSet, bool _isNew, long id, long _regID)
	: CDialog(CNewUnitDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewUnitDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	unitSet = _unitSet;
	isNew = _isNew;
	nextID = id;
	regID = _regID;
}


void CNewUnitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewUnitDialog)
	DDX_Control(pDX, IDC_NAME, name);
	DDX_Control(pDX, IDC_CODE, code);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewUnitDialog, CDialog)
	//{{AFX_MSG_MAP(CNewUnitDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewUnitDialog message handlers

BOOL CNewUnitDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!isNew)
	{
		CString str1, str2;
		unitSet->m_strFilter.Format("[UnitID] = %ld", nextID);
		unitSet->Requery();
		str1 = unitSet->m_Name.SpanExcluding(" ");
		code.SetWindowText(str1);
		str2 = unitSet->m_Name.Right(unitSet->m_Name.GetLength() - str1.GetLength() - 1);
		name.SetWindowText(str2);
		unitSet->m_strFilter.Format("[RegionID] = %ld", regID);
		unitSet->Requery();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewUnitDialog::OnOK() 
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
		unitSet->AddNew();
		unitSet->m_RegionID = regID;
		unitSet->m_UnitID = nextID;
	}
	else
	{
		unitSet->m_strFilter.Format("[UnitID] = %ld", nextID);
		unitSet->Requery();
		unitSet->Edit();
	}
	unitSet->m_Name = cd + " " + nm;
	unitSet->Update();
	//record appended
	unitSet->m_strFilter.Format("[RegionID] = %ld", regID);
	unitSet->Requery();
	CDialog::OnOK();
}
