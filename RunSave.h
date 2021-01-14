// RunSave.h: interface for the CRunSave class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_RUNSAVE_H__94819D20_805A_11D2_BD5A_E62729E9273D__INCLUDED_)
#define AFX_RUNSAVE_H__94819D20_805A_11D2_BD5A_E62729E9273D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FirePlusSet.h"
#include "ExtCheckBox.h"
#include "afxwin.h"
#include "runsset.h"



class CRunSave  
{
public:
	
	CRunSave(CFireplusSet *fpSet);
	virtual ~CRunSave();
	int SaveGeneral(int runID, CString Description, CFireplusSet *fpSet);
	int SaveGeneral(int runID, CString Description, CFireplusSet *fpSet, CString *newDesc);
	int SaveClimateOptions(int runID, CFireplusSet *fpSet);
	int SaveWxStation(int runID, CFireplusSet *fpSet);
	int SaveFireOptions(int runID, CFireplusSet *fpSet);
	int SaveRunOption(int runID, short reportID, short row, CString Variable, CString Value, CFireplusSet *fpSet);

	int RestoreGeneral(int runID,  CFireplusSet *fpSet);
	int RestoreClimateOptions(int runID, CFireplusSet *fpSet);
	int RestoreWxStation(int runID, CFireplusSet *fpSet);
	int RestoreFireOptions(int runID, CFireplusSet *fpSet);

	int RestoreRunOption(int runID, short reportID, short row, CString Variable, CString *Value, CFireplusSet *fpSet);
	bool m_saveIsOn;
};

class CRunSavePromptDialog : public CDialog
{
public:
	

	CRunSavePromptDialog(CWnd* pParent = NULL, CFireplusSet *_fpSet = NULL,CString  *_Description = NULL);   // standard constructor
	
    CString *Description;
	
	enum { IDD = IDD_RUNSAVEPROMPTDIALOG };


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunLoadDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	
	
	
	virtual BOOL OnInitDialog();
	
	
	virtual void OnOK();
	
	//DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};






#endif // !defined(AFX_RUNSAVE_H__94819D20_805A_11D2_BD5A_E62729E9273D__INCLUDED_)
