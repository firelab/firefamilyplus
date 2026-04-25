/***********************************************
	Ultimate Grid 97
	Copyright 1994 - 1997 Dundas Software Ltd.

	Class 
		CExtCheckBox
	Purpose
		checkbox cell type class
	Instructions
		- The checkbox changes state when clicked
		  with the left mouse button (over the check
		  box) or the space bar
		-The checkbox type always uses the same
		 id number since it is a built in type.
			ID Number: UGCT_CHECKBOX
		-There are several extended styles for this type
			UGCT_CHECKBOXFLAT
			UGCT_CHECKBOXCROSS
			UGCT_CHECKBOX3DRECESS
			UGCT_CHECKBOX3DRAISED
			UGCT_CHECKBOXCHECKMARK			
************************************************/
#include "pch.h"
#include "ExtCheckBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
//	

CExtCheckBox::CExtCheckBox(){

	m_canTextEdit	= FALSE;
	m_canOverLap	= FALSE;

	Lightpen.CreatePen(PS_SOLID,1,RGB(225,225,225));
	Darkpen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
	Facepen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));

	m_hbrDither     = NULL;
	HBITMAP hbmGray = CreateDitherBitmap();
	if( hbmGray != NULL )
	{
		ASSERT( m_hbrDither == NULL );
		m_hbrDither = ::CreatePatternBrush( hbmGray );
		::DeleteObject( (HGDIOBJ)hbmGray );
	}
}

//////////////////////////////////////////////////////////////////////
//	

CExtCheckBox::~CExtCheckBox()
{
	if( m_hbrDither != NULL )
	{
		::DeleteObject( (HGDIOBJ)m_hbrDither );
	}
}

//////////////////////////////////////////////////////////////////////
//	

LPCTSTR CExtCheckBox::GetName()
{
	return _T("Standard CheckBox Type");
}

//////////////////////////////////////////////////////////////////////
//	

LPCUGID CExtCheckBox::GetUGID()
{
	static const UGID ugid = { 0x93aab8d0, 0xf749, 0x11d0, 
							{ 0x9c, 0x7f, 0x0, 0x80, 0xc8, 
							0x3f, 0x71, 0x2f } };

	return &ugid;
}

//////////////////////////////////////////////////////////////////////
//	

BOOL CExtCheckBox::OnDClicked(int col,long row,RECT *rect,POINT *point)
{
	return OnLClicked(col,row,1,rect,point);
}

//////////////////////////////////////////////////////////////////////
//	

