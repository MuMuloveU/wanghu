#include "StdAfx.h"
#include "dxgameview.h"
#include "Resource.h"
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////

#define IDC_CHIP_IN				100	//下注按钮
#define IDC_CLEAR_CHIP			101	//清除下注

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDXGameView, CDxDialog)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHIP_IN,OnBnClickedChipIn)
	ON_BN_CLICKED(IDC_CLEAR_CHIP,OnBnClickedClearChip)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数 
CDXGameView::CDXGameView(void)
{
	//运行变量
	m_bShowEnd=0;
	m_wAllMoney=0;
	m_lMinBetScore=0;
	m_lMaxBetWin=0;
	m_wCurrentChip=50;
	m_bAllowBet=false;
	m_bZeroLight=false;
	m_bStatus=STATUS_FREE;
	ZeroMemory(m_wChipInfo,sizeof(m_wChipInfo));
	ZeroMemory(m_TipsMessage,sizeof(m_TipsMessage));
	ZeroMemory(m_bNumberLight,sizeof(m_bNumberLight));
	memset(m_bHistoryInfo,0xff,sizeof(m_bHistoryInfo));
	ZeroMemory(m_wHistoryChipInfo,sizeof(m_wHistoryChipInfo));
	ZeroMemory(m_wOtherUsersChips,sizeof(m_wOtherUsersChips));

	LONG lScoreIndextemp[JETTON_COUNT]={1L,5L,10L,50L,100L,500L};
	CopyMemory(lScoreIndex,lScoreIndextemp,sizeof(LONG)*6);
	//lScoreIndex[JETTON_COUNT]={1L,5L,10L,50L,100L,500L};

	m_bDrawMoveChips=false;
	m_iMoveChipsCount=0;
	m_bTimeLeave=0;

	//球控制
	m_vBallMovePath.x=0;
	m_vBallMovePath.y=0;
	m_vBallMovePath.z=0;
	m_fBallMoveAngel=0;
	m_bWinNumber=255;
	m_bTargetNumber=0;
	m_Ball_StartHeight=-310;
	m_Ball_MoveRadius=WIN_RADIUS;

	//贴图纹理
	m_pReflect=NULL;
	m_pBump=NULL;
}

//析构函数
CDXGameView::~CDXGameView(void)
{

}

//设置可选筹码
void CDXGameView::OnSetCellScore(long lCellScore)
{
	if(lCellScore<5)
	{
		LONG lScoreIndextemp[JETTON_COUNT]={1L,5L,10L,50L,100L,500L};
		CopyMemory(lScoreIndex,lScoreIndextemp,sizeof(LONG)*6);
	}
	if(lCellScore>=5&&lCellScore<10)
	{
		LONG lScoreIndextemp[JETTON_COUNT]={5L,10L,50L,100L,500L,1000L};
		CopyMemory(lScoreIndex,lScoreIndextemp,sizeof(LONG)*6);
	}
	if(lCellScore>10)
	{
		LONG lScoreIndextemp[JETTON_COUNT]={10L,50L,100L,500L,1000L,5000L};
		CopyMemory(lScoreIndex,lScoreIndextemp,sizeof(LONG)*6);
	}
	return;
}

//载入资源
void CDXGameView::InitScene(int nWidth,int nHeight)
{
	//创建按钮
	CRect CreateRect(0,0,0,0);
	if(!m_btChipIn.GetSafeHwnd()) m_btChipIn.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_CHIP_IN);
	if(!m_btClearChip.GetSafeHwnd()) m_btClearChip.Create(TEXT(""),WS_CHILD,CreateRect,this,IDC_CLEAR_CHIP);

	//设置按钮
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_btChipIn.SetButtonImage(IDB_CHIP_IN,hInstance,false);
	m_btClearChip.SetButtonImage(IDB_CLEAR_CHIP,hInstance,false);
	m_btChipIn.ShowWindow(SW_HIDE);
	m_btClearChip.ShowWindow(SW_HIDE);

	//结算框
	CRect rcCreate(0,0,0,0);
	if(!m_ScoreView.GetSafeHwnd()) m_ScoreView.Create(IDD_GAME_SCORE,this);

	//调整位置
	CRect rcButton;
	HDWP hDwp=BeginDeferWindowPos(32);
	m_btChipIn.GetWindowRect(&rcButton);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;
	DeferWindowPos(hDwp,m_btChipIn,NULL,nWidth/2-275-rcButton.Width()/2,nHeight/2+95-rcButton.Height()/2,0,0,uFlags);
	DeferWindowPos(hDwp,m_btClearChip,NULL,nWidth/2-196-rcButton.Width()/2,nHeight/2+157-rcButton.Height()/2,0,0,uFlags);
	EndDeferWindowPos(hDwp);

	//保存屏幕宽高
	m_nScreenWidth=nWidth;
	m_nScreenHeight=nHeight;
	m_GameLogic.SetDxWindowSize(m_nScreenWidth,m_nScreenHeight);

	try{
		//载入平面图片
		m_SurfaceTable.SetLoadInfo(g_pD3DDevice,"Round\\background.bmp");
		m_SurfaceBack.SetLoadInfo(g_pD3DDevice,"Round\\VIEW_BACK.bmp");
		m_NumberMask.SetLoadInfo(g_pD3DDevice,"Round\\light.png");

		m_Chip1.SetLoadInfo(g_pD3DDevice,"Round\\1.png");
		m_Chip5.SetLoadInfo(g_pD3DDevice,"Round\\5.png");
		m_Chip10.SetLoadInfo(g_pD3DDevice,"Round\\10.png");
		m_Chip50.SetLoadInfo(g_pD3DDevice,"Round\\50.png");
		m_Chip100.SetLoadInfo(g_pD3DDevice,"Round\\100.png");
		m_Chip500.SetLoadInfo(g_pD3DDevice,"Round\\500.png");
		m_Chip1000.SetLoadInfo(g_pD3DDevice,"Round\\1k.png");
		m_Chip5000.SetLoadInfo(g_pD3DDevice,"Round\\5k.png");
		m_Chip10000.SetLoadInfo(g_pD3DDevice,"Round\\10k.png");
		m_SprCurrentChip.SetLoadInfo(g_pD3DDevice,"Round\\hongquan.png");

		m_SprTimer[0].SetLoadInfo(g_pD3DDevice,"Round\\n0.png");
		m_SprTimer[1].SetLoadInfo(g_pD3DDevice,"Round\\n1.png");
		m_SprTimer[2].SetLoadInfo(g_pD3DDevice,"Round\\n2.png");
		m_SprTimer[3].SetLoadInfo(g_pD3DDevice,"Round\\n3.png");
		m_SprTimer[4].SetLoadInfo(g_pD3DDevice,"Round\\n4.png");
		m_SprTimer[5].SetLoadInfo(g_pD3DDevice,"Round\\n5.png");
		m_SprTimer[6].SetLoadInfo(g_pD3DDevice,"Round\\n6.png");
		m_SprTimer[7].SetLoadInfo(g_pD3DDevice,"Round\\n7.png");
		m_SprTimer[8].SetLoadInfo(g_pD3DDevice,"Round\\n8.png");
		m_SprTimer[9].SetLoadInfo(g_pD3DDevice,"Round\\n9.png");

		m_PlaneBall.SetLoadInfo(g_pD3DDevice,"Round\\xiaoqiu.png");
		m_PlatePlane.SetLoadInfo(g_pD3DDevice,"Round\\shuzi.png");
		m_PlatePlaneMask.SetLoadInfo(g_pD3DDevice,"Round\\PlaneMask.png");

		//载入模型
		m_vCenterPos.x=455;
		m_vCenterPos.y=-330;
		m_vCenterPos.z=456;
		m_MeshPlate.SetLoadInfo(g_pD3DDevice,"Round\\pan.x",true);
		m_MeshPlate.SetPosition(m_vCenterPos);
		m_MeshStock.SetLoadInfo(g_pD3DDevice,"Round\\waipan.x",true);
		m_MeshStock.SetPosition(m_vCenterPos);
		m_MeshShelfHigh.SetLoadInfo(g_pD3DDevice,"Round\\jiazi.x",true);
		m_MeshShelfHigh.SetPosition(m_vCenterPos);
		m_MeshShelfLow.SetLoadInfo(g_pD3DDevice,"Round\\jiazidi.x",true);
		m_MeshShelfLow.SetPosition(m_vCenterPos);
		m_MeshRing.SetLoadInfo(g_pD3DDevice,"Round\\huan.x",true);
		m_MeshRing.SetPosition(m_vCenterPos);
		m_MeshBall.SetLoadInfo(g_pD3DDevice,"Round\\ball.x",true);
		m_MeshBall.SetPosition(360,-310,515);

		//效果纹理
		//D3DXCreateTextureFromFile(g_pD3DDevice,TEXT("Round\\pan.bmp"),&m_pReflect);
		//D3DXCreateTextureFromFile(g_pD3DDevice,TEXT("Round\\jiazi.bmp"),&m_pBump);
		//m_MeshShelfHigh.SetBumpTexture(m_pBump);
		//m_MeshShelfHigh.SetReflectTexture(m_pReflect);
		//m_MeshShelfLow.SetReflectTexture(m_pReflect);
		
		//ID3DXEffect*      s_pEffect = NULL;   
		//D3DXCreateEffectFromFile( g_pD3DDevice, "Round\\test.fx", NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &s_pEffect, NULL );
		//s_pEffect->SetTexture(s_pEffect->GetParameterByName(NULL,"g_txCubeMap2"),m_pReflect);
	}catch(...)
	{
		StopRender();
		AfxMessageBox("资源加载失败，请尝试重新安装。");	
		AfxGetMainWnd()->SendMessage(WM_CLOSE);

		return;
	}

	//反锯齿 
	g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,TRUE);

	//开启灯光
	SetLight();	
}

