#include "StdAfx.h"
#include "Resource.h"
#include "CompanionManager.h"

//////////////////////////////////////////////////////////////////////////

//版本定义
#define VER_COMPANION_MAIN			1									//主版本
#define VER_COMPANION_SUB			0									//子版本

//文件头信息
struct tagCompanionFileHead
{
	WORD							wVersion;							//版本信息
	WORD							wItemSize;							//结构大小
	DWORD							dwFileSize;							//文件大小
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CCompanionManager::CCompanionManager(void)
{
	m_bNeedUpdate=false;
	m_DlgCompanionManager.SetCompanionManager(QUERY_ME_INTERFACE(IUnknownEx));
	return;
}

//析构函数
CCompanionManager::~CCompanionManager(void)
{
	//删除关系
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		SafeDelete(pCompanionItem);
	}
	for (INT_PTR i=0;i<m_CompanionItemRelease.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemRelease[i];
		SafeDelete(pCompanionItem);
	}
	m_CompanionItemActive.RemoveAll();
	m_CompanionItemRelease.RemoveAll();

	return;
}

//接口查询
void * __cdecl CCompanionManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ICompanionManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ICompanionManager,Guid,dwQueryVer);
	return NULL;
}

//加载信息
bool __cdecl CCompanionManager::LoadCompanion()
{
	//打开文件
	CFile CompanionFile;
	if (CompanionFile.Open(TEXT("Companion.DAT"),CFile::modeRead)==FALSE) return true;

	//判断大小
	ULONGLONG uFileSize=CompanionFile.GetLength();
	if (uFileSize<sizeof(tagCompanionFileHead)) return true;

	//读取文件头
	tagCompanionFileHead CompanionFileHead;
	memset(&CompanionFileHead,0,sizeof(CompanionFileHead));
	CompanionFile.Read(&CompanionFileHead,sizeof(CompanionFileHead));

	//效验文件头
	if (HIBYTE(CompanionFileHead.wVersion)!=VER_COMPANION_MAIN) return false;
	ASSERT(CompanionFileHead.dwFileSize==uFileSize);
	ASSERT(((CompanionFileHead.dwFileSize-sizeof(CompanionFileHead))%CompanionFileHead.wItemSize)==0);
	if (CompanionFileHead.dwFileSize!=uFileSize) return false;
	if (((CompanionFileHead.dwFileSize-sizeof(CompanionFileHead))%CompanionFileHead.wItemSize)!=0) return false;

	//删除关系
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		memset(pCompanionItem,0,sizeof(tagCompanionItem));
	}
	m_CompanionItemRelease.Append(m_CompanionItemActive);
	m_CompanionItemActive.RemoveAll();

	//读取信息
	WORD wItemSize=__min(sizeof(tagCompanionItem),CompanionFileHead.wItemSize);
	DWORD dwItemCount=(CompanionFileHead.dwFileSize-sizeof(CompanionFileHead))/CompanionFileHead.wItemSize;
	for (DWORD i=0;i<dwItemCount;i++)
	{
		//创建关系
		try { pCompanionItem=new tagCompanionItem; }
		catch (...) { return false; }
		if (pCompanionItem==NULL) return false;
		memset(pCompanionItem,0,sizeof(tagCompanionItem));

		//读取关系
		CompanionFile.Read(pCompanionItem,wItemSize);
		pCompanionItem->szNote[CountArray(pCompanionItem->szNote)-1]=0;
		pCompanionItem->szAccounts[CountArray(pCompanionItem->szAccounts)-1]=0;

		//插入关系
		if (SearchCompanionItem(pCompanionItem->dwUserID)!=NULL) m_CompanionItemRelease.Add(pCompanionItem);
		else m_CompanionItemActive.Add(pCompanionItem);
	}

	//设置变量
	m_bNeedUpdate=false;

	return true;
}

//保存关系
bool __cdecl CCompanionManager::SaveCompanion()
{
	//构造关系
	if (m_bNeedUpdate==true)
	{
		//打开文件
		CFile CompanionFile;
		if (CompanionFile.Open(TEXT("Companion.DAT"),CFile::modeCreate|CFile::modeWrite)==FALSE) return false;

		//构造文件头
		tagCompanionFileHead CompanionFileHead;
		CompanionFileHead.wItemSize=sizeof(tagCompanionItem);
		CompanionFileHead.wVersion=MAKEWORD(VER_COMPANION_SUB,VER_COMPANION_MAIN);
		CompanionFileHead.dwFileSize=(DWORD)sizeof(CompanionFileHead)+sizeof(tagCompanionItem)*GetCompanionItemCount();

		//写入文件
		tagCompanionItem * pCompanionItem=NULL;
		CompanionFile.Write(&CompanionFileHead,sizeof(CompanionFileHead));
		for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
		{
			pCompanionItem=m_CompanionItemActive[i];
			CompanionFile.Write(pCompanionItem,sizeof(tagCompanionItem));
		}

		//关闭文件
		CompanionFile.SetLength(CompanionFileHead.dwFileSize);
		CompanionFile.Flush();
		CompanionFile.Close();

		//设置变量
		m_bNeedUpdate=false;
	}

	return true;
}

