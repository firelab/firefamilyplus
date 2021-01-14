#pragma once
//#include "f:\source\fireplus\uginclude\ugdtasrc.h"
//#include "ugdtasrc.h"
#include "fireplus.h"
#include "FireEditSet.h"
#include "ugctrl.h"

class CFireSelectDataSource :
	public CUGDataSource
{
public:
	CFireSelectDataSource(void);
	~CFireSelectDataSource(void);
	
	CFireEditSet *records;
	// a few basics needed...
	virtual long GetNumRows();
	virtual int GetNumCols();
	virtual int	GetCell(int col,long row,CUGCell *cell);
	//virtual int FindNext(CString *string,int *col,long *row,int flags);
	void SetRecords(CFireEditSet *_records);
	long totalRecs;
	long virRec;

};
