#ifndef BALL_HEAD_FILE
#define BALL_HEAD_FILE

#pragma once

#define GRATING				0.994					//摩擦力系数
#define HIT_LOST			0.95					//碰撞损失 
#define LESS_SPEED			0.18					//最少速度
#define MAX_SPEED			10.0					//最大速度
#define BALL_COUNT			9						//球种类
#define SQR_RADII_2			14.14					//半径平方开方
#define DESK_SIDE			40						//桌子边
#define BALL_RADII			11.9						//球半径
#define BALL_RADII_A	    11.0						//球半径
#define BAG_WIGHT			1.65*BALL_RADII			//袋宽

#define BALL_BACK_COLOR		RGB(255,0,255)

//台球类

class CBall
{
	//变量定义 
public:
	static double			m_Grating;				//摩擦力

public:
	BYTE					m_bColor;				//球颜色
	BYTE					m_bIndex;				//球索引
	bool					m_bAccess;				//是否可以读取
	double					m_xPos;					//X 坐标
	double					m_yPos;					//Y 坐标
	double					m_xSpeed;				//X 速度
	double					m_ySpeed;				//Y 速度
	double					m_xPreSpeed;			//X 预速度
	double					m_yPreSpeed;			//Y 预速度
	double					m_xTurnSpeed;			//X 附加速度
	double					m_yTurnSpeed;			//Y 附加速度
	double					m_xPreTurnSpeed;		//X 附加预速度
	double					m_yPreTurnSpeed;		//Y 附加预速度
	double					m_PreSina;				//X 附加角度
	double					m_PreCosa;				//Y 附加角度

	//函数定义 
public:
	//构造函数 
	CBall(void);
	//移动函数 
	BOOL Move();
	//更新函数
	BOOL UpdateSpeed();
	//更新转向速度
	BOOL UpdateTurnSpeed();
	//设置位置
	BOOL SetStation(double xPos, double yPos);
	//设置速度 
	BOOL SetSpeed(double xSpeed, double ySpeed, double xTurnSpeed=0.0, double yTurnSpeed=0.0);
	//设置预速度
	BOOL SetPreSpeed(double xSpeed, double ySpeed);
	//碰撞检测
	BOOL CollideBall(CBall & Ball);
	//测试碰撞
	BOOL TestCollide(double x, double y);
	
};

//球袋
class CBallBag
{
	//变量定义
public:
	bool					m_bNeedMove;				//是否需要移动
	int						m_iBagLength;				//球袋长度
	int						m_iBallCount;				//球数目
	int						m_bBallStation[BALL_NUM];			//球位置
	CBall					* m_pBall[BALL_NUM];				//球引用

	//函数定义
public:
	//构造函数
	CBallBag();
	//进袋
	BOOL PushBall(CBall & Ball);
	//取球
	CBall * TakeBall(BYTE bBallColor);
	//移动
	BOOL Move();
};

//台球桌类 
class CBallDesk
{
	//变量定义
public:
	bool					m_bSound;					//声音效果
	bool					m_bNeedMove;				//是否需要移动
	long int				m_dwWidth;					//球桌宽
	long int				m_dwHeight;					//球桌长
	int						m_iBallCount;				//球数目
	int						m_iInBagCount;				//进袋的球数目
	int						m_iFirstHitBag;				//第一次集中的球索引
	CBall					* m_pBallList;				//球列表
	CBallBag				m_BallBag;					//球袋;
	CPoint					m_BagPoint[6];				//袋位置
	bool	bSoundInBag;
	bool	bSoundHitBall;
	bool 	bSoundHitDesk;
	BOOL    bIsGetStar;

	//字符信息
public:
	int						m_iTextPos;					//字符位置
	int						m_iScrollPos;				//信息滚动点
	TCHAR					m_szText[101];				//字符数组

	//函数定义
public:
	//构造函数 
	CBallDesk();
	//初始化台球桌
	virtual BOOL InitDesk()=0;

	//功能函数
public:
	//定时器函数
	int OnTimer();
	//击球函数
	void HitBall(int x, int y, double fPower, double xTurn, double yTurn,double HitDegree);
	//是否进袋
	bool IsBallInBag();
	//调整位置
	bool IsHitDesk(int iBallStation);
	//测试是否可以放置
	bool CanPlaceBall(double x, double y, bool bWhiteBall);
	//设置信息字符 
	BOOL SetMessageText(TCHAR * szMessage, int iScrollPos, int yPos,BOOL b=false);
};

//美式台球
class CAmericaDesk : public CBallDesk
{
	//变量定义 
public:
	CBall					m_Ball[BALL_NUM];
	//设置袋宽
	BOOL SetBagWight(BYTE  GameLayer);
	//函数定义
public:
	//初始化台球桌
	virtual BOOL InitDesk();
};

#endif
