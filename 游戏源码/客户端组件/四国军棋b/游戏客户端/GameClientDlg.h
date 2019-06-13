#pragma once

#include "Stdafx.h"
#include "historyrecord.h"
#include "GameLogic.h"
#include "GameClientView.h"


//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientDlg : public CGameFrameDlg
{
	//友元声明
	friend class CGameClientView ;

	//规则变量
protected:

	//配置变量
protected:

	//游戏变量
protected:
	BYTE							m_bChessColor;						//棋子颜色
	WORD							m_wCurrentUser;						//当前用户
	tagChessItem					m_Chess[17][17];					//棋子信息
	bool							m_bFinishPlacement;					//完成布局
	bool							m_bGameSart;						//游戏开始
	BYTE							m_bPlayerCount;						//游戏人数
	CHistoryRecord					m_HistoryRecord ;					//历史记录
	LONG							m_lCurrentStep ;					//目前步数
	bool							m_bDeasilOrder ;					//走棋顺序
	LONG							m_lCurrentStepCount;				//当前步数

	//辅助变量
protected:
	BYTE							m_bXSourcePos;						//选择信息
	BYTE							m_bYSourcePos;						//选择信息
	BYTE							m_bXTargetPos;						//选择信息
	BYTE							m_bYTargetPos;						//选择信息
	bool							m_bChooseChess;						//选择棋子

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	
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
	//完成布局
	bool OnSubFinishiPlacement(const void * pDataBuffer, WORD wDataSize) ;
	//初始布局
	bool OnSubPlacement(const void * pDataBuffer, WORD wDataSize) ;
	//移动棋子
	bool OnSubMoveChess(const void * pDataBuffer, WORD wDataSize) ;
	//玩家没棋
	bool OnSubLoseOne(const void * pDataBuffer, WORD wDataSize) ;
	//游戏结束
	bool OnSubGameEnd(const void * pDataBuffer, WORD wDataSize) ;
	//游戏棋谱
	bool OnSubChessManual(const void * pBuffer, WORD wDataSize) ;

	//辅助函数
protected:
	//初始棋子
	void InitCurrentUserChess() ;
	//坐标转换
	void ServerToClient(const BYTE bChessColor , const BYTE bServerX , const BYTE bServerY , BYTE &bClientX , BYTE &bClientY) ;
	//显示军棋
	void ShowJunQi(BYTE bChessColor) ;
	//保存棋局
	void ShowSave() ;
	//坐标转换
	void ClientToServer(const BYTE bChessColor , const BYTE bClientX , const BYTE bClientY , BYTE &bServerX , BYTE &bServerY) ;

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//布局按钮
	LRESULT OnHitPlacement(WPARAM wparam, LPARAM lparam);
	//点击棋盘
	LRESULT OnHitChessBoard(WPARAM wParam , LPARAM lParam) ;
	//改变鼠标
	LRESULT OnChangeCursor(WPARAM wParam , LPARAM lParam) ;
	//重新游戏
	LRESULT OnHitResetGame(WPARAM wParam , LPARAM lParam) ;	
	//保存布局
    LRESULT OnHitSavePlacement(WPARAM wParam , LPARAM lParam) ;	
	//装载布局	
	LRESULT OnHitLoadPlacement(WPARAM wParam , LPARAM lParam) ;	
	//载入棋局
	LRESULT OnHitLoadHistory(WPARAM wParam , LPARAM lParam) ;	
	//上一步棋			
	LRESULT OnHitPrevStep(WPARAM wParam , LPARAM lParam) ;	
	//下一步棋				
	LRESULT OnHitNextStep(WPARAM wParam , LPARAM lParam) ;	
	//旋转棋盘				
	LRESULT OnHitTurn(WPARAM wParam , LPARAM lParam) ;
	//玩家认输
	LRESULT OnHitLose(WPARAM wparam, LPARAM lparam);
	//放弃走棋
	LRESULT OnHitGiveUp(WPARAM wParam, LPARAM lParam);
	//请求和棋
	LRESULT OnHitPeace(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
