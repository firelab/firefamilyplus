// DPReportView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DPReportView.h"
#include "DPDoc.h"
#include "math.h"
#include "UserVarSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDPReportView

IMPLEMENT_DYNCREATE(CDPReportView, CEditView)

CDPReportView::CDPReportView()
{
	doRound = true;
}

CDPReportView::~CDPReportView()
{
}


BEGIN_MESSAGE_MAP(CDPReportView, CEditView)
	//{{AFX_MSG_MAP(CDPReportView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPReportView drawing

void CDPReportView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDPReportView diagnostics

#ifdef _DEBUG
void CDPReportView::AssertValid() const
{
	CEditView::AssertValid();
}

void CDPReportView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDPReportView message handlers

void CDPReportView::OnInitialUpdate() 
{
	CEditView::OnInitialUpdate();
	CDPDoc* pDoc = (CDPDoc *)GetDocument();
	doRound = true;
	if(pDoc->fires->varID > STATICVARIDS)
	{
		CUserVarSet uvs(pDoc->fires->pDB);
		uvs.m_strFilter.Format("[VarID] = %d", pDoc->fires->varID);
		uvs.Open();
		if(!uvs.IsBOF() && !uvs.IsEOF())
		{
			if(uvs.m_Decimals > 0)
				doRound = false;
		}
		uvs.Close();
	}
	else if(pDoc->fires->varID == 9)//precip
		doRound = false;



	fixedFont.CreatePointFont(85, "courier new");
	GetEditCtrl().SetFont(&fixedFont);
	CString temp;
	header = "FireFamily Plus Decision Points\r\n";
	temp.Format("%s\r\nVariable: %s\r\n\r\n", pDoc->fires->staStr,
			pDoc->fires->varName);
	header += temp;
	temp.Format(" Time Frame: %d/%d - %d/%d\r\n",
			pDoc->fires->start.GetMonth(),
			pDoc->fires->start.GetDay(),
			pDoc->fires->end.GetMonth(),
			pDoc->fires->end.GetDay());
	header += temp;
	temp.Format(" Data Years: %d - %d\r\n",
			pDoc->fires->start.GetYear(),
			pDoc->fires->end.GetYear());
	header += temp;
		switch(pDoc->fires->cause)
		{
		case 0://all pDoc->fires
			header += " Cause = All\r\n";
			break;
		case 1://lightning
			header += " Cause = Lightning\r\n";
			break;
		case 2://human
			header += " Cause = Human\r\n";
			break;
		}
	temp.Format(" Large Fire Day = %d acres\r\n"
			" Multiple Fire Day = %d fires\r\n\r\n",
			pDoc->fires->lfAcres, pDoc->fires->mfdFires);
	header += temp;
	if(strncmp(pDoc->fires->staStr, "Station", 7) == 0)
	{
			//a single station
		temp.Format(" %s - %s", pDoc->fires->staStr, pDoc->fires->strings[0]);
		header += temp;
	}
	else
	{
		temp.Format(" Stations in %s:\r\n", pDoc->fires->staStr);
		header += temp;
		for(int i = 0; i < pDoc->fires->nStrings; i++)
		{
			temp.Format(" %s", pDoc->fires->strings[i]);
			header += temp;
		}
	}
	header += "\r\n";
	header += 
		"                    Percentages Based On Current Class Definitions             |   Model Probabilities (%)\r\n";
	header += 
		"Cls     Index    All-Days     Fire-Days     Large Fire-Days    Multi-Fire-Days |  Fire      Large    Multi\r\n";
	header += 
		" #      Range      #   %      # %FD %AD      # %LFD %FD %AD     # %MFD %FD %AD |  Day       F-Day    F-Day\r\n";
	header +=
		"-----------------------*----------*--------------*------------------*----------   ------------------------\r\n";
	GetEditCtrl().SetWindowText(header);
}

