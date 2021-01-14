#pragma once


// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)

public:
	CColorButton();
	virtual ~CColorButton();
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
private:
	COLORREF color;
	HBRUSH brush;
public:
	COLORREF GetColor(void);
	COLORREF SetColor(COLORREF _color);
	afx_msg void OnBnClicked();
};


