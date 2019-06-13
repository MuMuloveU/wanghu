// MyListView.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "MyListView.h"
#include "GlobalOption.h"
#include ".\mylistview.h"
#include "GameClientView.h"

/////////////////////////////////////////////////////////////////////////
//聊天控制
#define			ID_EDIT					101		//消息框
#define			IDC_UP					102
#define			IDC_DOWN				103
#define			IDC_CHAT				104
#define			IDC_DATA				105
#define			IDC_COLOR				106
#define			IDC_SEND				107
#define			IDC_CLEAR				108
#define			IDC_FACE				109
//游戏控制
#define			IDC_START				120
#define			IDC_EXERCISE			121
#define			IDC_QUIT				122
#define			IDC_ADVISE				123
#define			IDC_ORDER				124
#define			IDC_SET					125
#define			IDC_QUIT2				126
#define			IDC_TEAM_A				127
#define			IDC_TEAM_B				128
#define			IDC_TEAM_FREE			129

//道具
#define			IDC_GUIDE				140
#define			IDC_RESET				141
#define			IDC_BOM					142
#define			IDC_STOPIT				143
#define			IDC_CLOG				144



#define			LIST_LEFT				15
#define			LIST_RIGHT				200
#define			LIST_TOP				250 
#define			LIST_BOTTOM				430


// CMyListView 对话框

IMPLEMENT_DYNAMIC(CMyListView, CDialog)


BEGIN_MESSAGE_MAP(CMyListView, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()

	ON_BN_CLICKED(IDC_UP,OnUp)
	ON_BN_CLICKED(IDC_DOWN,OnDown)
	ON_BN_CLICKED(IDC_CHAT,OnMakeChat)
	ON_BN_CLICKED(IDC_DATA,OnMakeData)

	ON_BN_CLICKED(IDC_FACE,OnBnClickedExpression)
	ON_BN_CLICKED(IDC_COLOR,OnBnClickedColorSet)
	ON_BN_CLICKED(IDC_CLEAR,ClearChat)
	ON_BN_CLICKED(IDC_SEND,OnBnClickedSendChat)
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnHitExpression)

	//按钮
	ON_BN_CLICKED(IDC_START,OnStart)
	ON_BN_CLICKED(IDC_EXERCISE,OnExecise)
	ON_BN_CLICKED(IDC_QUIT,OnQuit)

	//道具
	ON_BN_CLICKED(IDC_GUIDE, OnGuide)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_BOM, OnBom)
	//ON_BN_CLICKED(IDC_STOPIT, OnStopIt)
	//ON_BN_CLICKED(IDC_CLOG,OnClog)

	ON_BN_CLICKED(IDC_SET, OnSet)
	//ON_BN_CLICKED(IDC_ADVISE, OnAdvise)
	//ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_QUIT2, OnQuit2)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

CMyListView::CMyListView(CWnd* pParent /*=NULL*/)
	: CDialog(CMyListView::IDD, pParent)
{
	m_ImageListViewBg.SetLoadInfo(IDB_BK_RIGHT,AfxGetInstanceHandle());
	m_pIClientKernel=NULL;
	m_pCurrentUserData = NULL;

	m_bReDrawBk = true;
	InitCtrl();
}

CMyListView::~CMyListView()
{
	memDC.DeleteDC();
}

void CMyListView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// CMyListView 消息处理程序

