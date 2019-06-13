#include "StdAfx.h"
#include "MessageProxy.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CMessageProxy::CMessageProxy(void)
{
	m_pSkinRichEdit=NULL;
	m_MessageOption.crName=RGB(0,0,200);
	m_MessageOption.crSystemHead=RGB(200,0,0);
	m_MessageOption.crSystemString=RGB(0,0,200);
	lstrcpyn(m_MessageOption.szDefaultFont,TEXT("宋体"),CountArray(m_MessageOption.szDefaultFont));
	
	//获取目录
	GetCurrentDirectory(sizeof(m_szPath),m_szPath);
}

//析构函数
CMessageProxy::~CMessageProxy(void)
{
}

//接口查询
void * __cdecl CMessageProxy::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMessageProxy,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMessageProxy,Guid,dwQueryVer);
	return NULL;
}

//设置句柄
bool __cdecl CMessageProxy::SetRichEditHwnd(CSkinRichEdit * pSkinRichEdit)
{
	//效验参数
	ASSERT(pSkinRichEdit!=NULL);
	if (pSkinRichEdit==NULL) return false;

	//设置变量
	m_pSkinRichEdit=pSkinRichEdit;

	return true;
}

//设置颜色
bool __cdecl CMessageProxy::SetMessageOption(tagMessageOption & MessageOption)
{
	//设置参数
	m_MessageOption.crName=MessageOption.crName;
	m_MessageOption.crSystemHead=MessageOption.crSystemHead;
	m_MessageOption.crSystemString=MessageOption.crSystemString;
	if (MessageOption.szDefaultFont[0]==0) lstrcpyn(m_MessageOption.szDefaultFont,TEXT("宋体"),CountArray(m_MessageOption.szDefaultFont));
	else lstrcpyn(m_MessageOption.szDefaultFont,MessageOption.szDefaultFont,CountArray(m_MessageOption.szDefaultFont));

	return true;
}

//清除屏幕
bool __cdecl CMessageProxy::CleanScreen()
{
	if (m_pSkinRichEdit==NULL) return false;
	m_pSkinRichEdit->CleanScreen();
	return true;
}

//设置滚动
bool __cdecl CMessageProxy::SetAutoScroll(bool bScroll)
{
	if (m_pSkinRichEdit==NULL) return false;
	//m_pSkinRichEdit->SetAutoScroll(bScroll);
	return true;
}

//加载消息
bool __cdecl CMessageProxy::LoadMessage(LPCTSTR pszFileName)
{
	if (m_pSkinRichEdit==NULL) return false;
	return m_pSkinRichEdit->LoadMessage(pszFileName);
}

//保存信息
bool __cdecl CMessageProxy::SaveMessage(LPCTSTR pszFileName)
{
	if (m_pSkinRichEdit==NULL) return false;
	return m_pSkinRichEdit->SaveMessage(pszFileName);
}

//插入图片
bool __cdecl CMessageProxy::InsertImage(CBitmap * pBitmap)
{
	if (m_pSkinRichEdit==NULL) return false;
	return m_pSkinRichEdit->InsertImage(pBitmap);
}

