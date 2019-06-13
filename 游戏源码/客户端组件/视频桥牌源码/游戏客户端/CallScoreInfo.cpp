#include "StdAfx.h"
#include "CallScoreInfo.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCallScoreInfo, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCallScoreInfo::CCallScoreInfo()
{
	
	//设置数据
	ZeroMemory(m_lScore,sizeof(m_lScore));
	ZeroMemory(m_lColor,sizeof(m_lColor));
	ZeroMemory(m_szUserName,sizeof(m_szUserName));
	
	//加载资源
	m_ImageBack.SetLoadInfo(IDB_CALL_SCORE,AfxGetInstanceHandle());
	m_ImageColorFlag.SetLoadInfo(IDB_COLOR_FLAG,AfxGetInstanceHandle());

	return;
}

//析构函数
CCallScoreInfo::~CCallScoreInfo()
{
}

//建立消息
int CCallScoreInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//移动窗口
	CImageHandle ImageHandle(&m_ImageBack);
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth(),m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	return 0;
}

//鼠标消息
void CCallScoreInfo::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//重画函数
void CCallScoreInfo::OnPaint() 
{
	CPaintDC dc(this); 

	//加载资源
	CImageHandle HandleBack(&m_ImageBack);
	
	//绘画背景
	m_ImageBack.BitBlt(dc,0,0);


	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,160));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	BYTE bNum=0;
	
	//提示信息
	TCHAR szBuffer[128]=TEXT("");
	//显示分数
	for (WORD i=0;i<CountArray(m_szUserName);i++)
	{
		if (m_szUserName[i][0]!=0)
		{
			WORD k=i;
			//if (i==1) k=3;
			//if (i==3) k=1;
			//输出名字
			CRect rcName(k*40+10,10,k*40+50,60);
			dc.DrawText(m_szUserName[i],lstrlen(m_szUserName[i]),&rcName,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			
			//输出分数
			for (WORD j=0;j<60;j++)
			{
				if(m_lScore[i][j]!=0) 
				{
					CRect rcScore(k*40+2,(j+1)*19+10,k*40+50,(j+1)*19+60);
					switch (m_lScore[i][j])
					{
					case 200:
						{
							_snprintf(szBuffer,sizeof(szBuffer)," X");
							break;
						}
						
					case 210:
						{
							_snprintf(szBuffer,sizeof(szBuffer)," XX");
							break;
						}
					case 240:
						{
							_snprintf(szBuffer,sizeof(szBuffer)," --");
							break;
						}
					default:
						{
							if (m_lScore[i][j]!=250)
							{
								_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lScore[i][j]);
							}
							else
							{
								_snprintf(szBuffer,sizeof(szBuffer)," PASS");
							}
							break;
						}
					}
					dc.DrawText(szBuffer,lstrlen(szBuffer),&rcScore,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
					
					//255放弃 200加倍 210再加倍
					if (m_lColor[i][j]!=255&&m_lColor[i][j]!=200&&m_lColor[i][j]!=210&&m_lColor[i][j]!=240)
					{
						//加载资源
						CImageHandle HandleColorFlag(&m_ImageColorFlag);
						int nColorImageWidth=m_ImageColorFlag.GetWidth()/6;
						int nColorImageHeight=m_ImageColorFlag.GetHeight();

						//绘画标志
						int nXPos=k*40+30;
						int nYPos=(j+1)*19+27;
						m_ImageColorFlag.AlphaDrawImage(&dc,nXPos,nYPos,nColorImageWidth,nColorImageHeight,nColorImageWidth*(m_lColor[i][j]>>4),0,RGB(255,0,255));
					}
				
				}
			}
		}
	}

	return;
}

//设置积分
void CCallScoreInfo::SetCallGameInfo(WORD wChairID, BYTE bColor,BYTE bScore)
{
	if (wChairID==INVALID_CHAIR)
	{
		for(WORD i=0;i<4;i++)
		{
			for (WORD j=0;j<60;j++)
			{
				m_lScore[i][j]=0;
				m_lColor[i][j]=255;
			}
		}
	}
	else
	{
		for(WORD i=0;i<4;i++)
		{
			if (i==wChairID)
			{
				for (WORD j=0;j<60;j++)
				{
					if(m_lScore[i][j]==0) 
					{
						m_lScore[i][j]=bScore;
						m_lColor[i][j]=bColor;
						break;
					}
				}
			}
		}
	}
		
	//刷新界面
	Invalidate(FALSE);
	return;
}

//设置用户名称
void CCallScoreInfo::SetCallUserName(WORD wChairID,LPCTSTR pszUserName)
{
	lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));
	//刷新界面
	Invalidate(FALSE);
	return;
}

//////////////////////////////////////////////////////////////////////////
