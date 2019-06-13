#ifndef CMD_CHESS_HEAD_FILE
#define CMD_CHESS_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef VIDEO_GAME
#define KIND_ID						506									//视频游戏ID
#else 
#define KIND_ID						200									//游戏 I D
#endif

#define GAME_PLAYER					2									//游戏人数

#ifdef VIDEO_GAME
#define GAME_NAME					TEXT("视频中国象棋")					//游戏名字
#else 
#define GAME_NAME					TEXT("中国象棋")				//游戏名字
#endif

#define GAME_GENRE					GAME_GENRE_SCORE					//游戏类型

#define IS_VIDEO_GAME				0									//视频类型

//////////////////////////////////////////////////////////////////////////
//游戏定义
#define CountChar(String) (sizeof(String)/sizeof(String[0])-1)
#define	LEN_ACCOUNTS				32									//名字长度

//请求状态
#define GU_WAIT_PEACE				0x01								//求和等待
#define GU_WAIT_REGRET				0x02								//悔棋等待

//失败原因
#define FR_RULE_LIMIT				1									//规则限制
#define FR_COUNT_LIMIT				2									//次数限制
#define FR_PLAYER_OPPOSE			3									//玩家反对

//结束原因							
#define REASON_TWENTY_STEP			1									//二十以内
#define REASON_GIVE_UP				2									//玩家认输
#define REASON_OVER_TIME			3									//超时判输
#define REASON_CHESS_FEAZE			4									//困毙判输
#define REASON_CATCH_CHESS			5									//长捉犯规
#define REASON_CATCH_GENERA			6									//长将犯规
#define REASON_USER_EXIT			7									//强退判输
#define REASON_KILL_GENERA			8									//将死判输

#define REASON_AGREE_DRAW			9									//同意和局
#define REASON_OVER_STEP			10									//满步和局
#define REASON_LEISURE_STEP			11									//空步和局
#define REASON_TEN_STEP				12									//十步以内

//////////////////////////////////////////////////////////////////////////
//规则定义

//状态定义
#define GAME_SCENE_FREE				GS_FREE								//等待开始
#define GAME_SCENE_PLAY				GS_PLAYING							//游戏进行

//模式定义
#define MODE_FAST					1									//快棋模式
#define MODE_SLOW					2									//慢棋模式
#define MODE_CUSTOM					3									//自定模式

//数值范围
#define MAX_STEP_TIME				300									//步时时间
#define MAX_DRAW_TIME				7200								//局时时间
#define MAX_SECOND_TIME				300									//读秒时间

//快棋模式
#define MODE_FAST_STEP_TIME			60									//步时时间
#define MODE_FAST_DRAW_TIME			600									//局时时间
#define MODE_FAST_SECOND_TIME		10									//读秒时间

//慢棋模式
#define MODE_SLOW_STEP_TIME			180									//步时时间
#define MODE_SLOW_DRAW_TIME			1800								//局时时间
#define MODE_SLOW_SECOND_TIME		0									//读秒时间

//规则结构
struct tagGameRuleInfo
{
	//规则设置
	BYTE							cbDirections;						//提示标志
	BYTE							cbRegretFlag;						//悔棋标志
	BYTE							cbCurrentMode;						//当前模式

	//限时设置
	WORD							wRuleStepTime;						//步时时间
	WORD							wRuleDrawTime;						//局时时间
	WORD							wRuleSecondTime;					//读秒时间
};

//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START			100									//游戏开始
#define SUB_S_MOVE_CHESS			101									//移动棋子
#define SUB_S_REGRET_FAILE			102									//悔棋失败
#define SUB_S_REGRET_RESULT			103									//悔棋结果
#define SUB_S_REGRET_REQUEST		104									//悔棋请求
#define SUB_S_PEACE_RESPOND			105									//和棋响应
#define SUB_S_PEACE_REQUEST			106									//和棋请求
#define SUB_S_GAME_END				107									//游戏结束
#define SUB_S_CHESS_MANUAL			108									//棋谱信息
#define SUB_S_REQUEST_RULE			109									//请求设置
#define SUB_S_REQUEST_AGREE			110									//请求同意

//空闲状态
struct CMD_S_StatusFree
{
	BYTE							cbGameRule;							//规则标志
	WORD							wBankerUser;						//庄家玩家
	WORD							wRuleRoundID;						//规则标识
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//游戏状态
struct CMD_S_StatusPlay
{
	//游戏信息
	WORD							wBankerUser;						//庄家玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wLeaveDrawTime[GAME_PLAYER];		//剩余局时
	BYTE							cbRequestStatus[GAME_PLAYER];		//请求状态

	//游戏规则
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家玩家
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//放置棋子
struct CMD_S_MoveChess
{
	//用户信息
	WORD							wActionUser;						//动作用户
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wLeaveDrawTime;						//剩余局时

	//走棋信息
	BYTE							cbXSourcePos;						//棋子位置
	BYTE							cbYSourcePos;						//棋子位置
	BYTE							cbXTargetPos;						//棋子位置
	BYTE							cbYTargetPos;						//棋子位置
};

//游戏结束
struct CMD_S_GameEnd
{
	BYTE							cbReason;							//结束原因
	WORD							wWinerUser;							//胜利玩家
	LONGLONG						lGameScore[GAME_PLAYER];			//用户成绩
};

//悔棋失败
struct CMD_S_RegretFaile
{
	BYTE							cbFaileReason;						//失败原因
};

//悔棋结果
struct CMD_S_RegretResult
{
	WORD							wTargetStep;						//目标数目
	WORD							wRegretUser;						//悔棋玩家
	WORD							wCurrentUser;						//当前玩家
	WORD							wLeaveDrawTime[GAME_PLAYER];		//剩余局时
};

//请求设置
struct CMD_S_RequestRule
{
	WORD							wRuleRoundID;						//规则标识
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//请求同意
struct CMD_S_RequestAgree
{
	WORD							wRuleRoundID;						//规则标识
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_MOVE_CHESS			1									//移动棋子
#define SUB_C_PEACE_REQUEST			2									//和棋请求
#define SUB_C_PEACE_RESPOND			3									//和棋应答
#define SUB_C_REGRET_REQUEST		4									//悔棋请求
#define SUB_C_REGRET_RESPOND		5									//悔棋应答
#define SUB_C_CONCLUDE_REQUEST		6									//结束请求

//移动棋子
struct CMD_C_MoveChess
{
	//位置变量
	BYTE							cbXSourcePos;						//棋子位置
	BYTE							cbYSourcePos;						//棋子位置
	BYTE							cbXTargetPos;						//棋子位置
	BYTE							cbYTargetPos;						//棋子位置

	//时间变量
	WORD							wUsedTimeCount;						//使用时间
};

//和棋响应
struct CMD_C_PeaceRespond
{
	BYTE							cbApprove;							//同意标志
};

//悔棋响应
struct CMD_C_RegretRespond
{
	BYTE							cbApprove;							//同意标志
};

//定制规则
struct CMD_C_CustomizeRult
{
	tagGameRuleInfo					GameRuleInfo;						//游戏规则
};

//请求结束
struct CMD_C_ConcludeRequest
{
	BYTE							cbReason;						//结束原因
};

//////////////////////////////////////////////////////////////////////////

#endif
