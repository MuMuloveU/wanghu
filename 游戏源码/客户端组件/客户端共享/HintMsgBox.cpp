#include "StdAfx.h"
#include "Resource.h"
#include "HintMsgBox.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_MESSAGE					100									//消息标识

//窗口位置
#define SMB_WINDOW_WIDTH			230									//最小宽度

//图标偏移
#define SMB_ICON_LEFT				20									//图标偏移
#define SMB_ICON_WIDTH				32									//图标宽度
#define SMB_ICON_HEIGHT				32									//图标高度

//字符偏移
#define SMB_STRING_TOP				25									//字符偏移
#define SMB_STRING_LEFT				20									//字符偏移
#define SMB_STRING_RIGHT			40									//字符偏移
#define SMB_STRING_WIDTH			600									//最大宽度
#define SMB_STRING_HEIGHT			600									//最大宽度

//按钮偏移
#define SMB_BUTTON_TOP				20									//按钮偏移
#define SMB_BUTTON_LEFT				12									//按钮偏移
#define SMB_BUTTON_RIGHT			12									//按钮偏移
#define SMB_BUTTON_BUTTOM			13									//按钮偏移
#define SMB_BUTTON_SPACE			10									//按钮间隔
#define SMB_BUTTON_WIDTH			70									//按钮偏移
#define SMB_BUTTON_HEIGHT			22									//按钮偏移

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHintMsgBox, CSkinDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CHintMsgBox::CHintMsgBox(CWnd * pParentWnd) : CSkinDialogEx(IDD_HINT_MSG,pParentWnd)
{
	//设置变量
	m_uType=MB_OK;

	//辅助变量
	m_nElapse=0;
	m_nBtCount=0;
	m_hResInstance=NULL;

	return;
}

//析构函数
CHintMsgBox::~CHintMsgBox()
{
}

//初始化消息
BOOL CHintMsgBox::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(m_strCaption);

	//关闭按钮
	if((m_uType&MB_DEFBUTTON2)!=0)m_cbButtonState[BST_CLOSE]&=(~BUT_EXIST);
	else m_cbButtonState[BST_CLOSE]|=BUT_EXIST;

	//按钮区域
	CreateBoxButton();
	INT nButtonAreaHeight=SMB_BUTTON_HEIGHT+SMB_BUTTON_TOP+SMB_BUTTON_BUTTOM;
	INT nButtonAreaWidth=m_nBtCount*SMB_BUTTON_WIDTH+(m_nBtCount+1)*SMB_BUTTON_SPACE+SMB_BUTTON_LEFT+SMB_BUTTON_RIGHT;

	//字符空间
	CClientDC ClientDC(this);
	CRect rcString(0,0,SMB_STRING_WIDTH,SMB_STRING_HEIGHT);
	ClientDC.SelectObject(CSkinResourceManager::GetDefaultFont());
	ClientDC.DrawText(m_strString,rcString,DT_CALCRECT|DT_EXPANDTABS|DT_NOCLIP|DT_WORD_ELLIPSIS);

	//字符区域
	INT nStringAreaHeight=rcString.Height()+SMB_STRING_TOP;
	INT nStringAreaWidth=rcString.Width()+SMB_ICON_LEFT+SMB_ICON_WIDTH+SMB_STRING_LEFT+SMB_STRING_RIGHT;

	//窗口区域
	INT nWindowAreaHeight=nStringAreaHeight+nButtonAreaHeight;
	INT nWindowAreaWidth=__max(SMB_WINDOW_WIDTH,__max(nStringAreaWidth,nButtonAreaWidth));

	//输出位置
	if (nWindowAreaWidth>nStringAreaWidth)
	{
		m_rcString.top=m_SkinAttribute.m_nCaptionHeigth+SMB_STRING_TOP;
		m_rcString.left=GetXBorder()+SMB_ICON_LEFT+SMB_ICON_WIDTH+SMB_STRING_LEFT+(nWindowAreaWidth-nStringAreaWidth)/2;
		m_rcString.right=m_rcString.left+rcString.Width();
		m_rcString.bottom=m_rcString.top+rcString.Height();
	}
	else
	{
		m_rcString.top=m_SkinAttribute.m_nCaptionHeigth+SMB_STRING_TOP;
		m_rcString.left=GetXBorder()+SMB_ICON_LEFT+SMB_ICON_WIDTH+SMB_STRING_LEFT;
		m_rcString.right=m_rcString.left+rcString.Width();
		m_rcString.bottom=m_rcString.top+rcString.Height();
	}

	//调整窗口
	CRect rcWindow(0,0,0,0);
	rcWindow.right=nWindowAreaWidth+GetXBorder()*2;
	rcWindow.bottom=nWindowAreaHeight+m_SkinAttribute.m_nCaptionHeigth+GetYBorder();
	SetWindowPos(NULL,0,0,rcWindow.Width(),rcWindow.Height(),SWP_NOMOVE|SWP_NOZORDER);

	//调整按钮
	INT nYButton=m_SkinAttribute.m_nCaptionHeigth+nWindowAreaHeight-SMB_BUTTON_BUTTOM-SMB_BUTTON_HEIGHT;
	INT nXButton=GetXBorder()+(nWindowAreaWidth-m_nBtCount*SMB_BUTTON_WIDTH-(m_nBtCount+1)*SMB_BUTTON_SPACE)/2+SMB_BUTTON_SPACE;
	for (UINT i=0;i<m_nBtCount;i++)
	{
		m_btButton[i].SetWindowPos(NULL,nXButton,nYButton,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
		nXButton+=SMB_BUTTON_SPACE+SMB_BUTTON_WIDTH;
	}

	//默认按钮
	UINT uDefaultIndex=(m_uType&MB_DEFMASK)>>8;
	if (uDefaultIndex>=m_nBtCount) uDefaultIndex=0;
	m_btButton[uDefaultIndex].SetFocus();
	m_btButton[uDefaultIndex].SetButtonStyle(m_btButton[uDefaultIndex].GetButtonStyle()|BS_DEFPUSHBUTTON);

	//居中窗口
	CenterWindow(GetParent());

	//设置时间
	if (m_nElapse!=0) SetTimer(IDI_MESSAGE,1000,NULL);

	return FALSE;
}

