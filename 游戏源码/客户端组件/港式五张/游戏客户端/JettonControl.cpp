#include "StdAfx.h"
#include "GameClient.h"
#include "JettonControl.h"

//////////////////////////////////////////////////////////////////////////

//属性定义
#define LAYER_COUNT					6									//最大层数
#define LAYER_HEIGHT				5									//每层高度

#define RAND_WIDTH					150									//绘画最大宽
#define	RAND_HEIGHT					120									//绘画最大高

#define MAX_DRAW					50									//绘画最大筹码数

//////////////////////////////////////////////////////////////////////////

//构造函数
CJettonControl::CJettonControl()
{
	//设置变量
	m_lScore=0L;
	m_BenchmarkPos.SetPoint(0,0);
	m_DrawMode = enDrawMode_Layer;
	m_lDrawScore = 0L;

	//加载位图
	HINSTANCE hInst = AfxGetInstanceHandle();
	m_PngJetton.LoadImage(hInst,TEXT("JETTON_VIEW"));

	return;
}

//析构函数
CJettonControl::~CJettonControl()
{
}

//绘画控件
VOID CJettonControl::DrawJettonControl(CDC * pDC)
{
	//绘画判断 
	if (m_lScore<=0L) return;

	//获取属性
	INT nJettonHeight=m_PngJetton.GetHeight();
	INT nJettonWidth=m_PngJetton.GetWidth()/JETTON_COUNT;

	//层叠模式
	if( enDrawMode_Layer == m_DrawMode )
	{
		//变量定义
		LONG lScore=m_lScore;
		LONG lScoreArray[]={1L,5L,10L,50L,100L,500L,1000L,5000L,10000L,50000L,100000L,500000L,1000000L,5000000L};

		//计算筹码
		for (WORD i=0;i<LAYER_COUNT;i++)
		{
			//获取索引
			INT nImageIndex=0;
			for (INT j=0;j<CountArray(lScoreArray);j++)
			{
				if (lScore>=lScoreArray[CountArray(lScoreArray)-j-1])
				{
					//设置变量
					nImageIndex=CountArray(lScoreArray)-j-1;
					lScore-=lScoreArray[CountArray(lScoreArray)-j-1];
					break;
				}
			}

			//绘画筹码
			INT nXPos=m_BenchmarkPos.x-nJettonWidth/2;
			INT nYPos=m_BenchmarkPos.y-nJettonHeight/2-i*LAYER_HEIGHT;
			m_PngJetton.DrawImage(pDC,nXPos,nYPos,nJettonWidth,nJettonHeight,nJettonWidth*nImageIndex,0);

			//终止判断
			if (lScore==0L) break;
		}
	}
	//随机堆放模式
	else
	{
		INT nXPos,nYPos;
		WORD i,j = 0;
		for( i = 0; i < m_JetonDraw.GetCount() && j < MAX_DRAW; i++ )
		{
			tagJettonDraw &jd = m_JetonDraw[i];
			for( WORD k = 0; k < jd.wDrawCount; k++ )
			{
				nXPos = m_BenchmarkPos.x + m_DrawExcusions[j].x - nJettonWidth/2;
				nYPos = m_BenchmarkPos.y + m_DrawExcusions[j].y - nJettonHeight/2;
				//绘画筹码
				m_PngJetton.DrawImage(pDC,nXPos,nYPos,nJettonWidth,nJettonHeight,nJettonWidth*jd.wDrawIndex,0);
				j++;
			}
		}
	}

	return;
}

//基准位置
VOID CJettonControl::SetBenchmarkPos(INT nXPos, INT nYPos)
{
	//设置变量
	m_BenchmarkPos.SetPoint(nXPos,nYPos);

	return;
}

//设置筹码
VOID CJettonControl::SetScore(LONG lScore)
{
	if( m_lScore != lScore )
	{
		m_lScore = lScore;
		RectifyControl();
	}
}

//添加筹码
void CJettonControl::AddScore(LONG lScore)
{
	m_lScore += lScore;
	RectifyControl();
}

//调整控件
void CJettonControl::RectifyControl()
{
	//m_lScore为0,或随机堆放模式时
	if( m_lScore == 0 || enDrawMode_Rand == m_DrawMode && 
		m_lDrawScore != m_lScore )
	{
		LONG lDrawScore;
		if( m_lDrawScore > m_lScore )
		{
			lDrawScore = m_lScore;
			m_JetonDraw.RemoveAll();
			m_DrawExcusions.RemoveAll();
		}
		else lDrawScore = m_lScore - m_lDrawScore;
		m_lDrawScore = m_lScore;
		if( lDrawScore > 0L )
		{
			LONG lJettons[] = {1L,5L,10L,50L,100L,500L,1000L,5000L,10000L,50000L,100000L,500000L,1000000L,5000000L};
			for( int i = CountArray(lJettons)-1; ; i-- )
			{
				if( lDrawScore >= lJettons[i] )
				{
					//绘画结构
					tagJettonDraw jd;
					jd.wDrawCount = (WORD)(lDrawScore/lJettons[i]);
					jd.wDrawIndex = i;
					m_JetonDraw.Add(jd);
					lDrawScore -= jd.wDrawCount*lJettons[i];
					//添加位置
					for( WORD j = 0; j < jd.wDrawCount; j++ )
					{
						CPoint pt(rand()%RAND_WIDTH,rand()%RAND_HEIGHT);
						m_DrawExcusions.Add(pt);
					}
				}
				if( lDrawScore == 0 ) break;
			}
		}
	}
}

//设置绘画模式
void CJettonControl::SetDrawMode( enDrawMode DrawMode )
{
	if( m_DrawMode != DrawMode )
	{
		m_DrawMode = DrawMode;
		RectifyControl();
	}
}

//////////////////////////////////////////////////////////////////////////
