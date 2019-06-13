#ifndef GAME_FRAME_DLG_HEAD_FILE
#define GAME_FRAME_DLG_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "GameFrameView.h"

//类说明
class CGameFrameControl;
class CClientKernelSink;

//////////////////////////////////////////////////////////////////////////

//游戏主窗口
class GAME_FRAME_CLASS CGameFrameDlg : public CSkinDialog, public ISplitterSink
{
	//友元定义
	friend class CClientKernelSink;
	friend class CGameFrameControl;

	//界面状态
private:
	bool							m_bRectify;							//调整标志
	bool							m_bHideList;						//隐藏标志
	CRect							m_rcNormalSize;						//正常位置

	//界面辅助
private:
	bool							m_bReset;							//是否重置
	bool							m_bGameSceen;						//游戏场景
	bool							m_bInitDialog;						//初始标志

	//配置变量
private:
	bool							m_bAllowSound;						//允许声音

	//控件变量
protected:
	CSkinButton						m_btControl;						//控制按钮
	CSkinSplitter					m_VorSplitter;						//拆分控件
	CGameFrameView					* m_pGameFrameView;					//视图指针
	CGameFrameControl				* m_pGameFrameControl;				//控制视图

	//内核变量
private:
	CClientKernelSink				* m_pKernelSink;					//钩子接口
	CUserFaceResHelper				m_UserFaceResHelper;				//头像组件
	CClientKernelHelper				m_ClientKernelHelper;				//游戏逻辑
	CMessageProxyHelper				m_MessageProxyHelper;				//聊天代理

	//函数定义
public:
	//构造函数
	CGameFrameDlg(CGameFrameView * pGameFrameView);
	//析构函数
	virtual ~CGameFrameDlg();

	//拆分条接口
public:
	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame()=NULL;
	//重置框架
	virtual void ResetGameFrame()=NULL;
	//游戏设置
	virtual void OnGameOptionSet()=NULL;

	//系统事件
private:
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID)=NULL;
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize)=NULL;
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize)=NULL;
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize)=NULL;

	//特殊继承
private:
	//调整界面
	virtual void RectifyControl(int nWidth, int nHeight);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);

	//用户事件
private:
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户积分
	virtual void __cdecl OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户状态
	virtual void __cdecl OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser);

	//礼物事件
public:
	//初始道具
	virtual void __cdecl OnEventInitProperty(tagPropertyInfo *pPropertyInfo, int nInfoCount);
	//初始鲜花
	virtual void __cdecl OnEventInitFlower(tagFlowerInfo *pFlowerInfo, int nInfoCount);
	//鲜花消息
	virtual void __cdecl OnEventFlower(const tagUserData * pSendUserData, const tagUserData * pRecvUserData, UINT uFlowerID, UINT uFlowerEffectID);

	//窗口控制
public:
	//控制列表
	bool ShowViewControl(bool bShowControl);

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();
	//发送函数
	bool SendData(WORD wSubCmdID);
	//发送函数
	bool SendData(WORD wSubCmdID, void * pData, WORD wDataSize);
	//发送准备
	bool SendUserReady(void * pBuffer, WORD wDataSize);
	//设置定时器
	bool SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse);
	//删除定时器
	bool KillGameTimer(UINT nTimerID);
	//播放声音
	bool PlayGameSound(LPCTSTR pszSoundName);
	//播放声音
	bool PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName);

	//消息函数
public:
	//插入图片
	bool InsertImage(CBitmap * pBitmap);
	//插入名字
	bool InsertUserName(LPCTSTR pszUserName);
	//系统消息
	bool InsertSystemString(LPCTSTR pszString);
	//特色消息
	bool InsertShtickString(LPCTSTR pszString, COLORREF crColor, bool bNewLine);
	//普通消息
	bool InsertGeneralString(LPCTSTR pszString, COLORREF crColor, bool bNewLine);

	//辅助函数
public:
	//激活界面
	void ActiveGameFrame();
	//切换椅子
	WORD SwitchViewChairID(WORD wChairID);

	//声音控制
public:
	//是否启用
	bool IsEnableSound();
	//启用声音
	void EnableSound(bool bEnable);

	//旁观控制
public:
	//允许旁观
	bool IsAllowUserLookon();
	//旁观控制
	bool AllowUserLookon(DWORD dwUserID, bool bAllowLookon);

	//信息函数
public:
	//旁观模式
	bool IsLookonMode();
	//允许旁观
	bool IsAllowLookon();

	//信息函数
public:
	//自己位置
	WORD GetMeChairID();
	//时间位置
	WORD GetTimeChairID();
	//加入游戏
	VOID JoinInGameTable(WORD wChairID);
	//获取用户
	const tagUserData * GetUserData(WORD wChairID);
	//内核接口
	void * GetClientKernel(const IID & Guid, DWORD dwQueryVer);

	//游戏状态
public:
	//游戏状态
	BYTE GetGameStatus();
	//游戏状态
	void SetGameStatus(BYTE bGameStatus);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定消息
	virtual void OnOK();
	//取消消息
	virtual void OnCancel();

	//消息映射
private:
	//控制按钮
	VOID OnBnClickedControl();
	//位置改变
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//位置消息
	afx_msg void OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//位置改变
	afx_msg void OnWindowPosChanging(WINDOWPOS * lpwndpos);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif