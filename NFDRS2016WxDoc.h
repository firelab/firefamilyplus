#pragma once
#include "NFDRS2016WxSet.h"

// CNFDRS2016WxDoc document

class CNFDRS2016WxDoc : public CDocument
{
	DECLARE_DYNCREATE(CNFDRS2016WxDoc)

public:
	CNFDRS2016WxDoc();
	virtual ~CNFDRS2016WxDoc();
	CNFDRS2016WxSet *records;
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
