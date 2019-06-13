#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef VIDEO_GAME
	#define KIND_ID						527									//游戏 I D
#else
	#define KIND_ID						26									//游戏 I D
#endif
#define GAME_PLAYER					4									//游戏人数
#define GAME_NAME					TEXT("淮安掼蛋")					//游戏名字
#define GAME_GENRE					(GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型
#define PIECES						27									//扑克数每人

//游戏状态
#define GS_RF_FREE					GS_FREE								//等待开始
#define GS_RF_PLAYING				GS_PLAYING							//进行状态

//房间类型
enum enRoomType
{
	enRoomType_Normal,													//普通房间
	enRoomType_All														//整局房间
};


//进贡阶段
enum enPayStatus
{
	enNone,																//无
	enJin_Gong,															//进贡
	enHui_Gong,															//回贡
	enGet_Hui_Gong,														//获取回贡
	enKang_Gong															//抗贡
};	
//游戏级数
struct Series
{
	BYTE                            bOurSeries;                         //我方级数
	BYTE                            bOtherSeries;                       //对方级数
	BYTE                            bCurrentSeries;                     //本局级数
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START			101									//游戏开始
#define SUB_S_OUT_CARD				102									//用户出牌
#define SUB_S_PASS_CARD				103									//放弃出牌
#define SUB_S_PAY_TRIBUTE           104                                 //用户进贡
#define SUB_S_GAME_END				105									//游戏结束
#define SUB_S_END					106									//本盘结束
#define SUB_S_REQUEST_QUIT			107									//请求退出
#define SUB_S_ANSWER_QUIT           108									//回应退出
#define SUB_S_CARD					109									//对家的牌
#define SUB_S_SYSTEM_MESSAGE		110									//系统消息
#define SUB_S_LOOKON				111									//旁观请求

//系统消息数据包
struct CMD_S_MESSAGE
{
	WORD							wChairID;							//来自用户
	TCHAR							szBuffer[128];						//系统消息
};


//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
};

//游戏状态
struct CMD_S_StatusPlay{
	LONG							lBaseScore;							//基础积分
	WORD							wBankerUser;						//庄家用户
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[27];						//手上扑克
	BYTE							bCardCount[4];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[27];					//出牌列表
	LONG                            lAllTurnScore[4];					//总局得分
	LONG                            lLastTurnScore[4];					//上局得分
	Series                          series;                             //游戏级数
	int                             m_iGameResult[4];                   //胜负情况
	enRoomType						RoomType;							//房间类型
	WORD							wServerType;						//服务器类型
};

//发送扑克
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家用户
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							cbCardData[27];						//扑克列表
	Series                          series;                             //游戏级数
	int                             m_iGameResult[4];                   //胜负情况
	enRoomType						RoomType;							//房间类型
	WORD                            wServerType;						//服务器类型
	bool							bLiangPai;							//是否亮牌
	BYTE							bLiangCard;							//亮牌
};

//对家的牌
struct CMD_S_CARD
{
	WORD							wFromUser;							//对家ID
	BYTE							bCardData[28];						//扑克数据
	BYTE							bCardCount;							//扑克大小
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//扑克数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[27];						//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};

//用户进贡
struct CMD_S_PayTribute
{
	BYTE                            bPayType[4];						//进贡类型
	enPayStatus						bPayStatus;						//进贡阶段
	WORD                            wFromUser;                          //进贡用户
	WORD                            wToUser;                            //被进贡用户
	WORD                            wCurrentUser;                       //当前玩家
	BYTE                            bCard;                              //进贡扑克
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	LONG							lGameScore[4];						//游戏积分
	BYTE							bCardCount[4];						//扑克数目
	BYTE							bCardData[108];						//扑克列表
	Series                          series;                             //游戏级数
	int                             m_iGameResult[4];                   //胜负情况
};

//请求数据包
struct CMD_S_REQUEST
{
	WORD							wChairID;							//座位号
	TCHAR							szBuffer[50];						//退出原因
};

//答复消息
struct CMD_S_ANSWER
{
	WORD							wChairID;							//座位号
	bool                            bAnswer;							//回应答复
};
//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_OUT_CARD				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_PAY_TRIBUTE           4                                   //游戏进贡
#define SUB_C_START					5									//游戏开始
#define SUB_C_HALFWAY_QUIT			6									//中途退出
#define SUB_C_REQUEST_QUIT			7									//强求退出
#define SUB_C_ANSWER_QUIT           8									//回应答复
#define SUB_C_QUIT					9									//用户离开
#define SUB_C_SYSTEM_MESSAGE		10									//系统消息
#define SUB_C_LOOKON				11									//旁观请求

//系统消息数据包
struct CMD_C_MESSAGE
{
	WORD							wChairID;							//来自用户
	TCHAR							szBuffer[128];						//系统消息
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[27];						//扑克列表
};

//请求数据包
struct CMD_C_REQUEST
{
	WORD							wChairID;							//座位号
	TCHAR							szBuffer[50];						//退出原因
};

//答复消息
struct CMD_C_ANSWER
{
	WORD							wChairID;							//座位号
	bool                            bAnswer;							//回应答复
};
//用户进贡
struct CMD_C_PayTribute
{
	enPayStatus                     bPayType;							//进贡类型
	WORD                            wUser;                              //进贡用户
	WORD                            wToUser;							//受进贡者
	BYTE                            bCard;                              //进贡扑克
};
//////////////////////////////////////////////////////////////////////////

#endif