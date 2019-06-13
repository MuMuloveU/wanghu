#include "Stdafx.h"
#include "Resource.h"
#include "ServerItemView.h"

//宏定义
#define WN_SET_LINE_COLOR				(TV_FIRST+40)		//设置颜色消息

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServerItemView, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, OnNMRdblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnTvnItemexpanded)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//静态变量
IServerListSink		* CListItem::m_pIServerListSink=NULL;		//回调接口	

//析构函数
CListItem::~CListItem()
{
	m_dwData=0;
	m_pParentItem=NULL;
}

//枚举子项
CListItem * CListItem::EnumChildItem(INT_PTR nIndex)
{
	if (nIndex>=m_ListItemArray.GetCount()) return NULL;
	return m_ListItemArray[nIndex];
}

//查找子项
CListType * CListItem::SearchTypeItem(WORD wTypeID)
{
	for (INT_PTR i=0;i<m_ListItemArray.GetCount();i++)
	{
		CListItem * pListItem=m_ListItemArray[i];
		if (pListItem->GetItemGenre()==ItemGenre_Type)
		{
			CListType * pListType=(CListType *)pListItem;
			tagGameType * pGameType=pListType->GetItemInfo();
			if (pGameType->wTypeID==wTypeID) return pListType;
		}
	}

	return NULL;
}

//查找子项
CListKind * CListItem::SearchKindItem(WORD wKindID)
{
	for (INT_PTR i=0;i<m_ListItemArray.GetCount();i++)
	{
		CListItem * pListItem=m_ListItemArray[i];
		if (pListItem->GetItemGenre()==ItemGenre_Kind)
		{
			CListKind * pListKind=(CListKind *)pListItem;
			const tagGameKind * pGameKind=pListKind->GetItemInfo();
			if (pGameKind->wKindID==wKindID) return pListKind;
		}
	}

	return NULL;
}

//查找子项
CListStation * CListItem::SearchStationItem(WORD wKindID, WORD wStationID, bool bDeepSearch)
{
	//本层搜索
	for (INT_PTR i=0;i<m_ListItemArray.GetCount();i++)
	{
		CListItem * pListItem=m_ListItemArray[i];
		if (pListItem->GetItemGenre()==ItemGenre_Station)
		{
			CListStation * pListStation=(CListStation *)pListItem;
			const tagGameStation * pGameStation=pListStation->GetItemInfo();
			if ((pGameStation->wStationID==wStationID)&&(pGameStation->wKindID==wKindID)) return pListStation;
		}
	}

	//深度搜索
	if (bDeepSearch)
	{
		for (INT_PTR i=0;i<m_ListItemArray.GetCount();i++)
		{
			CListItem * pItemBase=m_ListItemArray[i];
			CListStation * pListStation=pItemBase->SearchStationItem(wKindID,wStationID,bDeepSearch);
			if (pListStation!=NULL) return pListStation;
		}
	}

	return NULL;
}

//查找子项
CListServer * CListItem::SearchServerItem(WORD wKindID, WORD wServerID, bool bDeepSearch)
{
	//本层搜索
	for (INT_PTR i=0;i<m_ListItemArray.GetCount();i++)
	{
		CListItem * pListItem=m_ListItemArray[i];
		if (pListItem->GetItemGenre()==ItemGenre_Server)
		{
			CListServer * pListServer=(CListServer *)pListItem;
			const tagGameServer * pGameServer=pListServer->GetItemInfo();
			if ((pGameServer->wServerID==wServerID)&&(pGameServer->wKindID==wKindID)) return pListServer;
		}
	}

	//深度搜索
	if (bDeepSearch)
	{
		for (INT_PTR i=0;i<m_ListItemArray.GetCount();i++)
		{
			CListItem * pItemBase=m_ListItemArray[i];
			CListServer * pListServer=pItemBase->SearchServerItem(wKindID,wServerID,bDeepSearch);
			if (pListServer!=NULL) return pListServer;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerListManager::CServerListManager()
{
	m_pIServerListSink=NULL;
}

//析构函数
CServerListManager::~CServerListManager()
{
	INT_PTR nCount=m_PtrArrayType.GetCount();
	for (int i=0;i<nCount;i++) delete m_PtrArrayType[i];
	m_PtrArrayType.RemoveAll();

	nCount=m_PtrArrayKind.GetCount();
	for (i=0;i<nCount;i++) delete m_PtrArrayKind[i];
	m_PtrArrayKind.RemoveAll();

	nCount=m_PtrArrayStation.GetCount();
	for (i=0;i<nCount;i++) delete m_PtrArrayStation[i];
	m_PtrArrayStation.RemoveAll();

	nCount=m_PtrArrayServer.GetCount();
	for (i=0;i<nCount;i++) delete m_PtrArrayServer[i];
	m_PtrArrayServer.RemoveAll();

	return;
}

//初始化函数
bool CServerListManager::InitServerListManager(IServerListSink * pIServerListSink)
{
	//设置变量
	m_pIServerListSink=pIServerListSink;
	CListItem::m_pIServerListSink=pIServerListSink;

	//插入数据
	tagGameInside GameInside;
	memset(&GameInside,0,sizeof(GameInside));
	lstrcpyn(GameInside.szDisplayName,szProductName,sizeof(GameInside.szDisplayName));
	InsertInsideItem(&GameInside,1);

	return true;
}

//展开列表
bool CServerListManager::ExpandListItem(CListItem * pListItem)
{
	ASSERT(m_pIServerListSink!=NULL);
	return m_pIServerListSink->ExpandListItem(pListItem);
}

//枚举类型
CListType * CServerListManager::EnumTypeItem(INT_PTR nIndex)
{
	if (nIndex>=m_PtrArrayType.GetCount()) return NULL;
	return m_PtrArrayType[nIndex];
}

//枚举游戏
CListKind * CServerListManager::EnumKindItem(INT_PTR nIndex)
{
	if (nIndex>=m_PtrArrayKind.GetCount()) return NULL;
	return m_PtrArrayKind[nIndex];
}

//枚举站点
CListStation * CServerListManager::EnumStationItem(INT_PTR nIndex)
{
	if (nIndex>=m_PtrArrayStation.GetCount()) return NULL;
	return m_PtrArrayStation[nIndex];
}

//枚举房间
CListServer * CServerListManager::EnumServerItem(INT_PTR nIndex)
{
	if (nIndex>=m_PtrArrayServer.GetCount()) return NULL;
	return m_PtrArrayServer[nIndex];
}

//枚举内部项
CListInside * CServerListManager::EnumInsideItem(INT_PTR nIndex)
{
	if (nIndex>=m_PtrArrayInside.GetCount()) return NULL;
	return m_PtrArrayInside[nIndex];
}

//插入子项
bool CServerListManager::InsertTypeItem(tagGameType GameType[], WORD wCount)
{
	//效验参数
	ASSERT(m_PtrArrayInside.GetCount()>0);
	if (wCount==0) return false;

	//变量定义
	CListType * pListType=NULL;
	CListInside * pRootListInside=m_PtrArrayInside[0];

	try
	{
		//变量定义
		CListType * pListTypeTemp=NULL;

		for (WORD i=0;i<wCount;i++)
		{
			//查找存在
			pListTypeTemp=pRootListInside->SearchTypeItem(GameType[i].wTypeID);
			if (pListTypeTemp!=NULL) continue;

			//插入操作
			pListType=new CListType(pRootListInside);
			CopyMemory(pListType->GetItemInfo(),&GameType[i],sizeof(tagGameType));
			m_PtrArrayType.Add(pListType);
			m_pIServerListSink->OnListItemInserted(pListType);
		}
		return true;
	}
	catch (...) { SafeDelete(pListType); }

	return false;
}

//插入子项
bool CServerListManager::InsertKindItem(tagGameKind GameKind[], WORD wCount)
{
	CListKind * pListKind=NULL;
	try
	{
		//变量定义
		WORD wTypeID=0;
		CListType * pListType=NULL;
		CListKind * pListKindTemp=NULL;

		for (WORD i=0;i<wCount;i++)
		{
			//寻找父项
			if ((pListType==NULL)||(GameKind[i].wTypeID!=wTypeID))
			{
				wTypeID=GameKind[i].wTypeID;
				pListType=SearchTypeItem(wTypeID);
				ASSERT(pListType!=NULL);
				if (pListType==NULL) continue;
			}

			//查找存在
			pListKindTemp=pListType->SearchKindItem(GameKind[i].wKindID);
			if (pListKindTemp!=NULL) continue;

			//插入操作
			CWinFileInfo WinFileInfo;
			pListKind=new CListKind(pListType);
			CopyMemory(pListKind->GetItemInfo(),&GameKind[i],sizeof(tagGameKind));
			pListKind->m_bInstall=WinFileInfo.OpenWinFile(GameKind[i].szProcessName);
			m_PtrArrayKind.Add(pListKind);
			m_pIServerListSink->OnListItemInserted(pListKind);
		}
		return true;
	}
	catch (...) { SafeDelete(pListKind); }
	
	return false;
}

//插入子项
bool CServerListManager::InsertStationItem(tagGameStation GameStation[], WORD wCount)
{
	CListStation * pListStation=NULL;
	try
	{
		//变量定义
		WORD wKindID=0,wStationID=0;
		CListKind * pListKind=NULL;
		CListItem * pListParent=NULL;
		CListStation * pListStationJoin=NULL;
		CListStation * pListStationTemp=NULL;

		for (WORD i=0;i<wCount;i++)
		{
			//寻找种类
			if ((pListKind==NULL)||(GameStation[i].wKindID!=wKindID))
			{
				pListKind=SearchKindItem(GameStation[i].wKindID);
				ASSERT(pListKind!=NULL);
				if (pListKind==NULL) continue;
				wKindID=GameStation[i].wKindID;
				pListParent=pListKind;
			}

			//查找存在
			pListStationTemp=pListKind->SearchStationItem(GameStation[i].wKindID,GameStation[i].wStationID,true);
			if (pListStationTemp!=NULL) continue;

			//寻找站点
			if (GameStation[i].wJoinID!=0)
			{
				if ((pListStationJoin==NULL)||(GameStation[i].wJoinID!=wStationID))
				{
					pListStationJoin=pListKind->SearchStationItem(GameStation[i].wKindID,GameStation[i].wJoinID,true);
					ASSERT(pListStationJoin!=NULL);
					if (pListStationJoin==NULL) continue;
					wStationID=GameStation[i].wJoinID;
				}
				pListParent=pListStationJoin;
			}

			//插入操作
			pListStation=new CListStation(pListParent,pListKind);
			CopyMemory(pListStation->GetItemInfo(),&GameStation[i],sizeof(tagGameStation));
			m_PtrArrayStation.Add(pListStation);
			m_pIServerListSink->OnListItemInserted(pListStation);
		}
		return true;
	}
	catch (...) { SafeDelete(pListStation); }

	return false;
}

//插入子项
bool CServerListManager::InsertServerItem(tagGameServer GameServer[], WORD wCount)
{
	CListServer * pListServer=NULL;
	try
	{
		//变量定义
		WORD wKindID=0,wStationID=0;
		CListKind * pListKind=NULL;
		CListItem * pListParent=NULL;
		CListStation * pListStation=NULL;
		CListServer * pListServerTemp=NULL;

		for (WORD i=0;i<wCount;i++)
		{
			//寻找种类
			if ((pListKind==NULL)||(GameServer[i].wKindID!=wKindID))
			{
				pListKind=SearchKindItem(GameServer[i].wKindID);
				ASSERT(pListKind!=NULL);
				if (pListKind==NULL) continue;
				wKindID=GameServer[i].wKindID;
				pListParent=pListKind;
			}

			//查找存在
			pListServerTemp=pListKind->SearchServerItem(GameServer[i].wKindID,GameServer[i].wServerID,true);
			if (pListServerTemp!=NULL) continue;

			//寻找站点
			if (GameServer[i].wStationID!=0)
			{
				if ((pListStation==NULL)||(GameServer[i].wStationID!=wStationID))
				{
					pListStation=pListKind->SearchStationItem(GameServer[i].wKindID,GameServer[i].wStationID,true);
					if (pListStation==NULL) continue;
					wStationID=GameServer[i].wStationID;
				}
				pListParent=pListStation;
			}

			//插入操作
			pListServer=new CListServer(pListParent,pListKind);
			CopyMemory(pListServer->GetItemInfo(),&GameServer[i],sizeof(tagGameServer));
			m_PtrArrayServer.Add(pListServer);
			m_pIServerListSink->OnListItemInserted(pListServer);
		}
		return true;
	}
	catch (...) { SafeDelete(pListServer); }

	return false;
}

//插入子项
bool CServerListManager::InsertInsideItem(tagGameInside GameInside[], WORD wCount)
{
	CListInside * pListInside=NULL;
	try
	{
		for (WORD i=0;i<wCount;i++)
		{
			pListInside=new CListInside(NULL);
			CopyMemory(pListInside->GetItemInfo(),&GameInside[i],sizeof(tagGameInside));
			m_PtrArrayInside.Add(pListInside);
			m_pIServerListSink->OnListItemInserted(pListInside);
		}
		return true;
	}
	catch (...) { SafeDelete(pListInside); }

	return false;
}

//查找子项
CListType * CServerListManager::SearchTypeItem(WORD wTypeID)
{
	for (INT_PTR i=0;i<m_PtrArrayType.GetCount();i++)
	{
		CListType * pListType=m_PtrArrayType[i];
		tagGameType * pGameType=pListType->GetItemInfo();
		if (pGameType->wTypeID==wTypeID) return pListType;
	}
	return NULL;
}

//查找子项
CListKind * CServerListManager::SearchKindItem(WORD wKindID)
{
	for (INT_PTR i=0;i<m_PtrArrayKind.GetCount();i++)
	{
		CListKind * pListKind=m_PtrArrayKind[i];
		tagGameKind * pGameKind=pListKind->GetItemInfo();
		if (pGameKind->wKindID==wKindID) return pListKind;
	}
	return NULL;
}

//查找子项
CListStation * CServerListManager::SearchStationItem(WORD wKindID, WORD wStationID)
{
	for (INT_PTR i=0;i<m_PtrArrayStation.GetCount();i++)
	{
		CListStation * pListStation=m_PtrArrayStation[i];
		tagGameStation * pGameStation=pListStation->GetItemInfo();
		if ((pGameStation->wStationID==wStationID)&&(pGameStation->wKindID==wKindID)) return pListStation;
	}
	return NULL;
}

//查找子项
CListServer * CServerListManager::SearchServerItem(WORD wKindID, WORD wServerID)
{
	for (INT_PTR i=0;i<m_PtrArrayServer.GetCount();i++)
	{
		CListServer * pListServer=m_PtrArrayServer[i];
		tagGameServer * pGameServer=pListServer->GetItemInfo();
		if ((pGameServer->wServerID==wServerID)&&(pGameServer->wKindID==wKindID)) return pListServer;
	}
	return NULL;
}

//更新类型
bool CServerListManager::UpdateGameKind(WORD wKindID)
{
	CListKind * pListKind=SearchKindItem(wKindID);
	if (pListKind!=NULL)
	{
		CWinFileInfo WinFileInfo;
		tagGameKind * pGameKind=pListKind->GetItemInfo();
		pListKind->m_bInstall=WinFileInfo.OpenWinFile(pGameKind->szProcessName);
		m_pIServerListSink->OnListItemUpdate(pListKind);
		return true;
	}

	return false;
}

//更新总数
bool CServerListManager::UpdateGameOnLineCount(DWORD dwOnLineCount)
{
	if (m_PtrArrayInside.GetCount()>0)
	{
		CListInside * pListInside=m_PtrArrayInside[0];
		tagGameInside * pGameInside=pListInside->GetItemInfo();
		_snprintf(pGameInside->szDisplayName,sizeof(pGameInside->szDisplayName),TEXT("%s [ %ld 人]"),szProductName,dwOnLineCount);
		m_pIServerListSink->OnListItemUpdate(pListInside);
		return true;
	}

	return false;
}

//类型人数
bool CServerListManager::UpdateGameKindOnLine(WORD wKindID, DWORD dwOnLineCount)
{
	//寻找类型
	CListKind * pListKind=SearchKindItem(wKindID);
	if (pListKind!=NULL)
	{
		tagGameKind * pGameKind=pListKind->GetItemInfo();
		pGameKind->dwOnLineCount=dwOnLineCount;
		m_pIServerListSink->OnListItemUpdate(pListKind);
		return true;
	}

	return false;
}

//房间人数
bool CServerListManager::UpdateGameServerOnLine(CListServer * pListServer, DWORD dwOnLineCount)
{
	//效验参数
	ASSERT(pListServer!=NULL);
	if (pListServer==NULL) return false;

	//更新信息
	tagGameServer * pGameServer=pListServer->GetItemInfo();
	pGameServer->dwOnLineCount=dwOnLineCount;
	m_pIServerListSink->OnListItemUpdate(pListServer);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerItemView::CServerItemView()
{
	m_bShowOnLineCount=false;
}

//析构函数
CServerItemView::~CServerItemView()
{
}

//显示人数
void CServerItemView::ShowOnLineCount(bool bShowOnLineCount)
{
	bShowOnLineCount=true;
	if (m_bShowOnLineCount!=bShowOnLineCount)
	{
		m_bShowOnLineCount=bShowOnLineCount;
	}

	return;
}

//配置函数
bool CServerItemView::InitServerItemView(ITreeCtrlSink * pITreeCtrlSink)
{	
	//设置控件
	SetItemHeight(20);
	SetTextColor(RGB(0,0,0));
	SetBkColor(RGB(230,249,255));
	ModifyStyle(0,TVS_HASBUTTONS|TVS_HASLINES|TVS_SHOWSELALWAYS|TVS_TRACKSELECT);
	SendMessage(WN_SET_LINE_COLOR,0,(LPARAM)RGB(72,79,63));

	//设置变量
	ASSERT(pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink=pITreeCtrlSink;

	//加载图片
	if (m_ImageList.GetSafeHandle()==NULL)
	{
		CBitmap ServerImage;
		ServerImage.LoadBitmap(IDB_SERVER_LIST_IMAGE);
		m_ImageList.Create(18,18,ILC_COLOR16|ILC_MASK,0,0);
		m_ImageList.Add(&ServerImage,RGB(255,0,255));
		SetImageList(&m_ImageList,LVSIL_NORMAL);
	}

	return true;
}

//展开列表
bool __cdecl CServerItemView::ExpandListItem(CListItem * pListItem)
{
	//效验参数
	ASSERT(pListItem!=NULL);

	//展开列表
	HTREEITEM hTreeItem=(HTREEITEM)pListItem->GetItemData();
	if (hTreeItem!=NULL) 
	{
		Expand(hTreeItem,TVE_EXPAND);
		return true;
	}

	return false;
}

//更新通知
void __cdecl CServerItemView::OnListItemUpdate(CListItem * pListItem)
{
	//效验参数
	ASSERT(pListItem!=NULL);

	//更新显示
	switch (pListItem->GetItemGenre())
	{
	case ItemGenre_Kind:		//游戏类型
		{
			//变量定义
			CListKind * pListKind=(CListKind *)pListItem;

			//获取信息
			TCHAR szItemTitle[128]=TEXT("");
			DWORD dwImageIndex=GetGameImageIndex(pListKind);
			GetGameItemTitle(pListKind,szItemTitle,sizeof(szItemTitle));

			//更新树项
			HTREEITEM hTreeItem=(HTREEITEM)pListKind->GetItemData();
			SetItem(hTreeItem,TVIF_IMAGE|TVIF_TEXT|TVIF_SELECTEDIMAGE,szItemTitle,dwImageIndex,dwImageIndex,0,0,0);

			return;
		}
	case ItemGenre_Server:		//游戏房间
		{
			//变量定义
			CListServer * pListServer=(CListServer *)pListItem;
			tagGameServer * pGameServer=pListServer->GetItemInfo();

			//获取信息
			TCHAR szItemTitle[128]=TEXT("");
			GetGameItemTitle(pListServer,szItemTitle,sizeof(szItemTitle));

			//更新显示
			HTREEITEM hTreeItem=(HTREEITEM)pListServer->GetItemData();
			SetItemText(hTreeItem,szItemTitle);

			//更新类型
			INT_PTR nIndex=0;
			CListItem * pListItemTemp=NULL;
			CListKind * pListKind=pListServer->GetListKind();
			tagGameKind * pGameKind=pListKind->GetItemInfo();
			pGameKind->dwOnLineCount=0L;
			do
			{
				pListItemTemp=pListKind->EnumChildItem(nIndex++);
				if (pListItemTemp==NULL) break;
				if (pListItemTemp->GetItemGenre()==ItemGenre_Server)
				{
					pListServer=(CListServer *)pListItemTemp;
					pGameKind->dwOnLineCount+=pListServer->GetItemInfo()->dwOnLineCount;
				}
			} while (true);

			//更新显示
			GetGameItemTitle(pListKind,szItemTitle,sizeof(szItemTitle));
			hTreeItem=(HTREEITEM)pListKind->GetItemData();
			SetItemText(hTreeItem,szItemTitle);

			return;
		}
	case ItemGenre_Inside:	//内部类型
		{
			//变量定义
			CListInside * pListInside=(CListInside *)pListItem;
			tagGameInside * pGameInside=pListInside->GetItemInfo();

			//消息处理
			HTREEITEM hTreeItem=(HTREEITEM)pListInside->GetItemData();
			SetItemText(hTreeItem,pGameInside->szDisplayName);

			return;
		}
	}

	return;
}

//插入通知
void __cdecl CServerItemView::OnListItemInserted(CListItem * pListItem)
{
	//效验参数
	ASSERT(pListItem!=NULL);

	//变量定义
	TV_INSERTSTRUCT InsertInf;
	memset(&InsertInf,0,sizeof(InsertInf));

	//设置变量
	InsertInf.item.cchTextMax=128;
	InsertInf.hInsertAfter=TVI_LAST;
	InsertInf.item.lParam=(LPARAM)pListItem;
	InsertInf.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_PARAM;

	//寻找父项
	CListItem * pListParent=pListItem->GetParentItem();
	if (pListParent!=NULL) InsertInf.hParent=(HTREEITEM)pListParent->GetItemData();

	//构造数据
	switch (pListItem->GetItemGenre())
	{
	case ItemGenre_Type:		//游戏类型
		{
			//变量定义
			CListType * pListType=(CListType *)pListItem;
			tagGameType * pGameType=pListType->GetItemInfo();

			//构造数据
			InsertInf.item.iImage=IND_TYPE;
			InsertInf.item.iSelectedImage=IND_TYPE;
			InsertInf.item.pszText=pGameType->szTypeName;
			pListType->SetItemData((DWORD_PTR)InsertItem(&InsertInf));
			SetItemState((HTREEITEM)pListType->GetItemData(),TVIS_BOLD,TVIS_BOLD);

			break;
		}
	case ItemGenre_Kind:		//游戏种类
		{
			//变量定义
			CListKind * pListKind=(CListKind *)pListItem;
			tagGameKind * pGameKind=pListKind->GetItemInfo();

			//获取信息
			TCHAR szItemTitle[128]=TEXT("");
			DWORD dwImageIndex=GetGameImageIndex(pListKind);
			GetGameItemTitle(pListKind,szItemTitle,sizeof(szItemTitle));

			//设置数据
			InsertInf.item.pszText=szItemTitle;
			InsertInf.item.iImage=dwImageIndex;
			InsertInf.item.iSelectedImage=dwImageIndex;
			pListKind->SetItemData((DWORD_PTR)InsertItem(&InsertInf));

			break;
		}
	case ItemGenre_Station:		//游戏站点
		{
			//变量定义
			CListStation * pListStation=(CListStation *)pListItem;
			tagGameStation * pGameStation=pListStation->GetItemInfo();

			//设置数据
			InsertInf.item.iImage=IND_STATION;
			InsertInf.item.iSelectedImage=IND_STATION;
			InsertInf.item.pszText=pGameStation->szStationName;
			pListStation->SetItemData((DWORD_PTR)InsertItem(&InsertInf));

			break;
		}
	case ItemGenre_Server:		//游戏房间
		{
			//变量定义
			CListServer * pListServer=(CListServer *)pListItem;
			tagGameServer * pGameServer=pListServer->GetItemInfo();

			//设置位置
			InsertInf.hInsertAfter=TVI_FIRST;

			//寻找子项
			if (InsertInf.hParent!=NULL)
			{
				//变量定义
				WORD wSortID=pGameServer->wSortID;
				HTREEITEM hTreeItem=GetChildItem(InsertInf.hParent);

				//搜索位置
				if (hTreeItem!=NULL)
				{
					do
					{
						//获取数据
						CListServer * pItemTemp=(CListServer *)GetItemData(hTreeItem);
						if (pItemTemp!=NULL)
						{
							//获取位置
							WORD wItemSortID=200;
							if (pItemTemp->GetItemGenre()==ItemGenre_Server)
							{
								wItemSortID=((CListServer *)pItemTemp)->GetItemInfo()->wSortID;
							}

							//位置判断
							if (wSortID>wItemSortID) InsertInf.hInsertAfter=hTreeItem;
							else break;
						}
						else InsertInf.hInsertAfter=hTreeItem;

						//切换树项
						hTreeItem=GetNextItem(hTreeItem,TVGN_NEXT);

					} while (hTreeItem!=NULL);
				}
			}

			//构造标题
			TCHAR szItemTitle[128]=TEXT("");
			DWORD dwImageIndex=GetGameImageIndex(pListServer);
			GetGameItemTitle(pListServer,szItemTitle,sizeof(szItemTitle));

			//设置数据
			InsertInf.item.pszText=szItemTitle;
			InsertInf.item.iImage=dwImageIndex;
			InsertInf.item.iSelectedImage=dwImageIndex;
			pListServer->SetItemData((DWORD_PTR)InsertItem(&InsertInf));

			break;
		}
	case ItemGenre_Inside:		//内部数据
		{
			//插入分析
			CListInside * pListInside=(CListInside *)pListItem;
			tagGameInside * pGameInside=pListInside->GetItemInfo();

			//设置数据
			InsertInf.item.iImage=pGameInside->iImageIndex;
			InsertInf.item.iSelectedImage=pGameInside->iImageIndex;
			InsertInf.item.pszText=pGameInside->szDisplayName;
			pListInside->SetItemData((DWORD_PTR)InsertItem(&InsertInf));
			SetItemState((HTREEITEM)pListInside->GetItemData(),TVIS_BOLD,TVIS_BOLD);

			break;
		}
	}
	
	return;
}

//左击列表
void CServerItemView::OnNMClick(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	CListItem * pListItem=NULL;
	HTREEITEM hTreeItem=GetCurrentTreeItem();

	//展开列表
	if (hTreeItem!=NULL)
	{
		Select(hTreeItem,TVGN_CARET);
		pListItem=(CListItem *)GetItemData(hTreeItem);
	}

	//发送消息
	ASSERT(m_pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink->OnTreeLeftClick(pListItem,hTreeItem,this);

	return;
}

//右击列表
void CServerItemView::OnNMRclick(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	CListItem * pListItem=NULL;
	HTREEITEM hTreeItem=GetCurrentTreeItem();

	//选择列表
	if (hTreeItem!=NULL)
	{
		Select(hTreeItem,TVGN_CARET);
		pListItem=(CListItem *)GetItemData(hTreeItem);
	}

	//发送消息
	ASSERT(m_pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink->OnTreeRightClick(pListItem,hTreeItem,this);

	return;
}

//左键双击
void CServerItemView::OnNMDblclk(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	CListItem * pListItem=NULL;
	HTREEITEM hTreeItem=GetCurrentTreeItem();

	//展开列表
	if (hTreeItem!=NULL)
	{
		Select(hTreeItem,TVGN_CARET);
		pListItem=(CListItem *)GetItemData(hTreeItem);
	}

	//发送消息
	ASSERT(m_pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink->OnTreeLeftDBClick(pListItem,hTreeItem,this);

	return;
}

//右键双击
void CServerItemView::OnNMRdblclk(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	CListItem * pListItem=NULL;
	HTREEITEM hTreeItem=GetCurrentTreeItem();

	//展开列表
	if (hTreeItem!=NULL)
	{
		Select(hTreeItem,TVGN_CARET);
		pListItem=(CListItem *)GetItemData(hTreeItem);
	}

	//发送消息
	ASSERT(m_pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink->OnTreeRightDBClick(pListItem,hTreeItem,this);

	return;
}

//选择改变
void CServerItemView::OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	//变量定义
	CListItem * pListItem=NULL;
	HTREEITEM hTreeItem=pNMTreeView->itemNew.hItem;
	if (hTreeItem!=NULL) pListItem=(CListItem *)pNMTreeView->itemNew.lParam;

	//发送消息
	ASSERT(m_pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink->OnTreeSelectChanged(pListItem,hTreeItem,this);

	return;
}

//列表展开
void CServerItemView::OnTvnItemexpanded(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	//变量定义
	CListItem * pListItem=NULL;
	HTREEITEM hTreeItem=pNMTreeView->itemNew.hItem;
	if (hTreeItem!=NULL) pListItem=(CListItem *)pNMTreeView->itemNew.lParam;

	//发送消息
	ASSERT(m_pITreeCtrlSink!=NULL);
	m_pITreeCtrlSink->OnTreeItemexpanded(pListItem,hTreeItem,this);

	return;
}

//按钮测试
HTREEITEM CServerItemView::GetCurrentTreeItem()
{
	TVHITTESTINFO HitTestInfo;
	memset(&HitTestInfo,0,sizeof(HitTestInfo));
	HitTestInfo.flags=TVHT_ONITEM;
	GetCursorPos(&HitTestInfo.pt);
	ScreenToClient(&HitTestInfo.pt);
	return TreeView_HitTest(m_hWnd,&HitTestInfo);
}

//获取图标
DWORD CServerItemView::GetGameImageIndex(CListKind * pListKind)
{
	//效验参数
	ASSERT(pListKind!=NULL);
	
	//安装判断
	if (pListKind->m_GameKind.dwMaxVersion==0L) return IND_KIND_UNKNOW;
	if ((pListKind->m_GameKind.dwMaxVersion!=0L)&&(pListKind->m_bInstall==false)) return IND_KIND_NODOWN;

	//获取图标
	tagGameKind * pGameKind=pListKind->GetItemInfo();
	DWORD dwImageIndex=GetGameImageIndex(pGameKind->szProcessName,pGameKind->wKindID);

	return dwImageIndex;
}

//获取图标
DWORD CServerItemView::GetGameImageIndex(CListServer * pListServer)
{
	return IND_SERVER_NORNAL;
}

//获取图标
DWORD CServerItemView::GetGameImageIndex(LPCTSTR pszProcess, WORD wKindID)
{
	//寻找现存
	tagGameResourceInfo * pGameResourceInfo=NULL;
	for (INT_PTR i=0;i<m_GameResourceArray.GetCount();i++)
	{
		pGameResourceInfo=&m_GameResourceArray[i];
		if (pGameResourceInfo->wKindID==wKindID) return pGameResourceInfo->dwImageIndex;
	}

	//加载资源
	HINSTANCE hInstance=AfxLoadLibrary(pszProcess);
	if (hInstance==NULL) return IND_KIND_UNKNOW;
	
	//加载标志
	CBitmap GameLogo;
	DWORD dwImagePos=0L;
	AfxSetResourceHandle(hInstance);
	if (GameLogo.LoadBitmap(TEXT("GAME_LOGO"))) dwImagePos=m_ImageList.Add(&GameLogo,RGB(255,0,255));
	AfxSetResourceHandle(GetModuleHandle(NULL));
	AfxFreeLibrary(hInstance);

	//保存信息
	if (dwImagePos!=0L)
	{
		tagGameResourceInfo GameResourceInfo;
		memset(&GameResourceInfo,0,sizeof(GameResourceInfo));
		GameResourceInfo.wKindID=wKindID;
		GameResourceInfo.dwImageIndex=dwImagePos;
		m_GameResourceArray.Add(GameResourceInfo);
		return GameResourceInfo.dwImageIndex;
	}
	
	return IND_KIND_UNKNOW;
}

//获取标题
LPCTSTR CServerItemView::GetGameItemTitle(CListKind * pListKind, LPTSTR pszTitle, WORD wBufferSize)
{
	//效验参数
	ASSERT(pszTitle!=NULL);
	ASSERT(pListKind!=NULL);

	//生成标题
	tagGameKind * pGameKind=pListKind->GetItemInfo();

	//特殊标题
	if (pGameKind->dwMaxVersion==0L)
	{
		lstrcpyn(pszTitle,pGameKind->szKindName,wBufferSize);
		return pszTitle;
	}

	//构造标题
	if (m_bShowOnLineCount==true)
	{
		if (pListKind->m_bInstall==true)
		{
			_snprintf(pszTitle,wBufferSize,TEXT("%s [ %ld ]"),pGameKind->szKindName,pGameKind->dwOnLineCount);
		}
		else
		{
			_snprintf(pszTitle,wBufferSize,TEXT("%s [ %ld ] （双击下载）"),pGameKind->szKindName,pGameKind->dwOnLineCount);
		}
	}
	else
	{
		if (pListKind->m_bInstall==true)
		{
			_snprintf(pszTitle,wBufferSize,TEXT("%s"),pGameKind->szKindName);
		}
		else
		{
			_snprintf(pszTitle,wBufferSize,TEXT("%s （双击下载）"),pGameKind->szKindName);
		}
	}

	return pszTitle;
}

//获取标题
LPCTSTR CServerItemView::GetGameItemTitle(CListServer * pListServer, LPTSTR pszTitle, WORD wBufferSize)
{
	//效验参数
	ASSERT(pszTitle!=NULL);
	ASSERT(pListServer!=NULL);

	//生成标题
	tagGameServer * pGameServer=pListServer->GetItemInfo();
	if (m_bShowOnLineCount==true)
	{
		_snprintf(pszTitle,wBufferSize,TEXT("%s [ %ld ]"),pGameServer->szServerName,pGameServer->dwOnLineCount);
	}
	else 
	{
		_snprintf(pszTitle,wBufferSize,TEXT("%s"),pGameServer->szServerName);
	}

	return pszTitle;
}

//////////////////////////////////////////////////////////////////////////
