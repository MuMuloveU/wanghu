#pragma once

class CDrawFunc
{
public:
	CDrawFunc(void);
	~CDrawFunc(void);

public:
	static bool IsValidRect(LPRECT lpRect);
	static bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);

private:
	
};
