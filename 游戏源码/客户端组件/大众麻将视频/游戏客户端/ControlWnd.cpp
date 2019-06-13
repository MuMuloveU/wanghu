#include "StdAfx.h"
#include "GameLogic.h"
#include "ControlWnd.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////

//按钮标识
#define IDC_CHIHU					100									//吃胡按钮
#define IDC_LISTEN					101									//听牌按钮
#define IDC_GIVEUP					102									//放弃按钮

//位置标识
#define ITEM_WIDTH					90									//子项宽度
#define ITEM_HEIGHT					55									//子项高度
#define CONTROL_WIDTH				173									//控制宽度
#define CONTROL_HEIGHT				41									//控制高度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CControlWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CHIHU, OnChiHu)
	ON_BN_CLICKED(IDC_LISTEN, OnListen)
	ON_BN_CLICKED(IDC_GIVEUP, OnGiveUp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CControlWnd::CControlWnd()
{
	//配置变量
	m_cbActionMask=0;
	m_cbCenterCard=0;
	m_PointBenchmark.SetPoint(0,0);
	ZeroMemory(m_cbGangCard,sizeof(m_cbGangCard));

	//状态变量
	m_cbItemCount=0;
	m_cbCurrentItem=0xFF;

	//加载资源
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageControlTop.SetLoadInfo(IDB_CONTROL_TOP,hInstance);
	m_ImageControlMid.SetLoadInfo(IDB_CONTROL_MID,hInstance);
	m_ImageControlButtom.SetLoadInfo(IDB_CONTROL_BOTTOM,hInstance);
	m_ImageActionExplain.SetLoadInfo(IDB_ACTION_EXPLAIN,hInstance);

	return;
}

//析构函数
CControlWnd::~CControlWnd()
{
}

//基准位置
void CControlWnd::SetBenchmarkPos(int nXPos, int nYPos)
{
	//位置变量
	m_PointBenchmark.SetPoint(nXPos,nYPos);

	//调整控件
	RectifyControl();

	return;
}

//设置扑克
void CControlWnd::SetControlInfo(BYTE cbCenterCard, BYTE cbActionMask, tagGangCardResult & GangCardResult)
{
	//设置变量
	m_cbItemCount=0;
	m_cbCurrentItem=0xFF;
	m_cbActionMask=cbActionMask;
	m_cbCenterCard=cbCenterCard;

	//杠牌信息
	ZeroMemory(m_cbGangCard,sizeof(m_cbGangCard));
	for (BYTE i=0;i<GangCardResult.cbCardCount;i++) 
	{
		m_cbItemCount++;
		m_cbGangCard[i]=GangCardResult.cbCardData[i];
	}

	//计算数目
	BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};
	for (BYTE i=0;i<CountArray(cbItemKind);i++) 
		if ((m_cbActionMask&cbItemKind[i])!=0) 
			m_cbItemCount++;

	//按钮控制
	m_btChiHu.EnableWindow(cbActionMask&WIK_CHI_HU);
	m_btListen.EnableWindow(cbActionMask&WIK_LISTEN);


	//调整控件
	RectifyControl();

	//显示窗口
	ShowWindow(SW_SHOW);

	return;
}

//调整控件
void CControlWnd::RectifyControl()
{
	//设置位置
	CRect rcRect;
	rcRect.right=m_PointBenchmark.x;
	rcRect.bottom=m_PointBenchmark.y;
	rcRect.left=m_PointBenchmark.x-CONTROL_WIDTH;
	rcRect.top=m_PointBenchmark.y-ITEM_HEIGHT*m_cbItemCount-CONTROL_HEIGHT;
	if (m_cbItemCount>0)
		rcRect.top-=5;

	//移动窗口
	MoveWindow(&rcRect);

	//调整按钮
	CRect rcButton;
	m_btChiHu.GetWindowRect(&rcButton);
	int nYPos=rcRect.Height()-rcButton.Height()-9;
	m_btChiHu.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()*3-14,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btListen.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()*2-11,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);
	m_btGiveUp.SetWindowPos(NULL,rcRect.Width()-rcButton.Width()-8,nYPos,0,0,SWP_NOZORDER|SWP_NOSIZE);

	return;
}

//吃胡按钮
void CControlWnd::OnChiHu()
{
	AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_CHI_HU,0);
	return;
}

//听牌按钮
void CControlWnd::OnListen()
{
	AfxGetMainWnd()->PostMessage(IDM_LISTEN_CARD,0,0);
	return;
}

//放弃按钮
void CControlWnd::OnGiveUp()
{
	AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_NULL,0);
	return;
}

