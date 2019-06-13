#ifndef CMD_QUICKRUN_HEAD_FILE
#define CMD_QUICKRUN_HEAD_FILE

#pragma pack(push,1)
//////////////////////////////////////////////////////////////////////////
//公共宏定义

#ifdef _TWICE
	#define KIND_ID							491									//游戏 I D
	#define GAME_NAME						TEXT("跑得快游戏（两副牌）")		//游戏名字
	#define PISACOUNT						27									//每个玩家的牌数
#else
	#define KIND_ID							490									//游戏 I D
	#define GAME_NAME						TEXT("跑得快游戏")					//游戏名字
	#define PISACOUNT						13									//每个玩家的牌数
#endif
#define GAME_PLAYER						4									//游戏人数
#define GAME_GENRE						(GAME_GENRE_SCORE|GAME_GENRE_GOLD|GAME_GENRE_MATCH)	//游戏类型

#define WM_OUTPISA						WM_USER+147							//玩家出牌消息
#define WM_USERREADY					WM_USER+148							//玩家准备好的消息


//游戏状态
#define GS_WK_FREE						GS_FREE								//等待开始
#define GS_WK_PLAYING					GS_PLAYING							//游戏进行

//牌花式定义
struct tagPisa{
	int line;	//行，4:黑桃，3:红桃，2:梅花，1:方片
	int row;	//列，1-10：1-10，11：J，12：Q，13：K，[4][14]：表示大王，[3][14]：表示小王
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构
#define MSG_SET_INITPISA				0x11			//服务器设置玩家自己初始的牌
#define MSG_SET_FOCUS					0x12			//设置当前出牌的人
#define MSG_SET_OTHERPISA				0x13			//服务器设置其他玩家的牌
#define MSG_OUTPISA_MSG					0x14			//玩家提交出的牌之后，服务器判断结果
#define MSG_GAME_END					0x15			//游戏结束

//////////////////////////////////////////////////////////////////////////
//客户端命令结构
#define MSG_SET_OUTPISA					0x06			//玩家出的牌

//////////////////////////////////////////////////////////////////////////

struct tagSetFocus{
	int nFoucsId;							//出牌人的ChairId
	int bClean;								//是否清掉现在桌面上的牌
};

struct tagSetInitPisa{
	int nOwnerIndex;						//设置自己相对的索引值，即自己的ChairId
	struct tagPisa tPisa[PISACOUNT];		//初始的牌
};

struct tagSetOtherPisa{
	int nChairId;							//玩家Id
	int nCount;								//玩家出的牌数目
	struct tagPisa tOutPisa[PISACOUNT];		//玩家出的牌
};

struct tagOutPisaMsg{
	int nResult;							//0:通过,1:不通过,后跟错误信息
	char chErrMsg[100];						//错误信息
};

struct tagGameEnd{
	LONG lGameTax;							//游戏税收
	LONG lGameScore[GAME_PLAYER];			//游戏积分
};

struct tagSetOutPisa{
	int nChairId;							//玩家Id
	int nCount;								//玩家出的牌子数目，为0表示Pass
	struct tagPisa tOutPisa[PISACOUNT];		//玩家出的牌
};

#pragma pack(pop)

#endif