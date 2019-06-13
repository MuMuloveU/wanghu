#ifndef GLOBAL_RIGHT_HEAD_FILE
#define GLOBAL_RIGHT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//玩家权限定义
#define UR_CANNOT_PLAY					0x00000001L				//不能进行游戏
#define UR_CANNOT_LOOKON				0x00000002L				//不能旁观游戏
#define UR_CANNOT_WISPER				0x00000004L				//不能发送私聊
#define UR_CANNOT_ROOM_CHAT				0x00000008L				//不能大厅聊天
#define UR_CANNOT_GAME_CHAT				0x00000010L				//不能游戏聊天

//标志权限定义
#define UR_MATCH_USER					0x10000000L				//比赛用户标志

//管理权限定义
#define UR_CAN_LIMIT_PLAY				0x00000001L				//允许禁止游戏
#define UR_CAN_LIMIT_LOOKON				0x00000002L				//允许禁止旁观
#define UR_CAN_LIMIT_WISPER				0x00000004L				//允许禁止私聊
#define UR_CAN_LIMIT_ROOM_CHAT			0x00000008L				//允许禁止聊天
#define UR_CAN_LIMIT_GAME_CHAT			0x00000010L				//允许禁止聊天
#define UR_CAN_CUT_USER					0x00000020L				//允许踢出用户
#define UR_CAN_FORBID_ACCOUNTS			0x00000040L				//允许封锁帐号
#define UR_CAN_CONFINE_IP				0x00000080L				//允许禁止地址
#define UR_CAN_SEE_USER_IP				0x00000100L				//允许查看地址
#define UR_CAN_SEND_WARNING				0x00000200L				//允许发送警告
#define UR_CAN_ISSUE_MESSAGE			0x00000400L				//允许发布消息
#define UR_CAN_BIND_GAME				0x00000800L				//允许游戏绑定
#define UR_CAN_BIND_GLOBAL				0x00001000L				//允许全局绑定
#define UR_CAN_SERVER_OPTION			0x00002000L				//允许配置房间

//////////////////////////////////////////////////////////////////////////

//权限辅助类
class CUserRight
{
	//玩家权限
public:
	//游戏权限
	static bool CanPlay(LONG lUserRight) { return (lUserRight&UR_CANNOT_PLAY)==0; }
	//旁观权限
	static bool CanLookon(LONG lUserRight) { return (lUserRight&UR_CANNOT_LOOKON)==0; }
	//私聊权限
	static bool CanWisper(LONG lUserRight) { return (lUserRight&UR_CANNOT_WISPER)==0; }
	//大厅聊天
	static bool CanRoomChat(LONG lUserRight) { return (lUserRight&UR_CANNOT_ROOM_CHAT)==0; }
	//游戏聊天
	static bool CanGameChat(LONG lUserRight) { return (lUserRight&UR_CANNOT_GAME_CHAT)==0; }

	//用户标识
public:
	//比赛标识
	static bool IsMatchUser(LONG lUserRight) { return (lUserRight&UR_MATCH_USER)!=0; }

	//管理权限
public:
	//禁止游戏
	static bool CanLimitPlay(LONG lMasterRight) { return ((lMasterRight&UR_CAN_LIMIT_PLAY)!=0); }
	//禁止旁观
	static bool CanLimitLookon(LONG lMasterRight) { return ((lMasterRight&UR_CAN_LIMIT_LOOKON)!=0); }
	//禁止私聊
	static bool CanLimitWisper(LONG lMasterRight) { return ((lMasterRight&UR_CAN_LIMIT_WISPER)!=0); }
	//禁止聊天
	static bool CanLimitRoomChat(LONG lMasterRight) { return ((lMasterRight&UR_CAN_LIMIT_ROOM_CHAT)!=0); }
	//禁止聊天
	static bool CanLimitGameChat(LONG lMasterRight) { return ((lMasterRight&UR_CAN_LIMIT_GAME_CHAT)!=0); }
	//踢出用户
	static bool CanCutUser(LONG lMasterRight) { return ((lMasterRight&UR_CAN_CUT_USER)!=0); }
	//封锁帐号
	static bool CanForbidAccounts(LONG lMasterRight) { return ((lMasterRight&UR_CAN_FORBID_ACCOUNTS)!=0); }
	//禁止地址
	static bool CanConfineIP(LONG lMasterRight) { return ((lMasterRight&UR_CAN_CONFINE_IP)!=0); }
	//查看地址
	static bool CanSeeUserIP(LONG lMasterRight) { return ((lMasterRight&UR_CAN_SEE_USER_IP)!=0); }
	//发布消息
	static bool CanIssueMessage(LONG lMasterRight) { return ((lMasterRight&UR_CAN_ISSUE_MESSAGE)!=0); }
	//发送警告
	static bool CanSendWarning(LONG lMasterRight) { return ((lMasterRight&UR_CAN_SEND_WARNING)!=0); }
	//游戏绑定
	static bool CanBindGame(LONG lMasterRight) { return ((lMasterRight&UR_CAN_BIND_GAME)!=0); }
	//全局绑定
	static bool CanBindGlobal(LONG lMasterRight) { return ((lMasterRight&UR_CAN_BIND_GLOBAL)!=0); }
	//配置房间
	static bool CanServerOption(LONG lMasterRight) { return ((lMasterRight&UR_CAN_SERVER_OPTION)!=0); }
};

//////////////////////////////////////////////////////////////////////////

#endif
