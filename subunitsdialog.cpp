// SubunitsDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "FireSubunitSet.h"
#include "SubunitsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubunitsDialog dialog


CSubunitsDialog::CSubunitsDialog(CWnd* pParent, CDatabase *_pDB, CString _unitName,
		long _unitID)
	: CDialog(CSubunitsDialog::IDD, pParent), subunitSet(_pDB)
{
	//{{AFX_DATA_INIT(CSubunitsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pDB = _pDB;
	unitName = _unitName;
	unitID = _unitID;
	subunitSet.m_strSort.Format("[SubunitID]");
	subunitSet.Open();
	subunitSet.MoveLast();
	nextID = subunitSet.m_SubunitID + 1;
}


void CSubunitsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubunitsDialog)
	DDX_Control(pDX, IDC_SUBUNITS, subunits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubunitsDialog, CDialog)
	//{{AFX_MSG_MAP(CSubunitsDialog)
	ON_BN_CLICKED(IDC_DELBUTTON, OnDelbutton)
	ON_BN_CLICKED(IDC_EDITBUTTON, OnEditbutton)
	ON_BN_CLICKED(IDC_NEWBUTTON, OnNewbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubunitsDialog message handlers

INT_PTR CSubunitsDialog::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	INT_PTR ret =  CDialog::DoModal();
	subunitSet.Close();
	return ret;
}

BOOL CSubunitsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	wp.rcNormalPosition.left += 30; 
	wp.rcNormalPosition.top += 18; 
	wp.rcNormalPosition.right += 30; 
	wp.rcNormalPosition.bottom += 18; 
	wp.showCmd = SW_SHOWNA;
	SetWindowPlacement(&wp);
	
	// TODO: Add extra initialization here
	CString str;
	str.Format("Create/Edit/Delete Subunits for %s", unitName);
	SetWindowText(str);
	FillSubunits();	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSubunitsDialog::FillSubunits()
{
	subunits.ResetContent();
	subunitSet.m_strFilter.Format("[UnitID] = %ld", unitID);
	subunitSet.Requery();
	if(!subunitSet.IsBOF() && !subunitSet.IsEOF())
	{
		subunitSet.MoveFirst();
		while(!subunitSet.IsEOF())
		{
			int loc = subunits.AddString(subunitSet.m_Name);
			if(loc >= 0)
				subunits.SetItemData(loc, subunitSet.m_SubunitID);
			subunitSet.MoveNext();
		}
	}
}

void CSubunitsDialog::OnDelbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = subunits.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No subunit selected");
		return;
	}
	CString str, nameStr;
	subunits.GetText(loc, nameStr);
	str.Format("Delete subunit '%s'?", nameStr);
	if(AfxMessageBox(str, MB_YESNO) == IDYES)
	{
		//DELETE unit and subunits
		CWaitCursor wait;
		subunitSet.m_strFilter.Format("[SubunitID] = %ld", subunits.GetItemData(loc));
		subunitSet.Requery();
		subunitSet.Delete();
		FillSubunits();
	}
	
}

void CSubunitsDialog::OnEditbutton() 
{
	// TODO: Add your control notification handler code here
	int loc = subunits.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("No subunit selected");
		return;
	}
	CNewSubunitDialog nsd(this, &subunitSet, false, subunits.GetItemData(loc), unitID);
	if(nsd.DoModal() == IDOK)
		FillSubunits();
	
}

void CSubunitsDialog::OnNewbutton() 
{
	// TODO: Add your control notification handler code here
	CNewSubunitDialog nsd(this, &subunitSet, true, nextID, unitID);
	if(nsd.DoModal() == IDOK)
	{
		nextID++;
		FillSubunits();
	}
	
}
/////////////////////////////////////////////////////////////////////////////
// CNewSubunitDialog dialog


CNewSubunitDialog::CNewSubunitDialog(CWnd* pParent, CFireSubunitSet *_subunitSet, bool _isNew, long id, long _unitID)
	: CDialog(CNewSubunitDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewSubunitDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	subunitSet = _subunitSet;
	isNew = _isNew;
	nextID = id;
	unitID = _unitID;
}


void CNewSubunitDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewSubunitDialog)
	DDX_Control(pDX, IDC_NAME, name);
	DDX_Control(pDX, IDC_CODE, code);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewSubunitDialog, CDialog)
	//{{AFX_MSG_MAP(CNewSubunitDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewSubunitDialog message handlers

BOOL CNewSubunitDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!isNew)
	{
		CString str1, str2;
		subunitSet->m_strFilter.Format("[SubunitID] = %ld", nextID);
		subunitSet->Requery();
		str1 = subunitSet->m_Name.SpanExcluding(" ");
		code.SetWindowText(str1);
		str2 = subunitSet->m_Name.Right(subunitSet->m_Name.GetLength() - str1.GetLength() - 1);
		name.SetWindowText(str2);
		subunitSet->m_strFilter.Format("[UnitID] = %ld", unitID);
		subunitSet->Requery();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewSubunitDialog::OnOK() 
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
		subunitSet->AddNew();
		subunitSet->m_UnitID = unitID;
		subunitSet->m_SubunitID = nextID;
	}
	else
	{
		subunitSet->m_strFilter.Format("[SubunitID] = %ld", nextID);
		subunitSet->Requery();
		subunitSet->Edit();
	}
	subunitSet->m_Name = cd + " " + nm;
	subunitSet->Update();
	//record appended
	subunitSet->m_strFilter.Format("[UnitID] = %ld", unitID);
	subunitSet->Requery();
	
	CDialog::OnOK();
}
