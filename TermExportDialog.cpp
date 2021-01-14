// TermExportDialog.cpp : implementation file
//
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "fireplus.h"
#include "fireplusset.h"
#include "TermsSet.h"
#include "TermDatesSet.h"
#include "TermExportDialog.h"
#include "TermsDialog.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

extern char *stripCRLF(char *src);

using namespace std;  


/////////////////////////////////////////////////////////////////////////////
// CTermExportDialog dialog

CTermExportDialog::CTermExportDialog(CTermsDialog* pParent /*=NULL*/, CFireplusSet *_fpSet, int _tID )
	: CDialog(CTermExportDialog::IDD, pParent)
{
	
	
	fpSet = _fpSet;
	tID = _tID;


	

	if(tID < 0)
	{
		AfxMessageBox("ERROR: At least one term record must be selected\nfor custom exports.");
		return;
	}
	
	CTermsSet *termsSet = new CTermsSet(fpSet->m_pDatabase);
	termsSet->m_strFilter.Format("[TermID] = %d", tID);
	termsSet->Open();

	if (termsSet->IsEOF()){
		termsSet->Close();
		CString msg;
	    msg.Format("ERROR: No term record was found for ID %d", tID);
		AfxMessageBox(msg);
		return;
	}

	


	CString buf, temp;
	long count = 0;
	
	
	CString msg;
	chdir(theApp.userDir);
	
	CFileDialog fd(FALSE, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"RERAP Term Files (*.trm)|*.trm|All Files (*.*)|*.*||", this);
	
	if(fd.DoModal() != IDOK)
		return;

	CWaitCursor wait;

	CString outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		CString msg;
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return;
	}

	termsSet->MoveFirst();
	fprintf(out,"\"Term\",\"\",\"Version 6.00\"\n");
	if (!termsSet->m_Name.IsEmpty())
	   fprintf(out,"\"%s\"\n",termsSet->m_Name);
	else
		fprintf(out,"\"Term\"\n");

	// multiline comments -> separate lines
	if (! termsSet->m_Comment.IsEmpty())
	{
			string commentString = termsSet->m_Comment.GetString();
			stringstream stream(commentString);
			string line;
			while(std::getline(stream, line)) 
			{
				char tempBuf[256];
				sprintf(tempBuf,"%s",line.c_str());
				stripCRLF(tempBuf);
				fprintf(out,"\"%s\"\n", tempBuf);
			}
	} else
	   fprintf(out,"\"\"\n");

	fprintf(out,"\"\"\n");  // end of header
	fprintf(out,"\"%d\",\"%d\"\n",termsSet->m_StartDate.GetMonth(), termsSet->m_StartDate.GetDay());
	fprintf(out,"\"%3.1f\"\n", termsSet->m_Coeff1);
	fprintf(out,"\"%6.4f\"\n",termsSet->m_Coeff2);

	CTermDatesSet dateSet(fpSet->m_pDatabase);
	dateSet.m_strFilter.Format("[TermID] = %d", tID);
	dateSet.m_strSort.Format("[Year],[Date]");
	dateSet.Open();
	
	while(!dateSet.IsEOF())
	{
		if (! dateSet.m_Comment.IsEmpty())
		{
			if (dateSet.m_Date.GetStatus()==dateSet.m_Date.valid)
				fprintf(out,"\"%d/%d/%d\",\"%s\"\n",
					dateSet.m_Date.GetMonth(), 
					dateSet.m_Date.GetDay(),
					dateSet.m_Year,
					dateSet.m_Comment);

			else
				fprintf(out,"\"%d/%d/%d\",\"%s\"\n",
					termsSet->m_StartDate.GetMonth(), 
					termsSet->m_StartDate.GetDay(),
					dateSet.m_Year,
					dateSet.m_Comment);
		}
		else 
		{ // no comment
			if (dateSet.m_Date.GetStatus()==dateSet.m_Date.valid)
				fprintf(out,"\"%d/%d/%d\"\n",
					dateSet.m_Date.GetMonth(), 
					dateSet.m_Date.GetDay(),
					dateSet.m_Year);

			else

				fprintf(out,"\"%d/%d/%d\"\n",
					termsSet->m_StartDate.GetMonth(), 
					termsSet->m_StartDate.GetDay(),
					dateSet.m_Year);
		}
		dateSet.MoveNext();
		count ++;
	}
	dateSet.Close();
	
	
	termsSet->Close();
	

	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("Term records export Complete!\n%ld records written to\n%s... ", count, outFileName);
	AfxMessageBox(msg);

	

	return;

}


	