BOOL CMyListView::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hand = AfxGetApp()->LoadCursor(IDC_MY_HAND);
	/*
	//透明属性
	//加入WS_EX_LAYERED扩展属性 
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000); 
	HINSTANCE hInst = LoadLibrary("User32.DLL"); 
	if(hInst)
	{
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD);
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针
		fun=(MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes");
		if(fun)fun(this->GetSafeHwnd(),0,160,2);
		FreeLibrary(hInst);
	}
	*/

	//起始位置
	m_ChatPos = 0;
	m_DataPos = 0;

	//当前类型
	m_CurrentType = CHAT;

	//是否可以滚动
	m_CanScroll = true;

	//
	m_XPos = 30;

	//当前选择用户
	m_CurrentSelectUser = 0;

	UpdateImplementState();

	//
	memDC.CreateCompatibleDC(GetDC());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMyListView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_SupperLink.RemoveAll();//clear

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
	CFont font_;
	font_.CreateFont(14,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(font_);	
	
	if(m_bReDrawBk)
	{
		CBitmap bit;
		CImageHandle ImageHandle(&m_ImageListViewBg);
		bit.CreateCompatibleBitmap(&dc,m_ImageListViewBg.GetWidth(),m_ImageListViewBg.GetHeight());
		memDC.SelectObject(&bit);
		//背景
		m_ImageListViewBg.BitBlt(memDC.m_hDC,0,0,m_ImageListViewBg.GetWidth(),m_ImageListViewBg.GetHeight(),0,28,SRCCOPY);
		//LOGO
		CImageHandle ImageHandle1(&m_ImageLogo);
		m_ImageLogo.BitBlt(memDC.m_hDC, 3,614,SRCCOPY);
		bit.DeleteObject();
		m_bReDrawBk = false;
	}
	CImageHandle ImageHandle(&m_ImageListViewBg);
	dc.BitBlt(0,0,m_ImageListViewBg.GetWidth(),m_ImageListViewBg.GetHeight(),&memDC,0,0,SRCCOPY);

	//用户信息
	if(m_pCurrentUserData)
	{
		dc.SetTextColor(RGB(255,255,0));
		dc.TextOut(68,115,m_pCurrentUserData->szName);
	}

	TCHAR buffer[128];
	switch(m_CurrentType)
	{
	case CHAT:
		{
			int row=0;
			COLORREF oldcolor = RGB(0,0,255);
			for(int i=m_ChatPos; i<m_Chat.GetCount(); i++,row++)
			{
				memset(buffer,0,128);
				_snprintf(buffer,128,TEXT("%s"),m_Chat.GetAt(i).m_name);
				switch(m_Chat.GetAt(i).m_type)
				{
				case CHATMESSAGE:
					{
						dc.SetTextColor(RGB(0,0,0));
						_snprintf(buffer,128,TEXT("[%s]说:"),m_Chat.GetAt(i).m_name);
					}
					break;
				case SYSMESSAGE:
					{
						dc.SetTextColor(RGB(255,0,0));
						_snprintf(buffer,128,TEXT("【%s】"),m_Chat.GetAt(i).m_name);
					}
					break;
				case INOROUT:
					{
						dc.SetTextColor(RGB(0,0,0));
						_snprintf(buffer,128,TEXT("[%s]"),m_Chat.GetAt(i).m_name);
					}
					break;
				default:
					break;
				}

				CRect rect;
				rect.left = LIST_LEFT;
				rect.right = LIST_RIGHT;
				rect.top = LIST_TOP+row;
				rect.bottom = rect.top+18;
				dc.DrawText(buffer, rect,DT_WORDBREAK|DT_LEFT|DT_CALCRECT);
				dc.DrawText(buffer,rect,DT_WORDBREAK|DT_LEFT);

				if(m_Chat.GetAt(i).m_type == 0)
				{
					dc.SetTextColor(m_Chat.GetAt(i).m_color);
				}

				rect = AutoDrawTextAndImage(&dc,
					CRect(rect.right,rect.bottom-13,LIST_RIGHT,LIST_BOTTOM),
					m_Chat.GetAt(i).m_message
					);

				row = rect.bottom-LIST_TOP;
				dc.SetTextColor(oldcolor);

				//是否超出绘制区域要自动滚动
				if(row>180 && m_CanScroll ==  true)
				{
					m_ChatPos++;
					InvalidateRect(CRect(11,246,199,430),NULL);
					return;
				}
				else
				{
					if(row>180)
					{
						return;
					}
				}
			}
		}
		break;
	case DATA:
		{
			if(m_CurrentSelectUser !=0)
				dc.FrameRect(CRect(LIST_LEFT,20*(m_CurrentSelectUser)+LIST_TOP-5,LIST_RIGHT-5,20*(m_CurrentSelectUser)+LIST_TOP+20-5),&m_Brush);

			dc.SetTextColor(RGB(0,0,0));
			dc.TextOut(30,LIST_TOP,TEXT("呢称"));
			//dc.TextOut(75,10,TEXT("ID"));
			dc.TextOut(90,LIST_TOP,TEXT("等级"));
			dc.TextOut(160,LIST_TOP,TEXT("积分"));

			dc.SetTextColor(RGB(132,94,43));
			int row=1;
			for(int i=m_DataPos; i<m_Data.GetCount(); i++,row++)
			{
				if (row>5)
					break;
				TCHAR temp[12];
				_snprintf(temp,10,m_Data.GetAt(i).m_name);
				temp[10] = '.';
				temp[11] = '\0';
				dc.TextOut(LIST_LEFT,20*(row)+LIST_TOP,temp);
				//_snprintf(buffer,128,TEXT("%d"),m_Data.GetAt(i).m_id);
				//dc.TextOut(90,20*(row)+10,buffer);
				dc.TextOut(100,20*(row)+LIST_TOP,m_Data.GetAt(i).m_grade);
				_snprintf(buffer,128,TEXT("%d"),m_Data.GetAt(i).m_score);
				dc.TextOut(160,20*(row)+LIST_TOP,buffer);
			}
		}
		break;
	default:
		break;
	}
}

//插入聊天纪录
bool CMyListView::InsertChat(LPCTSTR pszSendName, LPCTSTR str, COLORREF color)
{
	UserChat_List tempchat;
	tempchat.m_name = pszSendName;
	tempchat.m_message.Format("%s",str);
	tempchat.m_color = color;
	tempchat.m_type = CHATMESSAGE;
	m_Chat.InsertAt(m_Chat.GetCount(),tempchat);

	m_CanScroll = true;
	InvalidateRect(CRect(11,246,199,430),NULL);

	return true;
}

