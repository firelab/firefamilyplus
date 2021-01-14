/*************************************************************************
				Class Implementation : CUGEditBase
**************************************************************************
	Source file : UGEditBase.cpp
	Copyright © Dundas Software Ltd. 1994 - 2002, All Rights Reserved
*************************************************************************/

#include "stdafx.h"
#include "ugctrl.h"
//#include "UGEditBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/***************************************************
CUGEditBase - Constructor
****************************************************/
CUGEditBase::CUGEditBase() :
	m_continueCol(0),
	m_continueRow(0),
	m_continueFlag(0),
	m_cancel(0),
	m_lastKey(0),
	m_ctrl(NULL)
{
}

/***************************************************
~CUGEditBase - Destructor
****************************************************/
CUGEditBase::~CUGEditBase()
{
}

/***************************************************
MessageMap
****************************************************/
BEGIN_MESSAGE_MAP(CUGEditBase, CEdit)
	//{{AFX_MSG_MAP(CUGEditBase)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/***************************************************
WindowProc
	Provides all of the default functionality that
	any CEdit based Ultimate Grid edit control must
	have. 
	
	Most CEdit derived edit controls can be easily
	used in Ultimate Grid by just changing the base 
	class. There is generally no need to change any
	functions calls from CEdit::xxx to CUGEditBase::xxx
	
	All of the messages are put here instead of 
	in a Message Map, so that the so that they are 
	properly called, without having to switch any 
	CEdit::xxx calls over to CUGEditBase::xxx calls

	The only stipulation is that the CEdit derived
	class cannot override the WindowsProc. If it
	does, then the base funtionality will be rendered
	useless. In this case, the code below may be copied
	into the controls WindowsProc (also the member 
	variables will need to be copied over and initialized
	as well).
Params
	See CWnd::WindowsProc
Return
	See CWnd::WindowsProc
