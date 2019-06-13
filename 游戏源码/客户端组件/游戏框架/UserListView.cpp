#include "Stdafx.h"
#include "Resource.h"
#include "UserListView.h"

//静态变量
CImageList		CUserListView::m_StatusImage;						//状态位图

//排序信息结构
struct tagListSortInfo
{
	int						nItemIndex;							//排序子项
	BYTE					cbAscendSort;						//升序标志
	WORD					cbFieldKind;						//字段类型
	CUserListView			* pRoomListCtrl;					//列表指针
};

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUserListView, CSkinListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserListView::CUserListView()
{
	m_wKindID=0;
	m_wGameGenre=0;
	m_wColumnCount=0;
	m_cbAscendSort=FALSE;
	memset(m_wDataDescribe,0,sizeof(m_wDataDescribe));
}

//析构函数
CUserListView::~CUserListView()
{
}

//初始化列表
bool CUserListView::InitUserListView()
{
	//加载背景图
	if (m_StatusImage.GetSafeHandle()==NULL)
	{
		CBitmap Image;
		BITMAP ImageInfo;
		AfxSetResourceHandle(GetModuleHandle(GAME_FRAME_DLL_NAME));
		Image.LoadBitmap(IDB_USER_STATUS);
		AfxSetResourceHandle(GetModuleHandle(NULL));
		Image.GetBitmap(&ImageInfo);
		m_StatusImage.Create(IMGAE_SPACE,ImageInfo.bmHeight,ILC_COLOR16|ILC_MASK,0,0);
		m_StatusImage.Add(&Image,RGB(255,0,255));
	}
	SetImageList(&m_StatusImage,LVSIL_SMALL);

	//设置颜色
	SetBkColor(RGB(10,130,180));
	SetTextColor(RGB(232,249,253));
	SetTextBkColor(RGB(10,130,180));

	//命令行处理
	WORD wGameType=GAME_GENRE_SCORE;
	LPCTSTR lpszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (lpszCmdLine[0]!=0)
	{
		//提出 TOKEN
		int nStringLength=0;
		CString strRoomToken;
		LPCTSTR pszRoomToken=TEXT("/ServerType:");
		LPCTSTR lpszBeginString=lpszCmdLine;
		while (true)
		{
			LPCTSTR lpszEndString=_tcschr(lpszBeginString,TEXT(' '));
			nStringLength=(lpszEndString==NULL)?lstrlen(lpszBeginString):(int)(lpszEndString-lpszBeginString);

			//判断标识
			const int nTokenLength=lstrlen(pszRoomToken);
			if ((nStringLength>=nTokenLength)&&(memcmp(lpszBeginString,pszRoomToken,nTokenLength*sizeof(TCHAR))==0))
			{
				CopyMemory(strRoomToken.GetBufferSetLength(nStringLength-nTokenLength),lpszBeginString+nTokenLength,
					(nStringLength-nTokenLength)*sizeof(TCHAR));
				strRoomToken.ReleaseBuffer();
				wGameType=(WORD)atol(strRoomToken);
				break;
			}

			//设置变量
			if (lpszEndString==NULL) break;
			lpszBeginString=(lpszEndString+1);
		}
	}

	//插入列表
	if (wGameType==GAME_GENRE_GOLD)
	{
		m_wColumnCount=15;
		InsertColumn(0,TEXT("用户名"),LVCFMT_LEFT,115);
		InsertColumn(1,TEXT("游戏 ID"),LVCFMT_LEFT,60);
		InsertColumn(2,TEXT("游戏币"),LVCFMT_LEFT,60);
		InsertColumn(3,TEXT("魅力"),LVCFMT_LEFT,60);
		InsertColumn(4,TEXT("用户等级"),LVCFMT_LEFT,85);
		InsertColumn(5,TEXT("经验值"),LVCFMT_LEFT,50);
		InsertColumn(6,TEXT("总局"),LVCFMT_LEFT,60);
		InsertColumn(7,TEXT("胜局"),LVCFMT_LEFT,60);
		InsertColumn(8,TEXT("输局"),LVCFMT_LEFT,60);
		InsertColumn(9,TEXT("和局"),LVCFMT_LEFT,60);
		InsertColumn(10,TEXT("逃局"),LVCFMT_LEFT,60);
		InsertColumn(11,TEXT("胜率"),LVCFMT_LEFT,60);
		InsertColumn(12,TEXT("逃率"),LVCFMT_LEFT,60);
		InsertColumn(13,TEXT("个性签名"),LVCFMT_LEFT,200);
		InsertColumn(14,TEXT("旁观用户"),LVCFMT_LEFT,90);

		m_wDataDescribe[0]=DTP_USER_ACCOUNTS;
		m_wDataDescribe[1]=DTP_GAME_ID;
		m_wDataDescribe[2]=DTP_USER_SCORE;
		m_wDataDescribe[3]=DTP_LOVE_LINESS;
		m_wDataDescribe[4]=DTP_GAME_LEVEL;
		m_wDataDescribe[5]=DTP_USER_EXPERIENCE;
		m_wDataDescribe[6]=DTP_USER_PLAY_COUNT;
		m_wDataDescribe[7]=DTP_USER_WIN;
		m_wDataDescribe[8]=DTP_USER_LOST;
		m_wDataDescribe[9]=DTP_USER_DRAW;
		m_wDataDescribe[10]=DTP_USER_FLEE;
		m_wDataDescribe[11]=DTP_WIN_RATE;
		m_wDataDescribe[12]=DTP_FLEE_RATE;
		m_wDataDescribe[13]=DTP_UNDER_WRITE;
		m_wDataDescribe[14]=DTP_LOOK_ON_USER;

	}
	else
	{
		m_wColumnCount=15;										//	m_wColumnCount=16;
		InsertColumn(0,TEXT("用户名"),LVCFMT_LEFT,115);			//	InsertColumn(0,TEXT("用户名"),LVCFMT_LEFT,115);	
		InsertColumn(1,TEXT("游戏 ID"),LVCFMT_LEFT,60);			//	InsertColumn(1,TEXT("游戏 ID"),LVCFMT_LEFT,60);
		InsertColumn(2,TEXT("积分"),LVCFMT_LEFT,60);			//	InsertColumn(2,TEXT("积分"),LVCFMT_LEFT,60);
		InsertColumn(3,TEXT("魅力"),LVCFMT_LEFT,60);			//	InsertColumn(3, TEXT("银行"),LVCFMT_LEFT,60);
		InsertColumn(4,TEXT("用户等级"),LVCFMT_LEFT,85);		//	InsertColumn(4, TEXT("魅力"),LVCFMT_LEFT,60);
		InsertColumn(5,TEXT("经验值"),LVCFMT_LEFT,50);			//	InsertColumn(5, TEXT("用户等级"),LVCFMT_LEFT,85);
		InsertColumn(6,TEXT("总局"),LVCFMT_LEFT,60);			//	InsertColumn(6, TEXT("经验值"),LVCFMT_LEFT,50);
		InsertColumn(7,TEXT("胜局"),LVCFMT_LEFT,60);			//	InsertColumn(7, TEXT("总局"),LVCFMT_LEFT,60);
		InsertColumn(8,TEXT("输局"),LVCFMT_LEFT,60);			//	InsertColumn(8, TEXT("胜局"),LVCFMT_LEFT,60);
		InsertColumn(9,TEXT("和局"),LVCFMT_LEFT,60);			//	InsertColumn(9, TEXT("输局"),LVCFMT_LEFT,60);
		InsertColumn(10,TEXT("逃局"),LVCFMT_LEFT,60);			//	InsertColumn(10,TEXT("和局"),LVCFMT_LEFT,60);
		InsertColumn(11,TEXT("胜率"),LVCFMT_LEFT,60);			//	InsertColumn(11,TEXT("逃局"),LVCFMT_LEFT,60);
		InsertColumn(12,TEXT("逃率"),LVCFMT_LEFT,60);			//	InsertColumn(12,TEXT("胜率"),LVCFMT_LEFT,60);
		InsertColumn(13,TEXT("个性签名"),LVCFMT_LEFT,200);		//	InsertColumn(13,TEXT("逃率"),LVCFMT_LEFT,60);
		InsertColumn(14,TEXT("旁观用户"),LVCFMT_LEFT,90);		//	InsertColumn(14,TEXT("个性签名"),LVCFMT_LEFT,200);
																//	InsertColumn(15,TEXT("旁观用户"),LVCFMT_LEFT,90);
		m_wDataDescribe[0]=DTP_USER_ACCOUNTS;					//	
		m_wDataDescribe[1]=DTP_GAME_ID;							//	m_wDataDescribe[0]=DTP_USER_ACCOUNTS;
		m_wDataDescribe[2]=DTP_USER_SCORE;						//	m_wDataDescribe[1]=DTP_GAME_ID;
		m_wDataDescribe[3]=DTP_LOVE_LINESS;						//	m_wDataDescribe[2]=DTP_USER_SCORE;
		m_wDataDescribe[4]=DTP_GAME_LEVEL;						//	m_wDataDescribe[3]=DTP_USER_GOLD;
		m_wDataDescribe[5]=DTP_USER_EXPERIENCE;					//	m_wDataDescribe[4]=DTP_LOVE_LINESS;
		m_wDataDescribe[6]=DTP_USER_PLAY_COUNT;					//	m_wDataDescribe[5]=DTP_GAME_LEVEL;
		m_wDataDescribe[7]=DTP_USER_WIN;						//	m_wDataDescribe[6]=DTP_USER_EXPERIENCE;
		m_wDataDescribe[8]=DTP_USER_LOST;						//	m_wDataDescribe[7]=DTP_USER_PLAY_COUNT;
		m_wDataDescribe[9]=DTP_USER_DRAW;						//	m_wDataDescribe[8]=DTP_USER_WIN;
		m_wDataDescribe[10]=DTP_USER_FLEE;						//	m_wDataDescribe[9]=DTP_USER_LOST;
		m_wDataDescribe[11]=DTP_WIN_RATE;						//	m_wDataDescribe[10]=DTP_USER_DRAW;
		m_wDataDescribe[12]=DTP_FLEE_RATE;						//	m_wDataDescribe[11]=DTP_USER_FLEE;
		m_wDataDescribe[13]=DTP_UNDER_WRITE;					//	m_wDataDescribe[12]=DTP_WIN_RATE;
		m_wDataDescribe[14]=DTP_LOOK_ON_USER;					//	m_wDataDescribe[13]=DTP_FLEE_RATE;
	}															//	m_wDataDescribe[14]=DTP_UNDER_WRITE;
																//	m_wDataDescribe[15]=DTP_LOOK_ON_USER;
	//加载等级
	m_GameRankManagerHelper.CreateInstance();
	if (m_GameRankManagerHelper.GetInterface()!=NULL)
	{
		m_GameRankManagerHelper->LoadGameLevel(AfxGetApp()->m_pszExeName);
	}

	return true;
}

