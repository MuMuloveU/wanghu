// ChessView.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ChessView.h"
#include ".\chessview.h"

// CChessView

CChessView::CChessView()
{
	InitChessView();
}

CChessView::~CChessView()
{
	//释放所有图像资源
}


BEGIN_MESSAGE_MAP(CChessView, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CChessView 消息处理程序


int CChessView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	HINSTANCE hInstance=AfxGetInstanceHandle();

	//移动窗口
	SetWindowPos(NULL,0,0,100,100,SWP_NOMOVE|SWP_NOZORDER);

	return 0;
}

void CChessView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	return;
}


void CChessView::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void CChessView::InitChessView(void)
{
	_snprintf(m_per,sizeof(m_per),TEXT(""));
	_snprintf(m_class,sizeof(m_class),TEXT(""));

	m_ExerciseMode = false;
	m_isLose = false;
	m_first = true;
	m_score = 0;
	m_Iper = 0;
	m_StartPos = ERROR_POINT;
	m_EndPos = ERROR_POINT;
	m_result = ERROR_POINT;
	m_Bobm = false;
	_snprintf(m_scorestring,sizeof(m_scorestring),TEXT("得分:0"));

	m_BakeHit = false;
	m_BakeHitCount = 0;

	for(int i = 0; i<82; i++)
	{
		memset(&m_Chess[i],0,sizeof(m_Chess[i]));
	}

	for(int i=0; i<82; i++)
	{
		m_Result[i] = 0;
		m_tResult[i] = 0;
	}

	for(int i = 0; i<3; i++)
	{
		m_nextball[i] = 0;
        memset(&m_nextballcolor[i],0,sizeof(m_nextballcolor[i]));
		m_tnextball[i] = 0;
		memset(&m_tnextballcolor[i],0,sizeof(m_tnextballcolor[i]));
	}

	for(int i=0; i<5; i++)
	{
		m_firstball[i] = 0;
		memset(&m_firstballcolor[i],0,sizeof(m_firstballcolor[i]));
	}
	for(int i=0; i<9; i++)
	{
		memset(&m_DeadBall[i],0,sizeof(m_DeadBall[i]));
	}
	

}
//发球
void CChessView::TakeTheService()
{
	int i=3;
	
	srand(time(NULL));

	//第一次发球，要生成5个小球
	if(m_first)
	{
		i = 5;
		while(i)
		{
			int temp;
			int colorkind;
			colorkind = rand()%7+1;
			temp = rand()%80+1;
			if(m_Chess[temp].color==0)
			{
				m_firstball[i-1] = temp;
				m_firstballcolor[i-1].color = colorkind;

				m_Chess[temp].color = colorkind;
				//是否成功消去
				int t = m_Logic.IsSuccess(m_Chess,m_SuccessArray,temp);
				if(t)
				{
					if(!m_ExerciseMode)
					{
						::AfxGetMainWnd()->SendMessage(ID_SUCCESS,temp,t);
						::SendMessage(this->GetParent()->m_hWnd,ID_PSUCCESS,temp,t);
					}
					else
					{
						::SendMessage(this->GetParent()->m_hWnd,ID_PSUCCESS,temp,t);
					}
				}
				else
					m_BakeHit = false;
				--i;
			}
		}
		//发送发球消息
		if(!m_ExerciseMode)
		::AfxGetMainWnd()->SendMessage(ID_FIRSTBALL,0,0);
		m_first = false;		
	}

	//下一次选出的小球颜色
	for(int i=0; i<3; i++)
	{
		int colorkind;
		int myrand = rand()%40;
		if(!myrand && !m_ExerciseMode)
		{
			myrand = rand()%4+1;
			m_nextballcolor[i].time = 15;
		}
		else
			myrand = 0;

		colorkind = rand()%7+1;
		m_nextballcolor[i].color=colorkind;
		m_nextballcolor[i].prop = myrand;
	}
	//发送下一轮小球颜色消息
	if(!m_ExerciseMode)
		::AfxGetMainWnd()->SendMessage(ID_GETNEXTBALLCOLOR,0,0);
}

void CChessView::SuccessProcess(BYTE succ_array[], BYTE point)
{
	for(int i=0; i<9; i++)
		memset(&m_DeadBall,0,sizeof(m_DeadBall));
	//
	int c=0;
	memcpy(&m_DeadBall[c],&m_Chess[point],sizeof(m_Chess[point]));
	m_DeadBall[c++].standby = point;

	CString msg;
	msg.Format("%d",m_Chess[point].color);
	//AfxMessageBox(msg);
	//从棋盘中清除小球
	memset(&m_Chess[point],0,sizeof(m_Chess[point]));
	for(int i=0; succ_array[i]!=0; i++)
	{
		memcpy(&m_DeadBall[c],&m_Chess[succ_array[i]],sizeof(m_Chess[succ_array[i]]));
		m_DeadBall[c++].standby = succ_array[i];
		memset(&m_Chess[succ_array[i]],0,sizeof(m_Chess[succ_array[i]]));
	}
}