//输入控制
void CDXGameView::UpDateControl()
{
	//设置焦点位置
	g_Camera.SetFocus(&D3DXVECTOR3(0,0,0));
	return;

	g_UserControl.UpdateInput();
	
	float mouseX,mouseY,mouseZ;	//鼠标状态
	float dSide=0,dForward=0;	//偏移变量

	//按住鼠标右键旋转镜头
	if(g_UserControl.m_MouseState.rgbButtons[1]&0x80)
	{
		mouseX=g_UserControl.GetLX()*D3DX_PI/720.0f;	//每移动一个点，方向改变1/4度
		mouseY=g_UserControl.GetLY()*D3DX_PI/720.0f;
		g_Camera.YawPitchAt(-mouseX,mouseY);
		dSide+=mouseX;
	}

	//鼠标滚轮控制摄象机距离
	mouseZ=g_UserControl.GetLZ()*D3DX_PI/10;
	g_Camera.Move(0,mouseZ);

	return;
}

//渲染函数
void CDXGameView::DrawGameView2D()
{
	//轮盘桌面与背景
	m_SurfaceBack.DrawSurface(0,0,1);
	m_SurfaceTable.DrawSurface(m_nScreenWidth/2-m_SurfaceTable.GetWidth()/2,m_nScreenHeight/2-m_SurfaceTable.GetHeight()/2); 

	//平面轮盘
	D3DXMATRIX matRot;
	D3DXMatrixIdentity(&matRot);
	float   iAngle=0;
	if((int)m_Ball_MoveRadius!=WIN_RADIUS&&m_bWinNumber==255) //旋转轮盘
	{
		iAngle=1.5f*timeGetTime()/500.0f;   
	}
	else if(m_bWinNumber!=255)	//转到胜利数字
	{
		for(int i=0;i<37;i++) if(m_bWinNumber==numberArrayIndex[i]) iAngle=i*(2*D3DX_PI/37);;
	}
	D3DXMatrixTransformation2D(	&matRot,0,0,0,&D3DXVECTOR2((float)m_PlatePlane.GetWidth()/2,(float)m_PlatePlane.GetHeight()/2),iAngle,&D3DXVECTOR2((float)m_nScreenWidth/2+309-m_PlatePlane.GetWidth()/2,(float)m_nScreenHeight/2-68-m_PlatePlane.GetHeight()/2));
	m_PlatePlane.DrawSprite(0,0,D3DXVECTOR3(0,0,0), CRect(0,0,584,584),&matRot);
	m_PlatePlaneMask.DrawSprite(m_nScreenWidth/2,m_nScreenHeight/2-m_PlatePlaneMask.GetHeight()/2,0xffffffff);
	if(m_bWinNumber!=255) m_PlaneBall.DrawSprite(m_nScreenWidth/2+206,m_nScreenHeight/2-226,0xffffffff);

	//是否显示清除下注按纽
	if(GetAllMoney()!=0&&m_bStatus==STATUS_FREE) m_btClearChip.ShowWindow(SW_SHOW);
	else	m_btClearChip.ShowWindow(SW_HIDE);
	if(m_bStatus==STATUS_FREE) m_btChipIn.ShowWindow(SW_SHOW);
	else	m_btChipIn.ShowWindow(SW_HIDE);

	//鼠标提示信息
	DxDrawText(m_TipsMessage,m_nScreenWidth/2+170,m_nScreenHeight/2-330,RGB(100,255,100));
	if(m_bStatus==STATUS_READY) DxDrawText(TEXT("等待开始..."),m_nScreenWidth/2+170,m_nScreenHeight/2-330,RGB(100,255,100));
	if(m_bStatus==STATUS_PLAY) DxDrawText(TEXT("正在旋转..."),m_nScreenWidth/2+170,m_nScreenHeight/2-330,RGB(100,255,100));

	//总注信息
	TCHAR szBuffer[256];
	ZeroMemory(szBuffer,sizeof(szBuffer));
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("最大单注赢：\n %d"),m_lMaxBetWin);
	DxDrawText(szBuffer,m_nScreenWidth/2+26,m_nScreenHeight/2-282,RGB(0,0,0));
	_snprintf(szBuffer,sizeof(szBuffer),TEXT("总下注： %d"),GetAllMoney());
	DxDrawText(szBuffer,m_nScreenWidth/2+26,m_nScreenHeight/2-249,RGB(0,0,0));

	//剩余时间信息
	if(m_bStatus!=STATUS_PLAY)
	{
		m_SprTimer[m_bTimeLeave/10].DrawSprite(m_nScreenWidth/2+45,m_nScreenHeight/2-200);
		m_SprTimer[m_bTimeLeave%10].DrawSprite(m_nScreenWidth/2+65,m_nScreenHeight/2-200);
	}

	//提示当前所选择的筹码
	for(BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		if(m_wCurrentChip==lScoreIndex[i])
		{
			int xPos=m_nScreenWidth/2 + (ChooseChipArea[i].BasePoint.x+ChooseChipArea[i].VertexButtom.x/2) -m_SprCurrentChip.GetWidth()/2;
			int yPos=m_nScreenHeight/2+ (ChooseChipArea[i].BasePoint.y+(ChooseChipArea[i].VertexButtom.y)/2) -m_SprCurrentChip.GetHeight()/2;
			m_SprCurrentChip.DrawSprite(xPos,yPos,0xffffffff);	
		}
	}
	//绘画选择筹码
	if(lScoreIndex[0]==1)
	{
		int xPos=m_nScreenWidth/2 + (ChooseChipArea[0].BasePoint.x+ChooseChipArea[0].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		int yPos=m_nScreenHeight/2+ (ChooseChipArea[0].BasePoint.y+(ChooseChipArea[0].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip1.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[1].BasePoint.x+ChooseChipArea[1].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[1].BasePoint.y+(ChooseChipArea[1].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip5.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[2].BasePoint.x+ChooseChipArea[2].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[2].BasePoint.y+(ChooseChipArea[2].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip10.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[3].BasePoint.x+ChooseChipArea[3].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[3].BasePoint.y+(ChooseChipArea[3].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip50.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[4].BasePoint.x+ChooseChipArea[4].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[4].BasePoint.y+(ChooseChipArea[4].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip100.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[5].BasePoint.x+ChooseChipArea[5].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[5].BasePoint.y+(ChooseChipArea[5].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip500.DrawSprite(xPos,yPos,0xffffffff);
	}
	else if(lScoreIndex[0]==5)
	{
	int	 xPos=m_nScreenWidth/2 + (ChooseChipArea[0].BasePoint.x+ChooseChipArea[0].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
	int	 yPos=m_nScreenHeight/2+ (ChooseChipArea[0].BasePoint.y+(ChooseChipArea[0].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip5.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[1].BasePoint.x+ChooseChipArea[1].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[1].BasePoint.y+(ChooseChipArea[1].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip10.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[2].BasePoint.x+ChooseChipArea[2].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[2].BasePoint.y+(ChooseChipArea[2].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip50.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[3].BasePoint.x+ChooseChipArea[3].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[3].BasePoint.y+(ChooseChipArea[3].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip100.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[4].BasePoint.x+ChooseChipArea[4].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[4].BasePoint.y+(ChooseChipArea[4].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip500.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[5].BasePoint.x+ChooseChipArea[5].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[5].BasePoint.y+(ChooseChipArea[5].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip1000.DrawSprite(xPos,yPos,0xffffffff);
	}
	else if(lScoreIndex[0]==10)
	{
	int	 xPos=m_nScreenWidth/2 + (ChooseChipArea[0].BasePoint.x+ChooseChipArea[0].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
	int	 yPos=m_nScreenHeight/2+ (ChooseChipArea[0].BasePoint.y+(ChooseChipArea[0].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip10.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[1].BasePoint.x+ChooseChipArea[1].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[1].BasePoint.y+(ChooseChipArea[1].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip50.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[2].BasePoint.x+ChooseChipArea[2].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[2].BasePoint.y+(ChooseChipArea[2].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip100.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[3].BasePoint.x+ChooseChipArea[3].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[3].BasePoint.y+(ChooseChipArea[3].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip500.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[4].BasePoint.x+ChooseChipArea[4].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[4].BasePoint.y+(ChooseChipArea[4].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip1000.DrawSprite(xPos,yPos,0xffffffff);

		xPos=m_nScreenWidth/2 + (ChooseChipArea[5].BasePoint.x+ChooseChipArea[5].VertexButtom.x/2) -m_Chip1.GetWidth()/2;
		yPos=m_nScreenHeight/2+ (ChooseChipArea[5].BasePoint.y+(ChooseChipArea[5].VertexButtom.y)/2) -m_Chip1.GetHeight()/2;
		m_Chip5000.DrawSprite(xPos,yPos,0xffffffff);	
	}

	//绘画数字区鼠标响应
	if(m_bZeroLight)
	{
	int	 xPos=m_nScreenWidth/2 + (Bet0Area.BasePoint.x+Bet0Area.VertexButtom.x/2) -m_NumberMask.GetWidth()/2-12;
	int	 yPos=m_nScreenHeight/2+ (Bet0Area.BasePoint.y+(Bet0Area.VertexButtom.y)/2) -m_NumberMask.GetHeight()/2-8;
		m_NumberMask.DrawSprite(xPos,yPos,0xffffffff);	
	}
	for(BYTE i=0;i<3;i++)for(BYTE j=0;j<12;j++)
	{
		//写这么多，只是为了让图形居中
		if(m_bNumberLight[i][j]) 
		{
		int	 xPos=m_nScreenWidth/2 + (BetNumberArea[i][j].BasePoint.x+BetNumberArea[i][j].VertexRight.x/2) -m_NumberMask.GetWidth()/2;
		int	 yPos=m_nScreenHeight/2+ (BetNumberArea[i][j].BasePoint.y+BetNumberArea[i][j].VertexTop.y+(BetNumberArea[i][j].VertexButtom.y-BetNumberArea[i][j].VertexTop.y)/2 ) -m_NumberMask.GetHeight()/2;
			m_NumberMask.DrawSprite(xPos,yPos,0xffffffff);	
		}
	}

	//绘画筹码信息
	GetChipsInfo();

	//绘画历史记录
	DrawHistory();

	//绘画结束信息
	if(m_bShowEnd==0)
	{
		if(m_ScoreView.GetSafeHwnd()) 
		{
			m_ScoreView.SetGameScore(0);
			m_ScoreView.ShowWindow(SW_HIDE);
		}
	}
	if(m_bShowEnd==1)
	{
		if(m_ScoreView.GetSafeHwnd()&&m_ScoreView.IsWindowVisible()==false) 
		{
			m_ScoreView.SetGameScore(m_lGameEndScore);
			m_ScoreView.ShowWindow(SW_SHOW);
		}
	}
	if(m_bShowEnd==2)
	{
		if(m_ScoreView.GetSafeHwnd()&&m_ScoreView.IsWindowVisible()==false)
		{
			m_ScoreView.SetGameScore(m_lGameEndScore);
			m_ScoreView.ShowWindow(SW_SHOW);
		}
	}

	if(m_bDrawMoveChips)
	{
		for(int i=0;i<MAX_CHIPS;i++)
		{
			if(m_SprMoveChips[i].IsLoaded())
				m_SprMoveChips[i].DrawMoveSprite();
		}
	}

	return;
}

//渲染函数
void CDXGameView::DrawGameView3D()
{	
	//渲染模型
	float scale=0.65f;
	float rotate=0.5f;

	//外面的盘
	m_MeshStock.SetRenderState((float)scale);
	m_MeshStock.Render();
	//数字盘
	m_MeshPlate.SetRenderState((float)scale,rotate);
	m_MeshPlate.Render();
	//中间的架子底座
	m_MeshShelfLow.SetRenderState((float)scale,rotate);
	m_MeshShelfLow.Render();
	//隔开数字的环
	m_MeshRing.SetRenderState((float)scale,rotate);
	m_MeshRing.Render();
	//中间的架子上部
	m_MeshShelfHigh.SetRenderState((float)scale,rotate);
	m_MeshShelfHigh.Render();	
	//球
	m_MeshBall.SetRenderState((float)scale);
	calcPath(&m_vBallMovePath);
	m_MeshBall.SetPosition(m_vBallMovePath);
	m_MeshBall.Render();

	return;
}

//计算和设定球的路径
void CDXGameView::calcPath(D3DXVECTOR3 *movePath)
{
	//有胜利数字 （此为防止设备丢失后球位置错乱）
	if(m_bWinNumber!=255)
	{
		m_Ball_MoveRadius=WIN_RADIUS;
		//取得当前轮盘角度
		D3DXMATRIX plateRot= m_MeshPlate.GetRotate();
		float x=plateRot._11;
		float z=plateRot._13;
		//当前轮盘与中心点夹角(已经旋转的度数)
		float currentPlatAngel=0;
		float PlatAngel=asin(z)/D3DX_PI*180;
		if(x>0&&z>0) currentPlatAngel=PlatAngel;	//第一象限
		if(x<0&&z>0) currentPlatAngel=180-PlatAngel;//第二象限
		if(x<0&&z<0) currentPlatAngel=180-PlatAngel;//第三象限
		if(x>0&&z<0) currentPlatAngel=360+PlatAngel;//第四象限

		//胜利数字弧度
		float iAngle=0;
		for(int i=0;i<37;i++) if(m_bWinNumber==numberArrayIndex[i]) iAngle=i*(2*D3DX_PI/37);
		//转换为角度
		iAngle=iAngle/D3DX_PI*180;

		float BallAngel=(float)(((int)(iAngle+currentPlatAngel+5))%360);
		BallAngel=BallAngel/180*D3DX_PI;

		x=m_Ball_MoveRadius*cosf(BallAngel)+455;
		z=m_Ball_MoveRadius*sinf(BallAngel)+456;

		movePath->x=x;
		movePath->y=-325;
		movePath->z=z;
		return;
	}

	//已经转动到最小半径位置
	if((int)m_Ball_MoveRadius==WIN_RADIUS) 
	{
		//当前球相对与轮盘中心点坐标
		float	x=movePath->x-m_vCenterPos.x;
		float	z=movePath->z-m_vCenterPos.z;

		//当前球与轮盘中心点夹角(未旋转时数字0位置是0度)
		float currentAngel=0;
		float Angel=asin(z/m_Ball_MoveRadius)/D3DX_PI*180;
		if( Angel != Angel || Angel < 0.0f ) Angel=0.0f;
		if(x>0&&z>0) currentAngel=Angel;	//第一象限
		if(x<0&&z>0) currentAngel=180-Angel;//第二象限
		if(x<0&&z<0) currentAngel=180-Angel;//第三象限
		if(x>0&&z<0) currentAngel=360+Angel;//第四象限	

		//取得当前轮盘角度
		D3DXMATRIX plateRot= m_MeshPlate.GetRotate();
		x=plateRot._11;
		z=plateRot._13;
		//当前轮盘与中心点夹角(已经旋转的度数)
		float currentPlatAngel=0;
		float PlatAngel=asin(z)/D3DX_PI*180;
		if( PlatAngel != PlatAngel || PlatAngel < 0.0f ) PlatAngel=0.0f;
		if(x>0&&z>0) currentPlatAngel=PlatAngel;	//第一象限
		if(x<0&&z>0) currentPlatAngel=180-PlatAngel;//第二象限
		if(x<0&&z<0) currentPlatAngel=180-PlatAngel;//第三象限
		if(x>0&&z<0) currentPlatAngel=360+PlatAngel;//第四象限

		//得到球与轮盘夹角。从这个值可以知道球停在了盘的哪个数字上
		float thisAngel=currentAngel-currentPlatAngel;
		if(thisAngel<0) thisAngel=thisAngel+360;

		//胜利数字
		if(m_bWinNumber==255)
		{	
			m_bWinNumber=GetBallStopNumber(thisAngel);

			//发送信息
			if(m_bStatus==STATUS_PLAY)
			{	
				AfxGetMainWnd()->SendMessage(IDM_WIN_NUMBER,(WPARAM)m_bWinNumber);
				m_bStatus=STATUS_FREE;
			}
		}
		return;
	}

	//分段路径
	//转圈
	if((int)m_Ball_MoveRadius>=FALL_RADIUS)
	{
		//转动半径
		m_Ball_MoveRadius-=0.01f;

		//转动角度与高度
		m_fBallMoveAngel+=0.05f;
		//if(m_fBallMoveAngel>2*D3DX_PI) m_fBallMoveAngel=0;
		//m_Ball_StartHeight-=0.02f;

		//计算坐标
		float x=m_Ball_MoveRadius*cosf(m_fBallMoveAngel)+455;
		float z=m_Ball_MoveRadius*sinf(m_fBallMoveAngel)+456;

		//设定坐标
		movePath->x=x;
		movePath->y=m_Ball_StartHeight;
		movePath->z=z;
		return;
	}
	//下落
	else if((int)m_Ball_MoveRadius<FALL_RADIUS&&(int)m_Ball_MoveRadius>=JUMP_RADIUS)
	{
		//转动半径
		m_Ball_MoveRadius-=0.5f;

		//转动角度与高度
		m_fBallMoveAngel+=0.03f;
		//if(m_fBallMoveAngel>2*D3DX_PI) m_fBallMoveAngel=0;
		m_Ball_StartHeight-=0.26f;

		//计算坐标
		float x=m_Ball_MoveRadius*cosf(m_fBallMoveAngel)+455;
		float z=m_Ball_MoveRadius*sinf(m_fBallMoveAngel)+456;

		//设定坐标
		movePath->x=x;
		movePath->y=m_Ball_StartHeight;
		movePath->z=z;
		return;
	}
	//跳跃
	else if((int)m_Ball_MoveRadius<JUMP_RADIUS&&(int)m_Ball_MoveRadius>=WIN_RADIUS)
	{
		//转动半径
		m_Ball_MoveRadius-=0.20f;

		//转动角度与高度
		m_fBallMoveAngel+=0.02f;
		//if(m_fBallMoveAngel>2*D3DX_PI) m_fBallMoveAngel=0;

		int iAngle=((int)(m_fBallMoveAngel/D3DX_PI*180))%10;
		if(iAngle>=0&&iAngle<=5) m_Ball_StartHeight-=3.6f+rand()%2;
		else m_Ball_StartHeight+=3.6f+rand()%2;

		//计算坐标
		float x=m_Ball_MoveRadius*cosf(m_fBallMoveAngel)+455;
		float z=m_Ball_MoveRadius*sinf(m_fBallMoveAngel)+456;

		//控制跳起高度
		if(m_Ball_StartHeight<-320) m_Ball_StartHeight=-320;
		if(m_Ball_StartHeight>-310) m_Ball_StartHeight=-310;

		//设定坐标
		movePath->x=x;
		movePath->y=m_Ball_StartHeight;
		movePath->z=z;
		return;
	}

	return;
}

//得到球所停数字
BYTE CDXGameView::GetBallStopNumber(float angel)
{
	float nn=(float)360/37;
	int xx=(int)(angel/nn);
	return numberArrayIndex[xx];
}

//设置灯光
void CDXGameView::SetLight()
{	
	//设置灯光
	D3DLIGHT9 d3dLight;
	D3DXVECTOR3 vecDir;
	ZeroMemory(&d3dLight, sizeof(d3dLight));

	//1号方向光
	d3dLight.Type = D3DLIGHT_DIRECTIONAL;
	d3dLight.Diffuse.r  = 0.2f;
	d3dLight.Diffuse.g  = 0.2f;
	d3dLight.Diffuse.b  = 0.2f;

	d3dLight.Specular.r =1.0f;
	d3dLight.Specular.g =1.0f;
	d3dLight.Specular.b =1.0f;

	vecDir = D3DXVECTOR3(9, -13, 11);
	D3DXVec3Normalize((D3DXVECTOR3*)&d3dLight.Direction,&vecDir);
	d3dLight.Range        = 5000.0f;
	g_pD3DDevice->SetLight(1, &d3dLight);
	g_pD3DDevice->LightEnable( 1, TRUE );
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT,0x00a0a0a0);

	////2号聚光灯
	//d3dLight.Type=D3DLIGHT_SPOT;
	//d3dLight.Diffuse.r  = 1.0f;
	//d3dLight.Diffuse.g  = 1.0f;
	//d3dLight.Diffuse.b  = 1.0f;

	//d3dLight.Specular.r =1.0f;
	//d3dLight.Specular.g =1.0f;
	//d3dLight.Specular.b =1.0f;
	////d3dLight.Attenuation0=0.5f;
	//d3dLight.Position=D3DXVECTOR3(455,30,456);
	//d3dLight.Direction=D3DXVECTOR3(0.0f,-1.0f,0.0f);
	//d3dLight.Range        = 1000.0f;
	//d3dLight.Theta=D3DXToRadian(10.0f);
	//d3dLight.Phi=D3DXToRadian(60.0f);
	//d3dLight.Falloff=5.0f;
	//g_pD3DDevice->SetLight(2, &d3dLight);
	//g_pD3DDevice->LightEnable( 2, TRUE );
	//g_pD3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

	g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
}

//释放资源
void CDXGameView::ReleaseScene()
{
	m_Chip1.Release();
	m_Chip5.Release();
	m_Chip10.Release();
	m_Chip50.Release();
	m_Chip100.Release();
	m_Chip500.Release();
	m_Chip1000.Release();
	m_Chip5000.Release();
	m_Chip10000.Release();
	m_SprCurrentChip.Release();

	for(int i=0;i<MAX_CHIPS;i++)
	{
		m_SprMoveChips[i].Release();
	}
	for(int i=0;i<10;i++)
	{
		m_SprTimer[i].Release();
	}
	m_PlatePlane.Release();
	m_PlatePlaneMask.Release();
	m_PlaneBall.Release();

	m_NumberMask.Release();
	m_SurfaceTable.Release();
	m_SurfaceBack.Release();

	m_MeshPlate.Release();
	m_MeshStock.Release();
	m_MeshShelfHigh.Release();
	m_MeshShelfLow.Release();
	m_MeshRing.Release();
	m_MeshBall.Release();

	SAFE_RELEASE(m_pReflect);
	SAFE_RELEASE(m_pBump);
}

//取回压注信息并绘画筹码
void CDXGameView::GetChipsInfo()
{
	//绘画筹码
	int x=0,y=0;		//下注区域
	int xPos=0,yPos=0;	//筹码位置
	
	long lAllChips[256];
	ZeroMemory(lAllChips,sizeof(lAllChips));
	if(m_bStatus==STATUS_PLAY||m_bStatus==STATUS_READY)
	{
		CopyMemory(lAllChips,m_wOtherUsersChips,sizeof(lAllChips));
	}
	else
	{	for(int i=0;i<256;i++)
		{

			lAllChips[i]=m_wChipInfo[i]+m_wOtherUsersChips[i];
		}
	}

	//透视关系的下注区域编号排列顺序
	int temp[256]=
	{
		//最上面0区
		200,
		//最里面一列
		206,3,132,6,135,9,138,12,141,15,144,18,147,21,150,24,153,27,156,30,159,33,162,36,
		205,92,55,95,58,98,61,101,64,104,67,107,70,110,73,113,76,116,79,119,82,122,85,125,
		204,2,131,5,134,8,137,11,140,14,143,17,146,20,149,23,152,26,155,29,158,32,161,35,
		203,91,54,94,57,97,60,100,63,103,66,106,69,109,72,112,75,115,78,118,81,121,84,124,
		202,1,130,4,133,7,136,10,139,13,142,16,145,19,148,22,151,25,154,28,157,31,160,34,
		201,90,53,93,56,96,59,99,62,102,65,105,68,108,71,111,74,114,77,117,80,120,83,123,
	};
	for(int i=0;i<256;i++)
	{
		int index=temp[i];
		if(lAllChips[index]!=0)
		{		
			if(index>0&&index<37)//数字区域
			{
				x=(index-1)%3,y=(index-1)/3;
				xPos=m_nScreenWidth/2 + (BetNumberArea[x][y].BasePoint.x+BetNumberArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
				yPos=m_nScreenHeight/2+ (BetNumberArea[x][y].BasePoint.y+BetNumberArea[x][y].VertexTop.y+(BetNumberArea[x][y].VertexButtom.y-BetNumberArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
				DrawChips(xPos,yPos,lAllChips[index]);
			}
			if(index>=50&&index<86)//十字相交区域
			{
				x=(index-50)%3,y=(index-50)/3;
				xPos=m_nScreenWidth/2 + (CrossBetArea[x][y].BasePoint.x+CrossBetArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
				yPos=m_nScreenHeight/2+ (CrossBetArea[x][y].BasePoint.y+CrossBetArea[x][y].VertexTop.y+(CrossBetArea[x][y].VertexButtom.y-CrossBetArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
				DrawChips(xPos,yPos,lAllChips[index]);
			}
			if(index>=90&&index<126)//横向相交区域
			{
				x=(index-90)%3,y=(index-90)/3;
				xPos=m_nScreenWidth/2 + (WideSideCrossArea[x][y].BasePoint.x+WideSideCrossArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
				yPos=m_nScreenHeight/2+ (WideSideCrossArea[x][y].BasePoint.y+WideSideCrossArea[x][y].VertexTop.y+(WideSideCrossArea[x][y].VertexButtom.y-WideSideCrossArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
				DrawChips(xPos,yPos,lAllChips[index]);
			}
			if(index>=130&&index<166)//竖向相交区域
			{
				x=(index-130)%3,y=(index-130)/3;
				xPos=m_nScreenWidth/2 + (HeightSideCrossArea[x][y].BasePoint.x+HeightSideCrossArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
				yPos=m_nScreenHeight/2+ (HeightSideCrossArea[x][y].BasePoint.y+HeightSideCrossArea[x][y].VertexTop.y+(HeightSideCrossArea[x][y].VertexButtom.y-HeightSideCrossArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
				DrawChips(xPos,yPos,lAllChips[index]);
			}
			if(index==200)//0区域
			{
				xPos=m_nScreenWidth/2 + (Bet0Area.BasePoint.x+Bet0Area.VertexButtom.x/2) -m_NumberMask.GetWidth()/2-8;
				yPos=m_nScreenHeight/2+ (Bet0Area.BasePoint.y+(Bet0Area.VertexButtom.y)/2) -m_NumberMask.GetHeight()/2+8;
				DrawChips(xPos,yPos,lAllChips[index]);
			}
			if(index>200&&index<207)//0相交区域
			{
				int x=index-201;
				xPos=m_nScreenWidth/2 + (Cross0Area[x].BasePoint.x+Cross0Area[x].VertexRight.x/2) -m_Chip10.GetWidth()/2;
				yPos=m_nScreenHeight/2+ (Cross0Area[x].BasePoint.y+Cross0Area[x].VertexTop.y+(Cross0Area[x].VertexButtom.y-Cross0Area[x].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
				DrawChips(xPos,yPos,lAllChips[index]);
			}

		}
	}
	//周边区域
	for(int k=37;k<49;k++)
	{
		if(lAllChips[k]>0)
		{
			switch(k)
			{
			case BET_IN_ERROR:
				break;
			case BET_IN_1TO12://第一打
				{
					xPos=m_nScreenWidth/2 + (BetDozenArea[0].BasePoint.x+BetDozenArea[0].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetDozenArea[0].BasePoint.y+BetDozenArea[0].VertexTop.y+(BetDozenArea[0].VertexButtom.y-BetDozenArea[0].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_1TO12]);
					break;
				}
			case BET_IN_13TO24://第二打
				{
					xPos=m_nScreenWidth/2 + (BetDozenArea[1].BasePoint.x+BetDozenArea[1].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetDozenArea[1].BasePoint.y+BetDozenArea[1].VertexTop.y+(BetDozenArea[1].VertexButtom.y-BetDozenArea[1].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_13TO24]);
					break;
				}
			case BET_IN_25TO36://第三打
				{
					xPos=m_nScreenWidth/2 + (BetDozenArea[2].BasePoint.x+BetDozenArea[2].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetDozenArea[2].BasePoint.y+BetDozenArea[2].VertexTop.y+(BetDozenArea[2].VertexButtom.y-BetDozenArea[2].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_25TO36]);
					break;
				}
			case BET_IN_1TO18://1~18区
				{
					xPos=m_nScreenWidth/2 + (Bet1to18Area.BasePoint.x+Bet1to18Area.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (Bet1to18Area.BasePoint.y+Bet1to18Area.VertexTop.y+(Bet1to18Area.VertexButtom.y-Bet1to18Area.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_1TO18]);
					break;
				}
			case BET_IN_19TO36://19~36区
				{
					xPos=m_nScreenWidth/2 + (Bet19to36Area.BasePoint.x+Bet19to36Area.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (Bet19to36Area.BasePoint.y+Bet19to36Area.VertexTop.y+(Bet19to36Area.VertexButtom.y-Bet19to36Area.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_19TO36]);
					break;
				}
			case BET_IN_RED://红色区
				{
					xPos=m_nScreenWidth/2 + (BetRedArea.BasePoint.x+BetRedArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetRedArea.BasePoint.y+BetRedArea.VertexTop.y+(BetRedArea.VertexButtom.y-BetRedArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_RED]);
					break;
				}
			case BET_IN_BLACK://黑色区
				{
					xPos=m_nScreenWidth/2 + (BetBlackArea.BasePoint.x+BetBlackArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetBlackArea.BasePoint.y+BetBlackArea.VertexTop.y+(BetBlackArea.VertexButtom.y-BetBlackArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_BLACK]);
					break;
				}
			case BET_IN_SINGLE://单数区
				{
					xPos=m_nScreenWidth/2 + (BetSingleArea.BasePoint.x+BetSingleArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetSingleArea.BasePoint.y+BetSingleArea.VertexTop.y+(BetSingleArea.VertexButtom.y-BetSingleArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_SINGLE]);
					break;
				}
			case BET_IN_DOUBLE://双数区
				{
					xPos=m_nScreenWidth/2 + (BetDoubleArea.BasePoint.x+BetDoubleArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetDoubleArea.BasePoint.y+BetDoubleArea.VertexTop.y+(BetDoubleArea.VertexButtom.y-BetDoubleArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_DOUBLE]);
					break;
				}
			case BET_IN_LINE1://第一列
				{
					xPos=m_nScreenWidth/2 + (BetLineArea[0].BasePoint.x+BetLineArea[0].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetLineArea[0].BasePoint.y+BetLineArea[0].VertexTop.y+(BetLineArea[0].VertexButtom.y-BetLineArea[0].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_LINE1]);
					break;
				}
			case BET_IN_LINE2://第二列
				{
					xPos=m_nScreenWidth/2 + (BetLineArea[1].BasePoint.x+BetLineArea[1].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetLineArea[1].BasePoint.y+BetLineArea[1].VertexTop.y+(BetLineArea[1].VertexButtom.y-BetLineArea[1].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_LINE2]);
					break;
				}
			case BET_IN_LINE3://第三列
				{
					xPos=m_nScreenWidth/2 + (BetLineArea[2].BasePoint.x+BetLineArea[2].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					yPos=m_nScreenHeight/2+ (BetLineArea[2].BasePoint.y+BetLineArea[2].VertexTop.y+(BetLineArea[2].VertexButtom.y-BetLineArea[2].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					DrawChips(xPos,yPos,lAllChips[BET_IN_LINE3]);
					break;
				}
			default:
				{
					break;
				}
			}
		}
	}

	return;
}

//取得压注金额
long CDXGameView::GetAllMoney()
{
	long allmoney=0;
	for(int i=0;i<MAX_BET_POS;i++)
	{
		if(m_wChipInfo[i]!=0&&i!=BET_IN_ERROR)
			allmoney+=m_wChipInfo[i];
	}
	return allmoney;
}
//取得历史下注
long CDXGameView::GetHistoryMoney()
{
	long allmoney=0;
	for(int i=0;i<MAX_BET_POS;i++)
	{
		if(m_wHistoryChipInfo[i]!=0&&i!=BET_IN_ERROR)
			allmoney+=m_wHistoryChipInfo[i];
	}
	return allmoney;
}

//鼠标移动消息
void CDXGameView::OnMouseMove(UINT nFlags,CPoint point)
{
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	//不响应
	if(!m_bAllowBet) return;

	//清除标志
	m_bZeroLight=false;
	ZeroMemory(m_TipsMessage,sizeof(m_TipsMessage));
	ZeroMemory(m_bNumberLight,sizeof(m_bNumberLight));
	
	//检测响应
	if(m_GameLogic.RespondMouseInRegionX(point,m_bNumberLight,m_bZeroLight,m_TipsMessage))
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
	return __super::OnMouseMove(nFlags,point);
}

//左键按下消息
void CDXGameView::OnLButtonDown(UINT nFlags,CPoint point)
{
	if(!m_bAllowBet) return __super::OnLButtonDown(nFlags,point);;

	//点击区域
	WORD index=m_GameLogic.GetMouseClickRegion(point);
	if(index==BET_IN_ERROR) return __super::OnLButtonDown(nFlags,point);

	//选择筹码区域
	if(index>=900)
	{
		m_wCurrentChip=lScoreIndex[index-900];
		return __super::OnLButtonDown(nFlags,point);
	}

	//超过用户最大分数
	if((GetAllMoney()+m_wCurrentChip)>m_lMaxUserScore)
	{
		AfxMessageBox("您的分数不足以购买此筹码。");
		return __super::OnLButtonDown(nFlags,point);
	}

	//限制下注
	//单一数字区
	if(index>=1&&index<=36)
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/36))
		{
			m_wChipInfo[index]=m_lMaxBetWin/36;
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/36);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注500"),sizeof(TCHAR)*64);
		}
	}
	//列打区域
	if(index>=37&&index<=42) 
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/3))
		{
			m_wChipInfo[index]=m_lMaxBetWin/3;
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/3);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注8750"),sizeof(TCHAR)*64);
		}
	}
	//1~18或19~36单双，红黑
	if(index>=43&&index<=48)
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>m_lMaxBetWin/2)
		{
			m_wChipInfo[index]=m_lMaxBetWin/2;
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/2);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注17500"),sizeof(TCHAR)*64);
		}
	}
	//十字交叉最左边
	if(index>=53&&index<=83&&(index-53)%3==0)
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/6))
		{
			m_wChipInfo[index]=m_lMaxBetWin/6;
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/6);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注3500"),sizeof(TCHAR)*64);
		}
	}
	//十字交叉
	if(index>=53&&index<=85&&(index-53)%3!=0)
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/9))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/9);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/9);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注2187"),sizeof(TCHAR)*64);
		}
	}
	//竖相交边最左边
	if(index>=90&&index<=123&&index%3==0)
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/12))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/12);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/12);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注1590"),sizeof(TCHAR)*64);
		}
	}
	//竖相交边
	if(index>=90&&index<=125&&index%3!=0)
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/18))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/18);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/18);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注1030"),sizeof(TCHAR)*64);
		}
	}
	//横相交边
	if(index>=130&&index<=162)	
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/18))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/18);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/18);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注1030"),sizeof(TCHAR)*64);
		}
	}
	//零区域
	if(index==200)	
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/36))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/36);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/36);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注500"),sizeof(TCHAR)*64);
		}
	}
	if(index==201)		
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/9))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/9);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/9);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注2187"),sizeof(TCHAR)*64);
		}
	}
	if(index==203||index==205)		
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/12))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/12);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/12);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注1590"),sizeof(TCHAR)*64);
		}
	}
	if(index==202||index==204||index==206)	
	{
		if((m_wChipInfo[index]+m_wCurrentChip)>(m_lMaxBetWin/18))
		{
			m_wChipInfo[index]=(m_lMaxBetWin/18);
			_snprintf(m_TipsMessage,sizeof(TCHAR)*64,TEXT("达到最大下注 %d"),m_lMaxBetWin/18);
			return __super::OnLButtonDown(nFlags,point);
			//lstrcpyn(m_TipsMessage,TEXT("达到最大下注1030"),sizeof(TCHAR)*64);
		}
	}

	//下注区域
	m_wChipInfo[index]+=m_wCurrentChip;

	return __super::OnLButtonDown(nFlags,point);
}