//获取用户数据
void CMyListView::InsertUserItem(const tagUserData * pUserData)
{
	//列表
	UserData_List tempdata;
	tempdata.m_name = pUserData->szName;
	tempdata.m_id = pUserData->dwUserID;
	tempdata.m_score = pUserData->lScore;
	tempdata.m_grade = m_Logic.AssertUserClass(tempdata.m_score);
	m_Data.InsertAt(m_Data.GetCount(),tempdata);


	//用户进入消息
	UserChat_List tempchat;
	tempchat.m_name = pUserData->szName;
	tempchat.m_message.Format("进来了");
	tempchat.m_color = RGB(0,0,255);
	tempchat.m_type = INOROUT;
	m_Chat.InsertAt(m_Chat.GetCount(),tempchat);

	InvalidateRect(CRect(11,246,199,430),NULL);
}


//删除用户数据
void CMyListView::DeleteUserItem(tagUserData * pUserData)
{
	for(int i=0; i<m_Data.GetCount(); i++)
	{
		if(m_Data.GetAt(i).m_id == pUserData->dwUserID)
		{
			m_Data.RemoveAt(i);
		}
	}

	//用户离开消息
	UserChat_List tempchat;
	tempchat.m_name = pUserData->szName;
	tempchat.m_message.Format("离开了");
	tempchat.m_color = RGB(0,0,255);
	tempchat.m_type = INOROUT;
	m_Chat.InsertAt(m_Chat.GetCount(),tempchat);
	
	InvalidateRect(CRect(11,246,199,430),NULL);
	return;
}

//更新积分
void CMyListView::UpdateUserItem(tagUserData * pUserData)
{
	for(int i=0; i<m_Data.GetCount(); i++)
	{
		if(m_Data.GetAt(i).m_id == pUserData->dwUserID)
		{
			m_Data[i].m_score = pUserData->lScore;
			m_Data[i].m_grade = m_Logic.AssertUserClass(m_Data[i].m_score);
			InvalidateRect(CRect(11,246,199,430),NULL);
			return ;
		}
	}
	return;
}

//设置内核
void CMyListView::SetClientKernel(IClientKernel * pIClientKernel)
{
	m_pIClientKernel=pIClientKernel;
	return;
}

//发送按钮
void CMyListView::OnBnClickedSendChat()
{
	CString msg;
	GetDlgItem(ID_EDIT)->GetWindowText(msg);
	if (msg[0]!=0)
	{
		m_pIClientKernel->SendChatMessage(0,msg,g_GlobalOption.m_crChatTX);
	}
	GetDlgItem(ID_EDIT)->SetWindowText("");
	return;
}