//命令消息
BOOL CHintMsgBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//按钮标识
	UINT uButtonID=LOWORD(wParam);

	//关闭处理
	if (uButtonID==IDOK)
	{
		UINT uDefaultIndex=(m_uType&MB_DEFMASK)>>8;
		if (uDefaultIndex>=m_nBtCount) uDefaultIndex=0;
		EndDialog(GetWindowLong(m_btButton[uDefaultIndex],GWL_ID));
	}
	else if (uButtonID==IDCANCEL)
	{
		EndDialog(GetWindowLong(m_btButton[m_nBtCount-1],GWL_ID));
	}
	else EndDialog(uButtonID);

	return TRUE;
}

//显示消息
INT CHintMsgBox::ShowMessageBox(LPCTSTR pszString, UINT uType, UINT nElapse)
{
	return ShowMessageBox(TEXT("提示消息"),pszString,uType,nElapse);
}

//显示消息
INT CHintMsgBox::ShowMessageBox(LPCTSTR pszCaption, LPCTSTR pszString, UINT uType, UINT nElapse)
{
	//设置变量
	m_uType=uType;
	m_nElapse=nElapse;
	m_strString=pszString;
	m_strCaption=pszCaption;

	//显示窗口
	INT nResult=(INT)DoModal();

	return nResult;
}

//激活按钮
VOID CHintMsgBox::CreateBoxButton()
{
	//变量定义
	UINT uButtonID[3]={0,0,0};
	LPCTSTR pszString[3]={NULL,NULL,NULL};

	//设置按钮
	switch (m_uType&MB_TYPEMASK)
	{
	case MB_OKCANCEL:
		{
			m_nBtCount=2;
			uButtonID[0]=IDOK;
			uButtonID[1]=IDCANCEL;
			pszString[0]=TEXT("确定(&O)");
			pszString[1]=TEXT("取消(&C)");
			break;
		}
	case MB_YESNO:
		{
			m_nBtCount=2;
			uButtonID[0]=IDYES;
			uButtonID[1]=IDNO;
			pszString[0]=TEXT("是(&Y)");
			pszString[1]=TEXT("否(&N)");
			break;
		}
	case MB_YESNOCANCEL:
		{
			m_nBtCount=3;
			uButtonID[0]=IDYES;
			uButtonID[1]=IDNO;
			uButtonID[2]=IDCANCEL;
			pszString[0]=TEXT("是(&Y)");
			pszString[1]=TEXT("否(&N)");
			pszString[2]=TEXT("取消(&C)");
			break;
		}
	case MB_RETRYCANCEL:
		{
			m_nBtCount=2;
			uButtonID[0]=IDRETRY;
			uButtonID[1]=IDCANCEL;
			pszString[0]=TEXT("重试(&R)");
			pszString[1]=TEXT("取消(&C)");
			break;
		}
	case MB_ABORTRETRYIGNORE:
		{
			m_nBtCount=3;
			uButtonID[0]=IDABORT;
			uButtonID[1]=IDRETRY;
			uButtonID[2]=IDIGNORE;
			pszString[0]=TEXT("中止(&A)");
			pszString[1]=TEXT("重试(&R)");
			pszString[2]=TEXT("忽略(&I)");
			break;
		}
	default:
		{
			m_nBtCount=1;
			uButtonID[0]=IDOK;
			pszString[0]=TEXT("确定(&O)");
			break;
		}
	}

	//显示按钮
	CRect rcButton(0,0,SMB_BUTTON_WIDTH,SMB_BUTTON_HEIGHT);
	for (UINT i=0;i<m_nBtCount;i++) m_btButton[i].Create(pszString[i],WS_CHILD,rcButton,this,uButtonID[i]);

	return;
}

//绘画消息
VOID CHintMsgBox::OnPaint()
{
	CPaintDC dc(this);

	//绘画界面
	DrawCaption(&dc);
	DrawBackGround(&dc);
	DrawBorder(&dc);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载图标
	HICON hIcon=NULL;
	UINT uIconType=(m_uType&MB_ICONMASK);
	if (uIconType==MB_ICONHAND) hIcon=LoadIcon(NULL,IDI_HAND);
	else if (uIconType==MB_ICONQUESTION) hIcon=LoadIcon(NULL,IDI_QUESTION);
	else if (uIconType==MB_ICONEXCLAMATION) hIcon=LoadIcon(NULL,IDI_EXCLAMATION);
	else hIcon=LoadIcon(NULL,IDI_ASTERISK);

	//绘画图标
	INT nYPos=(m_rcString.bottom+m_rcString.top)/2-SMB_ICON_HEIGHT/2;
	DrawIconEx(dc,GetXBorder()+SMB_ICON_LEFT,nYPos,hIcon,SMB_ICON_WIDTH,SMB_ICON_HEIGHT,0,NULL,DI_NORMAL);

	//绘画字体
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(10,10,10));
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());
	dc.DrawText(m_strString,&m_rcString,DT_EXPANDTABS|DT_NOCLIP|DT_WORD_ELLIPSIS);

	return;
}

//时间消息
VOID CHintMsgBox::OnTimer(UINT_PTR nIDEvent)
{
	//倒数时间
	if (nIDEvent==IDI_MESSAGE)
	{
		//删除判断
		if (m_nElapse==0)
		{
			PostMessage(WM_CLOSE,0,0);
			return;
		}

		//设置界面
		TCHAR szTitle[32]=TEXT("");
		_sntprintf(szTitle,CountArray(szTitle),TEXT("%s （%ld 秒）"),m_strCaption,m_nElapse--);
		SetWindowText(szTitle);

		return;
	}

	__super::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////////

//显示消息
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformation(LPCTSTR pszString, UINT nElapse, UINT uType)
{
	CHintMsgBox HintMsgBox;

	return HintMsgBox.ShowMessageBox(pszString,uType,nElapse);
}

//显示消息
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformationEx(LPCTSTR pszString, UINT nElapse, UINT uType ,LPCTSTR pszCaption)
{
	CHintMsgBox HintMsgBox;

	return HintMsgBox.ShowMessageBox(pszCaption,pszString,uType,nElapse);
}

//////////////////////////////////////////////////////////////////////////


