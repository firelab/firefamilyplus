/*************************************************************************
				Class Declaration : CUGEditBase
**************************************************************************
	Source file : UGEditBase.cpp
	Header file : UGEditBase.h
	Copyright © Dundas Software Ltd. 1994 - 2002, All Rights Reserved

    Purpose
		This class will automatically size the font of any cell using 
		this celltype so that all of the text will be visible.
*************************************************************************/

#ifndef _UGEditBase_H_
#define _UGEditBase_H_

#include "ugctrl.h"

//CUGEditBase Declaration
class UG_CLASS_DECL CUGEditBase : public CEdit
{
// Construction
public:
	CUGEditBase();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUGEditBase)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// based on user action destination cell needs to be set 
	int m_continueCol;
	long m_continueRow; 
	BOOL m_cancel;
	BOOL m_continueFlag;

	// member variable that keeps track of the last pressed key
	UINT m_lastKey;
public:

	// Grid will try to set this pointer to itself
	CUGCtrl * m_ctrl;    

	virtual ~CUGEditBase();

	UINT  GetLastKey();
	// Generated message map functions
protected:
	//{{AFX_MSG(CUGEditBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //#ifndef _UGEditBase_H_