void CMyListView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//消息模拟
	//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	if(m_SupperLink.GetCount()!=0&&m_CurrentType == CHAT)
	{
		for(int i=0; i<m_SupperLink.GetCount(); i++)
		{
			if(point.x>m_SupperLink[i].m_LinkRect_1.left && point.x <m_SupperLink[i].m_LinkRect_1.right&&
				point.y>m_SupperLink[i].m_LinkRect_1.top && point.y <m_SupperLink[i].m_LinkRect_1.bottom)
			{
				OpenUrl(m_SupperLink[i].m_URL);
				return;
			}
			else
				if(point.x>m_SupperLink[i].m_LinkRect_2.left && point.x <m_SupperLink[i].m_LinkRect_2.right&&
					point.y>m_SupperLink[i].m_LinkRect_2.top && point.y <m_SupperLink[i].m_LinkRect_2.bottom)
				{
					OpenUrl(m_SupperLink[i].m_URL);
					return;
				}
				else
					if(point.x>m_SupperLink[i].m_LinkRect_3.left && point.x <m_SupperLink[i].m_LinkRect_3.right&&
						point.y>m_SupperLink[i].m_LinkRect_3.top && point.y <m_SupperLink[i].m_LinkRect_3.bottom)
					{
						OpenUrl(m_SupperLink[i].m_URL);
						return;
					}
		}
	}
	//选择用户时显示方框
	if(m_CurrentType == DATA)
	{
		if(point.x>LIST_LEFT && point.x<LIST_RIGHT&&point.y>LIST_TOP&&point.y<LIST_BOTTOM)
		{
			m_CurrentSelectUser = (point.y-30-235)/20+1;
		}
		
		InvalidateRect(CRect(11,246,199,430),NULL);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

int CMyListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect CreateRect(0,0,0,0);
	HCURSOR hand = AfxGetApp()->LoadCursor(IDC_MY_HAND);

	m_ImageFace.SetLoadInfo(IDB_FACE,AfxGetInstanceHandle());
	m_ImageLogo.SetLoadInfo(IDB_LLK_LOGO,AfxGetInstanceHandle());

	m_btAdvise.Create(NULL,WS_CHILD,CRect(35,20,50,40),this,IDC_ADVISE);
	m_btAdvise.SetButtonImage(IDB_BT_ADVISE,AfxGetInstanceHandle(),false);
	//m_btAdvise.ShowWindow(SW_SHOW);

	m_btOrder.Create(NULL,WS_CHILD,CRect(35,50,50,65),this,IDC_ORDER);
	m_btOrder.SetButtonImage(IDB_BT_ORDER,AfxGetInstanceHandle(),false);
	//m_btOrder.ShowWindow(SW_SHOW);

	m_btSet.Create(NULL,WS_CHILD,CRect(35,35,50,65),this,IDC_SET);
	m_btSet.SetButtonImage(IDB_BT_SET,AfxGetInstanceHandle(),false);
	m_btSet.ShowWindow(SW_SHOW);

	m_btQuit2.Create(NULL,WS_CHILD,CRect(125,35,150,65),this,IDC_QUIT2);
	m_btQuit2.SetButtonImage(IDB_BT_QUIT2,AfxGetInstanceHandle(),false);
	m_btQuit2.ShowWindow(SW_SHOW);

	//
	m_btUp.Create(NULL,WS_CHILD,CRect(200,235+11,210,235+21),this,IDC_UP);
	m_btUp.SetButtonImage(IDB_BT_UP,AfxGetInstanceHandle(),false);
	m_btUp.ShowWindow(SW_SHOW);

	m_btDown.Create(NULL,WS_CHILD,CRect(200,235+202,210,235+212),this,IDC_DOWN);
	m_btDown.SetButtonImage(IDB_BT_DOWN,AfxGetInstanceHandle(),false);
	m_btDown.ShowWindow(SW_SHOW);

	m_btChat.Create(NULL,WS_CHILD,CRect(100,235+200,20,235+40),this,IDC_CHAT);
	m_btChat.SetButtonImage(IDB_BT_TALK,AfxGetInstanceHandle(),false);
	m_btChat.ShowWindow(SW_SHOW);

	m_btData.Create(NULL,WS_CHILD,CRect(150,235+200,20,235+79),this,IDC_DATA);
	m_btData.SetButtonImage(IDB_BT_TAB,AfxGetInstanceHandle(),false);
	m_btData.ShowWindow(SW_SHOW);

	//
	m_btStart.Create(NULL,WS_CHILD,CRect(10,512,217,600),this,IDC_START);
	m_btStart.SetButtonImage(IDB_BT_START,AfxGetInstanceHandle(),false);
	m_btStart.ShowWindow(SW_SHOW);

	m_btExercise.Create(NULL,WS_CHILD,CRect(12,563,100,720),this,IDC_EXERCISE);
	m_btExercise.SetButtonImage(IDB_BT_EXERCISE,AfxGetInstanceHandle(),false);
	m_btExercise.ShowWindow(SW_SHOW);

	m_btQuit.Create(NULL,WS_CHILD,CRect(115,563,217,720),this,IDC_QUIT);
	m_btQuit.SetButtonImage(IDB_BT_QUIT,AfxGetInstanceHandle(),false);
	m_btQuit.ShowWindow(SW_SHOW);

	//道具
	m_btGuide.Create(NULL,WS_CHILD,CRect(10,180,34,203),this,IDC_GUIDE);
	m_btGuide.SetButtonImage(IDB_IMPLEMENT_GUIDE,AfxGetInstanceHandle(),false);
	m_btGuide.ShowWindow(SW_SHOW);

	m_btReset.Create(NULL,WS_CHILD,CRect(34,180,58,203),this,IDC_RESET);
	m_btReset.SetButtonImage(IDB_IMPLEMENT_RESET,AfxGetInstanceHandle(),false);
	m_btReset.ShowWindow(SW_SHOW);

	m_btBom.Create(NULL,WS_CHILD,CRect(58,180,82,203),this,IDC_BOM);
	m_btBom.SetButtonImage(IDB_IMPLEMENT_BOM,AfxGetInstanceHandle(),false);
	m_btBom.ShowWindow(SW_SHOW);

	//m_btStopIt.Create(NULL,WS_CHILD,CRect(82,180,106,203),this,IDC_STOPIT);
	//m_btStopIt.SetButtonImage(IDB_IMPLEMENT_STOPIT,AfxGetInstanceHandle(),false);
	//m_btStopIt.ShowWindow(SW_SHOW);

	//m_btClog.Create(NULL,WS_CHILD,CRect(106,180,130,203),this,IDC_CLOG);
	//m_btClog.SetButtonImage(IDB_IMPLEMENT_CLOG,AfxGetInstanceHandle(),false);
	//m_btClog.ShowWindow(SW_SHOW);

	//组队
	m_btTeamA.Create(NULL,WS_CHILD,CRect(28,545,30,560),this,IDC_TEAM_A);
	m_btTeamA.SetButtonImage(IDB_TEAM_A,AfxGetInstanceHandle(),false);
	m_btTeamA.ShowWindow(SW_HIDE);

	m_btTeamB.Create(NULL,WS_CHILD,CRect(83,545,100,560),this,IDC_TEAM_B);
	m_btTeamB.SetButtonImage(IDB_TEAM_B,AfxGetInstanceHandle(),false);
	m_btTeamB.ShowWindow(SW_HIDE);

	m_btTeamFree.Create(NULL,WS_CHILD,CRect(138,545,180,560),this,IDC_TEAM_FREE);
	m_btTeamFree.SetButtonImage(IDB_TEAM_FREE,AfxGetInstanceHandle(),false);
	m_btTeamFree.ShowWindow(SW_HIDE);

	CImageHandle ImageHandle(&m_ImageListViewBg);
	SetWindowPos(NULL,0,0,m_ImageListViewBg.GetWidth(),m_ImageListViewBg.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	m_Brush.CreateSolidBrush(RGB(0,255,0));

	//消息框
	m_messageEdit.Create(WS_CHILD|ES_AUTOHSCROLL,CRect(15,235+244,160,235+260),this,ID_EDIT);
	m_messageEdit.ShowWindow(SW_SHOW);

	//聊天按钮
	m_btSend.Create(NULL,WS_CHILD,CreateRect,this,IDC_SEND);
	m_btColor.Create(NULL,WS_CHILD,CreateRect,this,IDC_COLOR);
	m_btClear.Create(NULL,WS_CHILD,CreateRect,this,IDC_CLEAR);
	m_btFace.Create(NULL,WS_CHILD,CreateRect,this,IDC_FACE);


	m_btSend.ShowWindow(SW_SHOW);
	m_btColor.ShowWindow(SW_SHOW);
	m_btClear.ShowWindow(SW_SHOW);
	m_btFace.ShowWindow(SW_SHOW);
	m_btSend.SetButtonImage(IDB_BT_SEND,AfxGetInstanceHandle(),false);
	m_btColor.SetButtonImage(IDB_BT_COLOR,AfxGetInstanceHandle(),false);
	m_btClear.SetButtonImage(IDB_BT_CLEAR,AfxGetInstanceHandle(),false);
	m_btFace.SetButtonImage(IDB_BT_FACE,AfxGetInstanceHandle(),false);

	//按钮控件
	HDWP hDwp=BeginDeferWindowPos(4);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOSIZE;

	DeferWindowPos(hDwp,m_btFace,NULL,15,235+220,35,235+240,uFlags);
	DeferWindowPos(hDwp,m_btColor,NULL,40,235+220,60,235+240,uFlags);
	DeferWindowPos(hDwp,m_btClear,NULL,65,235+220,85,235+240,uFlags);
	DeferWindowPos(hDwp,m_btSend,NULL,170,235+236,190,235+260,uFlags);

	EndDeferWindowPos(hDwp);

	//道具信息
	m_AInfo.Create(NULL, NULL, 	WS_CHILD|WS_VISIBLE, CRect(10,166,40,190), this, NULL);
	m_AInfo.ShowWindow(SW_HIDE);
	return 0;
}

void CMyListView::OnUp(void)
{
	switch(m_CurrentType)
	{
	case CHAT:
		{
			if(m_ChatPos>0)
				m_ChatPos-- ;
		}
		break;
	case DATA:
		{
			if(m_DataPos>0)
				m_DataPos-- ;
		}
		break;
	default:
		break;
	}
	m_CanScroll = false;
	InvalidateRect(CRect(11,246,199,430),NULL);
}

void CMyListView::OnDown(void)
{
	switch(m_CurrentType)
	{
	case CHAT:
		{
			if(m_Chat.GetCount()>5&&m_ChatPos<m_Chat.GetCount())
				m_ChatPos ++;
		}
		break;
	case DATA:
		{
			if(m_Data.GetCount()>5&&m_DataPos<m_Data.GetCount())
				m_DataPos++;
		}
		break;
	default:
		break;
	}
	m_CanScroll = false;
	InvalidateRect(CRect(11,246,199,430),NULL);
}

void CMyListView::OnOK()
{
	OnBnClickedSendChat();
	return;
}

void CMyListView::OnCancel()
{
	return;
}

void CMyListView::OnMakeChat(void)
{
    m_CurrentType = CHAT;
	InvalidateRect(CRect(11,246,199,430),NULL);
}

void CMyListView::OnMakeData(void)
{
	m_CurrentType = DATA;
	InvalidateRect(CRect(11,246,199,430),NULL);
}



//自动输出函数
CRect CMyListView::AutoDrawTextAndImage(CDC * pDC, LPRECT lpRect,CString szString)
{
	 CRect rcInner(lpRect);
	 CPoint cpos;
	 cpos.x = rcInner.left;
	 cpos.y = rcInner.top ;

    if(rcInner.Width() ==0)
        return rcInner;

	//是否超级链接
	bool isLink=false;
	int Link_f;
	int Link_e;
	CRect LinkRect(0,0,0,0);
	CPoint fpos;

    for(int i=0; i<=szString.GetLength(); i++)
    {
		//到末尾
		if(cpos.y >= LIST_BOTTOM)
		{
			rcInner.bottom = cpos.y + 18;
			return rcInner;
		}

		//超级链接
		if(szString.Mid(i,7) == TEXT("http://" && isLink==false))
		{
			isLink = true;
			Link_f = i;
			LinkRect.left = cpos.x;
			LinkRect.top = cpos.y;
			fpos = cpos;
		}

		if(isLink == true)
		{
			if(((szString.GetAt(i)>='a'&& szString.GetAt(i)<='z')||
				(szString.GetAt(i) >='A' && szString.GetAt(i)<='Z')||
				(szString.GetAt(i)>='0'&&szString.GetAt(i)<='9') ||
				(szString.GetAt(i)== '/')||
				(szString.GetAt(i) == ':')||
				(szString.GetAt(i) == '?')||
				(szString.GetAt(i) == '.')||
				(szString.GetAt(i) == '_'))== false)
			{
				Link_e = i;
				SupperLink templink;
				templink.m_URL = szString.Mid(Link_f,Link_e -Link_f);

				if(LinkRect.top == cpos.y)
				{
					LinkRect.right = cpos.x;
					LinkRect.bottom = cpos.y + 18;
					templink.m_LinkRect_1 = LinkRect;
				}
				else
				{
					//第一行区域
					LinkRect.right = rcInner.right;
					LinkRect.bottom = LinkRect.top + 18;
					templink.m_LinkRect_1 = LinkRect;

					//末行区域
					LinkRect.left = m_XPos;
					LinkRect.right = cpos.x;
					LinkRect.top = cpos.y ;
					LinkRect.bottom = cpos.y+18;
					templink.m_LinkRect_2 = LinkRect;

					//中间区域
					int h = (cpos.y - fpos.y)/18;
					if(h>1)
					{
						LinkRect.left=m_XPos;
						LinkRect.right = rcInner.right;
						LinkRect.top = fpos.y + 18;
						LinkRect.bottom = cpos.y;
						templink.m_LinkRect_3 = LinkRect;
					}
				}

				m_SupperLink.InsertAt(m_SupperLink.GetCount(),templink);
				isLink = false;
			}
			
		}
		if(i == szString.GetLength())
		{
			rcInner.bottom = cpos.y + 18;
			return rcInner;
		}

		//处理图片
		if(szString.GetAt(i) == '/'&& szString.GetAt(i+1) == ':')
		{
			int id = atoi(szString.Mid(i+2,3));
			if(id >=1 && id <=121)
			{
				CString xx;
				xx.Format("%d",id-1);
				OutPutData(pDC,xx,&cpos,rcInner,false);
				if(id>=100) i+=4;
				else i+=3;
			}
			//不是图片，则输出普通字符
			else
			{
				OutPutData(pDC,szString.Mid(i,1),&cpos,rcInner,true);
			}
		}
		else
        //输出汉字或者宽字符
        if((unsigned char)szString.GetAt(i) >= 0xA1)
        {
			OutPutData(pDC,szString.Mid(i,2),&cpos,rcInner,true);
			i++;//跳过另半个字符
        }
        else
		//普通单个字符
        {
			OutPutData(pDC,szString.Mid(i,1),&cpos,rcInner,true);
        }
		
		//通知换行
		if(rcInner.bottom <=cpos.y)
			rcInner.bottom += cpos.y+18;
	}
	//输出区域的底部位置
	rcInner.bottom = cpos.y + 18;

	return rcInner;
}

//自动吞吐
bool CMyListView::OutPutData(CDC * pDC, CString szString, CPoint *cpos,LPRECT lpRect,bool textorimage)
{
	CRect rect(lpRect);
	if(!textorimage)//输出图片
	{
		if(cpos->x+16>rect.right)
		{
			cpos->x = m_XPos;
			
			cpos->y = cpos->y + 18;
		}
		rect.left = cpos->x;
		rect.top = cpos->y;
		int id = atoi(szString);
		try
		{
			CImage Image;
			CString strImage;
			strImage.Format(TEXT("Face\\%s.gif"), szString);
			Image.Load(strImage);
		//	Image.BitBlt(pDC->GetSafeHdc(), rect.left,rect.top-4);
			Image.TransparentBlt(pDC->GetSafeHdc(), rect.left,rect.top-4,21,21,Image.GetPixel(0,0));

			//CImageHandle ImageHandle(&m_ImageFace);
			//m_ImageFace.AlphaDrawImage(pDC,rect.left,rect.top-4,21,21,((id-1)%11)*26+3,((id-1)/11)*26+3,RGB(200,200,200));
		}
		catch(...){}

		cpos->x += 20;
	}else//输出字符
	{
		if(cpos->x+16>rect.right)
		{
			//到末尾
			if(cpos->y >= LIST_BOTTOM-18)
			{
				return true;
			}

			cpos->x = m_XPos;
			cpos->y = cpos->y + 18;
		}
		rect.left = cpos->x;
		rect.top = cpos->y;
		
		pDC->DrawText(szString, rect,DT_WORDBREAK|DT_LEFT|DT_CALCRECT);
		pDC->DrawText(szString,rect, DT_WORDBREAK|DT_LEFT);
		cpos->x = rect.right;
		cpos->y = rect.top;
	}
	
	return true;
}

void CMyListView::ClearChat(void)
{
	m_ChatPos = 0;
	m_Chat.RemoveAll();
	InvalidateRect(CRect(11,246,199,430),NULL);
}

bool CMyListView::SetRichEditHwnd(CSkinRichEdit * pSkinRichEdit)
{
	return true;
}

//设置颜色
bool CMyListView::SetMessageOption(tagMessageOption & MessageOption)
{
	return true;
}
	//清除屏幕
bool CMyListView:: CleanScreen()
{
	return true;
}
	//设置滚动
bool  CMyListView::SetAutoScroll(bool bScroll)
{
	return true;
}
	//加载消息
bool  CMyListView::LoadMessage(LPCTSTR pszFileName)
{
	return true;
}
	//保存信息
bool  CMyListView::SaveMessage(LPCTSTR pszFileName)
{
	return true;
}

	//插入图片
bool CMyListView:: InsertImage(CBitmap * pBitmap)
{
	return true;
}
	//插入时间
bool  CMyListView::InsertSystemTime(COLORREF crColor)
{
	return true;
}
	//用户名字
bool  CMyListView::InsertUserName(LPCTSTR pszUserName)
{
	return true;
}
	//系统消息
bool  CMyListView::InsertSystemString(LPCTSTR pszString, UINT uSize)
{
	UserChat_List tempchat;
	tempchat.m_name = TEXT("系统消息");
	tempchat.m_message.Format("%s",pszString);
	tempchat.m_type = SYSMESSAGE;
	m_Chat.InsertAt(m_Chat.GetCount(),tempchat);

	m_CanScroll = true;

	InvalidateRect(CRect(11,246,199,430),NULL);
	return true;
}
	//特色消息
bool  CMyListView::InsertShtickString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)
{
	return true;
}
	//普通消息
