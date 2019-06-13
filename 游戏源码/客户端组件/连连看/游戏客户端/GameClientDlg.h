#pragma once

#include "Stdafx.h"
#include "GameClientView.h"
#include "midi.h"
#include "DirectSound.h"


//////////////////////////////////////////////////////////////////////////


//类说明
class CGameFrameControl;

//游戏对话框
class CGameClientDlg : public CSkinDialog,public IClientKernelSink
{
private:
	//友元定义
	friend class CGameFrameControl;

	//屏幕区域大小
	CRect m_FullScreenRect; 

	//
	
	//界面辅助
private:
	bool								m_bReset;						//是否重置
	bool								m_bRectify;						//调整标志
	bool								m_bInitDialog;					//初始标志

	//配置变量
private:
	bool								m_bAllowSound;					//允许声音


	//内核变量
private:
	IClientKernelSink					* m_pKernelSink;				//钩子接口
	CUserFaceResHelper					m_UserFaceResHelper;			//头像组件
	CClientKernelHelper					m_ClientKernelHelper;			//游戏逻辑

	//控件变量
protected:
	CGameClientView						m_GameClientView;				//游戏视图

	CMIDI								m_midi;							//背景音乐
	CDirectSound						m_Bomb;
	CDirectSound						m_Link;
	CDirectSound						m_FlyStar;
	CDirectSound						m_Start;
	CDirectSound						m_Prop;
	CDirectSound						m_Warn;
	CDirectSound						m_Select;
	CDirectSound						m_End;
	
	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

public:
	//是否有效
	virtual bool __cdecl IsValid();
	//释放对象
	virtual void __cdecl Release();
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//常规继承
	//初始函数
	virtual bool InitGameFrame();
	//游戏设置
	virtual void OnGameOptionSet();
	//时间消息
	virtual bool OnTimerMessage(WORD wChairID, UINT nElapse, UINT nTimerID);
	//旁观状态
	virtual void OnLookonChanged(bool bLookonUser, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool OnGameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//游戏场景
	virtual bool OnGameSceneMessage(BYTE cbGameStatus, bool bLookonOther, const void * pBuffer, WORD wDataSize);

	//驱动接口
	//获取窗口
	virtual HWND __cdecl GetFrameWnd();
	//重置状态
	virtual void __cdecl ResetGameFrame();
	//关闭窗口
	virtual void __cdecl CloseGameFrame(bool bForceClose);

	//内核事件
	//时间消息
	virtual bool __cdecl OnEventTimer(WORD wChairID, UINT nElapse, UINT nTimerID);
	//框架消息
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pBuffer, WORD wDataSize);
	//网络消息
	virtual bool __cdecl OnEventSocket(const CMD_Command & Command, const void * pBuffer, WORD wDataSize);
	//进程消息
	virtual bool __cdecl OnEventProcess(const IPC_Head & Head, const void * pBuffer, WORD wDataSize, HWND hWndSend);

	//游戏事件
	//游戏状态
	virtual bool __cdecl OnEventGameScene(BYTE cbGameStation, bool bLookonOther, void * pBuffer, WORD wDataSize);
	//聊天信息
	virtual bool __cdecl OnUserChatMessage(tagUserData * pSendUserData, tagUserData * pRecvUserData, LPCTSTR pszChatMessage, COLORREF crColor);
	//用户进入
	virtual void __cdecl OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户离开
	virtual void __cdecl OnEventUserLeave(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户积分
	virtual void __cdecl OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//用户状态
	virtual void __cdecl OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser);
	//时间更新
	virtual void __cdecl OnEventTimerKilled(WORD wChairID, UINT nTimerID);
	//旁观状态
	virtual void __cdecl OnEventLookonChanged(bool bLookonUser, void * pBuffer, WORD wDataSize);

	//按钮消息
	virtual bool __cdecl OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton);
	//拆分条消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos);
	
	
	//调整界面
	void RectifyControl(int nWidth, int nHeight);
	
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);

public:

	afx_msg void OnTimer(UINT nIDEvent);

DECLARE_MESSAGE_MAP()
virtual BOOL OnInitDialog();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
protected:
	virtual void OnCancel();

	//播放选择按钮声音
	LRESULT OnPlayMusic(WPARAM wParam, LPARAM lParam);


public:
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

	//信息函数
public:
	//旁观模式
	bool IsLookonMode();
	//允许旁观
	bool IsAllowLookon();
	//自己位置
	WORD GetMeChairID();
	//时间位置
	WORD GetTimeChairID();
	//获取用户
	const tagUserData * GetUserData(WORD wChairID);
	//内核接口
	void * GetClientKernel(const IID & Guid, DWORD dwQueryVer);

	//游戏状态
	BYTE GetGameStatus();
	//游戏状态
	void SetGameStatus(BYTE bGameStatus);

public:
	///////////////////////////////////////////////////////////////
	//消息处理
	//游戏开始
	bool OnSubStart(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubEnd(const void * pBuffer, WORD wDataSize);
	//游戏失败
	bool OnSubLost(const void * pBuffer, WORD wDataSize);
	//游戏组队
	bool OnSubGroup(const void * pBuffer, WORD wDataSize);
	//连接处理
	bool OnSubLink(const void * pBuffer, WORD wDataSize);
	//重列处理
	bool OnSubRealign(const void * pBuffer, WORD wDataSize);
	//禁手处理
//	bool OnSubStopIt(const void * pBuffer, WORD wDataSize);
	//障碍处理
//	bool OnSubClog(const void * pBuffer, WORD wDataSize);

	///////////////////////////////////////////////////////////////
	//功能函数
	// 特殊椅子位置转换
	int ChairSwitch(int chair);

protected:
	//发送开始消息
	LRESULT OnStart(WPARAM wParam, LPARAM lParam);
	//发送连接消息
	LRESULT OnLink(WPARAM wParam, LPARAM lParam);
	//发送指南针消息
	LRESULT OnGuide(WPARAM wParam, LPARAM lParam);
	//发送重列消息
	LRESULT OnRealign(WPARAM wParam, LPARAM lParam);
	//发送炸弹消息
	LRESULT OnBomb(WPARAM wParam, LPARAM lParam);
	//发送定时器消息
	LRESULT OnTimerStep(WPARAM wParam, LPARAM lParam);
	//发送禁手消息
	//LRESULT OnStopIt(WPARAM wParam, LPARAM lParam);
	//发送障碍消息
	//LRESULT OnClog(WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnOK();
	LRESULT	OnQuit(WPARAM wParam, LPARAM lParam);
public:
//	LRESULT OnMusicNotify(WPARAM wParam, LPARAM lParam);

	
	//礼物事件
public:
	//会员等级
	virtual void __cdecl OnEventUserMemberOrder(tagUserData * pUserData, WORD wChairID, bool bLookonUser){return ;}
	//初始道具
	virtual void __cdecl OnEventInitProperty(tagPropertyInfo *pPropertyInfo, int nInfoCount){return ;}
	//初始鲜花
	virtual void __cdecl OnEventInitFlower(tagFlowerInfo *pFlowerInfo, int nInfoCount){return ;}
	//鲜花消息
	virtual void __cdecl OnEventFlower(const tagUserData * pSendUserData, const tagUserData * pRecvUserData, UINT uFlowerID, UINT uFlowerEffectID){return ;}

};

//////////////////////////////////////////////////////////////////////////