//右键按下消息
void CDXGameView::OnRButtonDown(UINT nFlags,CPoint point)
{
	if(!m_bAllowBet) return;

	//减少下注
	WORD index=m_GameLogic.GetMouseClickRegion(point);

	//选择筹码区域
	if(index>=900) return __super::OnRButtonDown(nFlags,point);

	//下注区域
	m_wChipInfo[index]-=m_wCurrentChip;
	if(m_wChipInfo[index]<0) m_wChipInfo[index]=0;

	return __super::OnRButtonDown(nFlags,point);
}

//确认下注
void CDXGameView::OnBnClickedChipIn()
{
	//读取历史信息
	if(GetHistoryMoney()>0&&GetAllMoney()==0)
	{
		//判断是否够分数重复下注
		if(GetHistoryMoney()>m_lMaxUserScore)
		{
			AfxMessageBox("您的筹码不足以重复上轮的注额，请选择下注。");
			return;
		}
		CopyMemory(m_wChipInfo,m_wHistoryChipInfo,sizeof(m_wChipInfo));
	}

	//未下注不开
	if(GetAllMoney()==0)
	{
		AfxMessageBox("请先下注。");
		return;
	}

	//判断有无位置下注小于最低下注
	for(int i=0;i<MAX_BET_POS;i++)
	{
		//有下注
		if(m_wChipInfo[i]>0)
		{
			//小于最低下注
			if(m_wChipInfo[i]<m_lMinBetScore)
			{
				TCHAR szBuffer[64];ZeroMemory(szBuffer,sizeof(szBuffer));
				_snprintf(szBuffer,sizeof(TCHAR)*64,TEXT("某个下注位置筹码小于最低下注 %ld ，请增加筹码。"),m_lMinBetScore);
				AfxMessageBox(szBuffer);
				return;
			}
		}
	}

	//发送下注信息
	AfxGetMainWnd()->SendMessage(IDM_CHIP_IN,(WPARAM)m_wChipInfo);

	//设置变量
	m_bShowEnd=0;
	m_bAllowBet=false;
	m_bStatus=STATUS_READY;
		
	m_bZeroLight=false;
	ZeroMemory(m_TipsMessage,sizeof(m_TipsMessage));
	ZeroMemory(m_bNumberLight,sizeof(m_bNumberLight));

	//保存到历史信息
	CopyMemory(m_wHistoryChipInfo,m_wChipInfo,sizeof(m_wChipInfo));

	m_btChipIn.ShowWindow(SW_HIDE);
	m_btClearChip.ShowWindow(SW_HIDE);
	m_ScoreView.ShowWindow(SW_HIDE);
	//m_Ball_MoveRadius=MAX_MOVE_RADIUS;

	return;
}

