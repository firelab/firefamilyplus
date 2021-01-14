#pragma once
#include "FireEditSet.h"

// CFiresDoc document

class CFiresDoc : public CDocument
{
	DECLARE_DYNCREATE(CFiresDoc)

public:
	CFiresDoc();
	virtual ~CFiresDoc();
	CFireEditSet *records;

#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
