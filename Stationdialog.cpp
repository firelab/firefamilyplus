// Stationdialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SIGStationset.h"
#include "Stationdialog.h"
#include "wxset.h"
#include "StationInSIGSet.h"
#include "EditStationDialog.h"
#include "StationExportDialog.h"
#include "WxStationSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationdialog dialog

CStationdialog::CStationdialog(CWnd* pParent /*=NULL*/, CSIGStationSet *_stations, CFireplusSet *_fpSet)
	: CDialog(CStationdialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStationdialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	stations = _stations;
	fpSet = _fpSet;
	showall = false;
}

void CStationdialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationdialog)
	DDX_Control(pDX, IDC_STATIONEDIT, editButton);
	DDX_Control(pDX, IDC_STATIONDELETE, deleteButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStationdialog, CDialog)
	//{{AFX_MSG_MAP(CStationdialog)
	ON_BN_CLICKED(IDC_STATIONDELETE, OnStationdelete)
	ON_BN_CLICKED(IDC_STATIONEDIT, OnStationedit)
	ON_BN_CLICKED(IDC_CHECK_SHOWALL, OnShowAllButton)
	ON_BN_CLICKED(IDC_NEW_STATION, OnStationnew)
	ON_BN_CLICKED(IDC_STATIONEXPORT,OnStationExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationdialog message handlers

BOOL CStationdialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);

	CRect rect;
	GetDlgItem(IDC_GRID)->GetClientRect(&rect);
	//****** Set the Rows and Columns
	int wid = rect.Width() / 5;

	m_grid.SetCurrentCellMode(2);
	m_grid.SetNumberCols(2);
	m_grid.SetColWidth(-1, 0);
	m_grid.SetColWidth(0,wid);
	m_grid.SetColWidth(1,wid*4);
	m_grid.QuickSetText(0, -1, "Station ID");

	m_grid.QuickSetText(1, -1, "Station Name");
	RedisplayTable();
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStationdialog::OnStationdelete()
{
	// TODO: Add your control notification handler code here
	//if(table.GetRowSet().GetCount() > 0)
	if(m_grid.GetNumberRows() > 0)
	{
		CString str;
		m_grid.QuickGetText(0, m_grid.GetCurrentRow(), &str);
		stations->MoveFirst();
		while(str.CompareNoCase(stations->m_StationID) != 0 &&!stations->IsEOF())
			stations->MoveNext();
		if(stations->IsEOF())
			return;
	}
		//stations->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	else
		return;
	CString strMsg;
	strMsg.Format("Delete Station: %s - %s?\n"
		"WARNING! This will delete all weather records for\n"
		"Station %s from the weather observations database.\n"
		"This action is permanent with no recovery available.\nProceed?",
		stations->m_StationID, stations->m_Name, stations->m_StationID);
	if(AfxMessageBox(strMsg, MB_YESNO) == IDYES)
	{
		//user wants to delete the Station
		//first, delete stations from WxObs
		CWxSet wxSet(stations->m_pDatabase);
		wxSet.m_strFilter = "[StationID] = '" + stations->m_StationID + "'";
		wxSet.Open();
		if (wxSet.IsOpen())
		{
			//remove any existing records
			while(!wxSet.IsEOF())
			{
				wxSet.Delete();
				wxSet.MoveNext();
			}
		}
		wxSet.Close();

		//delete station references from SIGSCatalog
		//first, delete stations from SIGSCatalog
		CStationInSIGSet staSigSet(stations->m_pDatabase);
		staSigSet.m_strFilter = "[StationID] = '" + stations->m_StationID + "'";
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
		//Now delete from WxStation table
		//stations->Delete();
		RedisplayTable();
	}
	return;
}