//清除下注
void CDXGameView::OnBnClickedClearChip()
{
	ClearAllMoney();
	return ;
}

//清除下注
void CDXGameView::ClearAllMoney()
{
	for(int i=0;i<MAX_BET_POS;i++)
	{
		m_wChipInfo[i]=0;
	}
	return;
}

//开始旋转
void CDXGameView::OnGameStart(BYTE bTargetNumber)
{
	//设置变量
	m_bWinNumber=255;
	m_bStatus=STATUS_PLAY;
	m_Ball_MoveRadius=MAX_MOVE_RADIUS;
	m_Ball_StartHeight=-310;
	//m_fBallMoveAngel=0.;
	m_bAllowBet=false;
	m_bTargetNumber=bTargetNumber;

	//清除标志
	m_bZeroLight=false;
	ZeroMemory(m_TipsMessage,sizeof(m_TipsMessage));
	ZeroMemory(m_bNumberLight,sizeof(m_bNumberLight));

	//取得当前轮盘角度
	D3DXMATRIX plateRot= m_MeshPlate.GetRotate();
	float x=plateRot._11;
	float z=plateRot._13;
	//当前轮盘与中心点夹角(已经旋转的度数)
	float currentPlatAngel=0;
	float PlatAngel=asin(z)/D3DX_PI*180;
	if(x>0&&z>0) currentPlatAngel=PlatAngel;	//第一象限
	if(x<0&&z>0) currentPlatAngel=180-PlatAngel;//第二象限
	if(x<0&&z<0) currentPlatAngel=180-PlatAngel;//第三象限
	if(x>0&&z<0) currentPlatAngel=360+PlatAngel;//第四象限

	//转到胜利数字
	float iAngle=0;
	for(int i=0;i<37;i++)
	{
		if(m_bTargetNumber==numberArrayIndex[i]) iAngle=i*(2*D3DX_PI/37);;
	}
	m_fBallMoveAngel=iAngle+(2*D3DX_PI/37)*4+((currentPlatAngel/180)*D3DX_PI);
	//if(m_fBallMoveAngel>2*D3DX_PI) m_fBallMoveAngel=m_fBallMoveAngel/(2*D3DX_PI);


	return;
}
//所有下注
void CDXGameView::OnUserChip(long lAllChip[256])
{
	CopyMemory(m_wOtherUsersChips,lAllChip,sizeof(m_wOtherUsersChips));
	return;
}
//结束旋转
void CDXGameView::OnRollStop(BYTE bWinNumber)
{
	m_bWinNumber=bWinNumber;
	m_bStatus=STATUS_FREE;
	AfxGetMainWnd()->SendMessage(IDM_GET_STOP);
}
//游戏结束
void CDXGameView::OnGameEnd()
{
	//设置移动筹码
	for(int i=0;i<MAX_BET_POS;i++)
	{
		if(m_wChipInfo[i]>0)
		{
			long temp[256];ZeroMemory(temp,sizeof(temp));
			temp[i]=m_wChipInfo[i];
			long lScore=m_GameLogic.ComputeScore(temp,m_bWinNumber);
			//此处得分
			if(lScore>0)
			{
				//AfxMessageBox("sdfd");
				CPoint point(0,0);
				GetChipPosition(i,&point);
				m_bDrawMoveChips=true;
				SetMoveChips(point.x,point.y,m_wChipInfo[i]);
			}
		}
	}

	m_bStatus=STATUS_FREE;
	ZeroMemory(m_wChipInfo,sizeof(m_wChipInfo));
	ZeroMemory(m_wOtherUsersChips,sizeof(m_wOtherUsersChips));
}