//加入用户
bool CUserListView::InsertUserItem(tagUserData * pUserData)
{
	//效验状态
	ASSERT(pUserData!=NULL);
	if (GetSafeHwnd()==NULL) return false;

	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserData;
	int iItem=FindItem(&FindInfo);

	//用户名
	if (iItem==-1)
	{
		int nImageStation=GetImageStation(pUserData);
		int nInsertStation=GetInsertStation(pUserData);
		iItem=InsertItem(nInsertStation,pUserData->szName,nImageStation);
		SetItemData(iItem,(DWORD_PTR)pUserData);
	}

	//填写资料
	FillUserListItem(iItem,pUserData);

	return true;
}

//更新用户
bool CUserListView::UpdateUserItem(tagUserData * pUserData)
{
	//效验状态
	ASSERT(pUserData!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserData;
	int iItem=FindItem(&FindInfo);
	if (iItem!=-1)
	{
		FillUserListItem(iItem,pUserData);
		return true;
	}
	
	return false;
}

//删除用户
bool CUserListView::DeleteUserItem(tagUserData * pUserData)
{
	//效验状态
	ASSERT(pUserData!=NULL);
	if (GetSafeHwnd()==NULL) return false;
	
	//查找用户
	LVFINDINFO FindInfo;
	memset(&FindInfo,0,sizeof(FindInfo));
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pUserData;
	int iItem=FindItem(&FindInfo);
	if (iItem!=-1)
	{
		DeleteItem(iItem);
		return true;
	}

	return false;
}

//绘画函数
void CUserListView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	int iItem=lpDrawItemStruct->itemID;
	int iCount=GetHeaderCtrl()->GetItemCount();
	int iBeen=rcItem.left;
	TCHAR szBuffer[1024];
	tagUserData *pIUserItem = (tagUserData *)GetItemData(iItem);

	GetItemRect(iItem, rcItem, LVIR_LABEL);

	for (int i=0;i<iCount;i++)
	{
		CRect rcSubItem;
		int iWidth=GetColumnWidth(i);

		GetSubItemRect(iItem,i,LVIR_LABEL,rcSubItem);
		if ( i == 0 )
		{
			GetSubItemRect(iItem,i,LVIR_ICON,rcSubItem);

			if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->FillSolidRect(&rcSubItem,RGB(10,36,106)); 
			else pDC->FillSolidRect(&rcSubItem,GetBkColor());

			if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->SetTextColor(RGB(255,255,255));
			else if ( 0 < pIUserItem->cbMemberOrder && (DTP_USER_ACCOUNTS == m_wDataDescribe[i])) pDC->SetTextColor(RGB(255,0,0));
			else pDC->SetTextColor(RGB(0,0,0));

			//绘画标志
			UINT uImageIndex=GetImageStation(pIUserItem);
			m_StatusImage.Draw(pDC,uImageIndex,CPoint(rcSubItem.left,rcSubItem.top),ILD_TRANSPARENT);

			GetItemText(iItem,i,szBuffer,sizeof(szBuffer));

			GetSubItemRect(iItem,i,LVIR_LABEL,rcSubItem);

			if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->FillSolidRect(&rcSubItem,RGB(10,36,106)); 
			else pDC->FillSolidRect(&rcSubItem,GetBkColor());

			rcSubItem.top+=3;
			pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcSubItem,DT_LEFT|DT_END_ELLIPSIS);
		}
		else
		{		
			if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->FillSolidRect(&rcSubItem,RGB(10,36,106)); 
			else pDC->FillSolidRect(&rcSubItem,GetBkColor());

			if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->SetTextColor(RGB(255,255,255));
			else if ( 0 < pIUserItem->cbMemberOrder && DTP_GAME_ID == m_wDataDescribe[i]) pDC->SetTextColor(RGB(255,0,0));
			else pDC->SetTextColor(RGB(0,0,0));

			GetItemText(iItem,i,szBuffer,sizeof(szBuffer));

			CRect rcTempItem =rcSubItem;
			rcTempItem.left +=2; 
			rcTempItem.right +=2; 
			rcTempItem.top +=3; 
			pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcTempItem,DT_LEFT|DT_END_ELLIPSIS);
		}
	}

	//绘画选择
	if (lpDrawItemStruct->itemState&ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//获取位图
int CUserListView::GetImageStation(tagUserData * pUserData)
{
	//变量定义
	BYTE cbUserStatus=pUserData->cbUserStatus;

	//权限对比
	int nRightExcursion=0;
	if (CUserRight::IsMatchUser(pUserData->dwUserRight)==true) nRightExcursion=9;
	else if (pUserData->cbCompanion==enCompanion_Friend) nRightExcursion=1;
	else if (pUserData->cbCompanion==enCompanion_Detest) nRightExcursion=2;
	else if (pUserData->cbMasterOrder!=0) nRightExcursion=pUserData->cbMasterOrder+7;
	else if (pUserData->cbMemberOrder!=0) nRightExcursion=pUserData->cbMemberOrder+2;

	//计算偏移
	return nRightExcursion*6+(cbUserStatus>=US_FREE?(cbUserStatus-US_FREE):0);
}

//获取插入
int CUserListView::GetInsertStation(tagUserData * pUserData)
{
	return GetItemCount();
}

//填写资料
void CUserListView::FillUserListItem(int iItem, tagUserData * pUserData)
{
	//更新用户
	if (m_wColumnCount==0)return;

	//用户头像
	int nImageStation=GetImageStation(pUserData);
	SetItem(iItem,0,LVIF_IMAGE|LVIF_TEXT,GetDisplayText(pUserData,0),nImageStation,0,0,0);

	for (WORD i=1;i<m_wColumnCount-1;i++) 
	{
		SetItemText(iItem,i,GetDisplayText(pUserData,i));
	}

	//获取行数
	int nItemCount = GetItemCount();

	//旁观信息
	if(nItemCount>1)
	{
		for ( int j=0; j<nItemCount;j++)
		{
			//获取玩家
			tagUserData *pLookUserData = (tagUserData *)GetItemData(j);
			if(pLookUserData->cbUserStatus!=US_LOOKON)continue;

			for ( int i=0; i<nItemCount;i++)
			{
				if(i==j)continue;
				tagUserData *pTargetUserData = (tagUserData *)GetItemData(i);
				if ( pTargetUserData == NULL ) continue;
				if ( pTargetUserData->dwUserID == pLookUserData->dwUserID ) continue;
				if ( pTargetUserData->wChairID != pLookUserData->wChairID ) continue;
				SetItemText(j,m_wColumnCount-1,pTargetUserData->szName);
				break;
			}
			if(i==nItemCount)SetItemText(j,m_wColumnCount-1,TEXT("该用户已离开"));
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////

//点击列消息
void CUserListView::OnColumnclick(NMHDR * pNMHDR, LRESULT * pResult) 
{
	NM_LISTVIEW * pNMListView=(NM_LISTVIEW *)pNMHDR;

	//构造数据
	tagListSortInfo SortInfo;
	int iSubItem=pNMListView->iSubItem;
	SortInfo.cbAscendSort=m_cbAscendSort;
	SortInfo.pRoomListCtrl=this;
	SortInfo.nItemIndex=iSubItem;
	SortInfo.cbFieldKind=m_wDataDescribe[iSubItem];

	//排列列表
	SortItems(SortFun,(LPARAM)&SortInfo);
	m_cbAscendSort=!m_cbAscendSort;

	return;
}

//排列函数
int CALLBACK CUserListView::SortFun(LPARAM lParam1, LPARAM lParam2, LPARAM lParamList)
{
	//定义变量
	tagUserData * pUserData1=(tagUserData *)lParam1;
	tagUserData * pUserData2=(tagUserData *)lParam2;
    tagListSortInfo * pSortInfo=(tagListSortInfo *)lParamList;
	CUserListView * pListCtrl=pSortInfo->pRoomListCtrl;
	ASSERT(pUserData1!=NULL);
	ASSERT(pUserData2!=NULL);
	ASSERT(pSortInfo!=NULL);

	//对比数据
	switch (pSortInfo->cbFieldKind)
	{
	case DTP_USER_ID:
		{
			int iCompRes=pUserData1->dwUserID>pUserData2->dwUserID?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_GAME_ID:
		{
			int iCompRes=pUserData1->dwGameID>pUserData2->dwGameID?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_LOVE_LINESS:
		{
			int iCompRes=pUserData1->lLoveliness>pUserData2->lLoveliness?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_ACCOUNTS:
		{
			//好友放置顶
			BYTE cbConnection1=pUserData1->cbCompanion;
			BYTE cbConnection2=pUserData2->cbCompanion;
			if (((cbConnection1==enCompanion_Friend)&&(cbConnection2!=enCompanion_Friend))
				||((cbConnection1!=enCompanion_Friend)&&(cbConnection2==enCompanion_Friend)))
			{
				if (cbConnection1==enCompanion_Friend) return -1;
				else return 1;
			}
			
			//会员置顶
			if (pUserData1->cbMemberOrder!=pUserData2->cbMemberOrder)
			{
				if (pUserData1->cbMemberOrder>pUserData2->cbMemberOrder) return -1;
				else return 1;
			}

			int iCompRes=strcmp(pUserData1->szName,pUserData2->szName);
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_UNDER_WRITE:
		{
			int iCompRes=strcmp(pUserData1->szUnderWrite,pUserData2->szUnderWrite);
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_SCORE:
		{
			int iCompRes=pUserData1->lScore>pUserData2->lScore?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_INSURE_SCORE:
		{
			int iCompRes=pUserData1->lInsureScore>pUserData2->lInsureScore?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_GAME_LEVEL:
		{
			int iCompRes=pUserData1->lScore>pUserData2->lScore?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_TABLE:
		{
			int iCompRes=pUserData1->wTableID>pUserData2->wTableID?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_EXPERIENCE:
		{
			int iCompRes=pUserData1->lExperience>pUserData2->lExperience?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_GROUP_NAME:
		{
			int iCompRes=strcmp(pUserData1->szGroupName,pUserData2->szGroupName);
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_WIN:
		{
			int iCompRes=pUserData1->lWinCount>pUserData2->lWinCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_LOST:
		{
			int iCompRes=pUserData1->lLostCount>pUserData2->lLostCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_DRAW:
		{
			int iCompRes=pUserData1->lDrawCount>pUserData2->lDrawCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_FLEE:
		{
			int iCompRes=pUserData1->lFleeCount>pUserData2->lFleeCount?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_WIN_RATE:
		{
			long lAllCount1=pUserData1->lWinCount+pUserData1->lLostCount;
			long lAllCount2=pUserData2->lWinCount+pUserData2->lLostCount;
			double dWinRate1=0.0,dWinRate2=0.0;
			if (lAllCount1!=0) dWinRate1=(double)(pUserData1->lWinCount)/(double)(lAllCount1);
			if (lAllCount2!=0) dWinRate2=(double)(pUserData2->lWinCount)/(double)(lAllCount2);
			int iCompRes=dWinRate1>dWinRate2?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
			break;
		}
	case DTP_FLEE_RATE:
		{
			long lAllCount1=pUserData1->lWinCount+pUserData1->lLostCount+pUserData1->lDrawCount+pUserData1->lFleeCount;
			long lAllCount2=pUserData2->lWinCount+pUserData2->lLostCount+pUserData2->lDrawCount+pUserData2->lFleeCount;
			double dFleeRate1=0.0,dFleeRate2=0.0;
			if (lAllCount1!=0) dFleeRate1=(double)(pUserData1->lFleeCount)/(double)(lAllCount1);
			if (lAllCount2!=0) dFleeRate2=(double)(pUserData2->lFleeCount)/(double)(lAllCount2);
			int iCompRes=dFleeRate1>dFleeRate2?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	case DTP_USER_PLAY_COUNT:
		{
			long lAllCount1=pUserData1->lWinCount+pUserData1->lLostCount+pUserData1->lDrawCount+pUserData1->lFleeCount;
			long lAllCount2=pUserData2->lWinCount+pUserData2->lLostCount+pUserData2->lDrawCount+pUserData2->lFleeCount;
			int iCompRes=lAllCount1>lAllCount2?1:-1;
			return (pSortInfo->cbAscendSort)?iCompRes:-iCompRes;
		}
	}

	return 0;
}

//获取字符
LPCTSTR CUserListView::GetDisplayText(tagUserData * pUserData, WORD wColumnPos)
{
	//变量定义
	static TCHAR szBuffer[512];

	//生成显示
	switch (m_wDataDescribe[wColumnPos])
	{
	case DTP_USER_ID:			//用户 ID
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->dwUserID);
			return szBuffer;
		}
	case DTP_GAME_ID:			//游戏 ID
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->dwGameID);
			return szBuffer;
		}
	case DTP_INSURE_SCORE:		//存储金币
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lInsureScore);
			return szBuffer;
		}
	case DTP_GAME_GOLD:			//游戏金币
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lGameGold);
			return szBuffer;
		}
	case DTP_LOVE_LINESS:		//用户魅力
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lLoveliness);
			return szBuffer;
		}
	case DTP_USER_ACCOUNTS:		//用户名字
		{
			return pUserData->szName;
		}
	case DTP_UNDER_WRITE:		//个性签名
		{
			return pUserData->szUnderWrite;
		}
	case DTP_USER_TABLE:		//游戏桌号
		{
			WORD wTableID=pUserData->wTableID;
			if (wTableID!=INVALID_TABLE) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%3d"),wTableID+1);
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_USER_SCORE:		//用户积分
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lScore);
			return szBuffer;
		}
	case DTP_USER_EXPERIENCE:	//用户经验
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lExperience);
			return szBuffer;
		}
	case DTP_USER_WIN:			//胜局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lWinCount);
			return szBuffer;
		}
	case DTP_USER_LOST:			//输局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lLostCount);
			return szBuffer;
		}
	case DTP_USER_DRAW:			//和局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lDrawCount);
			return szBuffer;
		}
	case DTP_USER_FLEE:			//逃局盘数
		{
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),pUserData->lFleeCount);
			return szBuffer;
		}
	case DTP_USER_PLAY_COUNT:	//总局盘数
		{
			LONG lAllCount=pUserData->lWinCount+pUserData->lLostCount+pUserData->lDrawCount+pUserData->lFleeCount;
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),lAllCount);
			return szBuffer;
		}
	case DTP_WIN_RATE:			//用户胜率
		{
			if (pUserData->lWinCount>0)
			{
				LONG lAllCount=pUserData->lWinCount+pUserData->lLostCount+pUserData->lDrawCount+pUserData->lFleeCount;
				double dWinRate=(double)(pUserData->lWinCount*100)/(double)lAllCount;
				if (dWinRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dWinRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_LOST_RATE:			//用户输率
		{
			if (pUserData->lLostCount>0)
			{
				LONG lAllCount=pUserData->lWinCount+pUserData->lLostCount+pUserData->lDrawCount+pUserData->lFleeCount;
				double dLostRate=(double)(pUserData->lLostCount*100)/(double)lAllCount;
				if (dLostRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dLostRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_DRAW_RATE:			//用户和率
		{
			if (pUserData->lDrawCount>0)
			{
				LONG lAllCount=pUserData->lWinCount+pUserData->lLostCount+pUserData->lDrawCount+pUserData->lFleeCount;
				double dDrawRate=(double)(pUserData->lDrawCount*100)/(double)lAllCount;
				if (dDrawRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dDrawRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_FLEE_RATE:			//用户逃率
		{
			if (pUserData->lFleeCount>0)
			{
				LONG lAllCount=pUserData->lWinCount+pUserData->lLostCount+pUserData->lDrawCount+pUserData->lFleeCount;
				double dFleeRate=(double)(pUserData->lFleeCount*100)/(double)lAllCount;
				if (dFleeRate>=0.01) _snprintf(szBuffer,sizeof(szBuffer),TEXT("%5.2f%%"),dFleeRate);
				else szBuffer[0]=0;
			}
			else szBuffer[0]=0;
			return szBuffer;
		}
	case DTP_GAME_LEVEL:		//游戏等级
		{
			szBuffer[0]=0;
			if (m_GameRankManagerHelper.GetInterface()!=NULL)
			{
				LPCTSTR pszGameRank=m_GameRankManagerHelper->GetLevelDescribe(pUserData->lScore);
				lstrcpyn(szBuffer,pszGameRank,sizeof(szBuffer));
			}
			return szBuffer;
		}
	case DTP_USER_GROUP_NAME:	//社团名字
		{
			return pUserData->szGroupName;
		}
	case DTP_LOOK_ON_USER:		//旁观用户
		{
			//if ( pUserData->cbUserStatus == US_LOOKON )
			//{
			//	//获取行数
			//	int nItemCount = GetItemCount();

			//	//获取玩家
			//	for ( int nIndex = 0; nIndex < nItemCount; ++nIndex )
			//	{
			//		tagUserData *pTargetUserData = (tagUserData *)GetItemData(nIndex);
			//		if ( pTargetUserData == NULL ) continue;
			//		if ( pTargetUserData->dwUserID == pUserData->dwUserID ) continue;
			//		if ( pTargetUserData->wChairID == pUserData->wChairID ) return pTargetUserData->szName;
			//	}
			//}

			return NULL;
		}
	}

	//默认显示
	szBuffer[0]=0;

	return szBuffer;
}

//////////////////////////////////////////////////////////////////////////
