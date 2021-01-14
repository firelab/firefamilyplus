#pragma once
//#include <TabView.h> 
#include "BatchOutputDoc.h"


// CFFPTabView view

class CFFPTabView : public CTabView
{
	DECLARE_DYNCREATE(CFFPTabView)

protected:
	CFFPTabView();           // protected constructor used by dynamic creation
	virtual ~CFFPTabView();
	
public:
	CQuickBatch *pDoc;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


