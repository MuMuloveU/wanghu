// ---------------------------------------------------------------------------------------------------------------------------------
//  _   _   _ _       _____  _____ ____                       
// | | | | | (_)     |  __ \|_   _|  _ \                      
// | | | | | |_ _ __ | |  | | | | | |_) |     ___ _ __  _ __  
// | | | | | | | '_ \| |  | | | | |  _ <     / __| '_ \| '_ \ 
// |  V _ V  | | | | | |__| |_| |_| |_) | _ | (__| |_) | |_) |
//  \__/ \__/|_|_| |_|_____/|_____|____/ (_) \___| .__/| .__/ 
//                                               | |   | |    
//                                               |_|   |_|    
//
// Generic windows DIBsection class for quick and painless output to a windows window
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//  * This software is 100% free
//  * If you use this software (in part or in whole) you must credit the author.
//  * This software may not be re-distributed (in part or in whole) in a modified
//    form without clear documentation on how to obtain a copy of the original work.
//  * You may not use this software to directly or indirectly cause harm to others.
//  * This software is provided as-is and without warrantee. Use at your own risk.
//
// For more information, visit HTTP://www.FluidStudios.com
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h" // If this line gives you an error, comment it out
#include "winDIB.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --------------------------------------------------------------------------------------------------------------------------------

static	unsigned char	blendTable8[256][256];
static	bool		blendTableInitialized;

// --------------------------------------------------------------------------------------------------------------------------------

static	inline		unsigned int	scaleAlpha(const unsigned int color, const int scale)
{
	// alpha * scale, where scale is a scalar (0...255) -- only color's alpha is modified
	unsigned int	a = ((color >> 8) & 0x00ff0000) * scale;
	return	(a & 0xff000000) | (color & 0x00ffffff);
}

// --------------------------------------------------------------------------------------------------------------------------------

