#include "Stdafx.h"
#include "Resource.h"
#include "TableFrame.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//显示格式
#define	NT_RIGHT						0x01								//右对齐
#define	NT_CENTER						0x02								//中对齐
#define	NT_LEFT							0x04								//左对齐
#define	NT_TOP							0x10								//上对齐
#define	NT_VCENTER						0x20								//中对齐
#define	NT_BOTTOM						0x40								//下对齐

//////////////////////////////////////////////////////////////////////////

#define RESOURCE_VER					1									//资源版本

//资源配置结构
struct tagTableResourceInfo
{
	COLORREF							crName;								//名字颜色
	COLORREF							crNote;								//备注颜色
	COLORREF							crTableID;							//号码颜色
	POINT								ptLock;								//锁位置
	RECT								rcNote;								//备注位置
	RECT								rcTableID;							//桌号位置
	RECT								rcName[MAX_CHAIR];					//名字位置
	RECT								rcChair[MAX_CHAIR];					//椅子位置
	POINT								ptReady[MAX_CHAIR];					//准备位置
	INT									nDrawStyle[MAX_CHAIR];				//输出格式
};

//资源配置结构
struct tagTableResourceInfo_Normal
{
	COLORREF							crName;								//名字颜色
	COLORREF							crNote;								//备注颜色
	COLORREF							crTableID;							//号码颜色
	POINT								ptLock;								//锁位置
	RECT								rcNote;								//备注位置
	RECT								rcTableID;							//桌号位置
	RECT								rcName[MAX_CHAIR_NORMAL];			//名字位置
	RECT								rcChair[MAX_CHAIR_NORMAL];			//椅子位置
	POINT								ptReady[MAX_CHAIR_NORMAL];			//准备位置
	INT									nDrawStyle[MAX_CHAIR_NORMAL];		//输出格式
};

//配置文件结构
struct tagTableConfigFile
{
	WORD								wFileSize;							//文件大小
	WORD								wStructVer;							//结构版本
	tagTableResourceInfo				TableResourceInfo;					//配置参数
};

//配置文件结构
struct tagTableConfigFile_Normal
{
	WORD								wFileSize;							//文件大小
	WORD								wStructVer;							//结构版本
	tagTableResourceInfo_Normal			TableResourceInfo;					//配置参数
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableResource::CTableResource()
{
	//标志变量
	m_bDZShowHand=false;

	//设置变量
	m_wWidth=0;
	m_wHeight=0;
	m_crName=RGB(0,0,0);
	m_crNote=RGB(0,0,0);
	m_crTableID=RGB(0,0,0);
	m_crBackGround=RGB(0,0,0);
	memset(m_nDrawStyle,0,sizeof(m_nDrawStyle));
	return;
}

//析构函数
CTableResource::~CTableResource()
{
	DestroyResource();
	return;
}

//加载资源
bool CTableResource::LoadFromFiles(LPCTSTR pszGameName)
{
	try
	{
		//加载资源
		CFile ResourceFile;
		TCHAR szResourcePath[MAX_PATH];
		_snprintf(szResourcePath,sizeof(szResourcePath),TEXT("%s%s\\TableFrame.DAT"),g_GlobalUnits.GetWorkDirectory(),pszGameName);
		if (ResourceFile.Open(szResourcePath,CFile::modeRead,NULL)==FALSE) throw 0;

		//读取配置
		tagTableConfigFile TableConfig;
		ULONGLONG uLong=ResourceFile.GetLength();
		switch (uLong)
		{
		case sizeof(tagTableConfigFile):
			{
				UINT uReadCount=ResourceFile.Read(&TableConfig,sizeof(TableConfig));
				if (uReadCount!=sizeof(TableConfig)) throw 0;
				if (TableConfig.wFileSize!=sizeof(tagTableConfigFile)) throw 0;
				break;
			}
		case sizeof(tagTableConfigFile_Normal):
			{
				tagTableConfigFile_Normal TableConfig_Normal;
				UINT uReadCount=ResourceFile.Read(&TableConfig_Normal,sizeof(TableConfig_Normal));
				if (uReadCount!=sizeof(TableConfig_Normal)) throw 0;
				if (TableConfig_Normal.wFileSize!=sizeof(TableConfig_Normal)) throw 0;
				if (TableConfig_Normal.wStructVer!=1) throw 0;

				TableConfig.TableResourceInfo.crName=TableConfig_Normal.TableResourceInfo.crName;
				TableConfig.TableResourceInfo.crNote=TableConfig_Normal.TableResourceInfo.crNote;
				TableConfig.TableResourceInfo.crTableID=TableConfig_Normal.TableResourceInfo.crTableID;
				TableConfig.TableResourceInfo.ptLock=TableConfig_Normal.TableResourceInfo.ptLock;
				TableConfig.TableResourceInfo.rcNote=TableConfig_Normal.TableResourceInfo.rcNote;
				TableConfig.TableResourceInfo.rcTableID=TableConfig_Normal.TableResourceInfo.rcTableID;
				for (int i=0;i<8;i++)
				{
					TableConfig.TableResourceInfo.rcName[i]=TableConfig_Normal.TableResourceInfo.rcName[i];
					TableConfig.TableResourceInfo.rcChair[i]=TableConfig_Normal.TableResourceInfo.rcChair[i];
					TableConfig.TableResourceInfo.ptReady[i]=TableConfig_Normal.TableResourceInfo.ptReady[i];
					TableConfig.TableResourceInfo.nDrawStyle[i]=TableConfig_Normal.TableResourceInfo.nDrawStyle[i];
				}
				break;
			}
		default: throw 0;
		}

		//德州扑克
		if (lstrcmp(pszGameName,TEXT("DZShowHand"))==0)
		{
			m_bDZShowHand=true;
			_snprintf(szResourcePath,sizeof(szResourcePath),TEXT("%s%s\\Player.BMP"),g_GlobalUnits.GetWorkDirectory(),pszGameName);
			m_ImagePeople.Load(szResourcePath);
			if (m_ImagePeople.IsNull()) throw 0;
		}

		//加载锁位图
		_snprintf(szResourcePath,sizeof(szResourcePath),TEXT("%s%s\\Locker.BMP"),g_GlobalUnits.GetWorkDirectory(),pszGameName);
		m_ImageLock.Load(szResourcePath);
		if (m_ImageLock.IsNull()) 
		{
			m_ImageLock.LoadFromResource(AfxGetInstanceHandle(),IDB_TABLE_LOCK);
			if (m_ImageLock.IsNull()) throw 0;
		}

		//加载准备标志
		_snprintf(szResourcePath,sizeof(szResourcePath),TEXT("%s%s\\UserReady.BMP"),g_GlobalUnits.GetWorkDirectory(),pszGameName);
		m_ImageReady.Load(szResourcePath);
		if (m_ImageReady.IsNull()) 
		{
			m_ImageReady.LoadFromResource(AfxGetInstanceHandle(),IDB_TABLE_READY);
			if (m_ImageReady.IsNull()) throw 0;
			throw 0;
		}

		//加载开始标志
		_snprintf(szResourcePath,sizeof(szResourcePath),TEXT("%s%s\\TablePlaying.BMP"),g_GlobalUnits.GetWorkDirectory(),pszGameName);
		m_ImagePlay.Load(szResourcePath);
		if (m_ImagePlay.IsNull()) throw 0;

		//加载桌子位图
		_snprintf(szResourcePath,sizeof(szResourcePath),TEXT("%s%s\\TableNormal.BMP"),g_GlobalUnits.GetWorkDirectory(),pszGameName);
		m_ImageTable.Load(szResourcePath);
		if (m_ImageTable.IsNull()) throw 0;

		//读取参数
		m_wWidth=m_ImageTable.GetWidth();
		m_wHeight=m_ImageTable.GetHeight();
		m_crBackGround=m_ImageTable.GetPixel(0,0);

		//设置参数
		tagTableResourceInfo * pResourceInfo=&TableConfig.TableResourceInfo;
		m_ptLock=pResourceInfo->ptLock;
		m_rcNote=pResourceInfo->rcNote;
		m_crName=pResourceInfo->crName;
		m_crNote=pResourceInfo->crNote;
		m_crTableID=pResourceInfo->crTableID;
		m_rcTableID=pResourceInfo->rcTableID;
		for (WORD i=0;i<MAX_CHAIR;i++)
		{
			m_ptReady[i]=pResourceInfo->ptReady[i];
			m_rcName[i]=pResourceInfo->rcName[i];
			m_rcChair[i]=pResourceInfo->rcChair[i];
			m_nDrawStyle[i]=pResourceInfo->nDrawStyle[i];
		}

		return true;
	}
	catch (...) { DestroyResource(); }

	return false;
}

//卸载资源
void CTableResource::DestroyResource()
{
	//设置变量
	m_bDZShowHand=false;

	//释放位图
	m_ImageLock.Destroy();
	m_ImagePlay.Destroy();
	m_ImageReady.Destroy();
	m_ImageTable.Destroy();
	m_ImagePeople.Destroy();

	//设置变量
	m_wWidth=0;
	m_wHeight=0;
	m_crName=RGB(0,0,0);
	m_crNote=RGB(0,0,0);
	m_crTableID=RGB(0,0,0);
	m_crBackGround=RGB(0,0,0);
	memset(m_nDrawStyle,0,sizeof(m_nDrawStyle));

	return;
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTableFrame, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

//构造函数
CTableView::CTableView()
{
	m_bMouseDown=false;
	m_bFocusFrame=false;
	m_pITableFrameView=NULL;
	m_wHoverChairID=INVALID_CHAIR;
	memset(&m_TableState,0,sizeof(m_TableState));
}

//析构函数
CTableView::~CTableView()
{
}

//接口查询
void * __cdecl CTableView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableView,Guid,dwQueryVer);
	return NULL;
}

