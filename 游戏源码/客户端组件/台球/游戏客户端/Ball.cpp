#include "StdAfx.h"
#include "Ball.h"
#include "Math.h"
#include "GameClientView.h"

double			CBall::m_Grating=GRATING;				//摩擦力
//绘画信息
extern struct DrawInfoStruct			g_DrawInfo;

//构造函数 
CBall::CBall(void)
{
	m_bColor=1;
	m_bIndex=0;
	m_xPos=0.0;
	m_yPos=0.0;
	m_xSpeed=0.0;
	m_ySpeed=0.0;
	m_bAccess=false;
	m_xPreSpeed=0.0;
	m_yPreSpeed=0.0;
	m_xTurnSpeed=0.0;
	m_yTurnSpeed=0.0;
	m_xPreTurnSpeed=0.0;
	m_yPreTurnSpeed=0.0;
	m_PreSina=0.0;			
	m_PreCosa=0.0;
}

//移动函数 
BOOL CBall::Move()
{
	BOOL bMove=TRUE;
	//转移速度
	//增加位移
	m_xPos+=(m_xSpeed);
	m_yPos+=(m_ySpeed);
	m_xSpeed+=m_xTurnSpeed;
	m_ySpeed+=m_yTurnSpeed;
	//摩擦力
	m_xTurnSpeed*=0.994;
	m_yTurnSpeed*=0.994;			
	m_xPreSpeed=(m_xSpeed*=m_Grating);
	m_yPreSpeed=(m_ySpeed*=m_Grating);

	//判断速度
	if ((m_xSpeed>-LESS_SPEED)&&(m_xSpeed<LESS_SPEED)&&
		(m_ySpeed>-LESS_SPEED)&&(m_ySpeed<LESS_SPEED)&&
		(m_xTurnSpeed>-LESS_SPEED)&&(m_xTurnSpeed<LESS_SPEED)&&
		(m_yTurnSpeed>-LESS_SPEED)&&(m_yTurnSpeed<LESS_SPEED))
	{
		bMove=FALSE;
		m_xSpeed=0.0;
		m_xPreSpeed=0.0;
		m_xTurnSpeed=0.0;
		m_ySpeed=0.0;
		m_yPreSpeed=0.0;
		m_yTurnSpeed=0.0;
		m_xPreTurnSpeed=0.0;
		m_yPreTurnSpeed=0.0;
	}
	return bMove;
}

//设置位置
BOOL CBall::SetStation(double xPos,double yPos)
{
	m_xPos=xPos;
	m_yPos=yPos;
	m_xPreTurnSpeed=0.0;
	m_yPreTurnSpeed=0.0;
	m_xTurnSpeed=m_xPreSpeed=m_xSpeed=0.0;
	m_yTurnSpeed=m_yPreSpeed=m_ySpeed=0.0;
	m_bAccess=true;
	return TRUE;
}

//设置速度 
BOOL CBall::SetSpeed(double xSpeed, double ySpeed, double xTurnSpeed, double yTurnSpeed)
{
	m_xPreTurnSpeed=0.0;
	m_yPreTurnSpeed=0.0;
	m_xTurnSpeed=xTurnSpeed;
	m_yTurnSpeed=yTurnSpeed;
	m_xTurnSpeed=0;
	m_yTurnSpeed=0;			
	m_xPreSpeed=m_xSpeed=xSpeed;
	m_yPreSpeed=m_ySpeed=ySpeed;
	return TRUE;
}

//设置预速度
BOOL CBall::SetPreSpeed(double xSpeed, double ySpeed)
{
	m_xPreSpeed+=xSpeed;
	m_yPreSpeed+=ySpeed;
	return TRUE;
}

//更新函数
BOOL CBall::UpdateSpeed()
{
	if ((m_xPreSpeed!=0.0)||(m_yPreSpeed!=0.0))
	{
		m_xSpeed=m_xPreSpeed;
		m_ySpeed=m_yPreSpeed;
		UpdateTurnSpeed();
	}	
	return TRUE;
}

//更新转向速度
BOOL CBall::UpdateTurnSpeed()
{
	m_xPreTurnSpeed=0.0;
	m_yPreTurnSpeed=0.0;
	return TRUE;
}

//测试碰撞
BOOL CBall::TestCollide(double x, double y)
{
	double iNowDistance=(x-m_xPos)*(x-m_xPos)+(y-m_yPos)*(y-m_yPos);
	if (iNowDistance<BALL_RADII*BALL_RADII*4.0)	return TRUE;
	return FALSE;
}
//碰撞检测
BOOL CBall::CollideBall(CBall & Ball)
{
	if (&Ball!=this)
	{
		//距离检测
		double iNowDistance=(Ball.m_xPos-m_xPos)*(Ball.m_xPos-m_xPos)+(Ball.m_yPos-m_yPos)*(Ball.m_yPos-m_yPos);
		double iNextX=(Ball.m_xPos+Ball.m_xSpeed-m_xPos-m_xSpeed);
		double iNextY=(Ball.m_yPos+Ball.m_ySpeed-m_yPos-m_ySpeed);
		double iNextDistance=iNextX*iNextX+iNextY*iNextY;
		if (iNextDistance<iNowDistance)
		{
			iNowDistance=sqrt(iNowDistance);
			if (iNowDistance<BALL_RADII*2)
			{
				double sina=(Ball.m_yPos-m_yPos)/iNowDistance;
				double cosa=(Ball.m_xPos-m_xPos)/iNowDistance;
				double xPre=(m_xSpeed*cosa*cosa+m_ySpeed*sina*cosa)*HIT_LOST;
				double yPre=(m_xSpeed*cosa*sina+m_ySpeed*sina*sina)*HIT_LOST;
				Ball.SetPreSpeed(xPre,yPre);
				if(abs(m_PreSina)>0.01||abs(m_PreCosa)>0.01)
				{
					SetPreSpeed((xPre*sin(m_PreSina)+yPre*sin(m_PreCosa)-xPre),(yPre*sin(m_PreSina)+xPre*sin(m_PreCosa)-yPre));
					m_PreSina=0.0;
					m_PreCosa=0.0;
				}
				else
				{
					SetPreSpeed(-xPre,-yPre);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//构造函数 
CBallDesk::CBallDesk()
{
	m_bSound=true;
	m_bNeedMove=true;
	bIsGetStar=FALSE;
	m_iInBagCount=0;
	m_iFirstHitBag=-1;
	m_szText[0]=0;
	m_iTextPos=0;
	m_iScrollPos=0;
	m_dwWidth=0L;
	m_dwHeight=0L;
	m_iBallCount=0;
	m_pBallList=NULL;
}

//测试是否可以放置
bool CBallDesk::CanPlaceBall(double x, double y, bool bWhiteBall)
{
	//判断是否出界
	if ((x<=BALL_RADII)||(x>=m_dwWidth-BALL_RADII)||(y<=BALL_RADII)||(y>=m_dwHeight-BALL_RADII)) return false;

	//判断白球
	if (bWhiteBall)
	{
		if ((x<=108-DESK_SIDE)||(x>=179-DESK_SIDE)||(y<=150-DESK_SIDE)||(y>=302-DESK_SIDE)) return false;
		if (((x-179+DESK_SIDE)*(x-179+DESK_SIDE)+(y-226+DESK_SIDE)*(y-226+DESK_SIDE))>=5476) return false;
		//if ((x<=138-DESK_SIDE)||(x>=209-DESK_SIDE)||(y<=180-DESK_SIDE)||(y>=332-DESK_SIDE)) return false;
//		if (((x-198.0+DESK_SIDE)*(x-198.0+DESK_SIDE)+(y-241.0+DESK_SIDE)*(y-241.0+DESK_SIDE))>=5476) return false;
		//if (((x-209+DESK_SIDE)*(x-209+DESK_SIDE)+(y-256+DESK_SIDE)*(y-256+DESK_SIDE))>=5476) return false;
	}

	//检测是否碰撞
	for (int i=0;i<m_iBallCount;i++)
	{
		if (m_pBallList[i].m_bAccess==false) continue;
		if (m_pBallList[i].TestCollide(x,y)) return false;
	}

	//判断是否进袋
	for (int iBagStation=0;iBagStation<sizeof(m_BagPoint)/sizeof(m_BagPoint[0]);iBagStation++)
	{
		int XMid=abs((int)x-m_BagPoint[iBagStation].x);
		int YMid=abs((int)y-m_BagPoint[iBagStation].y);
		if ((XMid<=BAG_WIGHT)&&(YMid<=BAG_WIGHT))
		{
			int iMid=YMid*YMid+YMid*YMid;
			if (iMid<=BAG_WIGHT*BAG_WIGHT) return false;
		}
	}

	if(!bWhiteBall)
	{
		//......
	}
	return true;
}

//定时器函数
int CBallDesk::OnTimer()
{
	bSoundInBag=false;
	bSoundHitBall=false;
	bSoundHitDesk=false;
	//滚动信息
	if ((m_iScrollPos!=0)&&(m_iTextPos!=0))	
	{
		m_iTextPos+=m_iScrollPos;
		if ((m_iTextPos>=m_dwHeight)||(m_iTextPos<=50))
		{
			m_iTextPos=0;
			m_iScrollPos=0;
			m_szText[0]=0;
		}
	}
	
	m_BallBag.Move();

	if (m_bNeedMove)
	{
		int iMoveCount=0;
		for (int i=0;i<22;i++)
		{
			if (m_pBallList[i].m_bAccess&&m_pBallList[i].Move())
			{
				iMoveCount++;
			}
		}
		//检测碰撞
		if (iMoveCount>0)
		{
			m_bNeedMove=true;
			bool bHitBall=false,bHitDesk=false;
			for (int i=0;i<m_iBallCount;i++)
			{
				if (m_pBallList[i].m_bAccess==false) continue;
				for (int j=0;j<m_iBallCount;j++)
				{
					if (m_pBallList[j].m_bAccess==false) continue;
					if(i!=0)
					{
						m_pBallList[i].m_PreSina=0.0;			
						m_pBallList[i].m_PreCosa=0.0;
					}
					if (m_pBallList[i].CollideBall(m_pBallList[j]))
					{
						bHitBall=true;
						if ((i==0)&&(m_iFirstHitBag==-1))
						{
							m_iFirstHitBag=j;
						}
					}
				}
				bHitDesk=IsHitDesk(i)?true:bHitDesk;
			}

			//判断是否进袋
			bool bInBag=IsBallInBag();
			//更新速度
			for (int i=0;i<m_iBallCount;i++) m_pBallList[i].UpdateSpeed();


			bSoundInBag=bInBag;
			bSoundHitBall=bHitBall;
			bSoundHitDesk=bHitDesk;
		}
		else
		{
			m_bNeedMove=false;
		}
	}

	return 0;
}

//是否进袋
bool CBallDesk::IsBallInBag()
{
	bool bInBag=false;
	double XMid,YMid;
	for (int iBagStation=0;iBagStation<sizeof(m_BagPoint)/sizeof(m_BagPoint[0]);iBagStation++)
	{
		for (int i=0;i<m_iBallCount;i++)
		{
			if (m_pBallList[i].m_bAccess==false) continue;
			XMid=abs((int)m_pBallList[i].m_xPos-m_BagPoint[iBagStation].x);
			YMid=abs((int)m_pBallList[i].m_yPos-m_BagPoint[iBagStation].y);
			if ((XMid<=BAG_WIGHT)&&(YMid<=BAG_WIGHT))
			{
				double iMid=YMid*YMid+YMid*YMid;
				if (iMid<=BAG_WIGHT*BAG_WIGHT)
				{
					if(g_DrawInfo.m_StarPos==iBagStation&&g_DrawInfo.m_StarPos<6)
					{
						bIsGetStar=TRUE;
					}
					bInBag=true;
					m_iInBagCount++;
					m_pBallList[i].m_bAccess=false;
					m_BallBag.PushBall(m_pBallList[i]);
				}
			}
			////下面搞定一些特殊点
			//if(iBagStation==0)
			//{}
			//if(iBagStation==2)
			//{}
			//if(iBagStation==3)
			//{}
			//if(iBagStation==5)
			//{}
		}
	}
	return bInBag;
}

//桌边调整位置
bool CBallDesk::IsHitDesk(int iBallStation)
{
	bool bHitDesk=false;
	
	//中袋边
	if ((m_pBallList[iBallStation].m_xPos>((double)m_dwWidth/2.0-3.0*BALL_RADII))&&(m_pBallList[iBallStation].m_xPos<((double)m_dwWidth/2.0+3.0*BALL_RADII)))
	{
		if (m_pBallList[iBallStation].m_xPos<((double)m_dwWidth/2.0-BALL_RADII/2.0))
		{
			double iXMid=m_pBallList[iBallStation].m_xPos-((double)m_dwWidth/2.0-3.0*BALL_RADII);
			double iYIn=sqrt(2.5*2.5*BALL_RADII*BALL_RADII-iXMid*iXMid)-1.5*BALL_RADII;
			m_pBallList[iBallStation].m_yPos=__max(m_pBallList[iBallStation].m_yPos,iYIn);
			m_pBallList[iBallStation].m_yPos=__min(m_pBallList[iBallStation].m_yPos,(double)m_dwHeight-iYIn);

			//改变速度
			if (m_pBallList[iBallStation].m_yPos<=iYIn)
			{
				bHitDesk=true;
				double SpeedTemp=m_pBallList[iBallStation].m_xPreSpeed;
				m_pBallList[iBallStation].m_xTurnSpeed=0.0;
				m_pBallList[iBallStation].m_yTurnSpeed=0.0;
				m_pBallList[iBallStation].m_xPreSpeed=-m_pBallList[iBallStation].m_yPreSpeed;
				m_pBallList[iBallStation].m_yPreSpeed=-SpeedTemp;
			}
			else if (m_pBallList[iBallStation].m_yPos>=((double)m_dwHeight-iYIn))
			{
				bHitDesk=true;
				double SpeedTemp=m_pBallList[iBallStation].m_xPreSpeed;
				m_pBallList[iBallStation].m_xTurnSpeed=0.0;
				m_pBallList[iBallStation].m_yTurnSpeed=0.0;
				m_pBallList[iBallStation].m_xPreSpeed=m_pBallList[iBallStation].m_yPreSpeed;
				m_pBallList[iBallStation].m_yPreSpeed=SpeedTemp;
			}
		}
		else if (m_pBallList[iBallStation].m_xPos>((double)m_dwWidth/2.0+BALL_RADII/2.0))
		{
			double iXMid=((double)m_dwWidth/2.0+3.0*BALL_RADII)-m_pBallList[iBallStation].m_xPos;
			double iYIn=sqrt(2.5*2.5*BALL_RADII*BALL_RADII-iXMid*iXMid)-1.5*BALL_RADII;
			m_pBallList[iBallStation].m_yPos=__max(m_pBallList[iBallStation].m_yPos,iYIn);
			m_pBallList[iBallStation].m_yPos=__min(m_pBallList[iBallStation].m_yPos,(double)m_dwHeight-iYIn+3);

			//改变速度
			if (m_pBallList[iBallStation].m_yPos<=iYIn)
			{
				bHitDesk=true;
				double SpeedTemp=m_pBallList[iBallStation].m_xPreSpeed;
				m_pBallList[iBallStation].m_xTurnSpeed=0.0;
				m_pBallList[iBallStation].m_yTurnSpeed=0.0;
				m_pBallList[iBallStation].m_xPreSpeed=m_pBallList[iBallStation].m_yPreSpeed;
				m_pBallList[iBallStation].m_yPreSpeed=SpeedTemp;
			}
			else if (m_pBallList[iBallStation].m_yPos>=((double)m_dwHeight-iYIn))
			{
				bHitDesk=true;
				double SpeedTemp=m_pBallList[iBallStation].m_xPreSpeed;
				m_pBallList[iBallStation].m_xTurnSpeed=0.0;
				m_pBallList[iBallStation].m_yTurnSpeed=0.0;
				m_pBallList[iBallStation].m_xPreSpeed=-m_pBallList[iBallStation].m_yPreSpeed;
				m_pBallList[iBallStation].m_yPreSpeed=-SpeedTemp;
			}

		}
		else
		{
			m_pBallList[iBallStation].m_yPos=__max(m_pBallList[iBallStation].m_yPos,-BALL_RADII);
			m_pBallList[iBallStation].m_yPos=__min(m_pBallList[iBallStation].m_yPos,(double)m_dwHeight+BALL_RADII);
		}
	}
	else	//普通桌面边
	{
		//if ((m_pBallList[iBallStation].m_xPos<BALL_RADII)||(m_pBallList[iBallStation].m_xPos>((double)m_dwWidth-BALL_RADII)))
		//{
		//	bHitDesk=true;
		//	m_pBallList[iBallStation].m_xTurnSpeed=0.0;
		//	m_pBallList[iBallStation].m_yTurnSpeed=0.0;
		//	m_pBallList[iBallStation].m_xPos=__max(BALL_RADII,__min((double)m_dwWidth-BALL_RADII,m_pBallList[iBallStation].m_xPos));
		//	m_pBallList[iBallStation].m_xPreSpeed=-m_pBallList[iBallStation].m_xPreSpeed;
		//}
		//if ((m_pBallList[iBallStation].m_yPos<BALL_RADII)||(m_pBallList[iBallStation].m_yPos>((double)m_dwHeight-BALL_RADII))) 
		//{
		//	bHitDesk=true;
		//	m_pBallList[iBallStation].m_xTurnSpeed=0.0;
		//	m_pBallList[iBallStation].m_yTurnSpeed=0.0;
		//	m_pBallList[iBallStation].m_yPos=__max(BALL_RADII,__min((double)m_dwHeight-BALL_RADII,m_pBallList[iBallStation].m_yPos));
		//	m_pBallList[iBallStation].m_yPreSpeed=-m_pBallList[iBallStation].m_yPreSpeed;
		//}
		if ((m_pBallList[iBallStation].m_xPos<BALL_RADII)||(m_pBallList[iBallStation].m_xPos>((double)m_dwWidth-BALL_RADII)))
		{
			bHitDesk=true;
			m_pBallList[iBallStation].m_xTurnSpeed=0.0;
			m_pBallList[iBallStation].m_yTurnSpeed=0.0;
			m_pBallList[iBallStation].m_xPos=__max(BALL_RADII,__min((double)m_dwWidth-BALL_RADII,m_pBallList[iBallStation].m_xPos));
			m_pBallList[iBallStation].m_xPreSpeed=-m_pBallList[iBallStation].m_xPreSpeed;
		}
		if ((m_pBallList[iBallStation].m_yPos<BALL_RADII)||(m_pBallList[iBallStation].m_yPos>((double)m_dwHeight-BALL_RADII))) 
		{
			bHitDesk=true;
			m_pBallList[iBallStation].m_xTurnSpeed=0.0;
			m_pBallList[iBallStation].m_yTurnSpeed=0.0;
			m_pBallList[iBallStation].m_yPos=__max(BALL_RADII,__min((double)m_dwHeight-BALL_RADII,m_pBallList[iBallStation].m_yPos));
			m_pBallList[iBallStation].m_yPreSpeed=-m_pBallList[iBallStation].m_yPreSpeed;
		}
	}

	return bHitDesk;
}

//击球函数
void CBallDesk::HitBall(int x, int y, double fPower, double xTurn, double yTurn,double HitDegree)
{
	m_iInBagCount=0;
	m_iFirstHitBag=-1;
	bIsGetStar=FALSE;
	if (((int)m_pBallList[0].m_xPos!=x)||((int)m_pBallList[0].m_yPos)!=y)
	{
		double iMid=sqrt((x-m_pBallList[0].m_xPos)*(x-m_pBallList[0].m_xPos)+(m_pBallList[0].m_yPos-y)*(m_pBallList[0].m_yPos-y));
		double sina=(y-m_pBallList[0].m_yPos)/iMid;
		double cosa=(x-m_pBallList[0].m_xPos)/iMid;
		//xTurn*=fPower*0.05/MAX_SPEED;
		//yTurn*=fPower*0.05/MAX_SPEED;
		//m_pBallList[0].SetSpeed(fPower*cosa,fPower*sina,xTurn*cosa-yTurn*sina,xTurn*sina+yTurn*cosa);
		m_pBallList[0].SetSpeed(fPower*cosa,fPower*sina);
	}
	else 
		m_pBallList[0].SetSpeed(fPower,0);
	m_pBallList[0].m_PreSina=xTurn;			
	m_pBallList[0].m_PreCosa=yTurn;
	m_bNeedMove=true;
	return;
}

//设置信息字符
BOOL CBallDesk::SetMessageText(TCHAR * szMessage, int iScrollPos, int yPos,BOOL b)
{
	if(m_iTextPos!=0 && !b)
		return TRUE;
	m_iTextPos=0;
	m_iScrollPos=0;
	if (szMessage!=NULL)
	{
		lstrcpy(m_szText,szMessage);
		m_iTextPos=yPos;
		m_iScrollPos=iScrollPos;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CBallBag::CBallBag()
{
	m_iBagLength=0;
	m_iBallCount=0;
	m_bNeedMove=false;
	::memset(m_pBall,0,sizeof(m_pBall));
	::memset(m_bBallStation,0,sizeof(m_bBallStation));
}

//进袋
BOOL CBallBag::PushBall(CBall & Ball)
{
	if (m_iBallCount<sizeof(m_bBallStation)/sizeof(m_bBallStation[0]))
	{
		m_bNeedMove=true;
		m_bBallStation[m_iBallCount]=(int)BALL_RADII+3;
		//if(Ball.m_bColor==0)
		Ball.m_bAccess=false;
		m_pBall[m_iBallCount++]=&Ball;
		return TRUE;
	}
	return FALSE;
}

//取球
CBall * CBallBag::TakeBall(BYTE bBallColor)
{
	for (int i=m_iBallCount-1;i>=0;i--)
	{
		if (m_pBall[i]->m_bColor==bBallColor)
		{
			m_iBallCount--;
			for (int j=i;j<m_iBallCount;j++) m_pBall[j]=m_pBall[j+1];
			return m_pBall[i];
		}
	}
	return NULL;
}

//移动
BOOL CBallBag::Move()
{
	if (m_bNeedMove)
	{
		int iMove=0;
		m_bNeedMove=false;
		for (int i=0;i<m_iBallCount;i++)
		{
			if (i==0)
			{
				iMove=m_iBagLength-3-(int)(BALL_RADII*g_DrawInfo.dwTimes)-m_bBallStation[0];
			}
			else
			{
				iMove=m_bBallStation[i-1]-m_bBallStation[i]-2*(int)(BALL_RADII*g_DrawInfo.dwTimes);
			}
			if (iMove>2) 
			{ 
				iMove=2;
				m_bNeedMove=true;
			}
			else
			{
				if (iMove<0) 
				{ 
					iMove=0;
					m_bNeedMove=true; 
				}
			}
			m_bBallStation[i]+=iMove;
		}
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//初始化台球桌
BOOL CAmericaDesk::InitDesk()
{
	//m_dwWidth=855;//932
	//m_dwHeight=427;//502
	m_dwWidth=719;  //792
	m_dwHeight=352; //435
	m_pBallList=m_Ball;
	m_iBallCount=sizeof(m_Ball)/sizeof(m_Ball[0]);
	for (int i=0;i<sizeof(m_Ball)/sizeof(m_Ball[0]);i++) m_Ball[i].m_bIndex=i;
	//初始化球
	m_Ball[0].SetStation(140,214);
	//m_Ball[0].SetStation(10,14);
	m_Ball[0].m_bColor=0;
	//第一牌
	m_Ball[1].SetStation(535,176);
	//第二牌
	m_Ball[2].SetStation(556,164);
	m_Ball[3].SetStation(556,188);

	//第三牌
	m_Ball[4].SetStation(577,152);
	m_Ball[5].SetStation(577,176);
	m_Ball[6].SetStation(577,200);

	//第四牌
	m_Ball[7].SetStation(598,140);
	m_Ball[8].SetStation(598,164);
	m_Ball[9].SetStation(598,188);
	m_Ball[10].SetStation(598,212);

	//第五牌
	m_Ball[11].SetStation(619,128);
	m_Ball[12].SetStation(619,152);
	m_Ball[13].SetStation(619,176);
	m_Ball[14].SetStation(619,200);
	m_Ball[15].SetStation(619,224);

	//彩球
	m_Ball[16].SetStation(140,252);		//GSQ:黄
	//m_Ball[16].SetStation(10,10);
	m_Ball[17].SetStation(140,110);		//GSQ:绿
	m_Ball[18].SetStation(140,176);		//GSQ:灰
	m_Ball[19].SetStation(358,176);		//GSQ:蓝
	m_Ball[20].SetStation(506,176);		//GSQ:粉
	m_Ball[21].SetStation(654,176);		//GSQ:黑*/
	//初始化球
	/*m_Ball[0].SetStation(170,254);
	m_Ball[0].m_bColor=0;

	//第一牌
	m_Ball[1].SetStation(655,216);

	//第二牌
	m_Ball[2].SetStation(676,204);
	m_Ball[3].SetStation(676,228);

	//第三牌
	m_Ball[4].SetStation(697,192);
	m_Ball[5].SetStation(697,216);
	m_Ball[6].SetStation(697,240);

	//第四牌
	m_Ball[7].SetStation(718,180);
	m_Ball[8].SetStation(718,204);
	m_Ball[9].SetStation(718,228);
	m_Ball[10].SetStation(718,252);

	//第五牌
	m_Ball[11].SetStation(739,168);
	m_Ball[12].SetStation(739,192);
	m_Ball[13].SetStation(739,216);
	m_Ball[14].SetStation(739,240);
	m_Ball[15].SetStation(739,264);

	//彩球
	m_Ball[16].SetStation(170,216);		//GSQ:黄
	m_Ball[17].SetStation(170,292);		//GSQ:绿
	m_Ball[18].SetStation(170,140);		//GSQ:灰
	m_Ball[19].SetStation(428,216);		//GSQ:蓝
	m_Ball[20].SetStation(606,216);		//GSQ:粉
	m_Ball[21].SetStation(784,216);		//GSQ:黑*/
	for (int i=0;i<15;i++) m_Ball[i+1].m_bColor=1;
	for (int i=0;i<6;i++) m_Ball[i+16].m_bColor=i+2;

	//初始化袋
	m_BagPoint[0].x=5;//1;
	m_BagPoint[0].y=5;//1;
	m_BagPoint[1].x=m_dwWidth/2;
	m_BagPoint[1].y=(long int)(-BALL_RADII)-1;
	m_BagPoint[2].x=m_dwWidth-5;//m_dwWidth-1;
	m_BagPoint[2].y=5;//1;
	m_BagPoint[3].x=5;//1;
	m_BagPoint[3].y=m_dwHeight-6;//m_dwHeight-2;
	m_BagPoint[4].x=m_dwWidth/2;
	m_BagPoint[4].y=m_dwHeight+(long int)BALL_RADII;
	m_BagPoint[5].x=m_dwWidth-5;//m_dwWidth-1;
	m_BagPoint[5].y=m_dwHeight-6;//m_dwHeight-2;
	m_BallBag.m_iBallCount=0;
	for (int i=0;i<m_iBallCount;i++) m_Ball[i].m_bAccess=true;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
