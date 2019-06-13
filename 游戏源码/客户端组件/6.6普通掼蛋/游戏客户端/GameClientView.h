#pragma once

#include "Stdafx.h"
#include "CardControl.h"
#include "ScoreView.h"
//////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_START                         (WM_USER+100)                 //开始消息
#define IDM_OUT_CARD                      (WM_USER+101)                 //出牌消息
#define IDM_PASS_CARD                     (WM_USER+102)                 //放弃消息
#define IDM_PAY_TRIBUTE                   (WM_USER+103)                 //进贡消息
#define IDM_SORT_CARD					  (WM_USER+104)					//排序消息
#define IDM_AI							  (WM_USER+105)					//托管消息

//////////////////////////////////////////////////////////////////////////


//游戏视图
class CGameClientView : public CGameFrameView
{
	//友元定义
	friend class CGameClientDlg;
	//配置变量
protected:
    bool                            m_bDeasilOrder;                      //出牌顺序
	//界面变量
protected:
    
    //状态变量
protected:
	CPoint							m_PointUserPass[GAME_PLAYER];		//放弃位置
	CPoint							m_PointStick[GAME_PLAYER];			//抗贡位置
	CPoint                          m_PointGameResult[GAME_PLAYER];     //成绩位置
	bool                            m_bPass[GAME_PLAYER];               //放弃数祖
	BYTE                            m_bCardCount[GAME_PLAYER];          //扑克数目
	int                             m_bProgress;                        //进度位置
	int                             m_bGameResult[GAME_PLAYER];         //游戏成绩
	Series                          m_Series;                           //游戏级数
	BYTE							m_HongTao;							//红桃主牌
	BOOL							m_bGamePlaying;						//游戏状态
	WORD							m_wFirstID;							//开始用户
	BOOL							m_bFirstShow;						//是否显示
	BYTE                            m_bLiangCard;						//显示亮牌
	bool							m_bJinGong[GAME_PLAYER];			//显示回贡
	BOOL							m_bAI;								//是否托管
	bool							m_bKanGong[GAME_PLAYER];			//抗贡位置

    //历史积分
public:
	LONG                            m_lAllTurnScore[GAME_PLAYER];		//总局得分
	LONG                            m_lLastTurnScore[GAME_PLAYER];		//上局得分
	
	//控件变量
public:
	CSkinButton						m_btStart;							//开始按钮
	CSkinButton						m_btOutCard;						//出牌按钮
	CSkinButton						m_btPassCard;						//放弃按钮
    CSkinButton						m_btPayTribute;						//进贡按钮
	CSkinButton                     m_btChangeCard;						//交换按钮
	CSkinButton                     m_btResetCard;						//排序按钮
	CSkinButton						m_btColorSort;						//花色按钮
	CSkinButton						m_btBombSort;						//炸弹按钮
	CSkinButton						m_btAI;								//托管按钮
    
	//扑克控件
public:
	CCardControl                    m_HandCardControl;                  //手上扑克
	CCardControl                    m_UserCardControl[4];               //扑克视图
	CCardControl                    m_LeaveCardControl[3];              //剩余扑克

	//位图变量
protected:
	CSkinImage						m_ImageBack;						//背景资源
	CSkinImage						m_ImageUserPass;					//放弃标志
	CSkinImage						m_ImageCard;						//图片资源
	CSkinImage						m_ImageStick;						//抗贡图片
	CSkinImage						m_ImageViewFill;

protected:
	CScoreView						m_ScoreView;						//积分视图
protected:
	CToolTipCtrl					m_tip[7];							//按钮提示
#ifdef VIDEO_GAME
	CVideoServiceControl			m_DlgVideoService[4];				//视频窗口
#endif
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
#ifdef VIDEO_GAME
	void RectifyVideoView(int nWidth, int nHeight);
#endif
	//用户顺序
public:
	//设置顺序
	void SetUserOrder(bool bDeasilOrder);
	//获取顺序
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//功能函数
public:
	//设置数目
	void SetCardCount(WORD wChairID ,BYTE bCardCount);
	//设置放弃
	void SetPassFlag(WORD wChairID, bool bPass);
	//设置游戏状态
	void SetGamePlay(bool bPlaying);

	//消息映射
protected:
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //开始按钮
	afx_msg void OnStart();
	//出牌按钮
	afx_msg void OnOutCard();
	//放弃按钮
	afx_msg void OnPassCard();
	//进贡按钮
	afx_msg void OnPayTribute();
	//交换按钮
	afx_msg void OnChangeCard();
	//重新排序
	afx_msg void OnReset();
	//按花色排
	afx_msg void OnColorSort();
	//按炸弹排
	afx_msg	void OnBombSort();
	//鼠标右击
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//点击托管
	afx_msg void OnAI();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	DECLARE_MESSAGE_MAP()
	
	//功能函数	
public:
	//画开始用户
	void SetFirstUser(WORD wChairID,bool bshow,BYTE bCard=0);
	//提示进贡者
	void ShowJinGong(WORD wChairID,bool bShow);
	//画抗贡图
	void DrawStick(CDC * pDC, POINT x);

};

//////////////////////////////////////////////////////////////////////////
