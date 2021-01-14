// RecCounterDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "RecCounterDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecCounterDialog dialog


CRecCounterDialog::CRecCounterDialog(CWnd* pParent , CString _title , CString _statusStr)
	: CDialog(CRecCounterDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecCounterDialog)
	//}}AFX_DATA_INIT
	recCount = 0;
	m_nID = CRecCounterDialog::IDD;
	m_Parent = pParent;
	m_title = _title;
	m_statusStr = _statusStr;
	cancelled = FALSE;
}


void CRecCounterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecCounterDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecCounterDialog, CDialog)
	//{{AFX_MSG_MAP(CRecCounterDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecCounterDialog message handlers

void CRecCounterDialog::Increment()
{
	recCount++;
	if(m_hWnd)
	{
		//UpdateData(FALSE);
		CEdit *trg = (CEdit *)GetDlgItem(IDC_EDIT1);
		if(trg && trg->m_hWnd)
		{
			CString str;
			str.Format("%ld", recCount);
			trg->SetWindowText(str);
			UpdateWindow();
		}
	}
	//RedrawWindow();
}

void CRecCounterDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	cancelled = TRUE;
	DestroyWindow();
	//CDialog::OnCancel();
}

BOOL CRecCounterDialog::Create() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(m_nID, m_Parent);
}

void CRecCounterDialog::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	cancelled = TRUE;
	::PostQuitMessage(-1);
	//delete this;
	
}

void CRecCounterDialog::SetStatusString(CString str)
{
	m_statusStr = str;
	if(m_hWnd)
	{
		CEdit *trg = (CEdit *)GetDlgItem(IDC_EDIT3);
		if(trg && trg->m_hWnd)
			trg->SetWindowText(str);
		//UpdateData(FALSE);
		UpdateWindow();
	}
}

BOOL CRecCounterDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	// TODO: Add extra initialization here
	//statusString.SetWindowText("Preparing Query...");
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecCounterDialog::Reset()
{
	m_statusStr = "";
	recCount = 0;
	if(m_hWnd)
	{
		CEdit *trg = (CEdit *)GetDlgItem(IDC_EDIT1);
		if(trg && trg->m_hWnd)
			trg->SetWindowText("");
		trg = (CEdit *)GetDlgItem(IDC_EDIT3);
		if(trg && trg->m_hWnd)
			trg->SetWindowText("");
		//UpdateWindow();
	}
}



void CRecCounterDialog::SetTitle(CString str)
{
	CEdit *trg = (CEdit *)GetDlgItem(IDC_EDIT2);
	if(trg->m_hWnd)
		trg->SetWindowText(str);
}
