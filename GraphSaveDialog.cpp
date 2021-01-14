// GraphSaveDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "GraphSaveDialog.h"
#include <atlimage.h>
#include ".\graphsavedialog.h"

extern CFireplusApp theApp;

BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, PSTR pszFileName) 
{ 
	BOOL        bSuccess = TRUE; 
	CWaitCursor wait;
	CImage image;
	image.Attach(hBmp);
	HRESULT hResult = image.Save(pszFileName);
	if (FAILED(hResult)) 
	{
		CString fmt;
		fmt.Format("Save image failed.");
		AfxMessageBox(fmt);
		bSuccess = FALSE;
	}
	return bSuccess; 
} 


// CGraphSaveDialog dialog

IMPLEMENT_DYNAMIC(CGraphSaveDialog, CDialog)
CGraphSaveDialog::CGraphSaveDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphSaveDialog::IDD, pParent)
{
}

CGraphSaveDialog::~CGraphSaveDialog()
{
}

void CGraphSaveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPINGWIDTH, m_spinGWidth);
	DDX_Control(pDX, IDC_SPINGHEIGHT, m_spinGHeight);
}


BEGIN_MESSAGE_MAP(CGraphSaveDialog, CDialog)
	//ON_BN_CLICKED(IDC_GSAVE, OnGsave)
END_MESSAGE_MAP()


// CGraphSaveDialog message handlers
BOOL CGraphSaveDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	width = 640;
	height = 480;
	m_spinGWidth.SetRange(10, 10000);
	m_spinGHeight.SetRange(10, 10000);
	m_spinGWidth.SetPos(width);
	m_spinGHeight.SetPos(height);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CGraphSaveDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	width = m_spinGWidth.GetPos();
	height = m_spinGHeight.GetPos();
	CDialog::OnOK();
}