//重画函数
void CControlWnd::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC BufferDC;
	CBitmap BufferImage;
	BufferDC.CreateCompatibleDC(&dc);
	BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	BufferDC.SelectObject(&BufferImage);

	//加载资源
	CImageHandle HandleControlTop(&m_ImageControlTop);
	CImageHandle HandleControlMid(&m_ImageControlMid);
	CImageHandle HandleControlButtom(&m_ImageControlButtom);
	CImageHandle HandleActionExplain(&m_ImageActionExplain);

	//绘画背景
	m_ImageControlTop.BitBlt(BufferDC,0,0);
	for (int nImageYPos=m_ImageControlTop.GetHeight();nImageYPos<rcClient.Height();nImageYPos+=m_ImageControlMid.GetHeight())
	{
		m_ImageControlMid.BitBlt(BufferDC,0,nImageYPos);
	}
	m_ImageControlButtom.BitBlt(BufferDC,0,rcClient.Height()-m_ImageControlButtom.GetHeight());

	//变量定义
	int nYPos=5;
	BYTE cbCurrentItem=0;
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};

	//绘画扑克
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		if ((m_cbActionMask&cbItemKind[i])!=0)
		{
			//绘画扑克
			for (BYTE j=0;j<3;j++)
			{
				BYTE cbCardData=m_cbCenterCard;
				if (i<CountArray(cbExcursion))
					cbCardData=m_cbCenterCard+j-cbExcursion[i];
				g_CardResource.m_ImageTableBottom.DrawCardItem(&BufferDC,cbCardData,j*26+12,nYPos+5);
			}

			//计算位置
			int nXImagePos=0;
			int nItemWidth=m_ImageActionExplain.GetWidth()/7;
			if ((m_cbActionMask&cbItemKind[i])&WIK_PENG)
				nXImagePos=nItemWidth;

			//绘画标志
			int nItemHeight=m_ImageActionExplain.GetHeight();
			m_ImageActionExplain.BitBlt(BufferDC,126,nYPos+5,nItemWidth,nItemHeight,nXImagePos,0);

			//绘画边框
			if (cbCurrentItem==m_cbCurrentItem)
			{
				BufferDC.Draw3dRect(7,nYPos,rcClient.Width()-14,ITEM_HEIGHT,RGB(255,255,0),RGB(255,255,0));
			}

			//设置变量
			++cbCurrentItem;
			nYPos+=ITEM_HEIGHT;
		}
	}

	//杠牌扑克
	for (BYTE i=0;i<CountArray(m_cbGangCard);i++)
	{
		if (m_cbGangCard[i]!=0)
		{
			//绘画扑克
			for (BYTE j=0;j<4;j++)
			{
				g_CardResource.m_ImageTableBottom.DrawCardItem(&BufferDC,m_cbGangCard[i],j*26+12,nYPos+5);
			}

			//绘画边框
			if (cbCurrentItem==m_cbCurrentItem)
			{
				BufferDC.Draw3dRect(7,nYPos,rcClient.Width()-14,ITEM_HEIGHT,RGB(255,255,0),RGB(255,255,0));
			}

			//绘画标志
			int nItemWidth=m_ImageActionExplain.GetWidth()/7;
			int nItemHeight=m_ImageActionExplain.GetHeight();
			m_ImageActionExplain.BitBlt(BufferDC,126,nYPos+5,nItemWidth,nItemHeight,nItemWidth*3,0);

			//设置变量
			cbCurrentItem++;
			nYPos+=ITEM_HEIGHT;
		}
		else
			break;
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();

	return;
}

//建立消息
int CControlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//创建按钮
	CRect rcCreate(0,0,0,0);
	m_btListen.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_LISTEN);
	m_btGiveUp.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_GIVEUP);
	m_btChiHu.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_CHIHU);

	//设置位图
	m_btChiHu.SetButtonImage(IDB_BT_CHIHU,AfxGetInstanceHandle(),false);
	m_btListen.SetButtonImage(IDB_BT_LISTEN,AfxGetInstanceHandle(),false);
	m_btGiveUp.SetButtonImage(IDB_BT_GIVEUP,AfxGetInstanceHandle(),false);

	return 0;
}

//鼠标消息
void CControlWnd::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//索引判断
	if (m_cbCurrentItem!=0xFF)
	{
		//变量定义
		BYTE cbIndex=0;
		BYTE cbItemKind[4]={WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_PENG};

		//类型子项
		for (BYTE i=0;i<CountArray(cbItemKind);i++)
		{
			if (((m_cbActionMask&cbItemKind[i])!=0)&&(m_cbCurrentItem==cbIndex++))
			{
				AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,cbItemKind[i],m_cbCenterCard);
				return;
			}
		}

		//杠牌子项
		for (BYTE i=0;i<CountArray(m_cbGangCard);i++)
		{
			if ((m_cbGangCard[i]!=0)&&(m_cbCurrentItem==cbIndex++))
			{
				AfxGetMainWnd()->PostMessage(IDM_CARD_OPERATE,WIK_GANG,m_cbGangCard[i]);
				return;
			}
		}

		//错误断言
		ASSERT(FALSE);
	}

	return;
}

//光标消息
BOOL CControlWnd::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//位置测试
	if (m_cbItemCount!=0)
	{
		//获取位置
		CRect rcClient;
		CPoint MousePoint;
		GetClientRect(&rcClient);
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		
		//计算索引
		BYTE bCurrentItem=0xFF;
		CRect rcItem(5,5,rcClient.Width()-5,ITEM_HEIGHT*m_cbItemCount+5);

		if (rcItem.PtInRect(MousePoint))
			bCurrentItem=(BYTE)((MousePoint.y-7)/ITEM_HEIGHT);

		//设置索引
		if (m_cbCurrentItem!=bCurrentItem)
		{
			Invalidate(FALSE);
			m_cbCurrentItem=bCurrentItem;
		}

		//设置光标
		if (m_cbCurrentItem!=0xFF)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//////////////////////////////////////////////////////////////////////////