bool  CMyListView::InsertGeneralString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)
{
	return true;
}
	//用户聊天
bool  CMyListView::InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszString, COLORREF crColor, UINT uSize)
{
	InsertChat(pszSendName,pszString,crColor);
	return true;
}
	//用户聊天
bool  CMyListView::InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszRecvName, LPCTSTR pszString, COLORREF crColor, UINT uSize)
{
	return true;
}
//接口查询
void * __cdecl CMyListView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMessageProxy,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMessageProxy,Guid,dwQueryVer);
	return NULL;
}

void CMyListView::OnMouseMove(UINT nFlags, CPoint point)
{
	int t = 0;
	//道具10,189,40,220
	int tx = 10;
	int ty = 180;
	if(point.x> tx && point.x < tx+24 &&
		point.y> ty && point.y< ty+23)
		t = 1;
	if(point.x> tx+24 && point.x < tx+48 &&
		point.y> ty && point.y< ty+23)
		t = 2;
	if(point.x> tx +48&& point.x < tx+72 &&
		point.y> ty && point.y< ty+23)
		t = 3;
	/*if(point.x> tx+72 && point.x < tx+96 &&
		point.y> ty && point.y< ty+23)
		t = 4;
	if(point.x> tx +96&& point.x < tx+118&&	
		point.y> ty && point.y< ty+23)
		t = 5;
*/
	if(t == m_AInfoPos)
		return;
	if(t == 0)
	{
		m_AInfoPos = t;
		m_AInfo.ShowWindow(SW_HIDE);
		return;
	}else
	if(m_AInfoPos!=t)
	{
		Sleep(50);
	}

	CString temp;
	m_AInfo.SetTextColor(0);
	switch(t)
	{
	case 1:
		temp.Format("罗盘:%d",m_ImplementCount[t-1]);
		m_AInfo.SetWindowText(temp);
		break;
	case 2:
		temp.Format("重列:%d",m_ImplementCount[t-1]);
		m_AInfo.SetWindowText(temp);
		break;
	case 3:
		temp.Format("炸弹:%d",m_ImplementCount[t-1]);
		m_AInfo.SetWindowText(temp);
		break;
	/*case 4:
		temp.Format("禁手:%d",m_ImplementCount[t-1]);
		m_AInfo.SetWindowText(temp);
		break;
	case 5:
		temp.Format("障碍:%d",m_ImplementCount[t-1]);
		m_AInfo.SetWindowText(temp);
		break;*/
	default:
		return;
	}
	m_AInfoPos = t;
	m_AInfo.ShowWindow(SW_SHOW);

	__super::OnMouseMove(nFlags, point);
}

void CMyListView::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
}

BOOL CMyListView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	
	CString sClassName;
	::GetClassName(pWnd->GetSafeHwnd(),sClassName.GetBuffer(80),80);
	
	if (sClassName=="Button"  ) {
		CRect rect;
		pWnd->GetWindowRect(rect);
		CPoint point;
		point.x = rect.left+10-789;
		point.y = rect.top-10;
		TRACE("\n%d,%d",point.x,point.y);
		OnMouseMove(0,point);
		SetCursor(m_hand);
		return TRUE;
	}
	SetCursor(m_hand);
	return true;
}

void CMyListView::OpenUrl(CString strBuffer)
{
	//网页分析
	LPCTSTR pszHttp[]=
	{
		TEXT("http://s.cngame.com"),
		TEXT("http://bbs.cngame.com"),
		TEXT("http://www.cngame.com"),
		TEXT("http://club.cngame.com"),
		TEXT("http://service.cngame.com"),
	};
	for (int i=0;i<CountArray(pszHttp);i++)
	{
		if (strBuffer.GetLength()>=lstrlen(pszHttp[i]))
		{
			if (memcmp((LPCTSTR)strBuffer,pszHttp[i],lstrlen(pszHttp[i]))==0)
			{
				m_Logic.OpenUrl(strBuffer);
				return;
			}
		}
	}
}
//颜色按钮
void CMyListView::OnBnClickedColorSet()
{
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_CHATCOLOR);
	

	return;
}

