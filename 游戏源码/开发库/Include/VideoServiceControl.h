#ifndef VIDEO_SERVICE_CONTROL_HEAD_FILE
#define VIDEO_SERVICE_CONTROL_HEAD_FILE

#pragma once

#include "VideoSend.h"
#include "VideoRece.h"
#include "VideoServiceModule.h"

//////////////////////////////////////////////////////////////////////////

//视频窗口
class VIDEO_SERVICE_CLASS CVideoServiceControl : public CWnd
{
	//友元定义
	friend class CVideoServiceManager;

	//内核变量
private:
	bool							m_bInitVideo;						//配置标志

	//用户信息
private:
	DWORD							m_dwUserID;							//用户标识
	TCHAR							m_szAccounts[NAME_LEN];				//用户帐号

	//状态变量
private:
	float							m_fVolumePercent;					//语音比例
	CRect							m_rcPaintVolume;					//音波区域

	//菜单变量
private:
	tagUserData						*m_pUserData;						//玩家信息

	//送花变量
private:
	DWORD							m_dwTargerUserID;					//目标玩家

	//设备标志
private:
	bool							m_bVidicon;							//视频设备
	bool							m_bMicrophone;						//音频设备
	bool							m_bOccupy;							//占用设备

	//控制变量
private:
	bool							m_bSmallMode;						//小型模式
	bool							m_bLocalMode;						//本地标志
	bool							m_bEnableVideo;						//是否视频
	bool							m_bEnableAudio;						//是否音频

	//控钮控件
private:
	CSkinButton						m_btMore;							//更多按钮
	CSkinButton						m_btVideo;							//视频按钮
	CSkinButton						m_btAudio;							//音频按钮
	CSkinButton						m_btClone;							//克隆按钮
	CSkinButton						m_btFlower;							//送花按钮

	//控件变量
protected:
	CVideoSend						m_VideoSend;						//发送组件
	CVideoRece						m_VideoRece;						//接受组件
	CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	CFlowerEffectControl			m_FlowerEffectControl;				//鲜花控件

	//图片资源
private:
	CSkinImage						m_ImageSmallPhoto;					//缩小头像
	CSkinImage						m_ImageLargePhoto;					//放大头像
	CSkinImage						m_ImageSmallFrame;					//缩小框架
	CSkinImage						m_ImageLargeFrame;					//放大框架
	CSkinImage						m_ImageLargeVolume;					//放大框架
	CSkinImage						m_ImageLargeVolumeEx;				//放大框架
	CSkinImage						m_ImageSmallVolumeEx;				//放大框架
	CSkinImage						m_ImageSmallVolume;					//放大框架
	CSkinImage						m_ImageSmallDefFemale;				//默认头像
	CSkinImage						m_ImageSmallDefMale;				//默认头像
	CSkinImage						m_ImageLargeDefFemale;				//默认头像
	CSkinImage						m_ImageLargeDefMale;				//默认头像

	//函数定义
public:
	//构造函数
	CVideoServiceControl();
	//析构函数
	virtual ~CVideoServiceControl();

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//大小模式
public:
	//大小模式
	bool SetSmallMode(bool bSmallMode);
	//大小模式
	bool IsSamllMode() { return m_bSmallMode; }

	//远程用户
public:
	//远程用户
	DWORD GetVideoUserID() { return m_dwUserID; }
	//远程用户
	bool SetVideoUserInfo(DWORD dwUserID, LPCTSTR pszAccounts);

	//设备属性
public:
	//视频设备
	bool HaveVidicon() { return m_bVidicon; }
	//音频设备
	bool HaveMicrophone() { return m_bMicrophone; }

	//控制函数
public:
	//配置视频
	bool InitVideoService(bool bLocalMode, bool bSmallMode);

	//事件函数
public:
	//界面重置
	bool OnEventGameReset();
	//用户离开
	bool OnEventUserLeave(DWORD dwRemoteUserID);
	//用户进入
	bool OnEventUserEnter(DWORD dwRemoteUserID);
	//用户配置
	bool OnEventUserConfig(DWORD dwRemoteUserID, tagVideoNetwordInfo * pVideoNetwordInfo);
	//显示视频
	bool OnShowLocalVD(bool bShow);
	//关闭视频
	bool OnEventCloseVideo();
	//打开视频
	bool OnEventOpenVideo();
	//赠送鲜花
	bool OnEventFlower(UINT uFlowerEffectID);

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();