static	inline		unsigned int	alphaBlend(const unsigned int dst, const unsigned int src)
{
	// src * alpha + dst * (1-alpha), where alpha is src's alpha, resulting alpha is dst alpha
	unsigned int	a = src >> 24;
	unsigned int	rb = (((src & 0x00ff00ff) * a) + ((dst & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
	unsigned int	g  = (((src & 0x0000ff00) * a) + ((dst & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
	return (src & 0xff000000) | ((rb | g) >> 8);
}

// ---------------------------------------------------------------------------------------------------------------------------------

		winDIB::winDIB(CDC &dc)
		:_dc(dc), _frameBuffer(NULL), _zBuffer(NULL)
{
	_bmi[0].bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_bmi[0].bmiHeader.biPlanes = 1;
	_bmi[0].bmiHeader.biBitCount = 0;
	_bmi[0].bmiHeader.biXPelsPerMeter = 72;
	_bmi[0].bmiHeader.biYPelsPerMeter = 72;
	_bmi[0].bmiHeader.biClrUsed = 0;
	_bmi[0].bmiHeader.biClrImportant = 0;

	// Setup the blend table (only once)

	if (!blendTableInitialized)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			for (unsigned int j = 0; j < 256; j++)
			{
				blendTable8[i][j] = static_cast<unsigned char>((i*j) >> 8);
			}
		}

		blendTableInitialized = true;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void		winDIB::depth(const WORD bitdepth)
{
	unsigned int	bytesPerPixel;
	DWORD		*colors = reinterpret_cast<DWORD *>(_bmi[0].bmiColors);

	switch(bitdepth)
	{
		case 8:
			bytesPerPixel = 1;
			break;

		case 15:
			_bmi[0].bmiHeader.biCompression = BI_BITFIELDS;
			_bmi[0].bmiHeader.biBitCount = 16;
			colors[0] = 0x7C00;
			colors[1] = 0x03E0;
			colors[2] = 0x001F;
			bytesPerPixel = 2;
			break;

		case 16:
			_bmi[0].bmiHeader.biCompression = BI_BITFIELDS;
			_bmi[0].bmiHeader.biBitCount = 16;
			colors[0] = 0xF800;
			colors[1] = 0x07E0;
			colors[2] = 0x001F;
			bytesPerPixel = 2;
			break;

		case 24:
			_bmi[0].bmiHeader.biCompression = BI_RGB;
			_bmi[0].bmiHeader.biBitCount = 24;
			bytesPerPixel = 3;
			break;

		case 32:
			_bmi[0].bmiHeader.biCompression = BI_RGB;
			_bmi[0].bmiHeader.biBitCount = 32;
			bytesPerPixel = 4;
			break;

		default:
			bytesPerPixel = 0;
			_bmi[0].bmiHeader.biBitCount = 0;
			break;
	}

	_bmi[0].bmiHeader.biSizeImage = width() * height() * bytesPerPixel;
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned char	*winDIB::frameBuffer(unsigned char *fb, float *zb, const LONG width, const LONG height, const WORD bitdepth)
{
	_bmi[0].bmiHeader.biWidth = width;
	_bmi[0].bmiHeader.biHeight = -height;
	_frameBuffer = fb;
	_zBuffer = zb;
	if (bitdepth)	depth(bitdepth);
	return _frameBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int	winDIB::stretchToDisplay()
{
	int	srcX = _srcRect.left;
	int	srcY = _srcRect.top;
	int	srcW = _srcRect.Width();
	int	srcH = _srcRect.Height();
	int	dstX = _dstRect.left;
	int	dstY = _dstRect.top;
	int	dstW = _dstRect.Width();
	int	dstH = _dstRect.Height();
	return StretchDIBits(dc().GetSafeHdc(), dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH, _frameBuffer, _bmi, DIB_RGB_COLORS, SRCCOPY);
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int	winDIB::copyToDisplay()
{
	int	srcX = _srcRect.left;
	int	srcY = _srcRect.top;
	int	dstX = _dstRect.left;
	int	dstY = _dstRect.top;
	int	dstW = _dstRect.Width();
	int	dstH = _dstRect.Height();
	return SetDIBitsToDevice(dc().GetSafeHdc(), dstX, dstY, dstW, dstH, srcX, srcY, 0, dstH, _frameBuffer, _bmi, DIB_RGB_COLORS);
}

// ---------------------------------------------------------------------------------------------------------------------------------

const	bool	winDIB::clipLine(float &x0, float &y0, float &x1, float &y1) const
{
	// Our clipping rect

	CRect	clipRect(srcRect());
	clipRect.right -= 2;
	clipRect.bottom -= 2;

	// Gurarntee top->bottom

	if (y0 > y1)
	{
		float	temp;
		temp = x0; x0 = x1; x1 = temp;
		temp = y0; y0 = y1; y1 = temp;
	}
	
	// Off-screen?

	if (y1 < clipRect.top || y0 >= clipRect.bottom) return false;

	// Top-clip

	if (y0 < clipRect.top)
	{
		float	clipDelta = (clipRect.top - y0) / (y1 - y0);
		x0 += (x1 - x0) * clipDelta;
		y0 = static_cast<float>(clipRect.top);
	}

	// Bottom-clip

	if (y1 > clipRect.bottom)
	{
		float	clipDelta = (y1 - clipRect.bottom) / (y1 - y0);
		x1 -= (x1 - x0) * clipDelta;
		y1 = static_cast<float>(clipRect.bottom - 1);
	}

	// Gurarntee left->right

	if (x0 > x1)
	{
		float	temp;
		temp = x0; x0 = x1; x1 = temp;
		temp = y0; y0 = y1; y1 = temp;
	}
	
	// Off-screen?

	if (x1 < clipRect.left || x0 > clipRect.right) return false;

	// Left-clip

	if (x0 < clipRect.left)
	{
		float	clipDelta = (clipRect.left - x0) / (x1 - x0);
		x0 = static_cast<float>(clipRect.left);
		y0 += (y1 - y0) * clipDelta;
	}

	// Right-clip

	if (x1 > clipRect.right)
	{
		float	clipDelta = (x1 - clipRect.right) / (x1 - x0);
		x1 = static_cast<float>(clipRect.right);
		y1 -= (y1 - y0) * clipDelta;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void		winDIB::line(const float x0, const float y0, const float x1, const float y1, const unsigned int color)
{
	// For fixed-point stuff

	enum		{F_BITS = 16};
	enum		{F_RANG = (1 << F_BITS) - 1};
	enum		{F_HALF = 1 << (F_BITS - 1)};
	enum		{F_ONE  = 1 << F_BITS};
	enum		{F_SHIFT = 8}; // how many bits to shift right to get to 8 bits...

	// Make sure we have a frame buffer

	if (!frameBuffer()) return;

	// Clip the line

	float	cx0 = x0;
	float	cx1 = x1;
	float	cy0 = y0;
	float	cy1 = y1;
	if (!clipLine(cx0, cy0, cx1, cy1)) return;

	// Convert to fixed-point precision

	unsigned long	lx0 = static_cast<long>(cx0 * static_cast<float>(F_ONE));
	unsigned long	ly0 = static_cast<long>(cy0 * static_cast<float>(F_ONE));
	unsigned long	lx1 = static_cast<long>(cx1 * static_cast<float>(F_ONE));
	unsigned long	ly1 = static_cast<long>(cy1 * static_cast<float>(F_ONE));

	// Enforce top-down orientation

	if (ly0 > ly1)
	{
		unsigned long	temp;
		temp = lx0; lx0 = lx1; lx1 = temp;
		temp = ly0; ly0 = ly1; ly1 = temp;
	}

	// X direction

	int	xDir = lx0 < lx1 ? 1 : -1;

	// |deltas|

	long		dx  = lx1 - lx0; if (dx < 0) dx = -dx;
	unsigned long	aDX = static_cast<unsigned long>(dx);
	unsigned long	aDY = ly1 - ly0;

	// The line starts here

	unsigned int	pitch = width();
	unsigned int	*fb = reinterpret_cast<unsigned int *>(frameBuffer()) + (ly0 >> F_BITS) * pitch + (lx0 >> F_BITS);

	// Mask off the whole value

	lx0 &= F_RANG;
	ly0 &= F_RANG;

	// Primarily horizontal

	if (aDX > aDY)
	{
		// Step in Y

		unsigned long	dy = static_cast<unsigned long>(static_cast<float>(aDY) / static_cast<float>(aDX) * static_cast<float>(F_ONE));

		// Iterate along the line

		unsigned long	count = aDX >> F_BITS;

		while(count--)
		{
			// Step (with error correction)

			if (ly0 > F_ONE)
			{
				ly0 -= F_ONE;
				fb += pitch;
			}

			// Draw the pixel

			*fb = color;
			fb += xDir;
			ly0 += dy;
		}
	}

	// Primarily vertical

	else
	{
		// Step in X

		unsigned long	dx = static_cast<unsigned long>(static_cast<float>(aDX) / static_cast<float>(aDY) * static_cast<float>(F_ONE));

		// If the xDir is negative, then invert x0 so we can use a positive delta

		if (xDir < 0)	lx0 = F_ONE - lx0;

		// Iterate along the line

		unsigned long	count = aDY >> F_BITS;

		while(count--)
		{
			// Step (with error correction)

			if (lx0 > F_ONE)
			{
				lx0 -= F_ONE;
				fb += xDir;
			}

			// Draw the pixel

			*fb = color;
			fb += pitch;
			lx0 += dx;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Modified Wu anti-aliased line drawing.  Performance oriented, uses Wu combined with Bresenham, with modifications to allow the
// line to start on non-exact pixel boundaries.
// ---------------------------------------------------------------------------------------------------------------------------------

void		winDIB::aaLine(const float x0, const float y0, const float x1, const float y1, const unsigned int color)
{
	// For fixed-point stuff

	enum		{F_BITS = 16};
	enum		{F_RANG = (1 << F_BITS) - 1};
	enum		{F_HALF = 1 << (F_BITS - 1)};
	enum		{F_ONE  = 1 << F_BITS};
	enum		{F_SHIFT = 8}; // how many bits to shift right to get to 8 bits...

	// Make sure we have a frame buffer

	if (!frameBuffer()) return;

	// Clip the line

	float	cx0 = x0;
	float	cx1 = x1;
	float	cy0 = y0;
	float	cy1 = y1;
	if (!clipLine(cx0, cy0, cx1, cy1)) return;

	// Convert to fixed-point precision.  We add 0.5 to each so that our offset from center is the offset from the top/left
	// of the pixel.

	unsigned long	lx0 = static_cast<long>(cx0 * static_cast<float>(F_ONE));
	unsigned long	ly0 = static_cast<long>(cy0 * static_cast<float>(F_ONE));
	unsigned long	lx1 = static_cast<long>(cx1 * static_cast<float>(F_ONE));
	unsigned long	ly1 = static_cast<long>(cy1 * static_cast<float>(F_ONE));

	// Enforce top-down orientation

	if (ly0 > ly1)
	{
		unsigned long	temp;
		temp = lx0; lx0 = lx1; lx1 = temp;
		temp = ly0; ly0 = ly1; ly1 = temp;
	}

	// X direction

	int	xDir = lx0 < lx1 ? 1 : -1;

	// |deltas|

	long		dx  = lx1 - lx0; if (dx < 0) dx = -dx;
	unsigned long	aDX = static_cast<unsigned long>(dx);
	unsigned long	aDY = ly1 - ly0;

	// The line starts here

	unsigned int	pitch = width();
	unsigned int	*fb = reinterpret_cast<unsigned int *>(frameBuffer()) + (ly0 >> F_BITS) * pitch + (lx0 >> F_BITS);

	// Mask off the whole value

	lx0 &= F_RANG;
	ly0 &= F_RANG;

	// Primarily horizontal

	if (aDX > aDY)
	{
		// Step in Y

		unsigned long	dy = static_cast<unsigned long>(static_cast<float>(aDY) / static_cast<float>(aDX) * static_cast<float>(F_ONE));

		// Draw the first pixel

		unsigned long	fx = lx0 >> F_SHIFT;
		unsigned long	fy = ly0 >> F_SHIFT;
		if (xDir > 0)
		{
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[(1 << F_SHIFT) - fx][(1 << F_SHIFT) - fy])); fb += pitch;
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[(1 << F_SHIFT) - fx][                 fy])); fb -= pitch;
		}
		else
		{
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[fx][(1 << F_SHIFT) - fy])); fb += pitch;
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[fx][                 fy])); fb -= pitch;
		}

		// Step to the next pixel

		if (xDir > 0)	ly0 += ((F_ONE - lx0) * dy) >> F_BITS, fb++;
		else		ly0 += (         lx0  * dy) >> F_BITS;

		// Iterate along the line

		unsigned long	count = aDX >> F_BITS;

		while(count--)
		{
			// Step (with error correction)

			if (ly0 > F_ONE)
			{
				ly0 -= F_ONE;
				fb += pitch;
			}

			// Draw the pixel

			*fb = alphaBlend(*fb, scaleAlpha(color, (F_ONE - ly0) >> F_SHIFT)); fb += pitch;
			*fb = alphaBlend(*fb, scaleAlpha(color, (        ly0) >> F_SHIFT)); fb -= pitch;
			fb += xDir;
			ly0 += dy;
		}
	}

	// Primarily vertical

	else
	{
		// Step in X

		unsigned long	dx = static_cast<unsigned long>(static_cast<float>(aDX) / static_cast<float>(aDY) * static_cast<float>(F_ONE));

		// Draw the first pixel

		unsigned long	fx =          lx0  >> F_SHIFT;
		unsigned long	fy = (F_ONE - ly0) >> F_SHIFT;
		if (xDir > 0)
		{
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[fy][(1 << F_SHIFT) - fx])); fb += xDir;
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[fy][                 fx])); fb -= xDir;
		}
		else
		{
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[fy][                 fx])); fb += xDir;
			*fb = alphaBlend(*fb, scaleAlpha(color, blendTable8[fy][(1 << F_SHIFT) - fx])); fb -= xDir;
		}

		// If the xDir is negative, then invert x0 so we can use a positive delta

		if (xDir < 0)	lx0 = F_ONE - lx0, fb++;

		// Step to the next pixel

		lx0 += ((F_ONE - ly0) * dx) >> F_BITS;
		fb += pitch;

		// Iterate along the line

		unsigned long	count = aDY >> F_BITS;

		while(count--)
		{
			// Step (with error correction)

			if (lx0 > F_ONE)
			{
				lx0 -= F_ONE;
				fb += xDir;
			}

			// Draw the pixel

			*fb = alphaBlend(*fb, scaleAlpha(color, (F_ONE - lx0) >> F_SHIFT)); fb += xDir;
			*fb = alphaBlend(*fb, scaleAlpha(color, (        lx0) >> F_SHIFT)); fb -= xDir;
			fb += pitch;
			lx0 += dx;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	winDIB::fillRect(const int left, const int top, const int right, const int bottom, const unsigned int color)
{
	if (!frameBuffer()) return;

	// Clip

	int	t = top;
	int	l = left;
	int	r = right;
	int	b = bottom;

	// Totally off-screen?

	if (l >= static_cast<int>(width()) || r < 0 || t >= static_cast<int>(height()) || b < 0) return;

	// Clip it to the screen

	if (l < 0) l = 0;
	if (t < 0) t = 0;

	if (r >= static_cast<int>(width())) r = width() - 1;
	if (b >= static_cast<int>(height())) b = height() - 1;

	// Make sure it's ordered properly

	if (l > r || t > b) return;

	// Fill it

	int	x0 = l;
	int	y0 = t;
	int	x1 = r + 1;
	int	y1 = b + 1;

	// The alpha component

	unsigned int	alpha = color >> 24;

	// Full alpha?

	if (alpha == 0xff)
	{
		unsigned int	*fbRow = reinterpret_cast<unsigned int *>(frameBuffer()) + y0 * width() + x0;
		for (int y = y0; y < y1; y++, fbRow += width())
		{
			unsigned int	*fb = fbRow;
			for (int x = x0; x < x1; x++, fb++)
			{
				*fb = color;
			}
		}
	}
	else
	{
		unsigned int	*fbRow = reinterpret_cast<unsigned int *>(frameBuffer()) + y0 * width() + x0;
		for (int y = y0; y < y1; y++, fbRow += width())
		{
			unsigned int	*fb = fbRow;
			for (int x = x0; x < x1; x++, fb++)
			{
				unsigned int	dst = *fb;
				unsigned int	rb = (((color & 0x00ff00ff) * alpha) + ((dst & 0x00ff00ff) * (0xff - alpha))) & 0xff00ff00;
				unsigned int	g  = (((color & 0x0000ff00) * alpha) + ((dst & 0x0000ff00) * (0xff - alpha))) & 0x00ff0000;
				*fb = (rb | g) >> 8;
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// WinDIB.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

