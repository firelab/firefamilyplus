#include "stdafx.h"
//#include "fireplus.h"
#include ".\fireselectdatasource.h"

CFireSelectDataSource::CFireSelectDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CFireSelectDataSource::~CFireSelectDataSource(void)
{
}

long CFireSelectDataSource::GetNumRows()
{
	return totalRecs;
}

int CFireSelectDataSource::GetNumCols()
{
	return 4;
}

int	CFireSelectDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 1:
			cell->SetText("Discovery");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Acres");
			return UG_SUCCESS;
		case 3:
			cell->SetText("Name");
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
		str = records->m_Discovery.Format("%m/%d/%y");
		cell->SetText(str);
		break;
	case 2:
		cell->SetNumber(records->m_TotalAcres);
		break;
	case 3:
		cell->SetText(records->m_FireName.Trim());
		break;
	}
	virRec = row;
	return UG_SUCCESS;
}

void CFireSelectDataSource::SetRecords(CFireEditSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}
	if(totalRecs > 0)
		records->MoveFirst();
}

/*int CFireSelectDataSource::FindNext(CString *string,int *col,long *row,int flags)
{
	return TRUE;
}*/
