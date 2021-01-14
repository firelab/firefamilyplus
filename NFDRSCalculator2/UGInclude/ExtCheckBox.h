/***********************************************
	Ultimate Grid 97
	Copyright 1994 - 1997 Dundas Software Ltd.

	Class 
		CExtCheckBox
	Purpose
		checkbox cell type class
	Details
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

			UGCT_CHECKBOXUSEALIGN	-- used to specify if the alignment should be concidered
						   the text will not be drawn then
************************************************/
#include "UGCtrl.h"

#ifndef _ExtCheckBox_H_
#define _ExtCheckBox_H_

// new centerring check type
#define	UGCT_CHECKBOXUSEALIGN 4
//new extended style, tri-state checkbox
//same as a check box cell, except that the box can be dimmed as well as checked 
//if it's not defined the behavior is as a regular checkbox cell
#define UGCT_CHECKBOX3STATE		BIT13

class CExtCheckBox: public CUGCellType
{
	CPen	Darkpen;
	CPen	Lightpen;
	CPen	Facepen;

	CUGCell m_cell;

	HBRUSH m_hbrDither;

public:
	int AdjustRect ( RECT *rect, CUGCell *cell, int height, int top, int margin = 3, int squareSize = 12 );

	CExtCheckBox();
	~CExtCheckBox();

	virtual LPCTSTR GetName();
	virtual LPCUGID GetUGID();

	virtual BOOL OnLClicked(int col,long row,int updn,RECT *rect,POINT *point);
	virtual BOOL OnDClicked(int col,long row,RECT *rect,POINT *point);
	virtual BOOL OnCharDown(int col,long row,UINT *vcKey);
	virtual void OnDraw(CDC *dc,RECT *rect,int col,long row,CUGCell *cell,
		int selected,int current);

	HBITMAP CreateDitherBitmap();
	void FillDitheredRect( CDC* pDC, const CRect& rect );
};

#endif //_ExtCheckBox_H_