//初始化函数
void __cdecl CTableView::InitTableView(WORD wTableID, WORD wChairCount, ITableFrameView * pITableFrameView)
{
	m_TableState.wTableID=wTableID;
	m_TableState.wChairCount=wChairCount;
	m_pITableFrameView=pITableFrameView;
	return;
}

//设置用户信息
bool __cdecl CTableView::SetUserInfo(WORD wChairID, IUserItem * pIUserItem)
{
	if (wChairID>=CountArray(m_TableState.pIUserItem)) return false;
	m_TableState.pIUserItem[wChairID]=pIUserItem;
	m_pITableFrameView->UpdateTableView(m_TableState.wTableID,false);
	return true;
}

//设置鼠标按下
void __cdecl CTableView::SetMouseDown(bool bMouseDown)
{
	if (m_bMouseDown!=bMouseDown)
	{
		m_bMouseDown=bMouseDown; 
		m_pITableFrameView->UpdateTableView(m_TableState.wTableID,true);
	}
	return;
}

//设置焦点框架
void __cdecl CTableView::SetFocusFrame(bool bFocusFrame)
{
	if (m_bFocusFrame!=bFocusFrame)
	{
		m_bFocusFrame=bFocusFrame;
		m_pITableFrameView->UpdateTableView(m_TableState.wTableID,false);
	}
	return;
}

//设置盘旋位置
void __cdecl CTableView::SetHoverChairID(WORD wHoverChairID)
{
	if (m_wHoverChairID!=wHoverChairID)
	{
		m_wHoverChairID=wHoverChairID;
		m_pITableFrameView->UpdateTableView(m_TableState.wTableID,true);
	}
	return;
}

//设置游戏标志 
void __cdecl CTableView::SetPlayFlag(bool bPlaying)
{
	if (m_TableState.bPlaying!=bPlaying)
	{
		m_TableState.bPlaying=bPlaying; 
		m_pITableFrameView->UpdateTableView(m_TableState.wTableID,false);
	}
	return;
}

//设置密码标志
void __cdecl CTableView::SetPassFlag(bool bPass)
{
	if (m_TableState.bPass!=bPass)
	{
		m_TableState.bPass=bPass; 
		m_pITableFrameView->UpdateTableView(m_TableState.wTableID,false);
	}
	return;
}