****************************************************/
LRESULT CUGEditBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	switch(message){

		//***** WMCREATE *****
		case WM_CREATE:{
			
			if(m_ctrl == NULL)
				m_ctrl = (CUGCtrl*)GetParent();
			
			break;
		}
		//***** WM_SETFOCUS *****
		case WM_SETFOCUS:{
			
			if(m_ctrl == NULL)
				m_ctrl = (CUGCtrl*)GetParent();

			CEdit::WindowProc(message, wParam, lParam);

			m_lastKey = 0;
			m_cancel = FALSE;
			m_continueFlag = FALSE;

			SetSel(GetWindowTextLength(),0);

			if(m_ctrl->m_editCell.GetReadOnly())
				SetReadOnly(TRUE);

			return 0;
		}
		//***** WM_KILLFOCUS *****
		case WM_KILLFOCUS:{
			
			SetReadOnly(FALSE);
			
			CEdit::WindowProc(message, wParam, lParam);
			
			CWnd* pNewWnd = CWnd::FromHandle((HWND)wParam);

			CEdit::OnKillFocus(pNewWnd);

			if( m_ctrl->m_GI->m_bCancelMode != FALSE )
			{
				if(pNewWnd->GetSafeHwnd() != NULL)
				{
					if(pNewWnd != m_ctrl && pNewWnd->GetParent() != m_ctrl)
						m_cancel = TRUE;
				}
				else
					m_cancel = TRUE;
			}

			if( pNewWnd != m_ctrl )
				m_ctrl->OnKillFocus( UG_GRID, pNewWnd );

			if(pNewWnd->GetSafeHwnd() != NULL)
			{
				if( pNewWnd == m_ctrl )
					::SetFocus( m_ctrl->m_CUGGrid->GetSafeHwnd());
			}

			CString string;
			GetWindowText(string);

			if(m_ctrl->EditCtrlFinished(string,m_cancel,m_continueFlag,m_continueCol,m_continueRow) == FALSE)
			{
				m_ctrl->GotoCell(m_ctrl->m_editCol,m_ctrl->m_editRow);
				SetCapture();
				ReleaseCapture();
				SetFocus();
			}
			return 0;
		}
		//***** WM_KEYDOWN *****
		case WM_KEYDOWN:{

			m_lastKey = (UINT)wParam;

			switch(wParam){
				case VK_TAB:{

					if(GetKeyState(VK_SHIFT)<0){
						m_continueCol = m_ctrl->m_editCol -1;
						m_continueRow = m_ctrl->m_editRow;
					}
					else{
						m_continueCol = m_ctrl->m_editCol +1;
						m_continueRow = m_ctrl->m_editRow;
					}

					m_continueFlag = TRUE;
					m_ctrl->SetFocus();
					return 0;
				}
				case VK_RETURN:{

					if( GetKeyState(VK_CONTROL) < 0 &&
						m_ctrl->m_editCell.GetCellTypeEx() & UGCT_NORMALMULTILINE )
					{	// in mulitiline cells allow CTRL-RETURN to advance to a new line
						return 0;
					}

					m_continueCol = m_ctrl->m_editCol;
					m_continueRow = m_ctrl->m_editRow +1;
					m_continueFlag = TRUE;
					m_ctrl->SetFocus();

					return 0;
				}
				case VK_ESCAPE:{
					m_cancel = TRUE;
					m_continueFlag = FALSE;
					m_ctrl->SetFocus();
					return 0;
				}
				case VK_UP:{
					m_continueCol = m_ctrl->m_editCol;
					m_continueRow = m_ctrl->m_editRow-1;
					m_continueFlag = TRUE;
					m_ctrl->SetFocus();
					return 0;
				}
				case VK_DOWN:{
					m_continueCol = m_ctrl->m_editCol;
					m_continueRow = m_ctrl->m_editRow+1;
					m_continueFlag = TRUE;
					m_ctrl->SetFocus();
					return 0;
				}
				case VK_DELETE:{
					// The OnEditVerify should also called when user enters
					// the DELETE key.  ASCII representation of the DEL key is decimal 127
					UINT nChar = 127;
					int col = m_ctrl->GetCurrentCol();
					long row = m_ctrl->GetCurrentRow();

					// First the datasource has a look ...
					if(m_ctrl->m_GI->m_defDataSource->OnEditVerify(col,row,this,&nChar) == FALSE)
						return 0;
					// then the ctrl class
					if(m_ctrl->OnEditVerify(col,row,this,&nChar) == FALSE)
						return 0;
				}
			}

			break;
		}
		//***** WM_CHAR *****
		case WM_CHAR:{

			UINT nChar = (UINT)wParam;
			m_lastKey = nChar;

			if(nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE)
				return 0;

			// allow OnEditVerify a look at the char...
			int col = m_ctrl->GetCurrentCol();
			long row = m_ctrl->GetCurrentRow();
			
			// First the datasource has a look ...
			if(m_ctrl->m_GI->m_defDataSource->OnEditVerify(col,row,this,&nChar) == FALSE)
				return 0;

			// then the ctrl class
			if(m_ctrl->OnEditVerify(col,row,this,&nChar) == FALSE)
				return 0;

			// if the characted is allowed by both datasource and ctrl class
			// than set back any changes to the character that user could make
			wParam = (WPARAM)nChar;
			// and allow the edit control to process the new char.
			return CEdit::WindowProc(message, wParam, lParam);
		}
		//***** WM_GETDLGCODE *****
		case WM_GETDLGCODE:{

			return DLGC_WANTALLKEYS | DLGC_WANTARROWS;
		}
		//***** WM_MOUSEACTIVATE *****
		case WM_MOUSEACTIVATE:{
			
			return MA_ACTIVATE;	
		}
	}

	return CEdit::WindowProc(message, wParam, lParam);
}

/***************************************************
GetLastKey
	GetLastKey is handy if you need to know if
	ESC cancelled the edit.  Key state is not 
	saved (shift, ctrl etc)
Params:
	<none>
Returns:
	last key pressed
*****************************************************/
UINT CUGEditBase::GetLastKey()
{
	return m_lastKey;
}
