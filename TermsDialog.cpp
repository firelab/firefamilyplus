// TermsDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "TermsDialog.h"
//#include "DelRecsDialog.h"
#include "TermDatesDialog.h"
#include "TermDatesSet.h"
#include "TermPercentilesSet.h"
#include "TermExportDialog.h"
#include "SIGStationSet.h"
#include "SelSIGStationDialog.h"


int GetNextTermID(CDatabase *pDB);
extern char *stripCRLF(char *src);

void removeChar( char s[], char x )

{

    int i, j;

    for( i=0; s[i]!=0; ++i )

    {

        while(s[i]==x) /* copy all chars, including NULL at end, over char to left */

        {

            j=i;

            while(s[j]!=0)

            {

                s[j]=s[j+1];

                ++j;

            }

        }

    }  

}


void DeleteProbabilities(CDatabase *pDB, int termID)
{
	CTermPercentilesSet tpSet(pDB);
	tpSet.m_strFilter.Format("[TermID] = %d", termID);
	tpSet.Open();
	while(!tpSet.IsEOF())
	{
		tpSet.Delete();
		tpSet.MoveNext();
	}
	tpSet.Close();
}


void AddDefaultProbabilities(CDatabase *pDB, int termID)
{
	//first, see if we have records for this Term
	CTermPercentilesSet tpSet(pDB);
	tpSet.m_strFilter.Format("[TermID] = %d", termID);
	tpSet.Open();
	if(tpSet.IsEOF()) //none there...
	{
		CTermPercentilesSet tpSet2(pDB);
		tpSet2.m_strFilter.Format("[TermID] = -1");
		tpSet2.Open();
		while(!tpSet2.IsEOF())
		{
			tpSet.AddNew();
			tpSet.m_TermID = termID;
			tpSet.m_Percentile = tpSet2.m_Percentile;
			tpSet.Update();
			tpSet2.MoveNext();
		}
		tpSet2.Close();
	}
	tpSet.Close();
}

void CopyProbabilities(CDatabase *pDB, int srcID, int destID)
{
	DeleteProbabilities(pDB, destID);
	CTermPercentilesSet tpSet(pDB), tpSet2(pDB);
	tpSet.m_strFilter.Format("[TermID] = %d", srcID);
	tpSet.Open();
	//tpSet2.m_strFilter.Format("[TermID] = %d", destID);
	tpSet2.Open();
	while(!tpSet.IsEOF())
	{
		tpSet2.AddNew();
		tpSet2.m_TermID = destID;
		tpSet2.m_Percentile = tpSet.m_Percentile;
		tpSet2.Update();
		tpSet.MoveNext();
	}
	tpSet.Close();
	tpSet2.Close();

}

int CopyTerm(CDatabase *pDB, int srcID)
{
	CTermsSet srcSet(pDB);
	srcSet.m_strFilter.Format("[TermID] = %d", srcID);
	srcSet.Open();
	CTermsSet tSet(pDB);
	tSet.Open();
	int tID;
	tSet.AddNew();
	tSet.m_SIG_Station = srcSet.m_SIG_Station;
	tSet.m_Name = srcSet.m_Name;
	tSet.m_StartDate = srcSet.m_StartDate;
	tSet.m_Comment = srcSet.m_Comment;
	tID = tSet.m_TermID = GetNextTermID(pDB);
	tSet.Update();
	tSet.Close();
	srcSet.Close();
	//do the term dates
	CTermDatesSet oldSet(pDB), newSet(pDB);
	oldSet.m_strFilter.Format("[TermID] = %d", srcID);
	oldSet.Open();
	newSet.Open();
	while(!oldSet.IsEOF())
	{
		newSet.AddNew();
		newSet.m_TermID = tID;
		newSet.m_Date = oldSet.m_Date;
		newSet.m_Year = oldSet.m_Year;
		newSet.m_Comment = oldSet.m_Comment;
		newSet.Update();
		oldSet.MoveNext();
	}
	oldSet.Close();
	newSet.Close();
	CopyProbabilities(pDB, srcID, tID);
	return tID;
}

CTermsDataSource::CTermsDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CTermsDataSource::~CTermsDataSource(void)
{
}