//表情消息
LRESULT CMyListView::OnHitExpression(WPARAM wParam, LPARAM lParam)
{
	CExpressionItem * pExpressionItem=m_Expression.GetExpressItem((WORD)wParam);
	if (pExpressionItem!=NULL)
	{
		CString strBuffer;
		m_messageEdit.GetWindowText(strBuffer);
		strBuffer+=pExpressionItem->m_szTrancelate;
		m_messageEdit.SetWindowText(strBuffer);
		m_messageEdit.SetFocus();
		m_messageEdit.SetSel(strBuffer.GetLength(),strBuffer.GetLength());
	}
	return 0;
}

//表情按钮
void CMyListView::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_Expression.GetSafeHwnd()==NULL) m_Expression.CreateExpression(this);

	//移动窗口
	CRect rcButton;
	CSize ExpressSize;
	m_Expression.GetFixSize(ExpressSize);
	m_btSend.GetWindowRect(&rcButton);
	m_Expression.MoveWindow(rcButton.right-ExpressSize.cx,rcButton.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_Expression.ShowWindow(SW_SHOW);
	m_Expression.SetFocus();

	return;
}

// 设置用户信息
void CMyListView::SetUserDataItem(tagUserData* pUserData)
{
	m_pCurrentUserData = pUserData;
}