//取得筹码位置
void CDXGameView::GetChipPosition(int index,CPoint *pPosition)
{
	int x=0,y=0;
	if(index>0&&index<37)//单个数字区域
	{
		x=(index-1)%3,y=(index-1)/3;
		pPosition->x=m_nScreenWidth/2 + (BetNumberArea[x][y].BasePoint.x+BetNumberArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
		pPosition->y=m_nScreenHeight/2+ (BetNumberArea[x][y].BasePoint.y+BetNumberArea[x][y].VertexTop.y+(BetNumberArea[x][y].VertexButtom.y-BetNumberArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
		return;
	}
	if(index>=50&&index<86)//十字相交区域
	{
		x=(index-50)%3,y=(index-50)/3;
		pPosition->x=m_nScreenWidth/2 + (CrossBetArea[x][y].BasePoint.x+CrossBetArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
		pPosition->y=m_nScreenHeight/2+ (CrossBetArea[x][y].BasePoint.y+CrossBetArea[x][y].VertexTop.y+(CrossBetArea[x][y].VertexButtom.y-CrossBetArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
		return;
	}
	if(index>=90&&index<126)//横向相交区域
	{
		x=(index-90)%3,y=(index-90)/3;
		pPosition->x=m_nScreenWidth/2 + (WideSideCrossArea[x][y].BasePoint.x+WideSideCrossArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
		pPosition->y=m_nScreenHeight/2+ (WideSideCrossArea[x][y].BasePoint.y+WideSideCrossArea[x][y].VertexTop.y+(WideSideCrossArea[x][y].VertexButtom.y-WideSideCrossArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
		return;
	}
	if(index>=130&&index<166)//竖向相交区域
	{
		x=(index-130)%3,y=(index-130)/3;
		pPosition->x=m_nScreenWidth/2 + (HeightSideCrossArea[x][y].BasePoint.x+HeightSideCrossArea[x][y].VertexRight.x/2) -m_Chip10.GetWidth()/2;
		pPosition->y=m_nScreenHeight/2+ (HeightSideCrossArea[x][y].BasePoint.y+HeightSideCrossArea[x][y].VertexTop.y+(HeightSideCrossArea[x][y].VertexButtom.y-HeightSideCrossArea[x][y].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
		return;
	}
	if(index==200)//0区域
	{
		pPosition->x=m_nScreenWidth/2 + (Bet0Area.BasePoint.x+Bet0Area.VertexButtom.x/2) -m_NumberMask.GetWidth()/2-8;
		pPosition->y=m_nScreenHeight/2+ (Bet0Area.BasePoint.y+(Bet0Area.VertexButtom.y)/2) -m_NumberMask.GetHeight()/2+8;
		return;
	}
	if(index>200&&index<207)//0相交区域
	{
		int x=index-201;
		pPosition->x=m_nScreenWidth/2 + (Cross0Area[x].BasePoint.x+Cross0Area[x].VertexRight.x/2) -m_Chip10.GetWidth()/2;
		pPosition->y=m_nScreenHeight/2+ (Cross0Area[x].BasePoint.y+Cross0Area[x].VertexTop.y+(Cross0Area[x].VertexButtom.y-Cross0Area[x].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
		return;
	}

	////周边区域
	//for(int k=37;k<49;k++)
	//{
	//	if(m_wChipInfo[k]>0)
	//	{
	//		switch(k)
	//		{
	//		case BET_IN_ERROR:
	//			break;
			if(index== BET_IN_1TO12)//第一打
				{
					pPosition->x=m_nScreenWidth/2 + (BetDozenArea[0].BasePoint.x+BetDozenArea[0].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetDozenArea[0].BasePoint.y+BetDozenArea[0].VertexTop.y+(BetDozenArea[0].VertexButtom.y-BetDozenArea[0].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_13TO24)//第二打
				{
					pPosition->x=m_nScreenWidth/2 + (BetDozenArea[1].BasePoint.x+BetDozenArea[1].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetDozenArea[1].BasePoint.y+BetDozenArea[1].VertexTop.y+(BetDozenArea[1].VertexButtom.y-BetDozenArea[1].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_25TO36)//第三打
				{
					pPosition->x=m_nScreenWidth/2 + (BetDozenArea[2].BasePoint.x+BetDozenArea[2].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetDozenArea[2].BasePoint.y+BetDozenArea[2].VertexTop.y+(BetDozenArea[2].VertexButtom.y-BetDozenArea[2].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_1TO18)//1~18区
				{
					pPosition->x=m_nScreenWidth/2 + (Bet1to18Area.BasePoint.x+Bet1to18Area.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (Bet1to18Area.BasePoint.y+Bet1to18Area.VertexTop.y+(Bet1to18Area.VertexButtom.y-Bet1to18Area.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_19TO36)//19~36区
				{
					pPosition->x=m_nScreenWidth/2 + (Bet19to36Area.BasePoint.x+Bet19to36Area.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (Bet19to36Area.BasePoint.y+Bet19to36Area.VertexTop.y+(Bet19to36Area.VertexButtom.y-Bet19to36Area.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_RED)//红色区
				{
					pPosition->x=m_nScreenWidth/2 + (BetRedArea.BasePoint.x+BetRedArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetRedArea.BasePoint.y+BetRedArea.VertexTop.y+(BetRedArea.VertexButtom.y-BetRedArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_BLACK)//黑色区
				{
					pPosition->x=m_nScreenWidth/2 + (BetBlackArea.BasePoint.x+BetBlackArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetBlackArea.BasePoint.y+BetBlackArea.VertexTop.y+(BetBlackArea.VertexButtom.y-BetBlackArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_SINGLE)//单数区
				{
					pPosition->x=m_nScreenWidth/2 + (BetSingleArea.BasePoint.x+BetSingleArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetSingleArea.BasePoint.y+BetSingleArea.VertexTop.y+(BetSingleArea.VertexButtom.y-BetSingleArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_DOUBLE)//双数区
				{
					pPosition->x=m_nScreenWidth/2 + (BetDoubleArea.BasePoint.x+BetDoubleArea.VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetDoubleArea.BasePoint.y+BetDoubleArea.VertexTop.y+(BetDoubleArea.VertexButtom.y-BetDoubleArea.VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_LINE1)//第一列
				{
					pPosition->x=m_nScreenWidth/2 + (BetLineArea[0].BasePoint.x+BetLineArea[0].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetLineArea[0].BasePoint.y+BetLineArea[0].VertexTop.y+(BetLineArea[0].VertexButtom.y-BetLineArea[0].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_LINE2)//第二列
				{
					pPosition->x=m_nScreenWidth/2 + (BetLineArea[1].BasePoint.x+BetLineArea[1].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetLineArea[1].BasePoint.y+BetLineArea[1].VertexTop.y+(BetLineArea[1].VertexButtom.y-BetLineArea[1].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}
			if(index==  BET_IN_LINE3)//第三列
				{
					pPosition->x=m_nScreenWidth/2 + (BetLineArea[2].BasePoint.x+BetLineArea[2].VertexRight.x/2) -m_Chip10.GetWidth()/2;
					pPosition->y=m_nScreenHeight/2+ (BetLineArea[2].BasePoint.y+BetLineArea[2].VertexTop.y+(BetLineArea[2].VertexButtom.y-BetLineArea[2].VertexTop.y)/2 ) -m_Chip10.GetHeight()/2;
					return;
				}

	return;
}

//历史数字
void CDXGameView::SetHistoryNumber(BYTE bHistoryInfo[])
{
	CopyMemory(m_bHistoryInfo,bHistoryInfo,sizeof(m_bHistoryInfo));
	return;
}

//绘画历史记录
void CDXGameView::DrawHistory()
{
	//初始位置
	int StartX,StartY,x,y;
	x=StartX=m_nScreenWidth/2+325;
	y=StartY=m_nScreenHeight/2-275;

	//文字颜色
	COLORREF cTextColor=RGB(100,255,100);
	//绘画历史数字
	for(BYTE i=0;i<HISTORY_NUMBER;i++)
	{
		//绘画结束
		if(m_bHistoryInfo[i]==0xff) break;

		//判断位置
		BYTE color=m_GameLogic.GetNumberColor(m_bHistoryInfo[i]);
		//红色数字
		if(color==COLOR_RED) 
		{
			x=StartX;
			cTextColor=RGB(100,255,100);
		}
		//数字0
		if(color==COLOR_0)
		{
			x=StartX+18;
		}
		//黑色数字
		if(color==COLOR_BLACK)
		{
			x=StartX+36;
			cTextColor=RGB(255,0,100);
		}

		//转换文字
		CString nText;
		nText.Format("%d",m_bHistoryInfo[i]);
		DxDrawText(nText,x,y,cTextColor);

		//下移绘画
		y+=15;
	}
}

//绘画筹码
void CDXGameView::DrawChips(int x,int y,long money)
{
	long lScoreCount=money;
	//筹码种类
	LONG lScoreIndex[JETTON_COUNT]={1L,5L,10L,50L,100L,500L,1000L,5000L,10000L};

	//增加筹码
	for (BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		//计算数目
		BYTE cbScoreIndex=JETTON_COUNT-i-1;
		LONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//插入过虑
		if (lCellCount==0L) continue;

		//加入筹码
		for (LONG j=0;j<lCellCount;j++)
		{
			switch(cbScoreIndex)
			{
			case 0:{	m_Chip1.DrawSprite(x,y-=4,0xffffffff);	break;	}
			case 1:{	m_Chip5.DrawSprite(x,y-=4,0xffffffff);	break;	}
			case 2:{	m_Chip10.DrawSprite(x,y-=4,0xffffffff);	break;	}
			case 3:{	m_Chip50.DrawSprite(x,y-=4,0xffffffff);	break;	}
			case 4:{	m_Chip100.DrawSprite(x,y-=4,0xffffffff);break;	}
			case 5:{	m_Chip500.DrawSprite(x,y-=4,0xffffffff);break;	}
			case 6:{	m_Chip1000.DrawSprite(x,y-=4,0xffffffff);break;	}
			case 7:{	m_Chip5000.DrawSprite(x,y-=4,0xffffffff);break;	}
			case 8:{	m_Chip10000.DrawSprite(x,y-=4,0xffffffff);break;}
			default:break;
			}
		}

		//减少数目
		lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
	}

	return;
}

//绘画筹码移动
void CDXGameView::SetMoveChips(int x,int y,long money)
{
	long lScoreCount=money;
	//筹码种类
	LONG lScoreIndex[JETTON_COUNT]={1L,5L,10L,50L,100L,500L,1000L,5000L,10000L};
	
	//设置路径
	int startX=0,startY=0,stopX=0,stopY=0;
	startX=x;
	startY=y;
	stopX=WIN_CHIP_POS_X;
	stopY=WIN_CHIP_POS_Y;

	//增加筹码
	for (BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		//计算数目
		BYTE cbScoreIndex=JETTON_COUNT-i-1;
		LONG lCellCount=lScoreCount/lScoreIndex[cbScoreIndex];

		//插入过虑
		if (lCellCount==0L) continue;

		//加入筹码
		for (LONG j=0;j<lCellCount;j++)
		{
			switch(cbScoreIndex)
			{
			case 0:
			{
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\1.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;	
			}
			case 1:
			{
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\5.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;	
			}
			case 2:
			{
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\10.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;	
			}
			case 3:
			{	
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\50.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;
			}
			case 4:
			{
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\100.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;	
			}
			case 5:
			{
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\500.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;	
			}
			case 6:
			{	
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\1k.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;	
			}
			case 7:
			{	
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\5k.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;
			}
			case 8:
			{
				m_SprMoveChips[m_iMoveChipsCount].SetLoadInfo(g_pD3DDevice,TEXT("Round\\10k.png")); 
				m_SprMoveChips[m_iMoveChipsCount].SetMovePath(startX,startY-=4,stopX,stopY-=4);	
				m_iMoveChipsCount++;
				break;
			}
			default:
				break;
			}
		}

		//减少数目
		lScoreCount-=lCellCount*lScoreIndex[cbScoreIndex];
	}

	return;
}