//设置备注
void __cdecl CTableView::SetTableNote(LPCTSTR pszNote)
{
	lstrcpyn(m_TableState.szTableNote,pszNote,CountArray(m_TableState.szTableNote));
	m_pITableFrameView->UpdateTableView(m_TableState.wTableID,true);
	return;
}

//获取用户信息
IUserItem * __cdecl CTableView::GetUserInfo(WORD wChairID)
{
	if (wChairID>=CountArray(m_TableState.pIUserItem)) return NULL;
	return m_TableState.pIUserItem[wChairID];
}

//获取空椅子数
WORD __cdecl CTableView::GetNullChairCount(WORD & wNullChairID)
{
	WORD wNullCount=0;
	wNullChairID=INVALID_CHAIR;
	for (WORD i=0;i<m_TableState.wChairCount;i++)
	{
		if (m_TableState.pIUserItem[i]==NULL)
		{
			++wNullCount;
			wNullChairID=i;
		}
	}
	return wNullCount;
}

//绘画函数
void __cdecl CTableView::DrawTable(CDC * pDC, bool bHideInfo, CTableResource * pTableResource, bool bShowUser)
{
	//绘画底图
	if ((m_TableState.bPlaying)||((bHideInfo==true)&&(m_TableState.wTableID>0)))
	{
		pTableResource->m_ImagePlay.BitBlt(pDC->m_hDC,0,0);
	}
	else pTableResource->m_ImageTable.BitBlt(pDC->m_hDC,0,0);

	//绘画信息
	COLORREF crBackGround=pTableResource->m_crBackGround;
	IUserFaceRes * pIUserFaceRes=g_GlobalUnits.GetUserFaceRes();
	for (WORD i=0;i<m_TableState.wChairCount;i++)
	{
		//绘画用户
		if ((bHideInfo==true)&&(bShowUser==true))
		{
			//变量定义
			CRect rcDrawRect=pTableResource->m_rcName[i];
			LPCTSTR pszString=(m_TableState.wTableID==0)?TEXT("点击加入"):TEXT("分配位置");

			//位置变量
			int nYPos=0;
			int nDrawStyte=pTableResource->m_nDrawStyle[i];
			int nDrawFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS;
			int nNameLength=lstrlen(pszString);
			int nNameHeight=pDC->DrawText(pszString,nNameLength,&rcDrawRect,nDrawFormat|DT_CALCRECT);

			//调整位置
			if (nDrawStyte&NT_LEFT) nDrawFormat|=DT_LEFT;
			else if (nDrawStyte&NT_CENTER) nDrawFormat|=DT_CENTER;
			else nDrawFormat|=DT_RIGHT;
			if (nDrawStyte&NT_VCENTER) nYPos=(pTableResource->m_rcName[i].Height()-nNameHeight)/2;
			else if (nDrawStyte&NT_BOTTOM) nYPos=(pTableResource->m_rcName[i].Height()-nNameHeight);
			if (nYPos<0) nYPos=0;
			rcDrawRect.left=pTableResource->m_rcName[i].left;
			rcDrawRect.right=pTableResource->m_rcName[i].right;
			rcDrawRect.top=pTableResource->m_rcName[i].top+nYPos;
			rcDrawRect.bottom=pTableResource->m_rcName[i].bottom;

			//用户信息
			if (m_TableState.wTableID==0)
			{
				//用户名字
				pDC->SetTextColor(pTableResource->m_crName);
				pDC->DrawText(pszString,nNameLength,&rcDrawRect,nDrawFormat);
			}
			else
			{
				//用户头像
				pDC->FillSolidRect(&pTableResource->m_rcChair[i],crBackGround);
				pIUserFaceRes->DrawNormalFace(pDC,pTableResource->m_rcChair[i].left+1,
					pTableResource->m_rcChair[i].top+1,((m_TableState.wTableID-1)*m_TableState.wChairCount+i)%pIUserFaceRes->GetFaceCount(),0,0);

				//用户名字
				pDC->SetTextColor(pTableResource->m_crName);
				pDC->DrawText(pszString,nNameLength,&rcDrawRect,nDrawFormat);
			}
		}

		//绘画用户
		if ((bHideInfo==false)&&(bShowUser==true)&&(m_TableState.pIUserItem[i]!=NULL))
		{
			//变量定义
			CRect rcDrawRect=pTableResource->m_rcName[i];
			tagUserData * pUserData=m_TableState.pIUserItem[i]->GetUserData();

			//自定义头像
			tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();

			//用户头像
			if (pTableResource->m_bDZShowHand==true)
			{
				//变量定义
				BYTE cbGender=m_TableState.pIUserItem[i]->GetUserData()->cbGender;
				CSize SizePeople(pTableResource->m_ImagePeople.GetWidth()/8,pTableResource->m_ImagePeople.GetHeight()/2);

				//计算位置
				INT nXImagePos=SizePeople.cx*i;
				INT nYImagePos=(cbGender==0)?0:SizePeople.cy;
				pTableResource->m_ImagePeople.AlphaDrawImage(pDC,pTableResource->m_rcChair[i].left+1,pTableResource->m_rcChair[i].top+1,
					SizePeople.cx,SizePeople.cy,nXImagePos,nYImagePos,RGB(255,0,255));
			}
			else
			{
				pDC->FillSolidRect(&pTableResource->m_rcChair[i],crBackGround);
				pIUserFaceRes->DrawNormalFace(pDC,pTableResource->m_rcChair[i].left+1,
					pTableResource->m_rcChair[i].top+1,pUserData->wFaceID,pUserData->dwUserID,pUserData->dwCustomFaceVer);
			}

			//同意标志
			if ((m_TableState.bPlaying==false)&&(pUserData->cbUserStatus==US_READY))
			{
				pTableResource->m_ImageReady.AlphaDrawImage(pDC,pTableResource->m_ptReady[i].x,
					pTableResource->m_ptReady[i].y,RGB(255,0,255));
			}

			//位置变量
			int nYPos=0;
			int nDrawStyte=pTableResource->m_nDrawStyle[i];
			int nDrawFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS;
			int nNameLength=lstrlen(pUserData->szName);
			int nNameHeight=pDC->DrawText(pUserData->szName,nNameLength,&rcDrawRect,nDrawFormat|DT_CALCRECT);

			//调整位置
			if (nDrawStyte&NT_LEFT) nDrawFormat|=DT_LEFT;
			else if (nDrawStyte&NT_CENTER) nDrawFormat|=DT_CENTER;
			else nDrawFormat|=DT_RIGHT;
			if (nDrawStyte&NT_VCENTER) nYPos=(pTableResource->m_rcName[i].Height()-nNameHeight)/2;
			else if (nDrawStyte&NT_BOTTOM) nYPos=(pTableResource->m_rcName[i].Height()-nNameHeight);
			if (nYPos<0) nYPos=0;
			rcDrawRect.left=pTableResource->m_rcName[i].left;
			rcDrawRect.right=pTableResource->m_rcName[i].right;
			rcDrawRect.top=pTableResource->m_rcName[i].top+nYPos;
			rcDrawRect.bottom=pTableResource->m_rcName[i].bottom;

			//用户名字
			pDC->SetTextColor(pTableResource->m_crName);
			pDC->DrawText(pUserData->szName,nNameLength,&rcDrawRect,nDrawFormat);
		}

		//绘画边框
		if (m_wHoverChairID==i)
		{
			if (m_bMouseDown) pDC->Draw3dRect(&pTableResource->m_rcChair[i],RGB(125,125,125),RGB(255,255,255));
			else pDC->Draw3dRect(&pTableResource->m_rcChair[i],RGB(255,255,255),RGB(125,125,125));
		}
	}

	//绘画锁
	if (m_TableState.bPass==true)
	{
		int nXLock=pTableResource->m_ptLock.y-pTableResource->m_ImageLock.GetWidth()/2;
		int nYLock=pTableResource->m_ptLock.y-pTableResource->m_ImageLock.GetHeight()/2;
		pTableResource->m_ImageLock.AlphaDrawImage(pDC,nXLock,nYLock,RGB(255,0,255));
	}

	//绘画桌号
	if (m_TableState.szTableNote[0]==0)
	{
		TCHAR szBuffer[32];
		pDC->SetTextColor(pTableResource->m_crTableID);
		_snprintf(szBuffer,sizeof(szBuffer),TEXT("- %d -"),m_TableState.wTableID+1);
		pDC->DrawText(szBuffer,lstrlen(szBuffer),&pTableResource->m_rcTableID,DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
	}
	else
	{
		pDC->SetTextColor(pTableResource->m_crTableID);
		pDC->DrawText(m_TableState.szTableNote,lstrlen(m_TableState.szTableNote),&pTableResource->m_rcTableID,DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//静态变量
int					CTableFrame::m_iScrollPos=10;					//滚动象素
int					CTableFrame::m_iWhellTimes=2;					//滚轮倍数

//构造函数
CTableFrame::CTableFrame()
{
	//状态变量
	m_bLeftMouseDown=false;
	m_bRightMouseDown=false;
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;
	m_wFrameTableID=INVALID_TABLE;

	//滚动信息
	m_iCountRow=0;
	m_iXExcursion=0;
	m_iCurrentPos=0;
	m_iMaxScrollPos=0;	
	m_iCountAllLine=0;
	m_iCountVisibleLine=0;

	//配置变量
	m_wTableCount=0;
	m_wChairCount=0;
	m_bShowUser=false;
	m_bHideInfo=false;

	//设置指针
	m_pTableViewArray=NULL;
	m_pITableFrameSink=NULL;

	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	DestroyTableFrame();
	return;
}

//接口查询
void * __cdecl CTableFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameView,Guid,dwQueryVer);
	return NULL;
}

//创建函数
bool __cdecl CTableFrame::CreateTableFrame(CWnd * pParentWnd, UINT uWndID)
{
	CRect rcFrameRect(0,0,0,0);
	DWORD dwStyle=WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	Create(NULL,NULL,dwStyle,rcFrameRect,pParentWnd,uWndID);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	return true;
}

//配置函数
bool __cdecl CTableFrame::InitTableFrame(WORD wTableCount, WORD wChairCount, bool bHideInfo, const tagGameKind * pGameKind, IUnknownEx * pIUnknownEx)
{
	//效验参数
	ASSERT(pGameKind!=NULL);
	ASSERT(wChairCount<=MAX_CHAIR);

	//变量定义
	bool bLoadResource=false;

	//获取资源
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	GetGameResDirectory(pGameKind,szDirectory,CountArray(szDirectory));

	//文件加载
	if (m_TableResource.LoadFromFiles(szDirectory)==false) throw TEXT("游戏桌子资源加载失败，请尝试重新安装游戏解决问题！");

	//创建游戏桌子
	m_bHideInfo=bHideInfo;
	m_wTableCount=wTableCount;
	m_wChairCount=wChairCount;
	m_pTableViewArray=new CTableView[m_wTableCount];
	if (m_pTableViewArray==NULL) throw TEXT("可能系统资源不足，无法创建游戏桌子！");
	for (WORD i=0;i<m_wTableCount;i++) (m_pTableViewArray+i)->InitTableView(i,wChairCount,this);

	//设置接口
	m_pITableFrameSink=(ITableFrameSink *)pIUnknownEx->QueryInterface(IID_ITableFrameSink,VER_ITableFrameSink);
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink==NULL) throw TEXT("接口查询失败，无法创建游戏桌子！");

	return true;
}

//销毁函数
void __cdecl CTableFrame::DestroyTableFrame()
{
	//销毁控件
	if (m_hWnd!=NULL) DestroyWindow();
	SafeDeleteArray(m_pTableViewArray);
	m_TableResource.DestroyResource();

	//滚动变量
	m_iCountRow=0;
	m_iXExcursion=0;
	m_iCurrentPos=0;
	m_iMaxScrollPos=0;	
	m_iCountAllLine=0;
	m_iCountVisibleLine=0;

	//状态变量
	m_bLeftMouseDown=false;
	m_bRightMouseDown=false;
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;
	m_wFrameTableID=INVALID_TABLE;

	//配置变量
	m_wTableCount=0;
	m_wChairCount=0;
	m_bShowUser=false;

	//设置指针
	m_pTableViewArray=NULL;
	m_pITableFrameSink=NULL;

	return;
}

//更新桌子
void __cdecl CTableFrame::UpdateTableView(WORD wTableID, bool bMustUpdate)
{
	if ((m_hWnd!=NULL)&&((m_bShowUser==true)||(bMustUpdate==true)))
	{
		ITableView * pITableView=GetTableArrayPtr(wTableID);
		if ((pITableView!=NULL)&&(m_iCountRow!=0))
		{
			RECT rcTableView;
			rcTableView.left=(wTableID%m_iCountRow)*m_TableResource.m_wWidth+m_iXExcursion;
			rcTableView.top=(wTableID/m_iCountRow)*m_TableResource.m_wHeight-m_iCurrentPos;
			rcTableView.right=rcTableView.left+m_TableResource.m_wWidth;
			rcTableView.bottom=rcTableView.top+m_TableResource.m_wHeight;
			InvalidateRect(&rcTableView,FALSE);
		}
	}

	return;
}

//获取游戏桌指针
ITableView * __cdecl CTableFrame::GetTableArrayPtr(WORD wTableID)
{
	if ((m_pTableViewArray==NULL)||(wTableID>=m_wTableCount)) return NULL;
	return (m_pTableViewArray+wTableID);
}

//设置显示
bool __cdecl CTableFrame::ShowUserInfo(bool bShowUser)
{
	m_bShowUser=bShowUser;
	if (m_hWnd!=NULL) Invalidate(FALSE);
	return true;
}

//设置视图桌子
bool __cdecl CTableFrame::EnsureVisibleTable(WORD wTableID)
{
	//效验参数
	ASSERT(wTableID<m_wTableCount);
	if (wTableID>=m_wTableCount) return false;

	//计算位置
	int iLastPos=m_iCurrentPos;
	int iXPos=(wTableID%m_iCountRow)*m_TableResource.m_wWidth+m_iXExcursion;
	int iYPos=(wTableID/m_iCountRow)*m_TableResource.m_wHeight-m_iCurrentPos;

	//构造位置
	CRect rcTable,rcClient;
	GetClientRect(&rcClient);
	rcTable.SetRect(iXPos,iYPos,iXPos+m_TableResource.m_wWidth,iYPos+m_TableResource.m_wHeight);
	if ((rcTable.bottom>rcClient.bottom)||(rcTable.top<0)) m_iCurrentPos+=rcTable.bottom-rcClient.bottom;
	m_iCurrentPos=__max(0,__min(m_iCurrentPos,m_iMaxScrollPos-m_iCountVisibleLine*m_TableResource.m_wHeight));

	//滚动视图
	if (iLastPos!=m_iCurrentPos)
	{
		SetScrollPos(SB_VERT,m_iCurrentPos);
		ScrollWindow(0,iLastPos-m_iCurrentPos,NULL,NULL);
	}

	return true;
}

//闪动椅子
bool __cdecl CTableFrame::FlashTable(WORD wTableID)
{
	//获取桌子
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	if (pITableView==NULL) return false;

	//计算位置
	CRect rcTable;
	int iTableRow=wTableID%m_iCountRow;
	int iTableLine=wTableID/m_iCountRow;
	rcTable.left=iTableRow*m_TableResource.m_wWidth+m_iXExcursion;
	rcTable.top=iTableLine*m_TableResource.m_wHeight-m_iCurrentPos;
	rcTable.right=rcTable.left+m_TableResource.m_wWidth;
	rcTable.bottom=rcTable.top+m_TableResource.m_wHeight;
	
	//定义参数
	const int nFlashTimes=18,nStepWidth=4;
	const int nExpandPos=nFlashTimes*nStepWidth;
	rcTable.InflateRect(nExpandPos,nExpandPos);

	//循环绘画
	UpdateWindow();
	CDC * pDC=GetDC();
	CPen LinePen(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));
	CPen * pOldPen=pDC->SelectObject(&LinePen);
	pDC->SetROP2(R2_XORPEN);
	for (int nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		pDC->MoveTo(rcTable.left,rcTable.top);
		pDC->LineTo(rcTable.right,rcTable.top);
		pDC->LineTo(rcTable.right,rcTable.bottom);
		pDC->LineTo(rcTable.left,rcTable.bottom);
		pDC->LineTo(rcTable.left,rcTable.top);
		Sleep((nFlashTimes-nTimes)*2);
		pDC->MoveTo(rcTable.left,rcTable.top);
		pDC->LineTo(rcTable.right,rcTable.top);
		pDC->LineTo(rcTable.right,rcTable.bottom);
		pDC->LineTo(rcTable.left,rcTable.bottom);
		pDC->LineTo(rcTable.left,rcTable.top);
		rcTable.DeflateRect(nStepWidth,nStepWidth);
	}
	pDC->SelectObject(pOldPen);
	ReleaseDC(pDC);

	return true;
}

