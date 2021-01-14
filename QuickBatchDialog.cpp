//<div class="moz-text-flowed" style="font-family: -moz-fixed">// QuickBatchDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "QuickBatchDialog.h"
#include ".\quickbatchdialog.h"
#include "SIGStationSet.h"
//#include "StationsUsed.h"
#include "updialog2.h"
#include "BatchOutputDoc.h"
#include "BatchOutputFrame.h"
//#include "wxSet.h"
#include "WxStationSet.h"
#include "SIGSet.h"

extern CFireplusApp theApp;

BatchCUG::BatchCUG()
{
}

BatchCUG::~BatchCUG()
{
}

void BatchCUG::OnSetup()
{
	m_iCheckBoxIndex = AddCellType( &m_checkBoxCT );
	CRect rect;
	GetClientRect(&rect);
	int cWid = rect.Width() / 6;
	SetCurrentCellMode(2);
	SetNumberCols(3);
	SetColWidth(-1, 0);//10 * cWid);
	//QuickSetText(-1, -1, "Variable");
	SetColWidth(0, cWid);
	QuickSetText(0, -1, "Run");
	SetColWidth(1, cWid);
	QuickSetText(1, -1, "Station");
	SetColWidth(2, 4 * cWid);
	QuickSetText(2, -1, "Name");
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 0, &cell );
	//GetColDefault( 1, &cell );
	//m_grid.QuickSetText(0, -1, "Station ID");
	//m_grid.QuickSetText(1, -1, "Station Name");
}

void BatchCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void BatchCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

/*int BatchCUG::OnEditStart(int col, long row,CWnd **edit)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(**edit);
	CUGCell cell;
	GetCellIndirect( col, row, &cell );
	DWORD dwStyle = 0;
	if ( cell.GetParam() == USE_COXNUMBER )
	{
		if ( ! m_cutNumeric.GetSafeHwnd())
		{
			m_cutNumeric.Create( WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, COXMASK_ID );
			m_cutNumeric.m_ctrl = this;
		}
		m_cutNumeric.SetDecimalSeparator( '.' );
		m_cutNumeric.SetGroupSeparator( ',' );
		m_cutNumeric.SetDecimalDigitCount(2);
		m_cutNumeric.SetShowLeadingZero(FALSE);
		//switch(col)
		//{
		//case 2:
		m_cutNumeric.SetFractionalDigitCount(0);
			//break;
		//case 3:
			//m_cutNumeric.SetFractionalDigitCount(2);
			//break;
		//}
		*edit = &m_cutNumeric;
	}
	return TRUE;
}*/

void BatchCUG::OnKeyDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);

	if ( *vcKey == VK_TAB )
	{
		int col = GetCurrentCol();
		long row = GetCurrentRow();

		if ( GetKeyState( VK_SHIFT ) < 0 )
		{
			col --;
			if ( col < 0 )
			{
				col = GetNumberCols() - 1;
				row--;
			}
		}
		else
		{
			col ++;
			if ( col >= GetNumberCols())
			{
				col = 0;
				row++;
			}
		}
		GotoCell( col, row );
	}
}

/*int BatchCUG::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	//UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(msg);
	//UNREFERENCED_PARAMETER(param);

	//if( ID == UGCT_CHECKBOX && msg == UGCT_CHECKBOXSET)
	if( msg == UGCT_CHECKBOXSET)
	{
		//check box value has changed....
		//CDTData cellData = m_table.GetCellSet().GetItem(Row,Column).GetValue();
		//LONG sel;
		//cellData.Get(&sel);
		CString tmp;
		QuickGetText(-1, row, &tmp);//m_table.GetCellSet().GetItem(Row, 0).GetText();
		switch(col)
		{
		case 0:
			tmp += " - Summary Statistics Table";
			break;
		case 1:
			tmp += " - Summary Statistics Graph";
			break;
		case 4:
			tmp += " - Daily Frequencies Table";
			break;
		case 5:
			tmp += " - Analysis Period Data Count Table";
			break;
		default:
			return TRUE;
		}
		((CClimateView *)GetParent())->AddRemoveString(tmp, param);
	}
	return TRUE;
}*/

// CQuickBatchDialog dialog

IMPLEMENT_DYNAMIC(CQuickBatchDialog, CDialog)
CQuickBatchDialog::CQuickBatchDialog(CWnd* pParent /*=NULL*/, CDatabase *pDB, CFireplusSet *pFpSet)
	: CDialog(CQuickBatchDialog::IDD, pParent)
{
	m_pDB = pDB;
	m_pFpSet = pFpSet;
}

CQuickBatchDialog::~CQuickBatchDialog()
{
}

void CQuickBatchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQuickBatchDialog, CDialog)
END_MESSAGE_MAP()

// CQuickBatchDialog message handlers

