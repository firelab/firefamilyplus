#pragma once

#include "DialogBarEx.h"

// CCandidatesBar
class CCandidatesDoc;

class CCandidatesBar : public CDialogBarEx
{
	DECLARE_DYNAMIC(CCandidatesBar)
	int m_FireDay;
	int m_LargeFireDay;
	int m_MultiFireDay;
public:
	CCandidatesBar();
	virtual ~CCandidatesBar();
	CCandidatesDoc *m_pDoc;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnFireDay();
	afx_msg void OnLargeFireDay();
	afx_msg void OnMultipleFireDay();
	DECLARE_MESSAGE_MAP()
};