//闪动椅子
bool __cdecl CTableFrame::FlashChair(WORD wTableID, WORD wChairID)
{
	//获取桌子
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	if (pITableView==NULL) return false;

	//获取位置
	CRect rcChair=m_TableResource.m_rcChair[wChairID];

	//获取视图位置
	int iTableRow=wTableID%m_iCountRow;
	int iTableLine=wTableID/m_iCountRow;
	int iXPos=iTableRow*m_TableResource.m_wWidth+m_iXExcursion;
	int iYPos=iTableLine*m_TableResource.m_wHeight-m_iCurrentPos;
	rcChair.OffsetRect(iXPos,iYPos);

	//定义参数
	const int nFlashTimes=18,nStepWidth=4;
	const int nExpandPos=nFlashTimes*nStepWidth;
	rcChair.InflateRect(nExpandPos,nExpandPos);

	//循环绘画
	UpdateWindow();
	CDC * pDC=GetDC();
	CPen LinePen(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));
	CPen * pOldPen=pDC->SelectObject(&LinePen);
	pDC->SetROP2(R2_XORPEN);
	for (int nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		pDC->MoveTo(rcChair.left,rcChair.top);
		pDC->LineTo(rcChair.right,rcChair.top);
		pDC->LineTo(rcChair.right,rcChair.bottom);
		pDC->LineTo(rcChair.left,rcChair.bottom);
		pDC->LineTo(rcChair.left,rcChair.top);
		Sleep((nFlashTimes-nTimes)*2);
		pDC->MoveTo(rcChair.left,rcChair.top);
		pDC->LineTo(rcChair.right,rcChair.top);
		pDC->LineTo(rcChair.right,rcChair.bottom);
		pDC->LineTo(rcChair.left,rcChair.bottom);
		pDC->LineTo(rcChair.left,rcChair.top);
		rcChair.DeflateRect(nStepWidth,nStepWidth);
	}
	pDC->SelectObject(pOldPen);
	ReleaseDC(pDC);

	return true;
}