BOOL CQuickBatchDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);

	ConfigureTable();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CQuickBatchDialog::ConfigureTable()
{
	CString temp;
	CUGCell cell;
	m_grid.SetNumberRows(0);
	try
	{
		//StationsUsed stnsUsed(m_pDB);
		//stnsUsed.Open(CRecordset::snapshot, "{call DistinctStation}");
		CSIGStationSet staSet(m_pDB);
		staSet.m_strSort = _T("[StationID]");
		staSet.Open();
		int row = 0;
		CWxStationSet usedStations(m_pDB);
		usedStations.Open(CRecordset::dynaset, "SELECT DISTINCT WXObs.StationID FROM WXObs   ORDER BY WXObs.StationID;");
		if (usedStations.IsOpen())
		{
			// Move through records
			while (!usedStations.IsEOF())
			{
				CString str = usedStations.m_StationID;
				str.TrimRight();
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", str);
				staSet.Requery();
				//add a record to the table
				m_grid.AppendRow();
				m_grid.GetCell(0, row, &cell);
				cell.SetNumber(0);
				m_grid.SetCell(0, row, &cell);
				m_grid.QuickSetText(1, row, str);
				m_grid.QuickSetText(2, row, staSet.m_Name);
				//go to next record
				usedStations.MoveNext();
				row++;
			}
		}
		usedStations.Close();
		//add SIGS
		CSIGSet sigSet(m_pDB);
		sigSet.Open();
		if (sigSet.IsOpen())
		{
			while (!sigSet.IsEOF())
			{
				CString str = "SIG - " + sigSet.m_SIG;
				m_grid.AppendRow();
				m_grid.GetCell(0, row, &cell);
				cell.SetNumber(0);
				m_grid.SetCell(0, row, &cell);
				m_grid.QuickSetText(1, row, str);
				m_grid.QuickSetText(2, row, sigSet.m_DESCRIPTION);
				row++;
				sigSet.MoveNext();
			}
		}
		sigSet.Close();
		CRecordset userStations(m_pDB);
		userStations.Open(CRecordset::dynaset, "SELECT DISTINCT ffpUserVals.StationID FROM ffpUserVals ORDER BY ffpUserVals.StationID;");
		while(!userStations.IsEOF())
		{
			CString str;
			CDBVariant var;
			userStations.GetFieldValue("StationID", var);
			str = *var.m_pstring;
			str.TrimRight();
			bool found = false;
			for(int r = 0; r < m_grid.GetNumberRows(); r++)
			{
				CString tSta = m_grid.QuickGetText(1, r);
				if(str.Compare(tSta) == 0)
				{
					found = true;
					break;
				}
			}
			if(!found)//add it
			{
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", str);
				staSet.Requery();
				m_grid.AppendRow();
				m_grid.GetCell(0, row, &cell);
				cell.SetNumber(0);
				m_grid.SetCell(0, row, &cell);
				m_grid.QuickSetText(1, row, str);
				m_grid.QuickSetText(2, row, staSet.m_Name);
				row++;
			}
			userStations.MoveNext();
		}
		userStations.Close();
		staSet.Close();
	}
	catch( CDBException* e )
	{
		// Do nothing--used for security violations     // when opening tables
		e->Delete( );
	}
	m_grid.RedrawAll();
}

bool QuickBatchRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CQuickBatch* pBatch = (CQuickBatch*)pCUPDUPData->GetAppData();
	pBatch->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->SetProgress(_T("Initializing.."),0);
	pBatch->GetData();

	return true;
}

void CQuickBatchDialog::OnOK()
{
	CUGCell cell;
	CStringList staList;
	for(int i = 0; i < m_grid.GetNumberRows(); i++)
	{
		m_grid.GetCell(0, i, &cell);
		if(cell.GetNumber() != 0)
		{
			m_grid.GetCell(1, i, &cell);
			staList.AddTail(cell.GetText());
		}
	}
	//anything selected?
	if(staList.GetCount() <= 0)
	{
		AfxMessageBox("No stations selected!");
		return;
	}
	//got stations, now get their data
	EnableWindow(FALSE);
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate =
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("BatchOutputView"))
		{
			CQuickBatch *pBatch = (CQuickBatch *)curTemplate->CreateNewDocument();//new CQuickBatch(m_pDB, &staList);
			pBatch->m_pDB = m_pDB;
			pBatch->m_pStartupSet = m_pFpSet;
			pBatch->CopyStationList(&staList);
			CUPDialog cupDlg(m_hWnd, QuickBatchRunProc, pBatch);
			INT_PTR ret = cupDlg.DoModal();
			if(ret == IDOK)//CANCEL)
			{
				CBatchOutputFrame* pFrame = (CBatchOutputFrame*)curTemplate->CreateNewFrame(pBatch, NULL);
				curTemplate->InitialUpdateFrame(pFrame, pBatch);
				CString titleStr = "Interactive Batch - ";
				POSITION pos = staList.GetHeadPosition();
				bool firstStr = true;
				while(pos)
				{
					CString tmp = staList.GetNext(pos);
					if(!firstStr)
						titleStr += ",";
					titleStr += tmp;
					firstStr = false;
				}
				pBatch->SetTitle(titleStr);
				pFrame->SetWindowText(titleStr);
				staList.RemoveAll();
			}
			else
			{
				EnableWindow(TRUE);
				curTemplate->RemoveDocument(pBatch);
				staList.RemoveAll();
				delete pBatch;
				return;
			}
		}
	}
	EnableWindow(TRUE);
	CDialog::OnOK();
}