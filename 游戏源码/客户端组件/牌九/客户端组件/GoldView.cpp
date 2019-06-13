#include "StdAfx.h"
#include "Resource.h"
#include "GoldView.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define GOLD_IMAGE_WIDTH		/*29*/	32									//筹码宽
#define GOLD_IMAGE_HEIGHT		33										//筹码高

//静态变量
bool							CGoldView::m_bInit=false;				//初始标志
CSkinImage						CGoldView::m_ImageGold;					//正常筹码

//////////////////////////////////////////////////////////////////////////

//构造函数
CGoldView::CGoldView()
{
	m_lGold=0L;
	m_lMaxLayer=4;
	memset(m_lGoldCount,0,sizeof(m_lGoldCount));
	if (m_bInit==false)
	{
		m_bInit=true;
		m_ImageGold.SetLoadInfo(IDB_GOLD,GetModuleHandle(NULL));
	}

	return;
}

//析构函数
CGoldView::~CGoldView()
{
}

//设置筹码
void CGoldView::SetGold(LONG lGold)
{
	if (m_lGold!=lGold)
	{
		m_lGold=lGold;
		RectifyGoldLayer();
	}
	return;
}

//设置层数
void CGoldView::SetMaxGoldLayer(LONG lMaxLayer)
{
	if (m_lMaxLayer!=lMaxLayer)
	{
		m_lMaxLayer=lMaxLayer; 
		RectifyGoldLayer();
	}
	return;
}

//绘画筹码
void CGoldView::DrawGoldView(CDC * pDC, int nXPos, int nYPos)
{
	//加载位图
	CImageHandle ImageHandle(&m_ImageGold);

	//绘画筹码
	int nYPosDraw=nYPos-GOLD_IMAGE_HEIGHT/2;
	for (LONG i=0;i<CountArray(m_lGoldCount);i++)
	{
		for (LONG j=0;j<m_lGoldCount[i];j++)
		{
			m_ImageGold.AlphaDrawImage(pDC,nXPos-GOLD_IMAGE_WIDTH/2,nYPosDraw,GOLD_IMAGE_WIDTH,GOLD_IMAGE_HEIGHT,
				i*GOLD_IMAGE_WIDTH,0,RGB(255,0,255));
			nYPosDraw-=3;
		}
	}

	//绘画数字
	if (m_lGold!=0L)
	{
		TCHAR szBuffer[64]=TEXT("￥");
		GetGlodString(m_lGold,szBuffer+2);
		CRect DrawRect(nXPos-50,nYPos+GOLD_IMAGE_HEIGHT/2+5,nXPos+46,nYPos+GOLD_IMAGE_HEIGHT/2+17);
		pDC->SetTextColor(RGB(250,250,250));
		pDC->DrawText(szBuffer,lstrlen(szBuffer),&DrawRect,DT_CENTER|DT_VCENTER|DT_NOCLIP|DT_SINGLELINE);
	}

	return;
}

//调整筹码层
void CGoldView::RectifyGoldLayer()
{
	//变量定义
	LONG lBasicGold[]={500L,100L,50L,10L,5L,1L},lGold=m_lGold,lGoldLayer=0L;
	//LONG lBasicGold[]={1000L,500L,100L,50L,10L},lGold=m_lGold,lGoldLayer=0L;
	memset(m_lGoldCount,0,sizeof(m_lGoldCount));

	//调整筹码层
	for (LONG i=0;i<CountArray(lBasicGold);i++)
	{
		if (lGold>=lBasicGold[i])
		{
			m_lGoldCount[i]=lGold/lBasicGold[i];
			lGold-=m_lGoldCount[i]*lBasicGold[i];
			lGoldLayer+=m_lGoldCount[i];
			if (lGoldLayer>=m_lMaxLayer)
			{
				m_lGoldCount[i]-=(lGoldLayer-m_lMaxLayer);
				return;
			}
			if (lGold==0L) break;
		}
	}

	return;
}

//整性变字符
LPCTSTR CGoldView::GetGlodString(LONG lGold, TCHAR szString[])
{
	//转换字符
	DWORD dwNumBit=0L,dwCharBit=0L;
	do
	{
		dwNumBit++;
		szString[dwCharBit++]=(TCHAR)(lGold%10+TEXT('0'));
		if (dwNumBit%3==0) szString[dwCharBit++]=TEXT(',');
		lGold/=10;
	} while (lGold!=0L);

	//调整字符
	if (szString[dwCharBit-1]==TEXT(',')) szString[dwCharBit-1]=0;
	szString[dwCharBit]=0;
	_tcsrev(szString);

	return szString;
}

//////////////////////////////////////////////////////////////////////////