//设置用户信息
bool __cdecl CTableFrame::SetUserInfo(WORD wTableID, WORD wChairID, IUserItem * pIUserItem)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView==NULL) return false;
	return pITableView->SetUserInfo(wChairID,pIUserItem);
}

//设置游戏标志 
void __cdecl CTableFrame::SetPlayFlag(WORD wTableID, bool bPlaying)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView!=NULL) pITableView->SetPlayFlag(bPlaying);
	return;
}

//设置密码标志
void __cdecl CTableFrame::SetPassFlag(WORD wTableID, bool bPass)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView!=NULL) pITableView->SetPassFlag(bPass);
	return;
}

//获取用户信息
IUserItem * __cdecl CTableFrame::GetUserInfo(WORD wTableID, WORD wChairID)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView==NULL) return NULL;
	return pITableView->GetUserInfo(wChairID);
}

//获取空椅子数
WORD __cdecl CTableFrame::GetNullChairCount(WORD wTableID, WORD & wNullChairID)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView==NULL) return 0;
	return pITableView->GetNullChairCount(wNullChairID);
}

//查询游戏标志
bool __cdecl CTableFrame::QueryPlayFlag(WORD wTableID)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView==NULL) return false;
	return pITableView->QueryPlayFlag();
}

//查询密码标志
bool __cdecl CTableFrame::QueryPassFlag(WORD wTableID)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView==NULL) return false;
	return pITableView->QueryPassFlag();
}

