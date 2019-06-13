#ifndef GLOBAL_PROPERTY_HEAD_FILE
#define GLOBAL_PROPERTY_HEAD_FILE

////////////////////////////////////////////////////////////////////////////////////////////////

//鲜花定义
#define FLOWER_1					0
#define FLOWER_2					1
#define FLOWER_3					2
#define FLOWER_4					3
#define FLOWER_5					4
#define FLOWER_6					5
#define FLOWER_7					6
#define FLOWER_8					7
#define FLOWER_9					8
#define FLOWER_10					9
#define FLOWER_11					10

BYTE const g_FlowerTypeList[]=
{
	FLOWER_1,
	FLOWER_2,
	FLOWER_3,
	FLOWER_4,
	FLOWER_5,
	FLOWER_6,
	FLOWER_7,
	FLOWER_8,
	FLOWER_9,
	FLOWER_10,
	FLOWER_11,
};

//鲜花数目
#define FLOWER_COUNT		(sizeof(g_FlowerTypeList)/sizeof(g_FlowerTypeList[0]))//礼物数目

//////////////////////////////////////////////////////////////////////////
//宏定义
#define CHARM_EXCHANGE_RATE			50									//兑换比率
#define MAX_FLOWER_COUNT			100									//最大数目

//鲜花结构
struct tagGiftInfo
{
	TCHAR							szName[64];							//鲜花名称
	TCHAR							szAction[128];						//动作描述
	TCHAR							szQuantifier1[20];					//数量词
	TCHAR							szResult[128];						//结果描述
	TCHAR							szQuantifier2[6];					//数量词
	TCHAR							szCharmResult[128];					//结果描述
};

