#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#ifdef VIDEO_GAME
#define KIND_ID						615									//游戏 I D
#else
#define KIND_ID						611									//游戏 I D
#endif

#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("A44对家")						//游戏名字

//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_SCORE					GS_PLAYING							//叫分状态
#define GS_WK_PLAYING				GS_PLAYING+1						//游戏进行

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_LAND_SCORE			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
//////////////////////////////////////////////////////////////////////////
#define SUB_S_GAME_CHA				106									//叉牌
#define SUB_S_GAME_NOCHA			107									//放弃叉牌
#define SUB_S_GAME_DIAN				108									//点牌
#define SUB_S_GAME_NODIAN			109									//放弃点牌

#define SUB_S_LI_GUN_REQUEST        110                                 //立棍请求
#define SUB_S_JUE_GUN               111                                 //可以撅棍
#define SUB_S_JUE_GUN_REQUEST       112                                 //撅棍请求
#define SUB_S_LI_GUN                113                                 //可以立棍
//////////////////////////////////////////////////////////////////////////
//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[GAME_PLAYER][15];			//手上扑克
	BYTE							bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[15];					//出牌列表
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	bool							gou;								//够
	bool							dian;								//点
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[25];						//扑克列表
};

//发送扑克
struct CMD_S_SendAllCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[GAME_PLAYER][15];			//扑克列表
	BYTE							cbCardNum[GAME_PLAYER];				//用户的扑克数目
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wCurrentUser;						//当前玩家	
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[15];						//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	LONG							lGameScore[GAME_PLAYER];			//游戏积分
	BYTE							bCardCount[GAME_PLAYER];			//扑克数目
	BYTE							bCardData[54];						//扑克列表 
};

/////////////////////////////////////////添加的数据包/////////////////////////////////////
//立棍请求
struct CMD_S_LiGunRequest
{
	WORD                            wRequest;
};

//满足撅棍条件
struct CMD_S_CanJueGun
{
	WORD                            wCanJueChair[2];
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_TRUSTEE				4									//托管消息
//////////////////////////////////////////////////////////////////////////
#define SUB_C_GAME_CHA				6									//叉牌
#define SUB_C_GAME_NOCHA			7									//放弃叉牌
#define SUB_C_GAME_DIAN				8									//点牌
#define SUB_C_GAME_NODIAN			9									//放弃点牌

#define SUB_C_LI_GUN                10                                  //立棍
#define SUB_C_JUE_GUN               11                                  //撅棍
#define SUB_C_AGREE                 12                                  //是否同意立撅棍
#define SUB_C_LI_GUN_SUPERTIME      13                                  //立棍超时
#define SUB_C_JUE_GUN_SUPERTIME     14                                  //撅棍超时
#define SUB_C_LI_GUN_ANSWER         15                                  //立棍答复
#define SUB_C_NOT_LI_GUN            16                                  //不立棍
#define SUB_C_NOT_JUE_GUN           17                                  //不撅棍
//////////////////////////////////////////////////////////////////////////
//托管结构
struct CMD_C_UserTrustee 
{
	WORD							wUserChairID;						//玩家椅子
	bool							bTrustee;							//托管标识
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[15];						//扑克列表
};

////////////////////////////////////////////////添加的数据包//////////////////////////////////////////
//玩家立棍
struct CMD_C_LiGun
{
	WORD                            wLiGunUserChairID;
};

struct CMD_C_LiGunAnswer
{
	BYTE                            cbAnswer;                          //标识，1为同意，0为不同意
	WORD                            wAgreeChairID;                     //同意（不同意）玩家的座位号
	BYTE                            cbLiOrJue;                         //标识，1为立棍，2为撅棍
};

struct CMD_C_NotLiOrJue
{
	WORD                            wNotLiOrJueChair;
};

//////////////////////////////////////////////////////////////////////////

#endif