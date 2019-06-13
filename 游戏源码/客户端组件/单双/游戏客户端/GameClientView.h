#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ApplyUserList.h"


//////////////////////////////////////////////////////////////////////////

//筹码定义
#define JETTON_COUNT				7									//筹码数目
#define JETTON_RADII				68									//筹码半径

//消息定义
#define IDM_PLACE_JETTON			WM_USER+200							//加住信息
#define IDM_APPLY_BANKER			WM_USER+201							//申请信息
#define IDM_UPDATE_CONTROL			WM_USER+202							//更新控件

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

#define INDEX_DICEBOX_NULL			0									//没有操作
#define INDEX_OPEN_DICEBOX			1									//打开骰盒
#define INDEX_CLOSE_DICEBOX			2									//关闭骰盒
#define INDEX_SHAKE_DICEBOX			3									//摇摆骰盒

//历史记录
#define MAX_SCORE_HISTORY			256									//历史个数

//////////////////////////////////////////////////////////////////////////
//结构定义

//筹码信息
struct tagJettonInfo
{
	int								nXPos;								//筹码位置
	int								nYPos;								//筹码位置
	BYTE							cbJettonIndex;						//筹码索引
};

//筹码数组
typedef CArrayTemplate<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//操作结果
enum enOperateResult
{
	enOperateResult_NULL,
	enOperateResult_Win,
	enOperateResult_Lost
};
//记录信息
struct tagClientGameRecord
{
	enOperateResult					enOperateFlags;						//操作标识
	BYTE							cbDiceCount;						//色子点数
};

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameView
{
	//下注信息
protected:
	LONG							m_lMeMaxScore;						//最大下注
	LONG							m_lMeOddScore;						//买单总注
	LONG							m_lMeEvenScore;						//买双总注

	//全体下注
protected:
	LONG							m_lAllOddScore;						//买单总注
	LONG							m_lAllEvenScore;					//买双总注

	//位置信息
protected:
	int								m_nWinFlagsExcursionX;				//偏移位置
	int								m_nWinFlagsExcursionY;				//偏移位置
	int								m_nScoreHead;						//成绩位置

	//历史信息
protected:
	WORD							m_wDrawCount;						//游戏局数
	LONG							m_lMeResultCount;					//游戏成绩
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];//游戏记录
	int								m_nRecordFirst;						//开始记录
	int								m_nRecordLast;						//最后记录

	//状态变量
protected:
	WORD							m_wMeChairID;						//我的位置
	BYTE							m_cbWinnerSide;						//胜利玩家
	BYTE							m_cbWinnerFlash;					//胜利玩家
	LONG							m_lCurrentJetton;					//当前筹码
	BYTE							m_cbPreJettonArea;					//之前区域
	bool							m_bShowCloseDicebox;				//显示色盅

	//庄家信息
protected:
	bool							m_bShowChangeBanker;				//轮换庄家
	LONG							m_lAreaLimitScore;					//区域限制
	WORD							m_wCurrentBankerChairID;			//当前庄家
	BYTE							m_cbBankerTime;						//做庄次数
	LONG							m_lBankerScore;						//庄家成绩
	LONG							m_lBankerTreasure;					//庄家金币
	BYTE							m_cbTimeIndex;						//时间索引

	//当局成绩
public:
	LONG							m_lMeCurGameScore;					//我的成绩
	LONG							m_lMeCurGameReturnScore;			//我的成绩
	LONG							m_lBankerCurGameScore;				//返还积分
	bool							m_bShowGameResult;					//显示积分

	//数据变量
protected:
	CPoint							m_PointJetton[2];					//筹码位置
	CJettonInfoArray				m_JettonInfoArray[2];				//筹码数组
	BYTE							m_cbDice[2];						//色子数据

	//控件变量
public:
	CSkinButton						m_btJetton100;						//筹码按钮
	CSkinButton						m_btJetton1000;						//筹码按钮
	CSkinButton						m_btJetton10000;					//筹码按钮
	CSkinButton						m_btJetton100000;					//筹码按钮
	CSkinButton						m_btJetton500;						//筹码按钮
	CSkinButton						m_btJetton50000;					//筹码按钮
	CSkinButton						m_btJetton500000;					//筹码按钮	
	
	CSkinButton						m_btApplyBanker;					//申请庄家
	CSkinButton						m_btCancelBanker;					//取消庄家

	CSkinButton						m_btScoreMoveL;						//移动成绩
	CSkinButton						m_btScoreMoveR;						//移动成绩

	//控件变量
public:
	CApplyUser						m_ApplyUser;						//申请列表

	//界面变量
protected:
	CSkinImage						m_ImageViewFill;					//背景位图
	CSkinImage						m_ImageViewBack;					//背景位图
	CSkinImage						m_ImageWinFlags;					//标志位图
	CSkinImage						m_ImageJettonView;					//筹码视图
	CSkinImage						m_ImageScoreNumber;					//数字视图
	CSkinImage						m_ImageMeScoreNumber;				//数字视图
	CSkinImage						m_ImageTimeFlag;					//时间标识

	//边框资源
