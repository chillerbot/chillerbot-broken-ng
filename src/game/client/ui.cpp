/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/system.h>
#include <base/math.h>

#include <engine/shared/config.h>
#include "ui.h"


/********************************************************
 UI
*********************************************************/

CUI::CUI()
{
	m_pHotItem = 0;
	m_pActiveItem = 0;
	m_pLastActiveItem = 0;
	m_pBecommingHotItem = 0;

	m_MouseX = 0;
	m_MouseY = 0;
	m_MouseWorldX = 0;
	m_MouseWorldY = 0;
	m_MouseButtons = 0;
	m_LastMouseButtons = 0;

	m_Screen.x = 0;
	m_Screen.y = 0;
	m_Screen.w = 848.0f;
	m_Screen.h = 480.0f;
}

int CUI::Update(float Mx, float My, float Mwx, float Mwy, int Buttons)
{
	return 0;
}

int CUI::MouseInside(const CUIRect *r)
{
	return 0;
}

void CUI::ConvertMouseMove(float *x, float *y)
{

}

void CUI::AndroidShowScreenKeys(bool shown)
{

}

void CUI::AndroidShowTextInput(const char *text, const char *hintText)
{

}

void CUI::AndroidBlockAndGetTextInput(char *text, int textLength, const char *hintText)
{

}

bool CUI::AndroidTextInputShown()
{
	return false;
}

CUIRect *CUI::Screen()
{
	return &m_Screen;
}

float CUI::PixelSize()
{
	return 0.0f;
}

void CUI::SetScale(float s)
{

}

float CUI::Scale()
{
	return 0.0f;
}

float CUIRect::Scale() const
{
	return 100.0f;
}

void CUI::ClipEnable(const CUIRect *r)
{

}

void CUI::ClipDisable()
{

}

void CUIRect::HSplitMid(CUIRect *pTop, CUIRect *pBottom) const
{

}

void CUIRect::HSplitTop(float Cut, CUIRect *pTop, CUIRect *pBottom) const
{

}

void CUIRect::HSplitBottom(float Cut, CUIRect *pTop, CUIRect *pBottom) const
{

}


void CUIRect::VSplitMid(CUIRect *pLeft, CUIRect *pRight) const
{

}

void CUIRect::VSplitLeft(float Cut, CUIRect *pLeft, CUIRect *pRight) const
{

}

void CUIRect::VSplitRight(float Cut, CUIRect *pLeft, CUIRect *pRight) const
{

}

void CUIRect::Margin(float Cut, CUIRect *pOtherRect) const
{

}

void CUIRect::VMargin(float Cut, CUIRect *pOtherRect) const
{

}

void CUIRect::HMargin(float Cut, CUIRect *pOtherRect) const
{

}

int CUI::DoButtonLogic(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{
	return 0;
}

int CUI::DoPickerLogic(const void *pID, const CUIRect *pRect, float *pX, float *pY)
{
	return 1;
}

void CUI::DoLabel(const CUIRect *r, const char *pText, float Size, int Align, int MaxWidth)
{

}

void CUI::DoLabelScaled(const CUIRect *r, const char *pText, float Size, int Align, int MaxWidth)
{

}