long CTermsDataSource::GetNumRows()
{
	return totalRecs;
}

int CTermsDataSource::GetNumCols()
{
	return 5;
}

int	CTermsDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 1:
			cell->SetText("SIG/Station");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Name");
			return UG_SUCCESS;
		case 3:
			cell->SetText("Start");
			return UG_SUCCESS;
		case 4:
			cell->SetText("Comment");
			return UG_SUCCESS;	
		default:
			cell->SetText("");
			return UG_SUCCESS;
		}
	}
	if(row == 0)
		records->MoveFirst();
	else if(row == totalRecs - 1)
		records->MoveLast();
	else if(row != virRec)
		records->Move(row - virRec);
	switch(col)
	{
	case 0:
		cell->SetNumber(row + 1);
		break;
	case 1:
		cell->SetText(records->m_SIG_Station.Trim());
		break;
	case 2:
		cell->SetText(records->m_Name.Trim());
		break;
	case 3:
		str = records->m_StartDate.Format("%m/%d");
		cell->SetText(str);
		break;
	case 4:
		cell->SetText(records->m_Comment.Trim());
		break;
	}
	virRec = row;
	return UG_SUCCESS;
}

void CTermsDataSource::SetRecords(CTermsSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	if(!records->IsBOF() || !records->IsEOF())
	{
		records->MoveFirst();
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		if(totalRecs > 0)
			records->MoveFirst();
	}
}

//CandidatesCUG
TermsCUG::TermsCUG()
{
}

TermsCUG::~TermsCUG()
{
}

void TermsCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(5);
	int wid = rect.Width() / 20;
	//int wid = 22;
	SetColWidth(-1, 0);
	SetColWidth(0, wid * 1);
	SetColWidth(1, wid * 3);
	SetColWidth(2, wid * 3);
	SetColWidth(3, wid * 3);
	SetColWidth(4, rect.Width() - wid * 10);

	CUGCell cell;
	for(int c = 0; c < 5; c++)
	{
		GetColDefault( c, &cell );
		cell.SetReadOnly(TRUE);
		SetColDefault( c, &cell );
	}
	//LockColumns(5);

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 

	EnableMenu(TRUE);

}

COLORREF TermsCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void TermsCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void TermsCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void TermsCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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


// CTermsDialog dialog

IMPLEMENT_DYNAMIC(CTermsDialog, CDialog)

CTermsDialog::CTermsDialog(CWnd* pParent /*=NULL*/, CTermsSet *pRecords /*=NULL*/, CString _StaSIG/* = ""*/, bool _IsGeneral/* = FALSE*/ , CFireplusSet *_fpSet)
	: CDialog(CTermsDialog::IDD, pParent)
{
	IsGeneral = _IsGeneral;
	m_fpSet = _fpSet;
	m_staID = _StaSIG;
	pDB = pRecords->m_pDatabase;
	records = pRecords;
	virRec = 0;
	totalRecs = 0;
	m_grid.m_data.SetRecords(records);
}

CTermsDialog::~CTermsDialog()
{
}


void CTermsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_EditButton);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_DeleteButton);
}


