#pragma once
#include "CandidateSet.h"

// CCandidatesDoc document

class CCandidatesDoc : public CDocument
{
	DECLARE_DYNCREATE(CCandidatesDoc)

public:
	CCandidatesDoc();
	virtual ~CCandidatesDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	CCandidateSet *records;
	int m_FireDay;
	int m_LargeFireDay;
	int m_MultiFireDay;

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
