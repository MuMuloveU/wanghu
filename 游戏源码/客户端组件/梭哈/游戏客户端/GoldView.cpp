#include "StdAfx.h"
#include "Resource.h"
#include "GoldView.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define GOLD_IMAGE_WIDTH		48										//筹码宽
#define GOLD_IMAGE_HEIGHT		41										//筹码高

//静态变量
bool							CGoldView::m_bInit=false;				//初始标志
CSkinImage						CGoldView::m_ImageGold;					//正常筹码

//////////////////////////////////////////////////////////////////////////

//构造函数
CGoldView::CGoldView()
{
	m_lGold=0L;
	m_nGoldIndex = 1;
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
	}
	return;
}

//绘画筹码
void CGoldView::DrawGoldView(CDC * pDC, int nXPos, int nYPos)
{
	//加载位图
	CImageHandle ImageHandle(&m_ImageGold);

	//绘画判断 
	if (m_lGold<=0L) return;

	//获取属性
	INT nJettonHeight=m_ImageGold.GetHeight();
	INT nJettonWidth=m_ImageGold.GetWidth()/3;

	//绘画筹码
	INT nXPos1=nXPos-nJettonWidth/2;
	INT nYPos1=nYPos-nJettonHeight/2;
	m_ImageGold.AlphaDrawImage(pDC,nXPos1,nYPos1,nJettonWidth,nJettonHeight,nJettonWidth*m_nGoldIndex,0,RGB(255,0,255));

	//画数字
	TCHAR szNum[64];
	//_snprintf( szNum,CountArray(szNum),TEXT("%ld"),m_lScore );
	GetGlodString( m_lGold,szNum );
	CRect rcNum( nXPos1,nYPos1,nXPos1+nJettonWidth,nYPos1+nJettonHeight );

	//创建字体
	CFont ViewFont;
	ViewFont.CreateFont(-12,0,0,0,700,0,0,0,134,3,2,1,1,TEXT("宋体"));
	COLORREF OldClr = pDC->SetTextColor(RGB(0x0, 0x10, 0x70));
	CFont * pOldFont=pDC->SelectObject(&ViewFont);
	pDC->DrawText( szNum,lstrlen(szNum),&rcNum,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS );
	pDC->SetTextColor(OldClr);
	//清理资源
	pDC->SelectObject(pOldFont);

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
		//if (dwNumBit%3==0) szString[dwCharBit++]=TEXT(',');
		lGold/=10;
	} while (lGold!=0L);

	//调整字符
	if (szString[dwCharBit-1]==TEXT(',')) szString[dwCharBit-1]=0;
	szString[dwCharBit]=0;
	_tcsrev(szString);

	return szString;
}

//
VOID CGoldView::SetGoldIndex( INT nIndex )
{
	m_nGoldIndex = nIndex;
}

//////////////////////////////////////////////////////////////////////////