void CChessView::DisplayTakeTheService(BYTE nball[3],DBall nballcolor[3])
{
	
	memcpy(m_nextball, nball,sizeof(nball));
	memcpy(m_nextballcolor,nballcolor, sizeof(nballcolor));

	//将小球放入棋盘
	for(int i=0; i<3; i++)
	{
		memcpy(&m_Chess[m_nextball[i]], &m_nextballcolor[i],sizeof(m_nextballcolor[i]));
		//判断是否成功消去
		int t = m_Logic.IsSuccess(m_Chess,m_SuccessArray,m_nextball[i]);
		if(t)
		{
			if(!m_ExerciseMode)
			{
				::AfxGetMainWnd()->SendMessage(ID_SUCCESS,m_nextball[i],t);
				::SendMessage(this->GetParent()->m_hWnd,ID_PSUCCESS,m_nextball[i],t);
			}
			else
			{
				::SendMessage(this->GetParent()->m_hWnd,ID_PSUCCESS,m_nextball[i],t);
			}
		}
		else
			m_BakeHit = false;
	}
}

bool CChessView::IsMove(void)
{
	//判断是否具备移动的条件
	if(m_StartPos == ERROR_POINT || m_EndPos == ERROR_POINT )
	{
		AfxMessageBox("请设置开始点和结束点");
		return true;
	}

	//是否有可以到达的路径
	int tempchess[82];
	for(int i=1; i<82; i++)
	{
		tempchess[i] = m_Chess[i].color;
	}
	if(m_Logic.Allpairs(tempchess, m_Result, m_StartPos, m_EndPos))
		//如果有则返回
		return true;
	else
	{
		return false;
	}
}

// 释放炸弹
void CChessView::BobmFunc(BYTE point,BYTE bombarray[])
{
	//查找炸弹的范围
	m_Logic.BobmFunc(m_Chess,point,bombarray);
	//取消炸弹状态
	m_Bobm = false;
}

// 移动和发球
void CChessView::MoveBall(BYTE end, DBall scolor)
{	
	//移动之后清理一下状态
	m_StartPos = ERROR_POINT;
	m_EndPos = ERROR_POINT;
	
	//将小球放入移动的目标位置
	memcpy(&m_Chess[end],&scolor,sizeof(scolor));

	//如果可以成功消息，发送成功消息，不再发球
	int t = m_Logic.IsSuccess(m_Chess,m_SuccessArray,end);
	if(t)
	{
		if(!m_ExerciseMode)
		{
		::AfxGetMainWnd()->SendMessage(ID_SUCCESS,end,t);
		::SendMessage(this->GetParent()->m_hWnd,ID_PSUCCESS,end,t);
		}
		else
		{
			::SendMessage(this->GetParent()->m_hWnd,ID_PSUCCESS,end,t);
		}
	}
	else
	{
		m_BakeHit = false;

		//选出随机位置
		BallRandFuc(m_Chess,m_nextball,3);

		//发送发球消息
		if(!m_ExerciseMode)
		{
			::AfxGetMainWnd()->SendMessage(ID_TAKE,0,0);
			//将小球放入棋盘
			DisplayTakeTheService(m_nextball, m_nextballcolor);
			//选出下一轮小球的颜色
			TakeTheService();
		}
		else
		{
			//将小球放入棋盘
			DisplayTakeTheService(m_nextball, m_nextballcolor);
			//选出下一轮小球的颜色
			TakeTheService();
		}
	}
}

void CChessView::OnTimer(UINT nIDEvent)
{

}

// 棋盘空位置随机函数
bool CChessView::BallRandFuc(DBall  wChess[], BYTE  wNextBall[], int num)
{
	BYTE NullPos[81];
	int index=0;
	for(int i=1; i<82; i++)
	{
		if(wChess[i].color==0)
			NullPos[index++] = i;
	}
	int temp0=ERROR_POINT;
	int temp1=ERROR_POINT;
	
	for(int i=0; i<num;)
	{
		int pos = NullPos[rand()%index];
		
		if(temp0!=pos && temp1!=pos)
		{
			wNextBall[i++] = pos;
			if(i==1)
				temp0 = pos;
			if(i==2)
				temp1 = pos;
		}
	}
	return false;
}