void CDPReportView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CString newStr, tmp;
	newStr = header;
	CDPDoc *pDoc = (CDPDoc *)GetDocument();
	double nextVal = 0.0, maxVal = 0.0;
	if(pDoc->fires->nv > 0)
	{
		if(!pDoc->goUp)
			maxVal = pDoc->fires->allVals[pDoc->fires->nv - 1];
	}
	else 
		maxVal = 1000.0;
	for(int i = 0; i < pDoc->nDP; i++)
	{
		nextVal = (i < pDoc->nDP - 1) ? pDoc->lowers[i + 1] : maxVal;
		if(doRound)
		{
			nextVal = Round(nextVal);
			nextVal -= 1.0;
			tmp.Format(" %d    %3.0f-%3.0f  %5ld %3.0f  %5ld %3.0f %3.0f  %5ld %3.0f %3.0f %3.0f  %5ld %3.0f %3.0f %3.0f    %3.0f-%3.0f  %3.0f-%3.0f  %3.0f-%3.0f\r\n",
				i + 1, Round(pDoc->lowers[i]), nextVal,
				pDoc->cd[i], pDoc->nd == 0 ? 0.0 : ((double)pDoc->cd[i]) / ((double)pDoc->nd) * 100.0,
				pDoc->cfd[i], pDoc->nfd == 0 ? 0.0 : ((double)pDoc->cfd[i]) / ((double)pDoc->nfd) * 100.0, pDoc->cd[i] == 0 ? 0.0 : ((double)pDoc->cfd[i]) / ((double)pDoc->cd[i]) * 100.0,
				pDoc->clfd[i], pDoc->nlfd == 0 ? 0.0 : ((double)pDoc->clfd[i]) / ((double)pDoc->nlfd) * 100.0, pDoc->cfd[i] == 0 ? 0.0 : ((double)pDoc->clfd[i]) / ((double)pDoc->cfd[i]) * 100.0, pDoc->cd[i] == 0 ? 0.0 : ((double)pDoc->clfd[i]) / ((double)pDoc->cd[i]) * 100.0,
				pDoc->cmfd[i], pDoc->nmfd == 0 ? 0.0 : ((double)pDoc->cmfd[i]) / ((double)pDoc->nmfd) * 100.0, pDoc->cfd[i] == 0 ? 0.0 : ((double)pDoc->cmfd[i]) / ((double)pDoc->cfd[i]) * 100.0, pDoc->cd[i] == 0 ? 0.0 : ((double)pDoc->cmfd[i]) / ((double)pDoc->cd[i]) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0fd + (-1.0 * pDoc->fires->b1fd) * pDoc->lowers[i])) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0fd + (-1.0 * pDoc->fires->b1fd) * nextVal)) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0lfd + (-1.0 * pDoc->fires->b1lfd) * pDoc->lowers[i])) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0lfd + (-1.0 * pDoc->fires->b1lfd) * nextVal)) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0mfd + (-1.0 * pDoc->fires->b1mfd) * pDoc->lowers[i])) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0mfd + (-1.0 * pDoc->fires->b1mfd) * nextVal)) * 100.0);
		}
		else
		{
			nextVal -= 0.01;
			tmp.Format(" %d%5.2f-%5.2f  %5ld %3.0f  %5ld %3.0f %3.0f  %5ld %3.0f %3.0f %3.0f  %5ld %3.0f %3.0f %3.0f    %3.0f-%3.0f  %3.0f-%3.0f  %3.0f-%3.0f\r\n",
				i + 1, pDoc->lowers[i], nextVal,
				pDoc->cd[i], pDoc->nd == 0 ? 0.0 : ((double)pDoc->cd[i]) / ((double)pDoc->nd) * 100.0,
				pDoc->cfd[i], pDoc->nfd == 0 ? 0.0 : ((double)pDoc->cfd[i]) / ((double)pDoc->nfd) * 100.0, pDoc->cd[i] == 0 ? 0.0 : ((double)pDoc->cfd[i]) / ((double)pDoc->cd[i]) * 100.0,
				pDoc->clfd[i], pDoc->nlfd == 0 ? 0.0 : ((double)pDoc->clfd[i]) / ((double)pDoc->nlfd) * 100.0, pDoc->cfd[i] == 0 ? 0.0 : ((double)pDoc->clfd[i]) / ((double)pDoc->cfd[i]) * 100.0, pDoc->cd[i] == 0 ? 0.0 : ((double)pDoc->clfd[i]) / ((double)pDoc->cd[i]) * 100.0,
				pDoc->cmfd[i], pDoc->nmfd == 0 ? 0.0 : ((double)pDoc->cmfd[i]) / ((double)pDoc->nmfd) * 100.0, pDoc->cfd[i] == 0 ? 0.0 : ((double)pDoc->cmfd[i]) / ((double)pDoc->cfd[i]) * 100.0, pDoc->cd[i] == 0 ? 0.0 : ((double)pDoc->cmfd[i]) / ((double)pDoc->cd[i]) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0fd + (-1.0 * pDoc->fires->b1fd) * pDoc->lowers[i])) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0fd + (-1.0 * pDoc->fires->b1fd) * nextVal)) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0lfd + (-1.0 * pDoc->fires->b1lfd) * pDoc->lowers[i])) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0lfd + (-1.0 * pDoc->fires->b1lfd) * nextVal)) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0mfd + (-1.0 * pDoc->fires->b1mfd) * pDoc->lowers[i])) * 100.0,
				1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0mfd + (-1.0 * pDoc->fires->b1mfd) * nextVal)) * 100.0);
		}
		newStr += tmp;
	}
	newStr +=
		"-----------------------*----------*--------------*------------------*----------   ------------------------\r\n";
	tmp.Format("              %6ld     %6ld         %6ld             %6ld\r\n",
		pDoc->nd, pDoc->nfd, pDoc->nlfd, pDoc->nmfd);
	newStr += tmp;
	newStr += "Values in columns denoted by an * are displayed in the bar charts.\r\n";
	int pos = GetEditCtrl().GetFirstVisibleLine();
	GetEditCtrl().SetWindowText(newStr);
	if(pos > 0)
		GetEditCtrl().LineScroll(pos);
}


void CDPReportView::SaveReport(CString fName)
{
	CWaitCursor wait;
	FILE *stream;
	stream = fopen(fName, "wb");
	if(!stream)
	{
		CString msg;
		msg.Format("Error: Could not create file:\n%s\n", fName);
		AfxMessageBox(msg);
		return;
	}
	CString txt;
	GetEditCtrl().GetWindowText(txt);
	fwrite( txt, sizeof( char ), txt.GetLength(), stream );
	fclose(stream);
}
