#ifndef CMD_BILLIARDS_HEAD_FILE
#define CMD_BILLIARDS_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						402									//游戏 I D
#define GAME_PLAYER					2									//游戏人数
#define GAME_NAME					TEXT("台球游戏")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_SCORE|GAME_GENRE_GOLD)	//游戏类型

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

//////////////////////////////////////////////////////////////////////////
/********************************************************************************/

// 台球游戏消息头文件  3.0

/********************************************************************************/

#define		MAXCLIENTVER			MAKELONG(5,4)
#define		LESSCLIENTVER			0
#define		GS_TQ_WAIT_BEGIN					GS_FREE
#define		GS_GAME_PLAY						GS_PLAYING
#define		SUB_GAME_BEGIN						100		//游戏开始
#define     BALL_NUM                22       //球的个数

/********************************************************************************/

//	游戏数据包

/********************************************************************************/
//球信息结构

struct tagGS_PlaceInfo
{
	tagGS_PlaceInfo()
	{
		::memset(this, 0, sizeof(*this));
	}

	double				xPos;
	double				yPos;
	BYTE				bBallIndex;
};
//游戏级别设置
struct tagGS_GameStation_1
{
	tagGS_GameStation_1()
	{
		::memset(this, 0, sizeof(*this));
	}
	BYTE bMatch;
};
//游戏状态数据包 （游戏中使用）
struct tagGS_GameStation_2
{
	tagGS_GameStation_2()
	{
		::memset(this, 0, sizeof(*this));
	}

	BYTE				bTargetBall;					//目标球
	BYTE				bNowHitUser;					//下一个击球者
	BYTE                mStarPos;
	BYTE                mStarPoint;               
	BYTE                bIsTakeWhite;
	BYTE				bInBallIndex[BALL_NUM];				//进袋的球索引
	BYTE				bBallAccess[BALL_NUM];				//能否访问
	int					iUserPoint[BALL_NUM];					//当前分数
	tagGS_PlaceInfo		bPlaceInfo[BALL_NUM];					//球的信息
};

struct tagGS_BeginStruct
{
	tagGS_BeginStruct()
	{
		::memset(this, 0, sizeof(*this));
		bIsTrain=FALSE;
	}
	BYTE				bFirstHit;						//先击球者
	BYTE                bIsTrain;
};
//游戏结束数据包
struct tagGS_GameFinishStruct
{
	tagGS_GameFinishStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	BYTE			FinishMode;							//结束方式(0正常结束，1非法结束)
	int				Point[2];							//记录玩家的分数
	BYTE            m_ReSetRule;
	BYTE            m_NowHitUser;                       //最后击进球的玩家
};


//击球信息
struct tagGS_HitInfoStruct
{
	tagGS_HitInfoStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	POINT						CursorPos;				//光标位置
	double                        iPower;
};

struct tagGS_CurrentPlayerStruct
{
	tagGS_CurrentPlayerStruct()
	{
		::memset(this, 0, sizeof(*this));
		Seat=0xff;
	}
	int					iPoint;
	BYTE				Seat;						//当前击球者
};

//击球动作

struct tagGS_HitBallStruct
{
	tagGS_HitBallStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	int					iPower;
	int					iXPole;
	int					iYPole;
	double				iXBallPoint;
	double				iYBallPoint;
	double              iPowePos;    
	double              iPoweAngle; 
};


//下一击球动作

struct tagGS_NextHitInfoStruct
{
	tagGS_NextHitInfoStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	int					iPoint;
	BYTE				bHitPeople;
	BYTE				bTargerBall;
	tagGS_PlaceInfo		bPlaceInfo[BALL_NUM];					//球的信息
	BYTE                mStarPos;
	BYTE                mStarPoint;
	BYTE                bIsGetStarPoint;
};


//击球完成
struct tagGS_HitFinishStruct
{
	tagGS_HitFinishStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	BYTE				bFirstHitBall;
	BYTE				bInBallCount;
	BYTE				bInBallIndex[BALL_NUM];
	BYTE				bBallAccess[BALL_NUM];
	BYTE                bIsGetStar;
	double				iXBallStation[BALL_NUM];
	double				iYBallStation[BALL_NUM];
};


//放置球信息（发向服务器）

struct tagCS_PlaceBallStruct
{
	tagCS_PlaceBallStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	BYTE				bBallCount;
	tagGS_PlaceInfo			bPlaceInfo[BALL_NUM];
};

//放置球信息（发向客户端）
struct tagGS_PlaceInfoStruct
{
	tagGS_PlaceInfoStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	BYTE				bHitPeople;
	BYTE				bBallCount;
	BYTE				bTargerBall;
	tagGS_PlaceInfo			bPlaceInfo[BALL_NUM];
};


//GSQ:彩球重新放置信息（发向客户端）
struct tagGS_ColorBallPlaceInfoStruct
{
	tagGS_ColorBallPlaceInfoStruct()
	{
		::memset(this, 0, sizeof(*this));
	}
	BYTE				bBallCount;			//要放置球的数量
	BYTE				bInBallIndex[BALL_NUM];	//欲放置球的索引
	BYTE				bPos;				//是谁让彩球进袋的
	bool				BallError;			//是不是进错球了
	small				BallValue;			//如果进错球，要扣的分数
	BYTE                Seat;
};
/********************************************************************************/

// 数据包处理辅助标识

/********************************************************************************/

#define ASS_TQ_HIT_INFO			50
#define ASS_TQ_HIT_BALL			51
#define ASS_TQ_NEXT_HIT			52
#define ASS_TQ_TAKE_WHITE		53
#define ASS_TQ_PLACE_WHITE		54
#define	ASS_TQ_HIT_FINISH		55
#define ASS_TQ_PLACE_COLOR		56
#define ASS_TQ_GAME_FINISH		57
#define ASS_TQ_1				59
#define ASS_TQ_2				61
#define ASS_TQ_HIT_GIVEUP       62
#define ASS_TQ_HIT_REPLACE      63
#define ASS_TQ_HIT_CONESSION    64
#define ASS_TQ_AGREE		    65
#define ASS_TQ_NOAGREE          68

/********************************************************************************/


#endif