//插入时间
bool __cdecl CMessageProxy::InsertSystemTime(COLORREF crColor)
{
	//效验参数
	ASSERT(m_pSkinRichEdit!=NULL);
	if (m_pSkinRichEdit==NULL) return false;

	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	TCHAR szTimeBuffer[64]=TEXT("");
	_snprintf(szTimeBuffer,sizeof(szTimeBuffer),TEXT("［%02d:%02d:%02d］"),SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

	//插入消息
	m_pSkinRichEdit->InsertString(szTimeBuffer,crColor);

	return true;
}

//用户名字
bool __cdecl CMessageProxy::InsertUserName(LPCTSTR pszUserName)
{
	//效验参数
	ASSERT(pszUserName!=NULL);
	ASSERT(m_pSkinRichEdit!=NULL);
	if (pszUserName==NULL) return false;
	if (m_pSkinRichEdit==NULL) return false;

	POINT ptCusor;
	GetCursorPos(&ptCusor);

	//插入名字
	m_pSkinRichEdit->InsertString(TEXT("［"),m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(pszUserName,m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(TEXT("］"),m_MessageOption.crName);

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//系统消息
bool __cdecl CMessageProxy::InsertSystemString(LPCTSTR pszString, UINT uSize)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(m_pSkinRichEdit!=NULL);
	if (pszString==NULL) return false;
	if (m_pSkinRichEdit==NULL) return false;

	POINT ptCusor;
	GetCursorPos(&ptCusor);

	//插入消息
	LPCTSTR pszSystem=TEXT("【系统消息】");
	m_pSkinRichEdit->InsertString(pszSystem,m_MessageOption.crSystemHead);
	m_pSkinRichEdit->InsertString(pszString,m_MessageOption.crSystemString);
	m_pSkinRichEdit->InsertString(TEXT("\n"),m_MessageOption.crSystemString);

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//特色消息
bool __cdecl CMessageProxy::InsertShtickString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(m_pSkinRichEdit!=NULL);
	if (pszString==NULL) return false;
	if (m_pSkinRichEdit==NULL) return false;

	POINT ptCusor;
	GetCursorPos(&ptCusor);

	//插入消息
	TranslateInsert(pszString,crColor,uSize,m_MessageOption.szDefaultFont);
	if (bNewLine) m_pSkinRichEdit->InsertString(TEXT("\n"),crColor);

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//普通消息
bool __cdecl CMessageProxy::InsertGeneralString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(m_pSkinRichEdit!=NULL);
	if (pszString==NULL) return false;
	if (m_pSkinRichEdit==NULL) return false;

	POINT ptCusor;
	GetCursorPos(&ptCusor);

	//插入消息
	m_pSkinRichEdit->InsertString(pszString,crColor);
	if (bNewLine) m_pSkinRichEdit->InsertString(TEXT("\n"),crColor);

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//用户聊天
bool __cdecl CMessageProxy::InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszString, COLORREF crColor, UINT uSize)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(pszSendName!=NULL);
	ASSERT(m_pSkinRichEdit!=NULL);
	if (pszString==NULL) return false;
	if (pszSendName==NULL) return false;
	if (m_pSkinRichEdit==NULL) return false;

	POINT ptCusor;
	GetCursorPos(&ptCusor);

	//插入名字
	m_pSkinRichEdit->InsertString(TEXT("［"),m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(pszSendName,m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(TEXT("］说："),m_MessageOption.crName);

	//插入消息
	TranslateInsert(pszString,crColor,uSize,m_MessageOption.szDefaultFont);
	m_pSkinRichEdit->InsertString(TEXT("\n"),crColor);

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//用户聊天
bool __cdecl CMessageProxy::InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszRecvName, LPCTSTR pszString, COLORREF crColor, UINT uSize)
{
	//效验参数
	ASSERT(pszString!=NULL);
	ASSERT(pszSendName!=NULL);
	ASSERT(pszRecvName!=NULL);
	ASSERT(m_pSkinRichEdit!=NULL);
	if (pszString==NULL) return false;
	if (pszSendName==NULL) return false;
	if (pszRecvName==NULL) return false;
	if (m_pSkinRichEdit==NULL) return false;

	POINT ptCusor;
	GetCursorPos(&ptCusor);

	//插入名字
	m_pSkinRichEdit->InsertString(TEXT("［"),m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(pszSendName,m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(TEXT("］对［"),m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(pszRecvName,m_MessageOption.crName);
	m_pSkinRichEdit->InsertString(TEXT("］说："),m_MessageOption.crName);

	//插入消息
	TranslateInsert(pszString,crColor,uSize,m_MessageOption.szDefaultFont);
	m_pSkinRichEdit->InsertString(TEXT("\n"),crColor);

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//解释插入
bool CMessageProxy::TranslateInsert(LPCTSTR pszString, COLORREF crColor, UINT uSize, LPCTSTR pszFontName)
{
	POINT ptCusor;
	GetCursorPos(&ptCusor);

	int nFaceIndex = -1;
	CString strBuffer=pszString;
	LPTSTR pszStringTemp=strBuffer.GetBuffer();
	int iCharLength=lstrlen(pszStringTemp),iHandleLength=0;
	TCHAR szFileName[MAX_PATH]=TEXT("");
	do
	{
		int iPassCharCount=0;
		nFaceIndex = TranslateChar(pszStringTemp+iHandleLength,iCharLength-iHandleLength,iPassCharCount);
		if (nFaceIndex!=-1)
		{
			//显示文字
			pszStringTemp[iHandleLength+iPassCharCount]=0;
			m_pSkinRichEdit->InsertString(pszStringTemp+iHandleLength,crColor);
			iHandleLength+=iPassCharCount+CExpression::m_ExpreesionItem[nFaceIndex].m_wTranceLength;

			//装载图片			
			_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\Face\\%d.gif"),m_szPath, nFaceIndex);
			m_pSkinRichEdit->InsertImage(szFileName);
		}
		else 
		{
			m_pSkinRichEdit->InsertString(pszStringTemp+iHandleLength,crColor);
			break;
		}
	} while (nFaceIndex!=-1);
	strBuffer.ReleaseBuffer();

	SetCursorPos(ptCusor.x, ptCusor.y);

	return true;
}

//解释字符
int CMessageProxy::TranslateChar(LPCTSTR szCharString, int iCharLength, int & iPassCharCount)
{
	POINT ptCusor;
	GetCursorPos(&ptCusor);

	CExpressionItem * pItem=NULL;
	int iItemLength=0,iInputLenth=0;
	for (int i=0;i<iCharLength-2;i++)
	{
		if ((szCharString[i]==TEXT('/'))&&(szCharString[i+1]==TEXT(':')))
		{
			for (int j=MAX_EXPRESS_ITEM-1;0<=j;j--)
			{
				pItem=&CExpression::m_ExpreesionItem[j];
				iInputLenth=iCharLength-i;
				iItemLength=pItem->m_wTranceLength;
				if (iItemLength>iInputLenth) continue;
				for (int k=2;k<iItemLength;k++)
				{
					if ((szCharString[i+k]!=pItem->m_szTrancelate[k])&&(szCharString[i+k]!=(pItem->m_szTrancelate[k]+32))) break;
				}
				if (k==iItemLength) 
				{
					iPassCharCount=i;
					return j;
				}
			}
			i++;
		}
	}

	SetCursorPos(ptCusor.x, ptCusor.y);

	return -1;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * __cdecl CreateMessageProxy(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CMessageProxy * pMessageProxy=NULL;
	try
	{
		pMessageProxy=new CMessageProxy();
		if (pMessageProxy==NULL) throw TEXT("创建失败");
		void * pObject=pMessageProxy->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pMessageProxy);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
