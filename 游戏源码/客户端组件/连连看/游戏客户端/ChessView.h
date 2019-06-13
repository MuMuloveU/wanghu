#ifndef CHESSVIEW_HEAD_FILE
#define CHESSVIEW_HEAD_FILE
#pragma once

//自定义消息
#define					    ID_PROCESS                          1111            //处理按键消息
#define						ID_GETNEXTBALLCOLOR					2222			//获取下一轮小球的消息
#define						ID_SUCCESS							3333			//成功消去
#define						ID_PSUCCESS							3334			//练习是给View发成功消息
#define						ID_FIRSTBALL						4444			//开球
#define						ID_TAKE								5555			//发球

#define						ID_GAMEOVER							6666			//游戏结束

#define     ERROR_POINT						100

// CChessView

class CChessView : public CWnd
{
public:
	CChessView();
	virtual ~CChessView();

public:
	CGameLogic      								m_Logic;								//游戏逻辑类
	bool											m_ExerciseMode;							//是否练习模式
	DBall											m_Chess[82];							//游戏棋盘，第0元素没有使用
	int												m_Result[82];							//存储结果信息
	int												m_tResult[82];
	BYTE											m_SuccessArray[9];						//存储成功消去时的位置
	DBall											m_DeadBall[9];							//将要消去的小球
	BYTE											m_StartPos;								//查询时开始位置
	BYTE											m_EndPos;								//查询是结束位置

	BYTE											m_firstball[5];						    //开球
	DBall											m_firstballcolor[5];					//开球的颜色

	BYTE											m_nextball[3];							//下一轮的三个球;
	DBall											m_nextballcolor[3];						//下一轮的三个球颜色;

	BYTE											m_tnextball[3];
	DBall											m_tnextballcolor[3];

	DBall											m_StartColor;							//开始位置颜色
	DBall											m_EndColor;								//结束位置颜色
	DBall											m_ResultColor;							//

	bool											m_first;								//第一次发球
    
	BYTE											m_result;								//查找时的每一个结果

	int												m_score;								//分数	
	TCHAR											m_scorestring[40];

	bool											m_Bobm;									//是否释放炸弹

	bool			m_BakeHit;								//上次是否消去
	int				m_BakeHitCount;							//连击次数

	bool			m_isLose;								//是否失败

	BYTE			m_GroupID;								//组队号

	TCHAR			m_class[40];
	TCHAR			m_per[40];
	int				m_Iper;
	
	void SuccessProcess(BYTE succ_array[], BYTE point);
	void TakeTheService();
	void DisplayTakeTheService(BYTE nball[3],DBall nballcolor[3]);
protected:	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void InitChessView(void);
	bool IsMove(void);
	// 释放炸弹
	void BobmFunc(BYTE point,BYTE bombarray[]);
	// 移动和发球
	void MoveBall(BYTE end, DBall scolor);
	afx_msg void OnTimer(UINT nIDEvent);
	// 棋盘空位置随机函数
	bool BallRandFuc(DBall  wChess[], BYTE  wNextBall[], int num);
};
#endif