// CandidatesBar.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "CandidatesBar.h"
#include "CandidatesDoc.h"


// CCandidatesBar

IMPLEMENT_DYNAMIC(CCandidatesBar, CDialogBar)

CCandidatesBar::CCandidatesBar()
{
	m_FireDay = m_LargeFireDay = m_MultiFireDay = 1;
	m_pDoc = NULL;
}

CCandidatesBar::~CCandidatesBar()
{
}


BEGIN_MESSAGE_MAP(CCandidatesBar, CDialogBar)
	ON_BN_CLICKED(IDC_CHECK_FIRE_DAY, OnFireDay)
	ON_BN_CLICKED(IDC_CHECK_LARGE_FIRE_DAY, OnLargeFireDay)
	ON_BN_CLICKED(IDC_CHECK_MULTIPLE_FIRE_DAY, OnMultipleFireDay)
END_MESSAGE_MAP()

void CCandidatesBar::DoDataExchange(CDataExchange* pDX) 
{
	if(pDX->m_bSaveAndValidate == 0 && m_pDoc)//init
	{
		m_FireDay = m_pDoc->m_FireDay;
		m_LargeFireDay = m_pDoc->m_LargeFireDay;
		m_MultiFireDay = m_pDoc->m_MultiFireDay;
	}
	DDX_Check(pDX, IDC_CHECK_FIRE_DAY, m_FireDay);
	DDX_Check(pDX, IDC_CHECK_LARGE_FIRE_DAY, m_LargeFireDay);
	DDX_Check(pDX, IDC_CHECK_MULTIPLE_FIRE_DAY, m_MultiFireDay);
	if(pDX->m_bSaveAndValidate && m_pDoc)
	{
		m_pDoc->m_FireDay = m_FireDay;
		m_pDoc->m_LargeFireDay = m_LargeFireDay;
		m_pDoc->m_MultiFireDay = m_MultiFireDay;
	}
	CDialogBarEx::DoDataExchange(pDX);
}



// CCandidatesBar message handlers
void CCandidatesBar::OnFireDay()
{
	UpdateData();
	m_pDoc->UpdateAllViews(NULL);
}

void CCandidatesBar::OnLargeFireDay()
{
	UpdateData();
	m_pDoc->UpdateAllViews(NULL);
}

void CCandidatesBar::OnMultipleFireDay()
{
	UpdateData();
	m_pDoc->UpdateAllViews(NULL);
}