	//内部函数
protected:
	//调整窗口
	VOID RectifyControl();

	//消息映射
protected:
	//重画函数
	VOID OnPaint();
	//使能通知
	VOID OnEventVideoEnable();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//创建函数
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	//重载函数
protected:
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//按钮消息
protected:
	//更多按钮
	VOID OnBnClickedMore();
	//视频按钮
	VOID OnBnClickedVideo();
	//音频按钮
	VOID OnBnClickedAudio();
	//克隆按钮
	VOID OnBnClickedClone();
	//送花按钮
	VOID OnBnClickedFlower();
	//音频消息
	void VolumeOutAV(unsigned long *Volume) ;

	//消息宏
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP() 
};

//////////////////////////////////////////////////////////////////////////

//视频管理
class VIDEO_SERVICE_CLASS CVideoServiceManager
{
	//友元定义
	friend class CVideoServiceControl;

	//视频信息
protected:
	WORD							m_wVideoPort;						//视频端口
	DWORD							m_dwVideoServer;					//视频地址
	DWORD							m_dwLocalUserID;					//本地用户
	bool							m_bOffLine[MAX_CHAIR];				//玩家断线

	//组件变量
protected:
	IUserFaceRes *					m_pIUserFaceRes;					//头像接口
	IClientKernel *					m_pIClientKernel;					//内核接口
	IMessageProxy *					m_pIMessageProxy;					//消息代理
	CVideoServiceControl *			m_VideoServiceControl[MAX_CHAIR];	//视频数组

	//资源变量
protected:
	static HINSTANCE				m_hResInstance;						//资源句柄
	static tagVideoFrameImage		m_VideoFrameImage;					//视频资源
	static tagPropertyMenuImage		m_PropertyMenuImage;				//菜单资源

	//静态变量
protected:
	static CVideoServiceManager *	m_pVideoServiceManager;				//管理对象

	//函数定义
public:
	//构造函数
	CVideoServiceManager();
	//析构函数
	virtual ~CVideoServiceManager();

	//配置函数
public:
	//设置接口
	bool SetClientKernel(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetMessageProxy(IUnknownEx * pIUnknownEx);
	//设置接口
	bool SetUserFaceRes(IUnknownEx * pIUnknownEx);
	//服务信息
	bool SetVideoServiceInfo(DWORD dwLoaclUserID, DWORD dwVideoServer, WORD wVideoPort);
	//设置对象
	bool SetVideoServiceControl(WORD wVideoIndex, CVideoServiceControl * pVideoServiceControl);

	//功能函数
public:
	//设置资源
	static void SetSkinResource(HINSTANCE hResInstance,tagVideoFrameImage *pVideoFrameImage,tagPropertyMenuImage *pPropertyMenuImage);
	//获取控件
	CVideoServiceControl *GetVideoServiceControl(WORD wVideoIndex) {return m_VideoServiceControl[wVideoIndex];}

	//事件接口
public:
	//界面重置
	bool OnEventGameReset();
	//用户离开
	bool OnEventUserLeave(WORD wVideoIndex, DWORD dwRemoteUserID);
	//用户进入
	bool OnEventUserEnter(WORD wVideoIndex, DWORD dwRemoteUserID);
	//用户配置
	bool OnEventUserConfig(WORD wVideoIndex, DWORD dwRemoteUserID, tagVideoNetwordInfo * pVideoNetwordInfo);
	//显示视频
	bool OnShowLocalVD(bool bShow);
	//更新用户
	bool OnUpdateUserItem( WORD wVideoIndex, BYTE cbUserStatus, DWORD dwRemoteUserID );
	//关闭视频
	bool OnEventCloseVideo(WORD wVideoIndex);
	//打开视频
	bool OnEventOpenVideo(WORD wVideoIndex);
	//赠送鲜花
	bool OnEventFlower(WORD wVideoIndex, UINT uFlowerEffectID);

	//静态函数
public:
	//获取实例
	static CVideoServiceManager * GetInstance() { return m_pVideoServiceManager; }
};

//////////////////////////////////////////////////////////////////////////

#endif