protected:
	CSkinImage						m_ImageFrameOdd;					//边框图片
	CSkinImage						m_ImageFrameEven;					//边框图片
	CSkinImage						m_ImageMeBanker;					//切换庄家
	CSkinImage						m_ImageChangeBanker;				//切换庄家
	CSkinImage						m_ImageNoBanker;					//切换庄家

	//结束资源
protected:
	CSkinImage						m_ImageGameEnd;						//成绩图片
	CSkinImage						m_ImageDicebox;						//色盅图片
	CSkinImage						m_ImageDice;						//色子图片
	CSkinImage						m_ImageCloseDicebox;				//色盅图片
	CSkinImage						m_ImageWinFlag;						//输赢标识
	CSkinImage						m_ImageEndSocre;					//数字图片

	//动画变量
protected:
	BYTE							m_cbOpenDiceboxIndex;				//打开索引
	BYTE							m_cbCloseDiceboxIndex;				//关闭索引
	BYTE							m_cbShakeDiceboxIndex;				//摇动索引

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual void ResetGameView();
	//调整控件
	virtual void RectifyGameView(int nWidth, int nHeight);
	//绘画界面
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	//功能函数
public:
	//设置信息
	void SetMeMaxScore(LONG lMeMaxScore);
	//最大下注
	void SetAreaLimitScore(LONG lAreaLimitScore);

	//设置信息
	void SetMeOddScore(LONG lMeOddScore);
	//设置信息
	void SetMeEvenScore(LONG lMeEvenScore);

	//设置筹码
	void SetCurrentJetton(LONG lCurrentJetton);
	//历史成绩
	void SetHistoryScore(WORD wDrawCount,LONG lMeResultCount);
	//历史记录
	void SetGameHistory(enOperateResult OperateResult, BYTE cbDiceCount);
	//轮换庄家
	void ShowChangeBanker( bool bChangeBanker );
	//庄家信息
	void SetBankerInfo( WORD wChairID, BYTE cbBankerTime, LONG lScore );
	//庄家金币
	void SetBankerTreasure(LONG lBankerTreasure);

	//成绩设置
	void SetGameScore(LONG lMeCurGameScore, LONG lMeCurGameReturnScore, LONG lBankerCurGameScore);
	//设置标识
	void SetShowGameFlag(bool bShowGameResult);
	//设置标识
	void ShowCloseDicebox(bool bShowCloseDicebox){ m_bShowCloseDicebox = bShowCloseDicebox; UpdateGameView(NULL);};
	//设置动画
	void SetCartoon(BYTE cbCartoonIdex, bool bStart);
	//设置色子
	void SetDice(BYTE cbDice[2]);

	//状态判断
	bool IsPlaceJettonStatus();
	//设置时间
	void SetTimeIndex(BYTE cbTimeIndex){m_cbTimeIndex = cbTimeIndex;UpdateGameView(NULL);}

	//计算限制
protected:
	//最大下注
	LONG GetMaxOddScore();
	//最大下注
	LONG GetMaxEvenScore();

	//界面函数
public:
	//清理筹码
	void CleanUserJetton();
	//设置筹码
	void PlaceUserJetton(BYTE cbViewIndex, LONG lScoreCount);
	//设置胜方
	void SetWinnerSide(BYTE cbWinnerSide);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawMeJettonNumber(CDC *pDC);

	//内联函数
public:
	//当前筹码
	inline LONG GetCurrentJetton() { return m_lCurrentJetton; }
	//我的位置
	inline void SetMeChairID(WORD wMeChairID) { m_wMeChairID=wMeChairID; }

	//内部函数
private:
	//获取区域
	BYTE GetJettonArea(CPoint MousePoint);
	//绘画数字
	void DrawNumberString(CDC * pDC, LONG lNumber, INT nXPos, INT nYPos, bool bMeScore = false);
	//绘画数字
	void DrawEndNumberString(CDC * pDC, LONG lNumber, INT nXPos, INT nYPos);
	//绘画标识
	void DrawWinFlags(CDC * pDC);
	//显示结果
	void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
	//透明绘画
	bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);
	//击中判断
	bool HitEvenAreaTest(CPoint &Point);
	//击中判断
	bool HitOddAreaTest(CPoint &Point);
	//绘画动画
	void DrawCartoon(CDC *pDC, int nWidth, int nHeight);

	//按钮消息
protected:
	//筹码按钮
	afx_msg void OnJettonButton500();
	//筹码按钮
	afx_msg void OnJettonButton50000();
	//筹码按钮
	afx_msg void OnJettonButton500000();
	//筹码按钮
	afx_msg void OnJettonButton100();
	//筹码按钮
	afx_msg void OnJettonButton1000();
	//筹码按钮
	afx_msg void OnJettonButton10000();
	//筹码按钮
	afx_msg void OnJettonButton100000();
	//上庄按钮
	afx_msg void OnApplyBanker();
	//下庄按钮
	afx_msg void OnCancelBanker();
	//移动按钮
	afx_msg void OnScoreMoveL();
	//移动按钮
	afx_msg void OnScoreMoveR();

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif