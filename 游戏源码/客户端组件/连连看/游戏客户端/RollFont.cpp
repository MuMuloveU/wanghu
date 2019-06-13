#include "StdAfx.h"
#include ".\rollfont.h"

CRollFont::CRollFont(void)
{
	m_pFont = NULL;
}

CRollFont::~CRollFont(void)
{
}

void CRollFont::DrawText(void)
{
	if (m_pFont==NULL)
		return;
	if(m_nFrameCount >0)
	{
		m_pFont->DrawTextA(NULL,m_sMessage,-1,&rect,DT_CALCRECT,0);
		m_pFont->DrawText(NULL,
			m_sMessage, //文本内容
			-1, //文本长度
			&rect, //文本的显示区域，使用窗口坐标
			DT_LEFT, //显示格式：左对齐
			D3DCOLOR_XRGB(255,0,0) ); //
		//step it
		--m_nFrameCount;
		rect.top -= m_nExcursion;
	}
}

void CRollFont::InitFont(int xPos, int yPos, CString msg, int nFrameCount, int nExcursion, LPD3DXFONT pFont)
{
	rect.left = xPos;
	rect.top = yPos;
	m_sMessage = msg;
	m_nFrameCount = nFrameCount;
	m_nExcursion = nExcursion;
	m_pFont = pFont;
}