BEGIN_MESSAGE_MAP(CTermsDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CTermsDialog::OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CTermsDialog::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CTermsDialog::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CTermsDialog::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CTermsDialog::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CTermsDialog::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// CTermsDialog message handlers

void CTermsDialog::OnBnClickedButtonNew()
{
	CString tStationID;
	if(IsGeneral)
	{//ask for friggin stationID
		CSIGStationSet stations(pDB);
		stations.m_strSort = "[StationID]";
		stations.Open();
		CSelSIGStationDialog sd(NULL, &stations);
		int stares = sd.DoModal();
		stations.Close();
		if (stares != IDOK)
			return;
		tStationID = sd.selectedID;
	}
	else
	{
		tStationID = m_staID;
	}
	int tID;
	records->AddNew();
	records->m_SIG_Station = tStationID;//m_fpSet->m_SIG_Station;
	records->m_Name = "";
	//COleDateTime tDate(m_fpSet->m_StartYear, MonthChtoInt(m_fpSet->m_StartMonth), m_fpSet->m_StartDay, 0, 0, 0);
	COleDateTime tDate(1985, 1, 1, 0, 0, 0);
	records->m_StartDate = tDate;
	records->m_Comment = "";
	tID = records->m_TermID = GetNextTermID(records->m_pDatabase);
	records->Update();
	AddDefaultProbabilities(records->m_pDatabase, tID);
	virRec = 0;
	m_grid.m_data.SetRecords(records);
	int index = m_grid.GetDefDataSource();
	m_grid.SetGridUsingDataSource(index); 
	m_grid.RedrawAll();
	/*if(m_fpSet->m_pTermDatesDialog)
	{
		m_fpSet->m_pTermDatesDialog->DestroyWindow();
		m_fpSet->m_pTermDatesDialog = NULL;
	}*/
	CTermDatesDialog *ctdd = new CTermDatesDialog(this->GetParent(), tID, pDB,m_fpSet);
	ctdd->Create();
	OnOK();
	/*CTermDatesDialog ctdd(this, tID, m_fpSet);
	if(IDOK == ctdd.DoModal())
	{
		//run Wiebull and produce report
		m_grid.m_data.SetRecords(records);
		index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		m_grid.RedrawAll();
		OnOK();
	}
	m_grid.RedrawAll();*/
}

void CTermsDialog::OnBnClickedButtonEdit()
{
	long row = m_grid.GetCurrentRow();
	if(row < 0)
		return;
	records->SetAbsolutePosition(row + 1);
/*	if(m_fpSet)
	{
		if(m_fpSet->m_pTermDatesDialog)
		{
			m_fpSet->m_pTermDatesDialog->DestroyWindow();
			m_fpSet->m_pTermDatesDialog = NULL;
		}
	}*/
	CTermDatesDialog *ctdd = new CTermDatesDialog(this->m_pParentWnd, records->m_TermID, pDB,m_fpSet);
	ctdd->Create();
	OnOK();
	/*CTermDatesDialog ctdd(this, records->m_TermID, m_fpSet);
	if(IDOK == ctdd.DoModal())
	{
		//run Wiebull and produce report
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		//this->Invalidate();
		//m_grid.RedrawAll();
		m_grid.RedrawRow(row);
		OnOK();
	}
	m_grid.RedrawRow(row);*/
}

void CTermsDialog::OnBnClickedButtonDelete()
{
	if(m_grid.GetNumberRows() <= 0)
		return;
	long row = m_grid.GetCurrentRow();
	if(row < 0)
		return;
	records->SetAbsolutePosition(row + 1);

	//CDelRecsDialog drd(this, records, m_grid.m_data.GetNumRows(), m_grid.GetCurrentRow() + 1);
	CString msg;
	msg.Format("Delete Term \"%s\"?", records->m_Name.Trim());
	if(IDYES == AfxMessageBox(msg, MB_YESNO))
	{
		DeleteProbabilities(records->m_pDatabase, records->m_TermID);
		//get rid of termDates records!!!
		CTermDatesSet tdSet(records->m_pDatabase);
		tdSet.m_strFilter.Format("[TermID] = %d", records->m_TermID);
		tdSet.Open();
		while(!tdSet.IsEOF())
		{
			tdSet.Delete();
			tdSet.MoveNext();
		}
		tdSet.Close();
		records->Delete();
		//reset table contents
		CWaitCursor wait;
		records->Requery();
		totalRecs = 0;
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		//m_grid.Invalidate();
		//this->Invalidate();
		m_grid.RedrawAll();
	}
}

BOOL CTermsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//EnableToolTips(TRUE);
	if(IsGeneral)
	{
		SetWindowText(_T("All Terms"));
	}
	else
	{
		CString str;
		if(m_staID.GetLength() > 6) //SIG
			str.Format("Terms for %s", m_staID);
		else
			str.Format("Terms for Station %s", m_staID);
		SetWindowText(str);
	}
	CWaitCursor wait;
	if(!records->IsBOF() || !records->IsEOF())
		records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int GetNextTermID(CDatabase *pDB)
{
	CTermsSet tSet(pDB);
	tSet.m_strSort = "[TermID]";
	int nextID = 1;
	tSet.Open();
	if(!tSet.IsBOF() && !tSet.IsEOF())
	{//has records
		if(tSet.m_TermID > nextID)
		{
			tSet.Close();
			return nextID;
		}
		nextID++;
		tSet.MoveNext();
		while(!tSet.IsEOF())
		{
			if(tSet.m_TermID > nextID)
			{
				tSet.Close();
				return nextID;
			}
			nextID++;
			tSet.MoveNext();
		}
	}
	tSet.Close();
	return nextID;
}

void CTermsDialog::OnBnClickedButtonImport()
{
	CFileDialog	dlg (TRUE, _T("trm"), NULL, OFN_HIDEREADONLY,
		_T("RERAP Term Files (*.trm)|*.trm|All Files (*.*)|*.*||"), this);
	if(dlg.DoModal() == IDOK)
	{
		char fName[256], tmp[512],tmp2[512];
		CString msg;
		strcpy_s(fName, dlg.GetPathName());
		FILE *in = fopen(fName, "rt");
		if(!in)
		{
			msg.Format("Unable to open %s", fName);
			AfxMessageBox(msg);
			return;
		}
		char buf[512];
		char *p;
		int tID;
		if(fgets(buf, 511, in))
		{
			if(strnicmp(buf, "\"Term\",\"\",\"Version 6.00\"", 24) != 0)
			{
				AfxMessageBox("File does not appear to be a version 6.00 Term file");
				fclose(in);
				return;
			}
			//assume it's gonna be good, add records!
			CString tStationID = m_staID;
			if(IsGeneral)
			{//ask for friggin stationID
				CSIGStationSet stations(pDB);
				stations.m_strSort = "[StationID]";
				stations.Open();
				CSelSIGStationDialog sd(NULL, &stations);
				int stares = sd.DoModal();
				stations.Close();
				if (stares != IDOK)
					return;
				tStationID = sd.selectedID;
			}

			records->AddNew();
			tID = records->m_TermID = GetNextTermID(records->m_pDatabase);
			records->m_SIG_Station = tStationID;//m_fpSet->m_SIG_Station;
			if(fgets(buf, 511, in))//Name
			{
				strcpy_s(tmp, &buf[1]);
				stripCRLF(tmp);
				int q = strlen(tmp) - 1;
				if(q >= 0)
					tmp[q] = 0;
				removeChar(tmp,'\"');
				records->m_Name = tmp;
				if(fgets(buf, 511, in))//Comment
				{
					int len = 0;
					int more = false;
					strcpy_s(tmp, &buf[1]);
					//stripCRLF(tmp);
					q = strlen(tmp) - 2;
					if(q>=0)
					{
						if (tmp[0]=='"' && tmp[1]=='"')
							more = false;
						else
						if(tmp[q] == '"')
						{
							more = true;
							tmp[q] = 0;
						}
						len = q + 2;
						//strcpy_s(tmp
						removeChar(tmp,'\"');
						records->m_Comment = tmp;
						while(more)
						{
							fgets(buf, 511, in);
							strcpy_s(tmp, buf);
							//stripCRLF(tmp);
							q = strlen(tmp) - 2;
							more = false;
							if (tmp[0]=='"' && tmp[1]=='"')
								more = false;
							else
							if(q>=0)
							{
								if(tmp[q] == '"')
								{
									more = true;
									tmp[q] = 0;
								}
								len += q + 1;
								records->m_Comment += "\n";
								removeChar(tmp,'\"');
								records->m_Comment += tmp;
							}
						}
					}
					//fgets(buf, 511, in);
					//should be a double quote line
					int m, d;
					if(fgets(buf, 511, in))//start date
					{
						sscanf(buf, "\"%d\",\"%d\"", &m, &d);
						records->m_StartDate.SetDate(2009, m, d);
					}
					float a, b;
					if(fgets(buf, 511, in))
					{
						sscanf(buf, "\"%f\"", &a);
						records->m_Coeff1 = a;
					}
					if(fgets(buf, 511, in))
					{
						sscanf(buf, "\"%f\"", &b);
						records->m_Coeff2 = b;
					}
					//now should be a stream of dates
					CTermDatesSet tdSet(records->m_pDatabase);
					tdSet.Open();
					
					while(!feof(in))
					{
						if(fgets(buf, 511, in))
						{

							stripCRLF(buf);

							if(sscanf(buf, "%[^','],%[^',']", tmp,tmp2) == 2)    // 09/2012 new format - date and a comment
							{
								
								removeChar(tmp,'\"');
								removeChar(tmp2,'\"');


								
								tdSet.AddNew();
								tdSet.m_Date.ParseDateTime(tmp, VAR_DATEVALUEONLY);
								tdSet.m_Year = tdSet.m_Date.GetYear();

								tdSet.m_TermID = records->m_TermID;

							
								tdSet.m_Comment.Format("%s",tmp2);
								
								tdSet.Update();
							} else
							if(sscanf(buf, "%s", tmp) == 1)  // old format - date only
							{
								removeChar(tmp,'\"');
								

								
								tdSet.AddNew();
								tdSet.m_TermID = records->m_TermID;
								tdSet.m_Comment = "";
								tdSet.m_Date.ParseDateTime(tmp, VAR_DATEVALUEONLY);
								tdSet.m_Year = tdSet.m_Date.GetYear();
								tdSet.Update();
							} 
							

						}
					}
					tdSet.Close();
					records->Update();
				}
			}

		}
		fclose(in);
		AddDefaultProbabilities(records->m_pDatabase, tID);
		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		m_grid.RedrawAll();
		/*if(m_fpSet->m_pTermDatesDialog)
		{
			m_fpSet->m_pTermDatesDialog->DestroyWindow();
			m_fpSet->m_pTermDatesDialog = NULL;
		}*/
		CTermDatesDialog *ctdd = new CTermDatesDialog(this->GetParent(), tID, pDB,m_fpSet);
		ctdd->Create();
		OnOK();
		/*CTermDatesDialog ctdd(this, tID, m_fpSet);
		if(IDOK == ctdd.DoModal())
		{
			//run Wiebull and produce report
			m_grid.m_data.SetRecords(records);
			index = m_grid.GetDefDataSource();
			m_grid.SetGridUsingDataSource(index); 
			m_grid.RedrawAll();
			OnOK();
		}
			m_grid.m_data.SetRecords(records);
			index = m_grid.GetDefDataSource();
			m_grid.SetGridUsingDataSource(index); 
			m_grid.RedrawAll();*/
	}
}

void CTermsDialog::OnBnClickedButtonCopy()
{
	long row = m_grid.GetCurrentRow();
	if(row < 0)
		return;
	records->SetAbsolutePosition(row + 1);
	int oldID = records->m_TermID;
	CString msg;
	msg.Format("Copy Term \"%s\"?", records->m_Name.TrimRight());
	if(AfxMessageBox(msg, MB_YESNO) == IDYES)
	{
		int tID = CopyTerm(records->m_pDatabase, oldID);
		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		m_grid.RedrawAll();
		/*if(m_fpSet->m_pTermDatesDialog)
		{
			m_fpSet->m_pTermDatesDialog->DestroyWindow();
			m_fpSet->m_pTermDatesDialog = NULL;
		}*/
		CTermDatesDialog *ctdd = new CTermDatesDialog(this->GetParent(), tID, pDB,m_fpSet);
		ctdd->Create();
		OnOK();
		/*CTermDatesDialog ctdd(this, tID, m_fpSet);
		if(IDOK == ctdd.DoModal())
		{
			//run Wiebull and produce report
			m_grid.m_data.SetRecords(records);
			index = m_grid.GetDefDataSource();
			m_grid.SetGridUsingDataSource(index); 
			m_grid.RedrawAll();
			OnOK();
		}
		records->Requery();
			m_grid.m_data.SetRecords(records);
			index = m_grid.GetDefDataSource();
			m_grid.SetGridUsingDataSource(index); 
			m_grid.RedrawAll();*/
	}
}


void CTermsDialog::OnBnClickedButtonExport()
{
	long row = m_grid.GetCurrentRow();
	if(row < 0)
		return;
	records->SetAbsolutePosition(row + 1);

	int tID = records->m_TermID;


	CTermExportDialog dlg(this, m_fpSet,tID);
}