BOOL CExtCheckBox::OnLClicked(int col,long row,int updn,RECT *rect,POINT *point)
{
	int top,height;
	CRect tempRect;
	m_ctrl->GetCellIndirect( col, row, &m_cell );
	
	if ( m_cell.GetReadOnly() == TRUE )
	{	// cell is read only
		return FALSE;
	}

	// calculate height of the rect
	height = rect->bottom - rect->top - 6;
	if( height > 12 )
		height = 12;
	top = (rect->bottom - rect->top - height) /2;

	if (!( m_cell.GetCellTypeEx() & UGCT_CHECKBOXUSEALIGN ))
	{
		// no alignment was set, draw normally
		if(point->x < rect->left+3 || point->x > rect->left+3+height)
			return FALSE;
		if(point->y < rect->top+top || point->y > rect->top+top+height)
			return FALSE;
	}
	else
	{
		tempRect = *rect;
		AdjustRect ( &tempRect, &m_cell, height, top );

		if(!tempRect.PtInRect ( *point ))
			return FALSE;
	}

	if( !updn )
	{
		int style = 0;
		if(m_cell.IsPropertySet(UGCELL_CELLTYPEEX_SET))
			style = m_cell.GetCellTypeEx();

		int val;
		val = (int)m_cell.GetNumber();
		m_cell.SetNumber( (val + 1) % ((style & UGCT_CHECKBOX3STATE) ? 3 : 2) );
	
		m_ctrl->SetCell(col,row,&m_cell);
		m_ctrl->RedrawCell(col,row);

		
		m_ctrl->OnCellTypeNotify(m_ID,col,row,UGCT_CHECKBOXSET,(long)m_cell.GetNumber());
		
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
//	

BOOL CExtCheckBox::OnCharDown(int col,long row,UINT *vcKey)
{
	m_ctrl->GetCellIndirect( col, row, &m_cell );

	if ( *vcKey == VK_SPACE && m_cell.GetReadOnly() == FALSE )
	{	
		int col = m_ctrl->GetCurrentCol();
		long row= m_ctrl->GetCurrentRow();

		m_ctrl->GetCell( col, row, &m_cell );
	
		if( m_cell.IsPropertySet( UGCELL_READONLY_SET ))
			if( m_cell.GetReadOnly())
				return FALSE;
				
		int style = 0;
		if(m_cell.IsPropertySet(UGCELL_CELLTYPEEX_SET))
			style = m_cell.GetCellTypeEx();

		int val;
		val = (int)m_cell.GetNumber();
		m_cell.SetNumber( (val = (val + 1) % ((style & UGCT_CHECKBOX3STATE) ? 3 : 2)) );

		m_ctrl->SetCell(col,row,&m_cell);
		m_ctrl->RedrawCell(col,row);

		//notify the user that the checkbox was checked
		m_ctrl->OnCellTypeNotify(m_ID,col,row,UGCT_CHECKBOXSET,(long)val);
				
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
//	

void CExtCheckBox::OnDraw(CDC *dc,RECT *rect,int col,long row,CUGCell *cell,
							int selected,int current)
{
	// draw border of the cell using build-in routine
	DrawBorder( dc, rect, rect, cell );
	
	int right = rect->right,
		left = rect->left,
		top,
		height,
		margin = 3,
		squareSize = 12;
	RECT checkrect;
	CPen * oldpen;

	int style = 0;
	if( cell->IsPropertySet( UGCELL_CELLTYPEEX_SET ))
		style = cell->GetCellTypeEx();

	height = rect->bottom - rect->top - ( margin * 2 );
	if( height > squareSize )
		height = squareSize;
	top = ( rect->bottom - rect->top - height ) / 2;

	//find the checkbox co-ords
	if ( !( style & UGCT_CHECKBOXUSEALIGN ))
	{
		// set co-ordinates of the check box
		checkrect.left	= rect->left + margin;
		checkrect.top	= rect->top + top;
		checkrect.right = checkrect.left + height;
		checkrect.bottom= checkrect.top + height;
		// adjust the rectangle
		// rect->right		= rect->left + (margin * 2) + height;
	}
	else
	{
		checkrect = *rect;
		// calculate possition and set of the check box
		AdjustRect( &checkrect, cell, height, top );
	}

	//*** draw the background ***
	if ( selected || ( current && m_ctrl->m_GI->m_currentCellMode & 2 ))
		dc->SetBkColor( cell->GetHBackColor() );
	else
		dc->SetBkColor( cell->GetBackColor() );

	dc->ExtTextOut( left, top, ETO_OPAQUE, rect, _T(""), 0, NULL );

	//*** draw the checkbox ***
	if( height >= ( margin * 2 ) )
	{
		//draw a 3D Recessed check box
		if( style & UGCT_CHECKBOX3DRECESS )
		{	
			oldpen = (CPen*)dc->SelectObject((CPen*)&Darkpen);
			dc->MoveTo(checkrect.left,checkrect.bottom);
			dc->LineTo(checkrect.left,checkrect.top);
			dc->LineTo(checkrect.right,checkrect.top);
			dc->SelectObject(&Lightpen);
			dc->LineTo(checkrect.right,checkrect.bottom);
			dc->LineTo(checkrect.left,checkrect.bottom);
			checkrect.top++;
			checkrect.left++;
			checkrect.right--;
			checkrect.bottom--;
			dc->SelectObject(&Facepen);
			dc->MoveTo(checkrect.left,checkrect.bottom);
			dc->LineTo(checkrect.right,checkrect.bottom);
			dc->LineTo(checkrect.right,checkrect.top);
			dc->SelectObject(GetStockObject(BLACK_PEN));
			dc->LineTo(checkrect.left,checkrect.top);
			dc->LineTo(checkrect.left,checkrect.bottom);
			dc->SelectObject(oldpen);

			checkrect.top++;
			checkrect.left++;
			if( cell->GetNumber() > 1 )
			{
				FillDitheredRect( dc, checkrect );
			}
			else
			{
				dc->SetBkColor(GetSysColor(COLOR_WINDOW));
				dc->ExtTextOut(left,top,ETO_OPAQUE,&checkrect,_T(""),0,NULL);
			}
		}
		//draw a 3D Raised check box
		else if( style & UGCT_CHECKBOX3DRAISED )
		{	
			oldpen = (CPen*)dc->SelectObject((CPen*)&Lightpen);
			dc->MoveTo(checkrect.left,checkrect.bottom);
			dc->LineTo(checkrect.left,checkrect.top);
			dc->LineTo(checkrect.right,checkrect.top);
			dc->SelectObject(GetStockObject(BLACK_PEN));
			dc->LineTo(checkrect.right,checkrect.bottom);
			dc->LineTo(checkrect.left,checkrect.bottom);
			checkrect.top++;
			checkrect.left++;
			checkrect.right--;
			checkrect.bottom--;
			dc->SelectObject(&Darkpen);
			dc->MoveTo(checkrect.left,checkrect.bottom);
			dc->LineTo(checkrect.right,checkrect.bottom);
			dc->LineTo(checkrect.right,checkrect.top);
			dc->SelectObject(&Facepen);
			dc->LineTo(checkrect.left,checkrect.top);
			dc->LineTo(checkrect.left,checkrect.bottom);
			dc->SelectObject(oldpen);

			checkrect.top++;
			checkrect.left++;
			if( cell->GetNumber() > 1 )
			{
				FillDitheredRect( dc, checkrect );
			}
			else
			{
				dc->SetBkColor(GetSysColor(COLOR_WINDOW));
				dc->ExtTextOut(left,top,ETO_OPAQUE,&checkrect,_T(""),0,NULL);
			}
		}
		//draw a plain check box
		else
		{	
			oldpen = (CPen*)dc->SelectObject((CPen*)CPen::FromHandle((HPEN)GetStockObject(BLACK_PEN)));
			dc->MoveTo(checkrect.left,checkrect.top);
			dc->LineTo(checkrect.right,checkrect.top);
			dc->LineTo(checkrect.right,checkrect.bottom);
			dc->LineTo(checkrect.left,checkrect.bottom);
			dc->LineTo(checkrect.left,checkrect.top);
			dc->SelectObject(oldpen);
				
			checkrect.left++;
			checkrect.top++;
			if( cell->GetNumber() > 1 )
			{
				FillDitheredRect( dc, checkrect );
			}
			else
			{
				dc->SetBkColor(GetSysColor(COLOR_WINDOW));
				dc->ExtTextOut(left,top,ETO_OPAQUE,&checkrect,_T(""),0,NULL);
			}
		}

		if ( cell->GetReadOnly() == TRUE )
		{	// cell is set to read only
			FillDitheredRect( dc, checkrect );
		}

		//draw the check
		if( cell->GetNumber() > 0 )
		{
			//draw a check mark
			if(style&UGCT_CHECKBOXCHECKMARK)
			{ 
				if( cell->GetNumber() > 1 )
				{
					oldpen = (CPen*)dc->SelectObject((CPen*)&Darkpen);
				}
				else
				{
					oldpen = (CPen*)dc->SelectObject((CPen*)CPen::FromHandle((HPEN)GetStockObject(BLACK_PEN)));
				}
				dc->MoveTo(checkrect.left+2,checkrect.bottom-4);
				dc->LineTo(checkrect.left+4,checkrect.bottom-2);
				dc->LineTo(checkrect.right+3,checkrect.top-1);
				if(height >9){
					dc->MoveTo(checkrect.left+2,checkrect.bottom-5);
					dc->LineTo(checkrect.left+4,checkrect.bottom-3);
					dc->LineTo(checkrect.right+3,checkrect.top-2);
					dc->MoveTo(checkrect.left+5,checkrect.bottom-2);
					dc->LineTo(checkrect.right+4,checkrect.top-1);
					dc->MoveTo(checkrect.left+2,checkrect.bottom-6);
					dc->LineTo(checkrect.left+5,checkrect.bottom-3);
				}
				dc->SelectObject(oldpen);
			}
			//draw the X mark
			else
			{
				if( cell->GetNumber() > 1 )
				{
					oldpen = (CPen*)dc->SelectObject((CPen*)&Darkpen);
				}
				else
				{
					oldpen = (CPen*)dc->SelectObject((CPen*)CPen::FromHandle((HPEN)GetStockObject(BLACK_PEN)));
				}
				checkrect.left++;
				checkrect.top++;
				checkrect.right-=2;
				checkrect.bottom-=2;
				dc->MoveTo(checkrect.left,checkrect.top);
				dc->LineTo(checkrect.right+1,checkrect.bottom+1);
				dc->MoveTo(checkrect.left,checkrect.bottom);
				dc->LineTo(checkrect.right+1,checkrect.top-1);
				if(height >9)
				{
					dc->MoveTo(checkrect.left+1,checkrect.top);
					dc->LineTo(checkrect.right+1,checkrect.bottom);
					dc->MoveTo(checkrect.left,checkrect.bottom-1);
					dc->LineTo(checkrect.right,checkrect.top-1);
					dc->MoveTo(checkrect.left,checkrect.top+1);
					dc->LineTo(checkrect.right,checkrect.bottom+1);
					dc->MoveTo(checkrect.left+1,checkrect.bottom);
					dc->LineTo(checkrect.right+1,checkrect.top);
				}
				dc->SelectObject(oldpen);
			}
		}
	}
	
	if (!( style & UGCT_CHECKBOXUSEALIGN ))
	{
		// adjust text rect
		rect->left += (( margin * 2 ) + height );
		rect->right = right;
		m_drawLabelText = TRUE;
		// draw the text using the default drawing routine
		CUGCellType::DrawText(dc,rect,0,col,row,cell,selected,current);
	}	

	// restore orriginal value of the left side
	rect->left = left;
}

//////////////////////////////////////////////////////////////////////
//	

int CExtCheckBox::AdjustRect( RECT *rect, CUGCell *cell, int height, int top, int margin, int squareSize )
{
	int alignment = cell->GetAlignment ();

	if( alignment & UG_ALIGNCENTER )
	{
		rect->left += (int)(( rect->right - rect->left ) / 2 - ( squareSize / 2 ));
	}
	else if( alignment & UG_ALIGNRIGHT )
	{
		rect->left = rect->right - squareSize - ( margin * 2 );
	}
	else
	{	// align left
		rect->left = rect->left + margin;
	}	

	if( alignment & UG_ALIGNVCENTER )
	{
		rect->top = rect->top + top;
	}
	else if( alignment & UG_ALIGNBOTTOM )
	{
		rect->top = rect->bottom - squareSize - ( margin * 2 );
	}
	else
	{	// align top
		rect->top = rect->top + margin;
	}

	// set remaining co-ordinates of the check box
	rect->right = rect->left + height;
	rect->bottom= rect->top + height;

	return 0;
}

//////////////////////////////////////////////////////////////////////
//	

HBITMAP CExtCheckBox::CreateDitherBitmap()
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD      bmiColors[16];
	}bmi;
	memset( &bmi, 0, sizeof( bmi ) );

	bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = ::GetSysColor( COLOR_BTNFACE );
	bmi.bmiColors[0].rgbBlue = GetBValue( clr );
	bmi.bmiColors[0].rgbGreen = GetGValue( clr );
	bmi.bmiColors[0].rgbRed = GetRValue( clr );

	clr = ::GetSysColor( COLOR_BTNHIGHLIGHT );
	bmi.bmiColors[1].rgbBlue = GetBValue( clr );
	bmi.bmiColors[1].rgbGreen = GetGValue( clr );
	bmi.bmiColors[1].rgbRed = GetRValue( clr );

	// initialize the brushes
	long patGray[8];
	for( int i = 0 ; i < 8 ; i++ )
	{
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;
	}

	HDC hDC = ::GetDC( NULL );
	HBITMAP hbm = ::CreateDIBitmap( hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS );
	::ReleaseDC( NULL, hDC );

	return hbm;
}

//////////////////////////////////////////////////////////////////////
//	

void CExtCheckBox::FillDitheredRect( CDC* pDC, const CRect& rect )
{
	ASSERT_VALID(pDC);

	if( m_hbrDither == NULL )
	{
		::FillRect( pDC->GetSafeHdc (), &rect, (HBRUSH) (COLOR_WINDOW+1) );
	}
	else
	{
		::FillRect( pDC->GetSafeHdc (), &rect, m_hbrDither );
	}
}

