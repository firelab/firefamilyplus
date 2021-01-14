// DelRecsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DelRecsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelRecsDialog dialog


CDelRecsDialog::CDelRecsDialog(CWnd* pParent, CRecordset *_records, long _maxRec, long _curRec, BOOL doDelete)
	: CDialog(CDelRecsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelRecsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	records = _records;
	maxRec = _maxRec;
	m_start = m_end = curRec = _curRec;
	m_performDeletion = doDelete;
}


void CDelRecsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelRecsDialog)
	//DDX_Control(pDX, IDC_END, endCtl);
	//DDX_Control(pDX, IDC_START, startCtl);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDITSTART, m_editStart);
	DDX_Control(pDX, IDC_SPINSTART, m_spinStart);
	DDX_Control(pDX, IDC_EDITEND, m_editEnd);
	DDX_Control(pDX, IDC_SPINEND, m_spinEnd);
}


BEGIN_MESSAGE_MAP(CDelRecsDialog, CDialog)
	//{{AFX_MSG_MAP(CDelRecsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelRecsDialog message handlers

BOOL CDelRecsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(maxRec > 0)
	{
		m_spinStart.SetDecimalPlaces (0);
		m_spinStart.SetTrimTrailingZeros (TRUE);
		if(maxRec > 1)
			m_spinStart.SetRangeAndDelta (1, maxRec, 1);		
		m_spinStart.SetBuddy(&m_editStart);
		//m_spinStart.SetPos(curRec);
		m_spinEnd.SetDecimalPlaces (0);
		m_spinEnd.SetTrimTrailingZeros (TRUE);
		if(maxRec > 1)
			m_spinEnd.SetRangeAndDelta (1, maxRec, 1);		
		m_spinEnd.SetBuddy(&m_editEnd);
		//m_spinEnd.SetPos(curRec);
		if(maxRec <= 1)
		{
			m_spinStart.EnableWindow(FALSE);
			m_editStart.EnableWindow(FALSE);
			m_spinEnd.EnableWindow(FALSE);
			m_editEnd.EnableWindow(FALSE);
		}
	}
	if(curRec > 0)
	{
		m_spinStart.SetPos(curRec);
		m_spinEnd.SetPos(curRec);
		//startCtl.SetValueInteger(curRec);
		//endCtl.SetValueInteger(curRec);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDelRecsDialog::OnOK() 
{
	long start, end;
	m_start = start = m_spinStart.GetPos();//startCtl.GetValueInteger();
	m_end = end = m_spinEnd.GetPos();//endCtl.GetValueInteger();
	if(end < start)
	{
		AfxMessageBox("ERROR: End record can not be less than start record.");
		return;
	}
	if(end > maxRec)
	{
		m_end = end = maxRec;
	}
	CString msg;
	msg.Format("Permanently Delete records %ld thru %ld?\nWARNING: This action is permanent!",
		start, end);
	if(AfxMessageBox(msg, MB_OKCANCEL) != IDOK)
		return;
	CWaitCursor wait;
	//records->SetAbsolutePosition(start - 1);
	if(this->m_performDeletion)
	{
		records->SetAbsolutePosition(start);
		records->Delete();
		for(int r = 0; r < end - start && !records->IsEOF(); r++)
		{
			records->MoveNext();
			if(!records->IsEOF())
				records->Delete();
		}
	}
	CDialog::OnOK();
}

