// SelSIGStationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SelSIGStationDialog.h"
#include "WxStationSet.h"
#include "sigset.h"

// CSelSIGStationDialog dialog

IMPLEMENT_DYNAMIC(CSelSIGStationDialog, CDialog)

CSelSIGStationDialog::CSelSIGStationDialog(CWnd* pParent /*=NULL*/, CSIGStationSet *_stations)
	: CDialog(CSelSIGStationDialog::IDD, pParent)
{
	stations = _stations;
	selectedID = "";
	showall = false;
}

CSelSIGStationDialog::~CSelSIGStationDialog()
{
}

void CSelSIGStationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSelSIGStationDialog, CDialog)
	ON_BN_CLICKED(IDC_SHOWALLSTATIONS, &CSelSIGStationDialog::OnBnClickedShowallstations)
END_MESSAGE_MAP()

// CSelSIGStationDialog message handlers
void CSelSIGStationDialog::RedisplayTable()
{
	int count = 0;
	int rows = 0;
	m_grid.SetNumberRows(0);
	if(stations->IsOpen())
	{
		try
		{    //reset records for table - something has probably changed
			stations->Requery();
			// Move through records

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
/*	int count = 0;
	m_grid.SetNumberRows(0);

	if(stations->IsOpen())
	{
	        int rows = 0;

			stations->Close();
			CString strSQL;
            CWaitCursor wait;

			if (showall){
               stations->Open();
               // count records
			   while (! stations->IsEOF()){
                  rows ++;
                  stations->MoveNext();
			   }
			} else {
			   strSQL.Format("SELECT * FROM WxStation WHERE WxStation.StationID IN (SELECT DISTINCT WxObs.StationID FROM WXObs   ORDER BY WxObs.StationID)");
			   stations->Open(CRecordset::dynaset, strSQL);

                // count records
			   while (! stations->IsEOF()){
                  rows ++;
                  stations->MoveNext();
			   }

			   if (rows < 1){
                  // no weather data - list all stations
			      stations->Close();
			      stations->Open();
			      stations->MoveFirst();

                  while (! stations->IsEOF()){
                     rows ++;
                     stations->MoveNext();
			       }
			   }
			}
			stations->MoveFirst();

			if (! stations->IsEOF()){
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
			}
		}
	*/
	//add SIGS
	CSIGSet sigSet(stations->m_pDatabase);
	sigSet.Open();
	if (sigSet.IsOpen())
	{
		while (!sigSet.IsEOF())
		{
			CString str = "SIG - " + sigSet.m_SIG;
			int row = rows;
			m_grid.AppendRow();
			m_grid.QuickSetText(0, row, str);
			m_grid.QuickSetText(1, row, sigSet.m_DESCRIPTION);
			rows++;
			sigSet.MoveNext();
		}
	}
	sigSet.Close();

	m_grid.RedrawAll();
}

BOOL CSelSIGStationDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_grid.AttachGrid(this, IDC_GRID);

	showall = false;

	CRect rect;
	GetDlgItem(IDC_GRID)->GetClientRect(&rect);
	//****** Set the Rows and Columns
	int wid = rect.Width() / 3;

	m_grid.SetCurrentCellMode(2);
	m_grid.SetNumberCols(2);
	m_grid.SetColWidth(-1, 0);
	m_grid.SetColWidth(0,wid);
	m_grid.SetColWidth(1,wid*2);
	m_grid.QuickSetText(0, -1, "Station ID");

	m_grid.QuickSetText(1, -1, "Station Name");
	RedisplayTable();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelSIGStationDialog::OnBnClickedShowallstations()
{
   if (showall)
	   showall = false;
   else
	   showall = true;
   RedisplayTable();
}

void CSelSIGStationDialog::OnOK()
{
   	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetText(&selectedID);
	CDialog::OnOK();
}