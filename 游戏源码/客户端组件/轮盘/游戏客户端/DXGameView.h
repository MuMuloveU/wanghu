#ifndef DX_GAME_VIEW_HEAD_FILE
#define DX_GAME_VIEW_HEAD_FILE
#pragma once
#include "GameLogic.h"
#include "ScoreView.h"

#include "DxDialog.h"
#include "SkinMesh.h"
#include "DxControl.h"
#include "Camera.h"
#include "SkinImageEx.h"
#include "SkinSprite.h"
/////////////////////////////////////////////////////////////////////


#define     MAX_CHIPS			1000//最多下注筹码个数
#define		JETTON_COUNT		9	//筹码种数
#define		HISTORY_NUMBER		13	//历史记录个数

#define		MAX_MOVE_RADIUS		120	//最大球运动半径
#define		WIN_RADIUS			75	//胜利半径
#define		JUMP_RADIUS			85	//跳跃半径
#define		FALL_RADIUS			116	//下落半径

#define		WIN_CHIP_POS_X		380	//筹码移动
#define		WIN_CHIP_POS_Y		600	//筹码移动

#define		IDM_CHIP_IN			(WM_USER+300)	//开始消息
#define		IDM_WIN_NUMBER		(WM_USER+301)	//胜利数字
#define		IDM_GET_STOP		(WM_USER+302)	//胜利数字

#define		STATUS_FREE			0	//空闲状态
#define		STATUS_READY		1	//准备状态
#define		STATUS_PLAY			2	//旋转状态

/////////////////////////////////////////////////////////////////////

//d3d窗口类
class CDXGameView : public CDxDialog
{
	//运行变量
protected:
	BYTE				m_bStatus;							//游戏状态
	bool				m_bZeroLight;						//0号数字灯
	bool				m_bNumberLight[3][12];				//1~36数字灯
	long				m_wAllMoney;						//所有下注筹码
	long				m_wHistoryChipInfo[MAX_BET_POS];	//历史下注信息
	long				m_wChipInfo[MAX_BET_POS];			//所有下注筹码信息数组
	long				m_wOtherUsersChips[MAX_BET_POS];	//其他用户下注
	int					m_nScreenWidth,m_nScreenHeight;		//dx窗口宽高
	TCHAR				m_TipsMessage[64];					//提示信息
	BYTE				m_bHistoryInfo[HISTORY_NUMBER];		//历史信息
	int					m_wCurrentChip;						//当前选择筹码
	LONG				lScoreIndex[JETTON_COUNT];			//筹码类型

	//球控制变量
	float				m_Ball_MoveRadius;	//运动半径
	float				m_Ball_StartHeight;	//开始高度
	float				m_fBallMoveAngel;	//移动角度
	D3DXVECTOR3			m_vBallMovePath;	//移动路径
	D3DXVECTOR3			m_vCenterPos;		//中心点位置
	BYTE				m_bWinNumber;		//胜利数字
	BYTE				m_bTargetNumber;	//目标数字

	//控制结算显示
public:
	BYTE				m_bShowEnd;//0:不显示 1:显示胜利 2:显示失败
	long				m_lGameEndScore;//结束分数
	long				m_lMinBetScore;//最小下注
	long				m_lMaxBetWin;//最大赢注
	long				m_lMaxUserScore;//最大用户分数
	bool				m_bDrawMoveChips;
	int					m_iMoveChipsCount;
	//////////////////////////////////////////////////////////////////////
	//以下变量在窗口重置时需要释放
	//平面图片
protected:
	CSkinSprite			m_Chip1;			//面额1筹码
	CSkinSprite			m_Chip5;			//面额5筹码
	CSkinSprite			m_Chip10;			//面额10筹码
	CSkinSprite			m_Chip50;			//面额50筹码
	CSkinSprite			m_Chip100;			//面额100筹码
	CSkinSprite			m_Chip500;			//面额500筹码
	CSkinSprite			m_Chip1000;			//面额1000筹码
	CSkinSprite			m_Chip5000;			//面额5000筹码
	CSkinSprite			m_Chip10000;		//面额10000筹码
	CSkinSprite			m_SprCurrentChip;	//当前选择筹码

	CSkinSprite			m_SprMoveChips[MAX_CHIPS];
	CSkinSprite			m_SprTimer[10];

	CSkinSprite			m_PlaneBall;		//平面球
	CSkinSprite			m_PlatePlane;		//平面轮盘
	CSkinSprite			m_PlatePlaneMask;	//轮盘掩盖

	CSkinSurface		m_SurfaceTable;		//轮盘桌面
	CSkinSurface		m_SurfaceBack;		//外围环境
	CSkinSprite			m_NumberMask;		//鼠标响应效果
	//场景模型
protected:
	CSkinMesh 			m_MeshPlate;		//带有数字的盘
	CSkinMesh 			m_MeshStock;		//外围的托盘
	CSkinMesh			m_MeshShelfHigh;	//中间的架子上部
	CSkinMesh			m_MeshShelfLow;		//中间的架子下部
	CSkinMesh			m_MeshRing;			//分隔数字的环
	CSkinMesh			m_MeshBall;			//球
	LPDIRECT3DTEXTURE9	m_pReflect;			//反射纹理
	LPDIRECT3DTEXTURE9	m_pBump;			//凹凸纹理
	//////////////////////////////////////////////////////////////////////

	//控制变量
public:
	CSkinButton			m_btChipIn;				//确认下注开始
	CSkinButton			m_btClearChip;			//清除下注按钮
	bool				m_bAllowBet;			//允许下注
	BYTE				m_bTimeLeave;//剩余时间

	//控件变量
protected:
	CGameLogic			m_GameLogic;
	CScoreView			m_ScoreView;

	//函数定义
public:
	//构造函数
	CDXGameView(void);
	//析构函数
	virtual ~CDXGameView(void);

	//继承函数
public:
	virtual void DrawGameView2D();
	virtual void DrawGameView3D();
	virtual void InitScene(int nWidth,int nHeight);
	virtual void ReleaseScene();
	virtual void UpDateControl();

	//扩展函数
public:
	//设置灯光
	void SetLight();
	//取得筹码信息
	void GetChipsInfo();
	//绘画筹码
	void DrawChips(int x,int y,long money);
	//绘画筹码移动
	void SetMoveChips(int x,int y,long money);
	//绘画历史
	void DrawHistory();
	//取得压注金额
	long GetAllMoney();
	//取得历史下注
	long GetHistoryMoney();
	//清除下注
	void ClearAllMoney();
	//计算球的运动路径
	void calcPath(D3DXVECTOR3 *movePath);
	//得到球所停数字
	BYTE GetBallStopNumber(float angel);
	//历史数字
	void SetHistoryNumber(BYTE bHistoryInfo[HISTORY_NUMBER]);
	//取得筹码位置
	void GetChipPosition(int index,CPoint *pPosition);

	//游戏信息
public:
	void OnGameStart(BYTE bTargetNumber);
	void OnUserChip(long lAllChip[256]);
	void OnRollStop(BYTE bWinNumber);
	void OnGameEnd();
	void OnSetCellScore(long lCellScore);
	//消息映射
protected:
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	//左键按下消息
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	//右键按下消息
	afx_msg void OnRButtonDown(UINT nFlags,CPoint point);
	//开始旋转
	afx_msg void OnBnClickedChipIn();
	//清除下注
	afx_msg void OnBnClickedClearChip();

	DECLARE_MESSAGE_MAP()
};

#endif
