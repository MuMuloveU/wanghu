#pragma once

#include "Stdafx.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	
	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	BYTE							m_bCardCount[4];					//扑克数目
	BYTE                            m_bHandCardCount;                   //扑克数目
	BYTE                            m_bHandCardData[28];                //手上扑克
	BYTE                            m_bPayTribute;						//是否进贡
	bool                            m_bGameEnd;						    //游戏是否结束
    WORD							m_wToUser;							//进贡者
	bool							m_bAI;								//是否托管
	WORD                            m_wTime;							//时间
	bool                            m_bLookon;							//是否允许看牌

	//出牌变量
protected:
	short							m_bTurnCardType;				    //出牌类型
	BYTE                            m_bTurnCardCount;					//出牌数目
	BYTE                            m_bTurnCardData[27];                //出牌数据

	//配置变量
protected:
	bool							m_bDeasilOrder;						//出牌顺序
	DWORD							m_dwCardHSpace;						//扑克象素
	enRoomType                      m_RoomType;							//房间类型
	WORD							m_wServerType;						//服务器类型
	
	//函数定义
public:
	//构造函数
	CGameClientDlg();
	//析构函数
	virtual ~CGameClientDlg();

	//常规继承
private:
	//初始函数
	virtual bool InitGameFrame();
	//重置框架
	virtual void ResetGameFrame();
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

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//放弃出牌
	bool OnSubPassCard(const void * pBuffer, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//进贡扑克
	bool OnSubPayTribute(const void * pBuffer, WORD wDataSize);

    
	//辅助函数
protected:
	//出牌判断
	bool VerdictOutCard(void);
    //不出判断
    bool VerdictPassCard();
	// 是否可交换
	bool VerdictChangeCard(void);
	//自动出牌
	void AutoOutCard(void);

	//消息映射
protected:
	//退出
	afx_msg void OnClose();
	//开始消息
	LRESULT OnStart			(WPARAM wParam, LPARAM lParam);
	//出牌消息
	LRESULT OnOutCard		(WPARAM wParam, LPARAM lParam);
	//放弃出牌
	LRESULT OnPassCard		(WPARAM wParam, LPARAM lParam);
	//进贡消息
	LRESULT OnPayTribute	(WPARAM wParam, LPARAM lParam);
	//右键扑克
	LRESULT OnLeftHitCard	(WPARAM wParam, LPARAM lParam);
	//左键扑克
	LRESULT OnRightHitCard	(WPARAM wParam, LPARAM lParam);
	//排序消息
    LRESULT OnSortCard		(WPARAM wParam,	LPARAM lParam);
	//改变位置
	LRESULT OnChangePosition(WPARAM wParam,	LPARAM lParam);
	//托管按钮
	LRESULT OnAI			(WPARAM wParam,	LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
	
	//请求退出函数
protected:
	//请求退出
	bool OnRequest(const void * pBuffer);
	//恢复退出
	bool OnAnswer(const void * pBuffer);
	
	//显示得分
protected:
	void ShowScore(void);

};

//////////////////////////////////////////////////////////////////////////