//获取桌子属性
const tagTableState * CTableFrame::GetTableAttrib(WORD wTableID)
{
	ITableView * pITableView=GetTableArrayPtr(wTableID);
	ASSERT(pITableView!=NULL);
	if (pITableView==NULL) return NULL;
	return pITableView->GetTableAttrib();
}

//重画消息
void CTableFrame::OnPaint()
{
	//创建 DC
	CPaintDC dc(this);

	//获取区域
	CRect rcClient,ClipRect;
	dc.GetClipBox(&ClipRect);
	GetClientRect(&rcClient);
	
	//绘画游戏桌
	if (m_pTableViewArray!=NULL)
	{
		//创建字体
		CFont DrawFont;
		DrawFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("宋体"));

		//定义变量
		int iTableWidth=m_TableResource.m_wWidth;
		int iTableHeight=m_TableResource.m_wHeight;

		//建立缓冲图
		CDC BufferDC;
		CBitmap BufferBmp;
		BufferDC.CreateCompatibleDC(&dc);
		BufferDC.SetBkMode(TRANSPARENT);
		BufferDC.SelectObject(&DrawFont);
		BufferBmp.CreateCompatibleBitmap(&dc,iTableWidth,iTableHeight);
		BufferDC.SelectObject(&BufferBmp);

		//绘画桌子
		RECT rcTableView;
		ITableView * pITableView=NULL;
		int iBeginRow=(ClipRect.left-m_iXExcursion)/iTableWidth;
		int iBeginLine=(m_iCurrentPos+ClipRect.top)/iTableHeight;
		int iEndDrawRow=(ClipRect.right-m_iXExcursion+iTableWidth-1)/iTableWidth;
		int iEndDrawLine=(m_iCurrentPos+ClipRect.bottom+iTableHeight-1)/iTableHeight;
		for (int iLine=iBeginLine;iLine<iEndDrawLine;iLine++)
		{
			for (int iRow=iBeginRow;iRow<iEndDrawRow;iRow++)
			{
				int iTableIndex=iLine*m_iCountRow+iRow;
				rcTableView.left=iRow*iTableWidth+m_iXExcursion;
				rcTableView.top=iLine*iTableHeight-m_iCurrentPos;
				rcTableView.right=rcTableView.left+iTableWidth;
				rcTableView.bottom=rcTableView.top+iTableHeight;
				if ((iRow<m_iCountRow)&&(iTableIndex<m_wTableCount))
				{
					pITableView=GetTableArrayPtr(iLine*m_iCountRow+iRow);
					ASSERT(pITableView!=NULL);
					pITableView->DrawTable(&BufferDC,m_bHideInfo,&m_TableResource,m_bShowUser);
					dc.BitBlt(rcTableView.left,rcTableView.top,iTableWidth,iTableHeight,&BufferDC,0,0,SRCCOPY);
				}
				else dc.FillSolidRect(&rcTableView,m_TableResource.m_crBackGround);
			}
		}

		//绘画空隙
		if (m_iXExcursion>ClipRect.left)
			dc.FillSolidRect(ClipRect.left,ClipRect.top,m_iXExcursion-ClipRect.left,ClipRect.bottom,m_TableResource.m_crBackGround);

		//清理资源
		BufferDC.DeleteDC();
		DrawFont.DeleteObject();
		BufferBmp.DeleteObject();
	}
	else dc.FillSolidRect(&ClipRect,m_TableResource.m_crBackGround);

	return;
}

//位置消息
void CTableFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//定义变量
	int iTableWidth=m_TableResource.m_wWidth;
	int iTableHeight=m_TableResource.m_wHeight;

	//调整位置
	if (m_wTableCount!=0)
	{
		int iViewWidth=__max(iTableWidth,cx);
		int iViewHeight=__max(iTableHeight,cy);
		m_iCountRow=iViewWidth/iTableWidth;
		m_iCountVisibleLine=iViewHeight/iTableHeight;
		m_iCountAllLine=(m_wTableCount+m_iCountRow-1)/m_iCountRow;
		m_iXExcursion=(iViewWidth-m_iCountRow*iTableWidth)/2;
	}
	
	//设置滚动数据
	m_iMaxScrollPos=m_iCountAllLine*iTableHeight;
	m_iCurrentPos=__min(m_iCurrentPos,m_iMaxScrollPos-m_iCountVisibleLine*iTableHeight);
	if (m_iCurrentPos<0) m_iCurrentPos=0;
	
	//设置滚动条
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize=sizeof(SCROLLINFO);
	ScrollInfo.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
	ScrollInfo.nMin=0;
	ScrollInfo.nMax=m_iMaxScrollPos;
	ScrollInfo.nPage=cy;
	ScrollInfo.nPos=m_iCurrentPos;
	SetScrollInfo(SB_VERT,&ScrollInfo,TRUE);

	return;
}

//消息解释
BOOL CTableFrame::PreTranslateMessage(MSG * pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_UP:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_LINEUP,m_iCurrentPos),NULL);
				return TRUE;
			}
		case VK_DOWN:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_LINEDOWN,m_iCurrentPos),NULL);
				return TRUE;
			}
		case VK_PRIOR:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEUP,m_iCurrentPos),NULL);
				return TRUE;
			}
		case VK_NEXT:
		case VK_SPACE:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEDOWN,m_iCurrentPos),NULL);
				return TRUE;
			}
		case VK_HOME:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_TOP,m_iCurrentPos),NULL);
				return TRUE;
			}
		case VK_END:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_BOTTOM,m_iCurrentPos),NULL);
				return TRUE;
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

//滚动消息
void CTableFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	//获取参数
	RECT rcClientRect;
	int iLastPos=m_iCurrentPos;
	GetClientRect(&rcClientRect);

	//移动坐标
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_iCurrentPos=0;
			break;
		}
	case SB_BOTTOM:
		{
			m_iCurrentPos=m_iMaxScrollPos-m_iCountVisibleLine*m_TableResource.m_wHeight;
			break;
		}
	case SB_LINEUP:
		{
			m_iCurrentPos-=m_iScrollPos;
			break;
		}
	case SB_PAGEUP:
		{
			m_iCurrentPos-=rcClientRect.bottom;
			break;
		}
	case SB_LINEDOWN:
		{
			m_iCurrentPos+=m_iScrollPos;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_iCurrentPos+=rcClientRect.bottom;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_iCurrentPos=nPos;
			break;
		}
	}
	
	//调整位置
	SetFocus();
	m_iCurrentPos=__min(m_iCurrentPos,m_iMaxScrollPos-rcClientRect.bottom);
	if (m_iCurrentPos<0) m_iCurrentPos=0;
	if (iLastPos!=m_iCurrentPos)
	{
		SetScrollPos(SB_VERT,m_iCurrentPos);
		ScrollWindow(0,iLastPos-m_iCurrentPos,NULL,NULL);
	}

	return;
}

//鼠标消息
void CTableFrame::OnLButtonDblClk(UINT nFlags, CPoint Point)
{
	SetFocus();
	m_pITableFrameSink->OnDoubleHitTable(m_wDownTableID,false);
	return;
}

//鼠标消息
void CTableFrame::OnLButtonDown(UINT nFlags, CPoint Point)
{
	SetFocus();
	if ((m_bRightMouseDown==false)&&(m_bLeftMouseDown==false))
	{
		SetCapture();
		m_bLeftMouseDown=true;
		m_wDownTableID=TableHitTest(Point);
		if (m_wDownTableID!=INVALID_TABLE)
		{
			ITableView * pITableView=GetTableArrayPtr(m_wDownTableID);
			ASSERT(pITableView!=NULL);
			CPoint TablePoint;
			TablePoint.x=(Point.x-m_iXExcursion)%m_TableResource.m_wWidth;
			TablePoint.y=(Point.y+m_iCurrentPos)%m_TableResource.m_wHeight;
			m_wDownChairID=ChairHitTest(TablePoint);
			if (m_wDownChairID!=INVALID_CHAIR) 
			{
				pITableView->SetMouseDown(m_bLeftMouseDown);
				pITableView->SetHoverChairID(m_wDownChairID);
			}
		}
	}
	return;
}

