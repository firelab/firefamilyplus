#pragma once
//#include "ClimAnalysis.h"
#include "RunSave.h"
#include "BatchSet.h"
#include "updialog2.h"
#include "ClimateSet.h"
#include "BatchOutputDoc.h"

const float NODATA_VAL = -9999.0;
class CClimAnalysis;
class CFireStats;
class CFireDay;
class CFireSummary;
class CSIGStationSet;


class CRawWxData
{
public:
	CRawWxData(int nYears = 0, int _varID = 0, CString _varName = "", int _optionType = 0);
	~CRawWxData();
	void AllocData(int nYears);
	void DestroyData();
	int m_nYears;
	int varID;
	int m_optionType;
	CString varName;
	float **m_vals;
};

class CRawClim
{
public:
	CRawClim(CDatabase *db, CString id);
	~CRawClim();
	void AllocData();
	//void CalcNFDRS2016Quick(CSIGStationSet *staSet);
	void GetFires();
	void DestroyData();
//	CString GetModelString(CSIGStationSet *stn);
	CString m_sigStaID;
	CString m_StnName;
//	CString m_modelStr;
	CRawWxData **m_wxData;
	int m_nVars;
	int m_nYears;
	int m_baseYear;
	int m_baseYear2016;
	int m_nYears2016;
	CDatabase *m_pDB;
	CUPDUPDATA* pCUPDUPData;
	CFireplusSet *m_fpSet;
	CClimateSet *m_climSet;
	CFireDay **m_fires;
	//for progress counting
	int m_recsDone;
	int m_maxPossibleRecs;
	bool m_usedExtremes;
	bool m_extremeIsMax[MAXVARIDS];
};

typedef CTypedPtrList<CPtrList, CRawClim *> CRawClimList;

//class CFFPTabView;
class CBatchOutputFrame;
class CQuickBatch : public CDocument
{
	DECLARE_DYNCREATE(CQuickBatch)
public:
	CQuickBatch();
	virtual ~CQuickBatch();
	void CopyStationList(CStringList *pList);
	void GetData();
	void GetAnalysis(CRawClim *pClim);
	void GetFireStats(void);
	void GetFireSummary(void);
	void SwitchTabView(int tab);
	void SwitchTreeView(CRawClim *pClim, int varNum);
	CUPDUPDATA* pCUPDUPData;
	CRawClimList rawClimList;
	CDatabase *m_pDB;
	CFireplusSet *m_pStartupSet;
	CStringList stns;
	CRawClim *activeClim;
	int activeView;
	int activeVar;
	CBatchOutputFrame *m_pFrame;
	CClimAnalysis *m_pAnalysis;
	CFireStats *m_pFireStats;
	CFireSummary *m_pFiresSummary;
	//CFFPTabView *m_pTabView;
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//CClimAnalysis *runs;
	//int nRuns;
	afx_msg void OnFileProperties();

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
};

class CRunOutput
{
public:
	CRunOutput();
	~CRunOutput();
	CClimAnalysis *analysis;
	CFireStats *fStats;
	int CreateReports(CClimateSet *climSet);
	int batchID;
	int runID;
	BOOL autoSave;
	BOOL killData;
};

typedef CTypedPtrList<CPtrList, CRunOutput *> RunOutputList;

class CBatchOutput
{
public:
	CBatchOutput();
	~CBatchOutput();
	void RunBatch();
	void SaveReports();
	void GenerateReports();

	RunOutputList runOutputs;
	int batchID;
	CUPDUPDATA* pCUPDUPData;
	CDatabase *m_pDB;
	CString m_pDBName;
	CString docDir;
	BOOL m_autoSave;
	//int nruns;
};
// CBatchOutputDoc document

class CBatchOutputDoc : public CDocument
{
	DECLARE_DYNCREATE(CBatchOutputDoc)

public:
	CBatchOutputDoc();
	virtual ~CBatchOutputDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//CClimAnalysis *runs;
	//int nRuns;

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
