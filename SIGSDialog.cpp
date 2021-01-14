// SIGSDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "sigset.h"
#include "SIGSDialog.h"
#include "sigeditdialog.h"
#include "StationInSIGSet.h"
#include "WeightsDialog.h"
#include "stationexportdialog.h"
#include ".\sigsdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSIGSDialog dialog


CSIGSDialog::CSIGSDialog(CWnd* pParent /*=NULL*/, CSIGSet *_sigSet, CFireplusSet *_fpSet)
	: CDialog(CSIGSDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSIGSDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	sigSet = _sigSet;
	fpSet = _fpSet;
}


void CSIGSDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSIGSDialog)
	DDX_Control(pDX, IDC_WEIGHTS, m_weights);
	DDX_Control(pDX, IDC_SIGSEDIT, m_edit);
	DDX_Control(pDX, IDC_SIGSDELETE, m_delete);
	DDX_Control(pDX, IDC_EXPORT_SIG, m_export);
	//}}AFX_DATA_MAP
//	RedisplayTable();
}


BEGIN_MESSAGE_MAP(CSIGSDialog, CDialog)
	//{{AFX_MSG_MAP(CSIGSDialog)
	ON_BN_CLICKED(IDC_SIGSNEW, OnSigsnew)
	ON_BN_CLICKED(IDC_SIGSEDIT, OnSigsedit)
	ON_BN_CLICKED(IDC_SIGSDELETE, OnSigsdelete)
	ON_BN_CLICKED(IDC_WEIGHTS, OnWeights)
	ON_BN_CLICKED(IDC_EXPORT_SIG, OnExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSIGSDialog message handlers
BOOL CSIGSDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	m_grid.GetClientRect(&rect);
	m_grid.SetDefColWidth(rect.Width() / 2);
	m_grid.SetCurrentCellMode(2);
	m_grid.SetNumberCols(2);
	m_grid.SetColWidth(-1, 0);
	m_grid.QuickSetText(0, -1, "SIG Name"); 
	m_grid.QuickSetText(1, -1, "Description");
	RedisplayTable();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSIGSDialog::RedisplayTable()
{
	int count = 0;
	m_grid.SetNumberRows(0);
	if(sigSet->IsOpen())
	{
		try
		{    //reset records for table - something has probably changed
			sigSet->m_strFilter.Format("");

			sigSet->Requery();
			int rows = 0;
			while( !sigSet->IsEOF( ) )    
			{  
				rows++;
				sigSet->MoveNext();
			}
			if(rows > 0)
			{
				sigSet->MoveFirst();
				m_grid.SetNumberRows(rows);
				int row = 0;
				// Move through records
				while( !sigSet->IsEOF( ) )    
				{   
					//add a record to the table
					m_grid.QuickSetText(0, row, sigSet->m_SIG);
					m_grid.QuickSetText(1, row, sigSet->m_DESCRIPTION);
					count++;
					row++;
					//go to next record
					sigSet->MoveNext( );
				}
			}
		}
		catch( CDBException* e )
		{
			// Do nothing--used for security violations     // when opening tables
			e->Delete( );
		}		
	}
	if(count == 0)
	{
		m_edit.EnableWindow(false);
		m_delete.EnableWindow(false);
		m_weights.EnableWindow(false);
	}
	else
	{
		m_edit.EnableWindow();
		m_delete.EnableWindow();
		m_weights.EnableWindow();
	}
	m_grid.RedrawAll();
}

void CSIGSDialog::OnSigsnew() 
{
	// TODO: Add your control notification handler code here
	CSIGEditDialog sigEdit(sigSet, this, true);
	if(sigEdit.DoModal() == IDOK)
		RedisplayTable();
}

void CSIGSDialog::OnSigsedit() 
{
	long row = m_grid.GetCurrentRow();
	if(row >= 0)
	{
		sigSet->SetAbsolutePosition(row + 1);
		CSIGEditDialog sigEdit(sigSet, this, false);
		if(sigEdit.DoModal() == IDOK)
			RedisplayTable();
	}
	
}



void CSIGSDialog::OnSigsdelete() 
{
	if(m_grid.GetNumberRows() > 0)
		sigSet->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	else
		return;
	CString strMsg;
	strMsg.Format("Delete SIG %s from database?.\n", sigSet->m_SIG);
	if(AfxMessageBox(strMsg, MB_YESNO) == IDYES)
	{
		//user wants to delete the SIG
		//first, delete stations from SIGCatalog
		CStationInSIGSet staSigSet(sigSet->m_pDatabase);
		staSigSet.m_strFilter = "[SIG] = '" + sigSet->m_SIG + "'";
		staSigSet.Open();
		if (staSigSet.IsOpen())
		{ 
			//remove any existing records
			while (!staSigSet.IsEOF())
			{
				staSigSet.Delete();
				staSigSet.MoveNext();
			}
		}
		staSigSet.Close();
		//Now delete from SIGS database
		sigSet->Delete();
		RedisplayTable();
	}
}

void CSIGSDialog::OnExport() 
{
	if(m_grid.GetNumberRows() > 0)
		sigSet->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	else
		return;
	CString *sigstr = new CString;
	sigstr->Format("%s",sigSet->m_SIG);
    CStationExportDialog se(this,fpSet,sigstr);
	se.DoModal();
	RedisplayTable();
	
}


void CSIGSDialog::OnWeights() 
{
	if(m_grid.GetNumberRows() > 0)
		sigSet->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	else
		return;
	CStationInSIGSet staSigSet(sigSet->m_pDatabase);
	staSigSet.m_strFilter = "[SIG] = '" + sigSet->m_SIG + "'";
	staSigSet.Open();
	CWeightsDialog wdlg(&staSigSet, this);
	wdlg.DoModal();
	staSigSet.Close();
}