void CStationdialog::OnStationedit()
{
	// TODO: Add your control notification handler code here
	//if(table.GetRowSet().GetCount() > 0)
	//	stations->SetAbsolutePosition(table.GetCurCell().GetRow());
	if(m_grid.GetNumberRows() > 0)
	{
		CString str;
		m_grid.QuickGetText(0, m_grid.GetCurrentRow(), &str);
		stations->MoveFirst();
		while(str.CompareNoCase(stations->m_StationID) != 0 &&!stations->IsEOF())
			stations->MoveNext();
		if(stations->IsEOF())
			return;
	}
	//long row = m_grid.GetCurrentRow();
	//stations->SetAbsolutePosition(row + 1);
	CEditStationDialog dlg(this, stations);
	if(dlg.DoModal() == IDOK)
		RedisplayTable();
}

/*void CStationdialog::OnStationnew()
{
	if(m_grid.GetNumberRows() > 0)
		stations->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	CEditStationDialog dlg(this, stations, true);
	if(dlg.DoModal() == IDOK)
		RedisplayTable();
}*/

void CStationdialog::OnStationExport()
{
	if(m_grid.GetNumberRows() > 0)
	{
		CString str;
		m_grid.QuickGetText(0, m_grid.GetCurrentRow(), &str);
		stations->MoveFirst();
		while(str.CompareNoCase(stations->m_StationID) != 0 &&!stations->IsEOF())
			stations->MoveNext();
		if(stations->IsEOF())
			return;
	}
	//if(m_grid.GetNumberRows() > 0)
	//	stations->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	CStationExportDialog dlg(this, fpSet,NULL);
	if(dlg.DoModal() == IDOK)
		RedisplayTable();
}

void CStationdialog::RedisplayTable()
{
	int count = 0;
	m_grid.SetNumberRows(0);
	if(stations->IsOpen())
	{
		try
		{    //reset records for table - something has probably changed
			stations->Requery();
			if (! stations->IsEOF())
				stations->MoveFirst();
			// Move through records
			int rows = 0;
			if(!showall)
			{
				CWxStationSet usedStations(stations->m_pDatabase);
				usedStations.Open(CRecordset::dynaset, "SELECT DISTINCT WXObs.StationID FROM WXObs   ORDER BY WXObs.StationID;");

				if (usedStations.IsOpen())
				{
					while (!usedStations.IsEOF())
					{
						rows++;
						usedStations.MoveNext( );
					}
				}
				if(rows > 0)
				{
					usedStations.MoveFirst();
					m_grid.SetNumberRows(rows);
					int row = 0;
					while( !usedStations.IsEOF( ) )
					{
						stations->MoveFirst();
						
						while(stations->m_StationID != usedStations.m_StationID)
						{
							stations->MoveNext();
						}
						if(!stations->IsEOF())
						{
							m_grid.QuickSetText(0, row, usedStations.m_StationID);
							m_grid.QuickSetText(1, row, stations->m_Name);
							count++;
							row++;
						}
						//go to next record
						usedStations.MoveNext( );
					}
				}
			}
			if(showall || rows <= 0)//use em all
			{
				stations->Requery();
				rows = 0;
				while(!stations->IsEOF())
				{
					rows++;
					stations->MoveNext();
				}
				//if (! stations->IsEOF()){
					stations->MoveFirst();
					m_grid.SetNumberRows(rows);
					int row = 0;
					while( !stations->IsEOF( ) )
					{
						m_grid.QuickSetText(0, row, stations->m_StationID);
						m_grid.QuickSetText(1, row, stations->m_Name);
						count++;
						row++;
					//go to next record
						stations->MoveNext( );
					}
				//}
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
		editButton.EnableWindow(false);
		deleteButton.EnableWindow(false);
	}
	else
	{
		editButton.EnableWindow();
		deleteButton.EnableWindow();
	}
	m_grid.RedrawAll();
}

void CStationdialog::OnShowAllButton(){
   if (showall)
	   showall = false;
   else
	   showall = true;
   RedisplayTable();
}

void CStationdialog::OnStationnew()
{
	if(m_grid.GetNumberRows() > 0)
		stations->SetAbsolutePosition(m_grid.GetCurrentRow() + 1);
	CEditStationDialog dlg(this, stations, true);
	if(dlg.DoModal() == IDOK)
		RedisplayTable();
}