#pragma once

#include "d3d9.h"
#include <d3dx9.h>
#include "d3dx9math.h"

class CRollFont
{
public:
	int						m_nFrameCount;
	int						m_nExcursion;

	CRect					rect;

	LPD3DXFONT				m_pFont;

	CString					m_sMessage;
	
	CRollFont(void);
	~CRollFont(void);
	void DrawText(void);
	void InitFont(int xPos, int yPos, CString msg, int nFrameCount, int nExcursion, LPD3DXFONT pFont);
};