//显示窗口
bool __cdecl CCompanionManager::ShowCompanionManager()
{
	if (m_DlgCompanionManager.m_hWnd==NULL)
	{
		AfxSetResourceHandle(GetModuleHandle(COMPANION_DLL_NAME));
		m_DlgCompanionManager.Create(IDD_COMPANION_MANAGER,AfxGetMainWnd());
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	m_DlgCompanionManager.ShowWindow(SW_SHOW);

	return true;
}

//加入关系
const tagCompanionItem * __cdecl CCompanionManager::InsertCompanionItem(tagCompanionItem & CompanionItem)
{
	//寻找关系
	bool bChangeCompanion=false;
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		ASSERT(pCompanionItem!=NULL);
		if (pCompanionItem->dwUserID==CompanionItem.dwUserID)
		{
			bChangeCompanion=(CompanionItem.Companion!=pCompanionItem->Companion);
			break;
		}
	}
	if (i==m_CompanionItemActive.GetCount()) pCompanionItem=NULL;

	//创建关系
	bool bInsert=false;
	if (pCompanionItem==NULL)
	{
		bInsert=true;
		INT_PTR nReleaseCount=m_CompanionItemRelease.GetCount();
		if (nReleaseCount>0)
		{
			pCompanionItem=m_CompanionItemRelease[nReleaseCount-1];
			m_CompanionItemRelease.RemoveAt(nReleaseCount-1);
		}
		else
		{
			try { pCompanionItem=new tagCompanionItem; }
			catch (...) { return NULL; }
		}
		m_CompanionItemActive.Add(pCompanionItem);
	}

	//写入信息
	m_bNeedUpdate=true;
	CompanionItem.szNote[CountArray(CompanionItem.szNote)-1]=0;
	CompanionItem.szAccounts[CountArray(CompanionItem.szAccounts)-1]=0;
	CopyMemory(pCompanionItem,&CompanionItem,sizeof(tagCompanionItem));

	//更新界面
	if (bInsert==true) m_DlgCompanionManager.InsertCompanionItem(pCompanionItem);
	else m_DlgCompanionManager.UpdateCompanionItem(pCompanionItem,bChangeCompanion);

	return pCompanionItem;
}

//更新关系
const tagCompanionItem * __cdecl CCompanionManager::UpdateCompanionItem(tagCompanionItem & CompanionItem)
{
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		ASSERT(pCompanionItem!=NULL);
		if (pCompanionItem->dwUserID==CompanionItem.dwUserID)
		{
			//保存数据
			bool bChangeCompanion=(pCompanionItem->Companion!=CompanionItem.Companion);

			//设置数据
			m_bNeedUpdate=true;
			CompanionItem.szNote[CountArray(CompanionItem.szNote)-1]=0;
			CompanionItem.szAccounts[CountArray(CompanionItem.szAccounts)-1]=0;
			CopyMemory(pCompanionItem,&CompanionItem,sizeof(tagCompanionItem));

			//更新界面
			m_DlgCompanionManager.UpdateCompanionItem(pCompanionItem,bChangeCompanion);

			return pCompanionItem;
		}
	}
	return NULL;
}

//更新关系
const tagCompanionItem * __cdecl CCompanionManager::UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszNote)
{
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		ASSERT(pCompanionItem!=NULL);
		if (pCompanionItem->dwUserID==dwUserID)
		{
			//设置数据
			m_bNeedUpdate=true;
			lstrcpyn(pCompanionItem->szNote,pszNote,CountArray(pCompanionItem->szNote));

			//更新界面
			m_DlgCompanionManager.UpdateCompanionItem(pCompanionItem,false);

			return pCompanionItem;
		}
	}
	return NULL;
}

//更新关系
const tagCompanionItem * __cdecl CCompanionManager::UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszAccounts, WORD wFaceID)
{
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		ASSERT(pCompanionItem!=NULL);
		if (pCompanionItem->dwUserID==dwUserID)
		{
			//设置数据
			m_bNeedUpdate=true;
			pCompanionItem->wFaceID=wFaceID;
			lstrcpyn(pCompanionItem->szAccounts,pszAccounts,CountArray(pCompanionItem->szAccounts));

			//更新界面
			m_DlgCompanionManager.UpdateCompanionItem(pCompanionItem,false);

			return pCompanionItem;
		}
	}
	return NULL;
}

//删除关系
bool __cdecl CCompanionManager::DeleteCompanionItem(DWORD dwUserID)
{
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		ASSERT(pCompanionItem!=NULL);
		if (pCompanionItem->dwUserID==dwUserID)
		{
			//更新界面
			m_DlgCompanionManager.DeleteCompanionItem(pCompanionItem);

			//设置数据
			m_bNeedUpdate=true;
			m_CompanionItemActive.RemoveAt(i);
			m_CompanionItemRelease.Add(pCompanionItem);
			memset(pCompanionItem,0,sizeof(tagCompanionItem));

			return true;
		}
	}
	return false;
}

//枚举关系
const tagCompanionItem * __cdecl CCompanionManager::EmunCompanionItem(WORD wIndex)
{
	if (wIndex>=m_CompanionItemActive.GetCount()) return NULL;
	return m_CompanionItemActive[wIndex];
}

//寻找关系
const tagCompanionItem * __cdecl CCompanionManager::SearchCompanionItem(DWORD dwUserID)
{
	tagCompanionItem * pCompanionItem=NULL;
	for (INT_PTR i=0;i<m_CompanionItemActive.GetCount();i++)
	{
		pCompanionItem=m_CompanionItemActive[i];
		ASSERT(pCompanionItem!=NULL);
		if (pCompanionItem->dwUserID==dwUserID) return pCompanionItem;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateCompanionManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CCompanionManager * pCompanionManager=NULL;
	try
	{
		pCompanionManager=new CCompanionManager();
		if (pCompanionManager==NULL) throw TEXT("创建失败");
		void * pObject=pCompanionManager->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pCompanionManager);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
