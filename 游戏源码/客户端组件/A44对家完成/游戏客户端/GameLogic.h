#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序

//////////////////////////////////////////////////////////////////////////
//数目定义

#define MAX_COUNT					14									//最大数目
#define FULL_COUNT					54									//全牌数目
#define BACK_COUNT					4									//底牌数目
#define NORMAL_COUNT				14									//常规数目

//////////////////////////////////////////////////////////////////////////

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_SINGLE_LINE				3									//单连类型

#define CT_THREE					4									//三条类型
#define CT_DOUBLE_LINE				5									//对连类型

#define CT_FOUR						6									//四条类型
#define CT_MISSILE_CARD				7									//火箭类型
#define	CT_A44_red_black			8									//混色A44
#define	CT_A44_black				9									//黑色A44
#define	CT_A44_red					10									//红色A44
//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbDoubleCount;						//两张数目
	BYTE							cbSignedCount;						//单张数目
	BYTE							cbFourCardData[MAX_COUNT];			//四张扑克
	BYTE							cbThreeCardData[MAX_COUNT];			//三张扑克
	BYTE							cbDoubleCardData[MAX_COUNT];		//两张扑克
	BYTE							cbSignedCardData[MAX_COUNT];		//单张扑克
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//逻辑函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//差比扑克
	bool CompareChaCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//点比扑克
	bool CompareDianCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult);

	//内部函数
public:
	//分析扑克
	void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);

public:
	//是否单牌1
	inline BOOL IsOnlyOne(const BYTE iCardList[], int iCardCount) { return iCardCount==1; };
	//是否对牌2
	BOOL IsDouble(const BYTE iCardList[], int iCardCount);
	//是否三条3
	BOOL IsThree(const BYTE iCardList[], int iCardCount);
	//是否单顺4
	BOOL IsSingleContinue(const BYTE iCardList[], int iCardCount);
	//是否双顺5
	BOOL IsDoubleContinue(const BYTE iCardList[], int iCardCount);
	//是否炸弹4张6
	BOOL IsBomb(const BYTE iCardList[], int iCardCount);
	//是否双王7
	BOOL IsDoubleKong(const BYTE iCardList[], int iCardCount);
	//是否44毙8910
	BOOL Is44A(const BYTE iCardList[], int iCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif