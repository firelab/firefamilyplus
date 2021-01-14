// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "ColorButton.h"
#include ".\colorbutton.h"


// CColorButton

IMPLEMENT_DYNAMIC(CColorButton, CButton)
CColorButton::CColorButton()
{
	color = RGB(0,0,0);
	brush = CreateSolidBrush(color);
}

CColorButton::~CColorButton()
{
	if(brush)
		::DeleteObject(brush);
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnBnClicked)
END_MESSAGE_MAP()



// CColorButton message handlers


void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  UINT uStyle = DFCS_BUTTONPUSH;

   // This code only works with buttons.
   ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

   // If drawing selected, add the pushed style to DrawFrameControl.
   if (lpDrawItemStruct->itemState & ODS_SELECTED)
      uStyle |= DFCS_PUSHED;

   // Draw the button frame.
   ::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
      DFC_BUTTON, uStyle);

   // Get the button's text.
   //CString strText;
   //GetWindowText(strText);
   CRect rect = lpDrawItemStruct->rcItem;
   rect.DeflateRect(2, 2);
   ::FillRect(lpDrawItemStruct->hDC, &rect, brush);
   // Draw the button text using the text color red.
   //COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(255,0,0));
   //::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), 
      //&lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
   //::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
   
}

COLORREF CColorButton::GetColor(void)
{
	return color;
}

COLORREF CColorButton::SetColor(COLORREF _color)
{
	color = _color;
	if(brush)
		::DeleteObject(brush);
	brush = CreateSolidBrush(color);
	return color;
}

void CColorButton::OnBnClicked()
{
	CColorDialog colorDialog(color, 0, this);
	if(colorDialog.DoModal() == IDOK)
	{
		SetColor(colorDialog.GetColor());	
		Invalidate();
	}

}
