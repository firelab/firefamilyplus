// AddToTermDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "AddToTermDialog.h"
#include <afxrich.h>
#include "eventlocatordoc.h"
#include "TermDatesDialog.h"
#include "fireplusSet.h"
#include "RichView.h"

extern int GetNextTermID(CDatabase *pDB);

// CAddToTermDialog dialog

IMPLEMENT_DYNAMIC(CAddToTermDialog, CDialog)

CAddToTermDialog::CAddToTermDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddToTermDialog::IDD, pParent)
{
	pDoc = NULL;
}

CAddToTermDialog::~CAddToTermDialog()
{
}

void CAddToTermDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TERMS, m_ExistingTermsCombo);
	DDX_Control(pDX, IDC_BUTTON_ADD_EXISTING, m_addExistingButton);
}


BEGIN_MESSAGE_MAP(CAddToTermDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TO_NEW, &CAddToTermDialog::OnBnClickedButtonAddToNew)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EXISTING, &CAddToTermDialog::OnBnClickedButtonAddExisting)
END_MESSAGE_MAP()


// CAddToTermDialog message handlers

void CAddToTermDialog::OnBnClickedButtonAddToNew()
{
	if(!pDoc || !pDoc->m_pDB || !pDoc->m_pDB->IsOpen())
		return;


	int tID = GetNextTermID(pDoc->m_pDB);
	CTermsSet termsSet(pDoc->m_pDB);
	termsSet.Open();
	termsSet.AddNew();
	termsSet.m_SIG_Station = pDoc->SIG_Station;
	termsSet.m_Name = "";
	COleDateTime tDate(pDoc->startYear, pDoc->startMonth, pDoc->startDay, 0, 0, 0);
	termsSet.m_StartDate = tDate;
	termsSet.m_Comment = "Event Locator created Term";
	termsSet.m_TermID = tID;
	termsSet.Update();
	termsSet.Close();
	//add blank records for each year in fpSet....
	CTermDatesSet tdSet(pDoc->m_fpSet->m_pDatabase);
	tdSet.Open();



	for(int y = pDoc->m_fpSet->m_StartYear; y <= pDoc->m_fpSet->m_EndYear; y++)
	{
		tdSet.AddNew();
		tdSet.m_TermID = tID;
		tdSet.m_Year = y;
		tdSet.SetFieldNull(&tdSet.m_Date);
		tdSet.m_Comment = "";
		tdSet.Update();
	}

	AddEventsToTerm(tID);

		if(pDoc->m_fpSet->m_pTermDatesDialog)
		{
			pDoc->m_fpSet->m_pTermDatesDialog->DestroyWindow();
			pDoc->m_fpSet->m_pTermDatesDialog = NULL;
		}


		CTermDatesDialog *ctdd = new CTermDatesDialog(this->GetParent(), tID, pDoc->m_fpSet->m_pDatabase,pDoc->m_fpSet);
		ctdd->Create();
	//CTermDatesDialog ctdd(this, tID, pDoc->m_fpSet);
	//ctdd.DoModal();
	CDialog::OnOK();
}

void CAddToTermDialog::OnBnClickedButtonAddExisting()
{
	if(!pDoc || !pDoc->m_pDB->IsOpen())
		return;
	int idx = m_ExistingTermsCombo.GetCurSel();
	
	int tID = m_ExistingTermsCombo.GetItemData(idx);
	// termsSet(pDoc->m_pDB);
	//termsSet.Open();
	//termsSet.AddNew();
	//termsSet.m_SIG_Station = pDoc->SIG_Station;
	//termsSet.m_Name = "";
	//COleDateTime tDate(pDoc->startYear, pDoc->startMonth, pDoc->startDay, 0, 0, 0);
	//termsSet.m_StartDate = tDate;
	//termsSet.m_Comment = "Event Locator created Term";
	//termsSet.m_TermID = tID;
	//termsSet.Update();
	//termsSet.Close();
	AddEventsToTerm(tID);
		if(pDoc->m_fpSet->m_pTermDatesDialog)
		{
			pDoc->m_fpSet->m_pTermDatesDialog->DestroyWindow();
			pDoc->m_fpSet->m_pTermDatesDialog = NULL;
		}
		CTermDatesDialog *ctdd = new CTermDatesDialog(this->GetParent(), tID, pDoc->m_fpSet->m_pDatabase,pDoc->m_fpSet);
		ctdd->Create();
	//CTermDatesDialog ctdd(this, tID, pDoc->m_fpSet);
	//ctdd.DoModal();
	CDialog::OnOK();
}

char find[] = "Event Definition:";
void CAddToTermDialog::AddEventsToTerm(int tID)
{


	CString fName = pDoc->GetPathName();
	if(fName && fName.GetLength() > 0)
	{
		CTermDatesSet tdSet(pDoc->m_pDB);
		tdSet.m_strFilter.Format("[TermID] = %d", tID);
		tdSet.m_strSort = "[Year]";
		tdSet.Open();
		char buf[512];
	
		
		COleDateTime thisTime, lastTime;
		//first, look for event definition
	
		 CStdioFile file(fName, CFile::modeRead); 
		 CString strLine; 


		while(file.ReadString(strLine)) 
		
		{

			strcpy_s(buf, (LPCTSTR) strLine);
	
			
			buf[10] = 0;
				thisTime.ParseDateTime(buf);
				if(thisTime.GetStatus() != COleDateTime::invalid)
				{
					bool use = true;
					if(lastTime.GetStatus() != COleDateTime::invalid) //check for same year
					{
						if(lastTime.GetYear() == thisTime.GetYear())
							use = false;
					}
					if(use)//first event of a year
					{
						tdSet.m_strFilter.Format("[TermID] = %d AND [Year] = %d", tID, thisTime.GetYear());
						tdSet.Requery();
						if(tdSet.IsBOF() && tdSet.IsEOF())//add a new record
						{
							tdSet.AddNew();
							tdSet.m_Year = thisTime.GetYear();
							tdSet.m_TermID = tID;
						}
						else
							tdSet.Edit();
						tdSet.m_Date = thisTime;
						tdSet.m_Comment = "Added from Event Locator";
						tdSet.Update();
					}
					lastTime = thisTime;
				}
				//fgets(buf, 511, stream);
			//}
			//fclose(stream);
		}
		tdSet.Close();
	}
}

BOOL CAddToTermDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTermsSet termsSet(pDoc->m_pDB);
	termsSet.m_strFilter.Format("[SIG_Station] = '%s'", pDoc->SIG_Station);
	termsSet.Open();
	while(!termsSet.IsEOF())
	{
		int index = m_ExistingTermsCombo.AddString(termsSet.m_Name);
		this->m_ExistingTermsCombo.SetItemData(index, termsSet.m_TermID);
		termsSet.MoveNext();
	}
	if(m_ExistingTermsCombo.GetCount() <= 0)
	{
		int index = m_ExistingTermsCombo.AddString("No Terms Exist");
		m_ExistingTermsCombo.SetCurSel(index);
		m_ExistingTermsCombo.EnableWindow(FALSE);
		m_addExistingButton.EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