//鼠标消息
void CTableFrame::OnLButtonUp(UINT nFlags, CPoint Point)
{
	if ((m_bLeftMouseDown==true)&&(m_bRightMouseDown==false))
	{
		//测试按键
		ReleaseCapture();
		m_bLeftMouseDown=false;
		WORD wTableID=TableHitTest(Point);
		ITableView * pITableView=GetTableArrayPtr(m_wDownTableID);
		if (pITableView!=NULL) 
		{
			pITableView->SetMouseDown(false);
			pITableView->SetHoverChairID(INVALID_CHAIR);
		}

		//获取位置
		WORD wChairID=INVALID_CHAIR;
		if (wTableID==m_wDownTableID)
		{
			ITableView * pITableView=GetTableArrayPtr(wTableID);
			if (pITableView!=NULL)
			{
				CPoint TablePoint;
				TablePoint.x=(Point.x-m_iXExcursion)%m_TableResource.m_wWidth;
				TablePoint.y=(Point.y+m_iCurrentPos)%m_TableResource.m_wHeight;
				wChairID=ChairHitTest(TablePoint);
			}
		}

		//发送消息
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
			m_pITableFrameSink->OnLButtonHitTable(wTableID,wChairID,false);
		}
		else
		{
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//鼠标消息
void CTableFrame::OnRButtonDown(UINT nFlags, CPoint Point)
{
	SetFocus();
	if ((m_bRightMouseDown==false)&&(m_bLeftMouseDown==false))
	{
		SetCapture();
		m_bRightMouseDown=true;
		m_wDownTableID=TableHitTest(Point);
		if (m_wDownTableID!=INVALID_TABLE)
		{
			ITableView * pITableView=GetTableArrayPtr(m_wDownTableID);
			ASSERT(pITableView!=NULL);
			CPoint TablePoint;
			TablePoint.x=(Point.x-m_iXExcursion)%m_TableResource.m_wWidth;
			TablePoint.y=(Point.y+m_iCurrentPos)%m_TableResource.m_wHeight;
			m_wDownChairID=ChairHitTest(TablePoint);
			if (m_wDownChairID!=INVALID_CHAIR) 
			{
				pITableView->SetMouseDown(m_bRightMouseDown);
				pITableView->SetHoverChairID(m_wDownChairID);
			}
		}
	}
	return;
}

//鼠标消息
void CTableFrame::OnRButtonUp(UINT nFlags, CPoint Point)
{
	if (m_bRightMouseDown==true)	
	{
		//测试按键
		ReleaseCapture();
		m_bRightMouseDown=false;
		WORD wTableID=TableHitTest(Point);
		ITableView * pITableView=GetTableArrayPtr(m_wDownTableID);
		if (pITableView!=NULL) 
		{
			pITableView->SetMouseDown(false);
			pITableView->SetHoverChairID(INVALID_CHAIR);
		}

		//获取位置
		WORD wChairID=INVALID_CHAIR;
		if (wTableID==m_wDownTableID)
		{
			ITableView * pITableView=GetTableArrayPtr(wTableID);
			if (pITableView!=NULL)
			{
				CPoint TablePoint;
				TablePoint.x=(Point.x-m_iXExcursion)%m_TableResource.m_wWidth;
				TablePoint.y=(Point.y+m_iCurrentPos)%m_TableResource.m_wHeight;
				wChairID=ChairHitTest(TablePoint);
			}
		}

		//发送消息
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
			m_pITableFrameSink->OnRButtonHitTable(wTableID,wChairID,false);
		}
		else
		{
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//鼠标消息
BOOL CTableFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	RECT rcClientRect;
	GetClientRect(&rcClientRect);
	int iLastPos=m_iCurrentPos;
	int iMaxPos=m_iMaxScrollPos-rcClientRect.bottom;
	m_iCurrentPos=__min(m_iCurrentPos-zDelta/m_iWhellTimes,iMaxPos);
	if (m_iCurrentPos<0) m_iCurrentPos=0;
	SetScrollPos(SB_VERT,m_iCurrentPos);
	ScrollWindow(0,iLastPos-m_iCurrentPos,NULL,NULL);
	return TRUE;
}

//光标消息
BOOL CTableFrame::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if ((m_bRightMouseDown==false)&&(m_bLeftMouseDown==false))
	{
		//变量定义
		int iTableWidth=m_TableResource.m_wWidth;
		int iTableHeight=m_TableResource.m_wHeight;

		//测试按键
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		WORD wNewChairID=INVALID_CHAIR;
		WORD wNewTableID=TableHitTest(MousePoint);
		if (wNewTableID<m_wTableCount)
		{
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_iXExcursion)%iTableWidth;
			TablePoint.y=(MousePoint.y+m_iCurrentPos)%iTableHeight;
			wNewChairID=ChairHitTest(TablePoint);
		}
		
		//更新界面
		if (((wNewTableID!=m_wDownTableID)||(wNewChairID!=m_wDownChairID))
			&&((wNewTableID!=INVALID_CHAIR)||(m_wDownChairID!=INVALID_CHAIR)))
		{
			if (m_wDownTableID!=INVALID_TABLE) GetTableArrayPtr(m_wDownTableID)->SetHoverChairID(INVALID_CHAIR);
			if (wNewTableID!=INVALID_TABLE) GetTableArrayPtr(wNewTableID)->SetHoverChairID(wNewChairID);
		}
		
		//设置数据
		m_wDownTableID=wNewTableID;
		m_wDownChairID=wNewChairID;
		if ((m_wDownTableID!=INVALID_TABLE)&&(m_wDownChairID!=INVALID_CHAIR))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//桌子测试
WORD CTableFrame::TableHitTest(POINT MousePoint)
{
	int iTableWidth=m_TableResource.m_wWidth;
	int iTableHeight=m_TableResource.m_wHeight;
	if ((iTableWidth!=0)&&(iTableHeight!=0))
	{
		MousePoint.y+=m_iCurrentPos;
		int iTableRow=(MousePoint.x-m_iXExcursion)/iTableWidth;
		int iTableLine=MousePoint.y/iTableHeight;
		if ((iTableRow>=0)&&(iTableRow<m_iCountRow))
		{
			WORD wTableID=iTableLine*m_iCountRow+iTableRow;
			if (wTableID<m_wTableCount) 
			{
				if ((wTableID>0)&&(m_bHideInfo==true)) return INVALID_TABLE;
				return wTableID;
			}
		}
	}
	return INVALID_TABLE;
}

//椅子测试
WORD CTableFrame::ChairHitTest(POINT MousePoint)
{
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableResource.m_rcChair[i].PtInRect(MousePoint)) return i;
	}
	return INVALID_CHAIR;
}

//资源目录
void CTableFrame::GetGameResDirectory(const tagGameKind * pGameKind, TCHAR szResDirectory[], WORD wBufferCount)
{
	//变量定义
	WORD wStringIndex=0;

	//构造目录
	ZeroMemory(szResDirectory,wBufferCount*sizeof(TCHAR));
	lstrcpyn(szResDirectory,(LPCTSTR)pGameKind->szProcessName,wBufferCount);
	while ((szResDirectory[wStringIndex]!=0)&&(szResDirectory[wStringIndex]!=TEXT('.'))) wStringIndex++;

	//字符终止
	szResDirectory[wStringIndex]=0;

	return;
}

//////////////////////////////////////////////////////////////////////////