//鲜花定义
tagGiftInfo const g_FlowerDescribe[FLOWER_COUNT]=
{
	{TEXT("掌声"),TEXT(" 深情地赠送 "),TEXT(" 掌声1次"),TEXT("，恭喜 "),TEXT("次 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("香吻"),TEXT(" 深情地赠送 "),TEXT(" 香吻1个"),TEXT("，恭喜 "),TEXT("个 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("鲜花"),TEXT(" 深情地赠送 "),TEXT(" 鲜花1朵"),TEXT("，恭喜 "),TEXT("朵 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("啤酒"),TEXT(" 深情地赠送 "),TEXT(" 啤酒1杯"),TEXT("，恭喜 "),TEXT("杯 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("香烟"),TEXT(" 深情地赠送 "),TEXT(" 香烟1包"),TEXT("，恭喜 "),TEXT("包 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("钻戒"),TEXT(" 深情地赠送 "),TEXT(" 钻戒1枚"),TEXT("，恭喜 "),TEXT("枚 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("轿车"),TEXT(" 深情地赠送 "),TEXT(" 轿车1辆"),TEXT("，恭喜 "),TEXT("辆 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("别墅"),TEXT(" 深情地赠送 "),TEXT(" 别墅1座"),TEXT("，恭喜 "),TEXT("座 "),TEXT(" 魅力值增加 %ld 点！")},
	{TEXT("臭蛋"),TEXT(" 狠狠地砸了 "),TEXT(" 臭蛋1个"),TEXT("，抱歉 "),TEXT("个 "),TEXT(" 魅力值减少 %ld 点！")},
	{TEXT("砖头"),TEXT(" 狠狠地砸了 "),TEXT(" 砖头1块"),TEXT("，抱歉 "),TEXT("块 "),TEXT(" 魅力值减少 %ld 点！")},
	{TEXT("炸弹"),TEXT(" 狠狠地扔了 "),TEXT(" 炸弹1枚"),TEXT("，抱歉 "),TEXT("枚 "),TEXT(" 魅力值减少 %ld 点！")}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//道具索引
#define PROP_DOUBLE						0									//双倍积分卡
#define PROP_FOURDOLD					1									//四倍积分卡
#define PROP_NEGAGIVE					2									//负分清零
#define PROP_FLEE						3									//清逃跑率
#define PROP_BUGLE						4									//小喇叭
#define PROP_KICK						5									//防踢卡
#define PROP_SHIELD						6									//护身符
#define PROP_MEMBER_1					7									//会员道具
#define PROP_MEMBER_2					8									//会员道具
#define PROP_MEMBER_3					9									//会员道具
#define PROP_MEMBER_4					10									//会员道具
#define PROP_MEMBER_5					11									//会员道具
#define PROP_MEMBER_6					12									//会员道具
#define PROP_MEMBER_7					13									//会员道具
#define PROP_MEMBER_8					14									//会员道具
BYTE const g_PropTypeList[]=
{
	PROP_DOUBLE	,
	PROP_FOURDOLD,
	PROP_NEGAGIVE,
	PROP_FLEE	,
	PROP_BUGLE	,
	PROP_KICK	,
	PROP_SHIELD	,
	PROP_MEMBER_1,
	PROP_MEMBER_2,
	PROP_MEMBER_3,
	PROP_MEMBER_4,
	PROP_MEMBER_5,
	PROP_MEMBER_6,
	PROP_MEMBER_7,
	PROP_MEMBER_8,
};
#define PROPERTY_COUNT			(sizeof(g_PropTypeList)/sizeof(g_PropTypeList[0]))//道具数目

//道具定义
#define MAX_PROPERTY_COUNT				100									//最大数目
#define BUGLE_MAX_CHAR					120									//喇叭个数
#define BULESSING_MAX_CHAR				100									//祝福个数

//道具描述
struct tagPropertyDescribe
{
	TCHAR							szName[32];							//道具名称
	TCHAR							szDescribe[255];					//道具价值
};

tagPropertyDescribe const g_PropertyDescribe[PROPERTY_COUNT] =
{
	{TEXT("双倍积分卡"),TEXT("拥有了双倍积分卡，玩游戏赢分翻倍，输了不多扣！购买即生效。多次购买时间累加。（注：只能在购买的游戏房间使用。）")},
	{TEXT("四倍积分卡"),TEXT("拥有了四倍积分卡，玩游戏赢分翻四倍，输了不多扣！购买即生效。多次购买时间累加。（注：只能在购买的游戏房间使用。）")},
	{TEXT("负分清零"),TEXT("使用该道具后您的游戏积分将恢复初始状态，不再为负分而烦恼！")},
	{TEXT("清逃跑率"),TEXT("使用该道具后您的逃跑率将恢复初始状态，不再为逃跑而烦恼！")},
	{TEXT("小喇叭"),TEXT("您发送的消息将在房间及房间内的所有游戏桌子上以醒目的方式显示！")},	
	{TEXT("防踢卡"),TEXT("购买该道具后您不再担心被会员踢出游戏桌！")},
	{TEXT("护身符"),TEXT("购买该道具后您输分不扣分，不再为负分而烦恼！（注：只能在购买的游戏房间使用。）")},
	{TEXT("红钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通玩家，有红钻会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示,优先进入人满房间！")},
	{TEXT("蓝钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通、红钻会员，有蓝钻会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示，优先进入人满房间！")},
	{TEXT("黄钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通、红钻、蓝钻玩家，有会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示，优先进入人满房间！")},
	{TEXT("紫钻会员卡"),TEXT("购买礼物/道具时可享受9折优惠，可踢普通、红钻、蓝钻、黄钻玩家，有会员特殊标记，会员期间，名字ID在大厅及房间中红色字体显示，优先进入人满房间！")},

	{TEXT("绿钻会员卡"),TEXT("会员具有大厅帐号红色显示，独特头像，踢人功能，道具购买打折，优先进入人满房间！")},
	{TEXT("黑钻会员卡"),TEXT("会员具有大厅帐号红色显示，独特头像，踢人功能，道具购买打折，优先进入人满房间！")},
	{TEXT("7彩钻会员卡"),TEXT("会员具有大厅帐号红色显示，独特头像，踢人功能，道具购买打折，优先进入人满房间！")},
	{TEXT("保留会员卡"),TEXT("会员具有大厅帐号红色显示，独特头像，踢人功能，道具购买打折，优先进入人满房间！")}
};

//道具信息结构
struct tagPropertyInfo
{
	int							nPropertyID;						//道具ID
	DWORD						dwPropCount1;						//道具数目
	DWORD						dwPropCount2;						//道具数目
	DWORD						dwPropCount3;						//道具数目
	DWORD						dwPropCount4;						//道具数目
	DWORD						dwPropCount5;						//道具数目
	DWORD						dwPropCount6;						//道具数目
	DWORD						dwPropCount7;						//道具数目
	DWORD						dwPropCount8;						//道具数目
	DWORD						dwPropCount9;						//道具数目
	DWORD						dwPropCount10;						//道具数目
	LONG						lPrice1;							//道具价格
	LONG						lPrice2;							//道具价格
	LONG						lPrice3;							//道具价格
	LONG						lPrice4;							//道具价格
	LONG						lPrice5;							//道具价格
	LONG						lPrice6;							//道具价格
	LONG						lPrice7;							//道具价格
	LONG						lPrice8;							//道具价格
	LONG						lPrice9;							//道具价格
	LONG						lPrice10;							//道具价格
	BYTE						cbDiscount;							//会员折扣
	bool						bNullity;							//禁止标识
};

//鲜花信息结构
struct tagFlowerInfo
{
	int							nFlowerID;							//鲜花ID
	LONG						lPrice;								//鲜花价格
	LONG						lSendUserCharm;						//赠送魅力
	LONG						lRcvUserCharm;						//接受魅力
	BYTE						cbDiscount;							//会员折扣
	bool						bNullity;							//禁止标识
};

//////////////////////////////////////////////////////////////////////////////

#endif