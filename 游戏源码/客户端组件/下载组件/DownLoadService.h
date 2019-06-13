#ifndef DOWN_LOAD_SERVICE_HEAD_FILE
#define DOWN_LOAD_SERVICE_HEAD_FILE

#pragma once

#include "AfxInet.h"
#include "Resource.h"
#include "DownLoad.h"

//类说明
class CDownLoadMission;
class CDownLoadService;
typedef CArrayTemplate<CDownLoadMission *> CDownLoadMissionArray;

//////////////////////////////////////////////////////////////////////////
//枚举定义

//下载状态
enum enDownLoadStatus
{
	enDownLoadStatus_Unknow,			//未知状态
	enDownLoadStatus_Ready,				//准备状态
	enDownLoadStatus_DownLoadIng,		//下载状态
	enDownLoadStatus_Finish,			//完成状态
	enDownLoadStatus_Fails,				//下载失败
};

//错误枚举
enum enDownLoadResult
{
	enDownLoadResult_Noknow,			//没有错误
	enDownLoadResult_Exception,			//异常错误
	enDownLoadResult_CreateFileFails,	//创建失败
	enDownLoadResult_InternetReadError,	//读取错误
};

//下载状态
struct tagDownLoadStatus
{
	WORD								wProgress;						//下载进度
	TCHAR								szStatus[128];					//状态描述
	enDownLoadStatus					DownLoadStatus;					//下载状态
};

//////////////////////////////////////////////////////////////////////////

//下载线程
class CDownLoadThread : public CServiceThread
{
	//变量定义
protected:
	bool							m_bPreparative;						//初始标志
	tagDownLoadRequest				* m_pDownLoadRequest;				//下载请求

	//信息变量
protected:
	TCHAR							m_szTempFile[MAX_PATH];				//临时文件
	TCHAR							m_szLocalFile[MAX_PATH];			//下载文件

	//状态变量
protected:
	CCriticalSection				m_CriticalSection;					//线程锁定
	enDownLoadStatus				m_DownLoadStatus;					//下载状态
	enDownLoadResult				m_DownLoadResult;					//结果状态
	DWORD							m_dwDownLoadSize;					//下载大小
	DWORD							m_dwTotalFileSize;					//文件大小

	//线程变量
protected:
	CFile							m_LocalFile;						//本地文件
	CHttpFile						* m_pHttpFile;						//下载文件
	CHttpConnection					* m_pHttpConnection;				//网络连接
	CInternetSession				m_InternetSession;					//网络会话

	//函数定义
public:
	//构造函数
	CDownLoadThread();
	//析构函数
	virtual ~CDownLoadThread();

	//功能函数
public:
	//初始化线程
	bool InitThread(tagDownLoadRequest * pDownLoadRequest);
	//下载状态
	void GetDownLoadStatus(tagDownLoadStatus & DownLoadStatus);
	//目标文件
	LPCTSTR GetDownLoadFileName();

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
	//关闭事件
	virtual bool OnEventThreadConclude();

	//线程函数
private:
	//下载准备
	void DownLoadPreparative();
	//下载清理
	void DownLoadCleanUp();
};

//////////////////////////////////////////////////////////////////////////

//任务回调接口
interface IDownLoadMissionSink
{
	//下载通知
	virtual void OnMissionFinish(enDownLoadStatus DownLoadStatus, CDownLoadMission * pDownLoadMission)=NULL;
};

//下载任务
class CDownLoadMission : public CSkinDialogEx
{
	//变量定义
protected:
	DWORD								m_dwMissionID;					//任务标识
	DWORD								m_dwDownLoadType;				//下载类型
	tagDownLoadRequest					m_DownLoadRequest;				//下载请求
	IDownLoadMissionSink				* m_pIDownLoadMissionSink;		//回调接口

	//控件变量
protected:
	CSkinButton							m_btReTry;						//打开按钮
	CSkinButton							m_btCancel;						//打开按钮
	CProgressCtrl						m_ProgressCtrl;					//进度控件
	CDownLoadThread						m_DownLoadThread;				//下载线程

	//函数定义
public:
	//构造函数
	CDownLoadMission(IDownLoadMissionSink * pIDownLoadMissionSink);
	//析构函数
	virtual ~CDownLoadMission();

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定函数
	virtual void OnOK() { return; }
	//取消消息
	virtual void OnCancel();

	//功能函数
public:
	//任务标识
	DWORD GetMissionID() { return m_dwMissionID; }
	//下载类型
	DWORD GetDownLoadType() { return m_dwDownLoadType; }
	//开始下载
	bool StartDownLoad(DWORD dwMissionID, DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest);
	//停止下载
	bool StopDownLoad();
	//对比请求
	bool CompareRequest(tagDownLoadRequest * pDownLoadRequest);
	//显示界面
	bool DisplayDownLoadFace(bool bDisplay);

	//消息函数
protected:
	//重试按钮
	afx_msg void OnBnClickedReTry();
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//下载服务
class CDownLoadService : public IDownLoadService, public IDownLoadMissionSink
{
	//变量定义
protected:
	DWORD								m_dwMissionID;					//任务标识
	IDownLoadServiceSink				* m_pIDownLoadServiceSink;		//回调接口
	CDownLoadMissionArray				m_DownLoadMissionActive;		//下载任务
	CDownLoadMissionArray				m_DownLoadMissionRelease;		//下载任务

	//函数定义
public:
	//构造函数
	CDownLoadService();
	//析构函数
	virtual ~CDownLoadService();

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//配置接口
public:
	//获取数目
	virtual INT_PTR __cdecl GetDownLoadMissionCount();
	//设置接口
	virtual bool __cdecl SetDownLoadServiceSink(IUnknownEx * pIUnknownEx);
	//下载请求
	virtual DWORD __cdecl AddDownLoadRequest(DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest);

	//事件接口
public:
	//下载通知
	virtual void OnMissionFinish(enDownLoadStatus DownLoadStatus, CDownLoadMission * pDownLoadMission);

	//内部函数
private:
	//查找下载
	CDownLoadMission * SearchMission(tagDownLoadRequest * pDownLoadRequest);
};

//////////////////////////////////////////////////////////////////////////

#endif