#include "StdAfx.h"
#include "ThreadDraw.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CThreadDraw::CThreadDraw(CWnd * pGameView) : m_pGameView(pGameView)
{
	m_uSpeed=0L;
	m_bExit=false;
	m_uTimeSpace=10;
	m_uDrawSpace=10;
	m_nClientWidth=0;
	m_nClientHeight=0;
	m_hDrawThreadHandle=NULL;
	m_hTimeThreadHandle=NULL;
}

//析构函数
CThreadDraw::~CThreadDraw()
{
	EndThread();
}

//设置范围
void CThreadDraw::SetClientRange(int nClientWidth, int nClientHeight) 
{ 
	m_nClientWidth=nClientWidth;
	m_nClientHeight=nClientHeight;
	return;
}

//初始化绘图数据
bool CThreadDraw::InitDraw(CDC * pDrawDC)
{
	//设置变量
	m_uSpeed=0L;
	m_uTimeSpace=10;
	m_uDrawSpace=10;

	return true;
}

//取消初始化数据
bool CThreadDraw::UnInitDraw()
{
	//设置变量
	m_uSpeed=0L;
	m_uTimeSpace=10L;
	m_uDrawSpace=10L;
	m_nClientWidth=0L;
	m_nClientHeight=0L;

	return true;
}

//停止线程
void CThreadDraw::SuspendDrawThread()
{
	if (m_hDrawThreadHandle!=NULL) ::SuspendThread(m_hDrawThreadHandle);
}
	
//启动线程
void CThreadDraw::ResumeDrawThread()
{
	if (m_hDrawThreadHandle!=NULL) ::ResumeThread(m_hDrawThreadHandle);
}

//开始线程
bool CThreadDraw::BeginThread()
{
	try
	{
		//启动线程
		m_bExit=false;
		UINT dwThreadID=0;
		m_hDrawThreadHandle=(HANDLE)_beginthreadex(NULL,0,ThreadDraw,this,CREATE_SUSPENDED,&dwThreadID);
		if (m_hDrawThreadHandle==NULL) throw TEXT("绘画线程启动错误");
		SetThreadPriority(m_hDrawThreadHandle,THREAD_PRIORITY_IDLE);
		m_hTimeThreadHandle=(HANDLE)_beginthreadex(NULL,0,ThreadTimer,this,CREATE_SUSPENDED,&dwThreadID);
		if (m_hTimeThreadHandle==NULL) throw TEXT("定时器线程启动错误");
		::ResumeThread(m_hDrawThreadHandle);
		::ResumeThread(m_hTimeThreadHandle);
	}
	catch (...)
	{
		EndThread();
		return false;
	}

	return true;
}

//结束线程
bool CThreadDraw::EndThread()
{
	//清理线程
	m_bExit=true;
	if (m_hDrawThreadHandle!=NULL)
	{
		if (WaitForSingleObject(m_hDrawThreadHandle,1000)==WAIT_TIMEOUT)
		{
			ASSERT(FALSE);
			TerminateThread(m_hDrawThreadHandle,1);
		}
		CloseHandle(m_hDrawThreadHandle);
		m_hDrawThreadHandle=NULL;
	}

	//清理线程
	if (m_hTimeThreadHandle!=NULL)
	{
		if (WaitForSingleObject(m_hTimeThreadHandle,1000)==WAIT_TIMEOUT) 
		{
			ASSERT(FALSE);
			TerminateThread(m_hTimeThreadHandle,1);
		}
		CloseHandle(m_hTimeThreadHandle);
		m_hTimeThreadHandle=NULL;
	}

	return true;
}

//画图线程
unsigned __stdcall CThreadDraw::ThreadDraw(LPVOID pThreadData)
{
	//初始化数据
	CDC	DCBuffer;
	CBitmap	BufferBmp;
	CThreadDraw * pThreadDraw=(CThreadDraw *)pThreadData;
	CDC * pDrawDC=pThreadDraw->m_pGameView->GetDC();

	try
	{
		//位置信息
		int nClientWidth=0;
		int nClientHeight=0;

		//建立双缓冲
		DCBuffer.CreateCompatibleDC(pDrawDC);
		DCBuffer.SetBkMode(TRANSPARENT);
		if (pThreadDraw->InitDraw(&DCBuffer)==false) throw TEXT("初始化错误");

		//帧数数据
		int uSpeed=0;
		DWORD dwBeginTime=GetTickCount();
		DWORD dwEndTime=dwBeginTime;

		//循环绘画
		while (true)
		{
			//效验位图大小
			if ((pThreadDraw->m_nClientWidth>nClientWidth)||(pThreadDraw->m_nClientHeight>nClientHeight))
			{
				nClientWidth=__max(nClientWidth,pThreadDraw->m_nClientWidth);
				nClientHeight=__max(nClientHeight,pThreadDraw->m_nClientHeight);
				BufferBmp.Detach();
				BufferBmp.CreateCompatibleBitmap(pDrawDC,nClientWidth,nClientHeight);
				DeleteObject(DCBuffer.SelectObject(&BufferBmp));
			}

			//绘画函数
			pThreadDraw->ThreadDrawView(&DCBuffer,nClientWidth,nClientHeight);
			pDrawDC->BitBlt(0,0,nClientWidth,nClientHeight,&DCBuffer,0,0,SRCCOPY);

			//计算帧数
			uSpeed++;
			dwEndTime=GetTickCount();
			if ((dwEndTime-dwBeginTime)>=1000L)
			{
				pThreadDraw->m_uSpeed=uSpeed;
				dwBeginTime=dwEndTime;
				uSpeed=0;
			}

			//检测停止
			if (pThreadDraw->m_bExit==true) break;
			Sleep(pThreadDraw->m_uDrawSpace);
		}
	}
	catch (...) {}

	//清理数据
	pThreadDraw->m_pGameView->ReleaseDC(pDrawDC);
	DCBuffer.DeleteDC();
	BufferBmp.DeleteObject();
	pThreadDraw->UnInitDraw();

	return 0;
}

//定时器线程
unsigned __stdcall CThreadDraw::ThreadTimer(LPVOID pThreadData)
{
	//定义变量
	CThreadDraw * pThreadDraw=(CThreadDraw *)pThreadData;
	CWnd * pWndHandle=pThreadDraw->m_pGameView->GetParent();

	//循环定时器
	try
	{
		while (true)
		{
			//发送消息
			Sleep(pThreadDraw->m_uTimeSpace);
			pWndHandle->PostMessage(WM_TIMER,THREAD_TIME_ID,0);

			//检测停止
			if (pThreadDraw->m_bExit==true) break;
		}
	}
	catch (...) {}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