// 初始控制选项
void CMyListView::InitCtrl(void)
{
	m_AInfoPos = 0;
	m_DisplayPropInfo = 0;
	//初始道具数量
	PropZero();
}

// 更新道具按钮显示状态
void CMyListView::UpdateImplementState(void)
{
	if(m_ImplementCount[PROP_GUIDE] > 0)m_btGuide.EnableWindow(TRUE);
		else m_btGuide.EnableWindow(FALSE);
	if(m_ImplementCount[PROP_RESET] > 0)m_btReset.EnableWindow(TRUE);
		else m_btReset.EnableWindow(FALSE);
	if(m_ImplementCount[PROP_BOM] > 0)m_btBom.EnableWindow(TRUE);
		else m_btBom.EnableWindow(FALSE);
//	if(m_ImplementCount[PROP_STOPIT] > 0)m_btStopIt.EnableWindow(TRUE);
//		else m_btStopIt.EnableWindow(FALSE);
//	if(m_ImplementCount[PROP_CLOG] > 0)m_btClog.EnableWindow(TRUE);
//		else m_btClog.EnableWindow(FALSE);
}

// 游戏开始
void CMyListView::OnStart(void)
{
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_START);
}

// 练习
void CMyListView::OnExecise(void)
{
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_EXERCISE);
}

// 游戏结束
void CMyListView::OnQuit(void)
{
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_QUIT);
}

// 指南针
void CMyListView::OnGuide(void)
{
	if(m_ImplementCount[PROP_GUIDE]<=0)
		return;
	--m_ImplementCount[PROP_GUIDE];
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_GUIDE);
	UpdateImplementState();
}

// 重列
void CMyListView::OnReset(void)
{
	if(m_ImplementCount[PROP_RESET]<=0)
		return;
	--m_ImplementCount[PROP_RESET];
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_RESET);
	UpdateImplementState();
}

// 炸弹
void CMyListView::OnBom(void)
{
	if(m_ImplementCount[PROP_BOM]<=0)
		return;
	--m_ImplementCount[PROP_BOM];
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_BOM);
	UpdateImplementState();
}

// 禁手
/*void CMyListView::OnStopIt(void)
{
	if(m_ImplementCount[PROP_STOPIT]<=0)
		return;
	--m_ImplementCount[PROP_STOPIT];
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_STOPIT);
	UpdateImplementState();
}

// 障碍
void CMyListView::OnClog(void)
{
	if(m_ImplementCount[PROP_CLOG]<=0)
		return;
	--m_ImplementCount[PROP_CLOG];
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_CLOG);
	UpdateImplementState();
}*/

// 道具请零
void CMyListView::PropZero(void)
{
	memset(m_ImplementCount,0,sizeof(m_ImplementCount));
	
}

void CMyListView::OnSet(void)
{
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->m_GameOption.ShowWindow(SW_SHOW);
}

void CMyListView::OnAdvise(void)
{
	//TCHAR szUrl[256]=TEXT("");
	//_snprintf(szUrl,CountArray(szUrl),TEXT("http://%s/client/tsjy.jsp"),szStationPage);
	ShellExecute(NULL,TEXT("open"),TEXT("http://www.hngame.net/bbs/list.asp?boardid=39"),NULL,NULL,SW_SHOWDEFAULT);

}
void CMyListView::OnOrder(void)
{
	//TCHAR szUrl[256]=TEXT("");
	//_snprintf(szUrl,CountArray(szUrl),TEXT("http://%s/game/taxis.jsp?KindID=37"),szStationPage);
	ShellExecute(NULL,TEXT("open"),TEXT("http://game.koogoo.cn/order/Order.aspx?src=ByScore.aspx?OrderType=0"),NULL,NULL,SW_SHOWDEFAULT);

}
void CMyListView::OnQuit2(void)
{
	CONTAINING_RECORD(this,CGameClientView,m_MyListView)->SendMessage(IDM_QUIT);
}