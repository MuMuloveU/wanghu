#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define CT_THREE_TIAO				3									//三条类型
#define	CT_SHUN_ZI					4									//顺子类型
#define CT_HU_LU					5									//葫芦类型
#define CT_GANG_BAN                 6                                   //钢板类型
#define CT_LIANG_LIAN_DUI           7                                   //两连对类型
#define CT_SI_ZHANG_BOMB            8                                   //四张炸弹
#define CT_WU_ZHANG_BOMB            9                                   //五张炸弹
#define CT_TONG_HUA_SHUN			10									//同花顺型
#define CT_LIU_ZHANG_BOMB           11                                  //六张炸弹
#define CT_QI_ZHANG_BOMB            12                                  //七张炸弹
#define CT_BA_ZHANG_BOMB            13                                  //八张炸弹
#define CT_JIU_ZHANG_BOMB           14                                  //九张炸弹
#define CT_SHI_ZHANG_BOMB           15                                  //十张炸弹
#define CT_FOUR_KING                16                                  //四王类型

//宏定义
#define MAX_COUNT					54									//最大数目
#define MAX_ROUND                   50                                  //最大轮数

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//有元定义
	friend class CGamePrompt;
	
	//变量定义
protected:
	static const BYTE				m_bCardListData[108];				//扑克数据
	BYTE                            m_bValue;                           //排型数值
public:
	static BYTE                     m_bSeries;                          //游戏级数
	
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	short GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//按花色排
	void SortByColor(BYTE  cbCardData[], BYTE cbCardCount);
	//按炸弹排
	void SortByBomb	(BYTE  cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
    //添加扑克
	bool Add(BYTE bAddCard, BYTE  cbCardData[],BYTE cbCardCount);
	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);

	//类型函数
public:
	//炸弹值计算
	int  BombCount(BYTE cbCardData[],BYTE cbCardCount);
	//是否对子
	bool IsDouble(BYTE cbCardData[], BYTE cbCardCount);
	//是否葫芦
	bool IsHuLu(BYTE cbCardData[], BYTE cbCardCount);
	//是否顺子
	bool IsShunZhi(BYTE cbCardData[], BYTE cbCardCount);
	//是否三条
	bool IsSanTiao(BYTE cbCardData[], BYTE cbCardCount);
	//是否四张炸弹
	bool IsSiZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否五张炸弹
	bool IsWuZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否同花顺
	bool IsTongHuaShun(BYTE cbCardData[], BYTE cbCardCount);
	//是否六张炸弹
	bool IsLiuZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否七张炸弹
	bool IsQiZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否八张炸弹
	bool IsBaZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否九张炸弹
	bool IsJiuZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否十张炸弹
	bool IsShiZhangBomb(BYTE cbCardData[], BYTE cbCardCount);
	//是否四王
	bool IsFourKing(BYTE cbCardData[], BYTE cbCardCount);
	//是否钢板
	bool IsGangBan(BYTE cbCardData[], BYTE cbCardCount);
	//是否两连队
	bool IsLiangLianDui(BYTE cbCardData[], BYTE cbCardCount);
    
	//功能函数
public:
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bNextList[], BYTE cbFirstCount, BYTE cbNextCount);
	//出牌判断
	bool SearchOutCard(BYTE cbCardData[], BYTE cbCardCount, BYTE bTurnCardData[], BYTE cbTurnCardCount, BYTE cbTurnOutType, BYTE cbOutCard[MAX_COUNT]);
	
};

//////////////////////////////////////////////////////////////////////////

#endif