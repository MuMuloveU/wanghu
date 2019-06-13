#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//常量定义

#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					42									//最大索引
#define MAX_COUNT					14									//最大数目
#define MAX_REPERTORY				144									//最大库存

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER					0x02								//中吃类型
#define WIK_RIGHT					0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG					0x10								//杠牌类型
#define WIK_CHI_HU					0x20								//吃胡类型


//////////////////////////////////////////////////////////////////////////
//胡牌定义

//非胡类型
#define CHK_NULL					0x0000								//非胡类型

//胡牌牌型
#define CHK_JI_HU					0x0001								//鸡胡类型

//胡牌权位
#define CHR_NULL					0x0000								//无权位
#define CHR_HUN_YI_SE				0x0001								//混一色
#define CHR_ZI_YI_SE				0x0002								//字一色	
#define CHR_PENG_PENG				0x0004								//碰碰胡	
#define CHR_ZI_MO					0x0008								//自摸	
#define CHR_QING_YI_SE				0x0010								//清色权位

//胡牌类型  算台
const BYTE KIND_JI_HU				= 0;								//鸡胡

//胡牌权位 算台
const BYTE RIGHT_HUN_YI_SE			= 2;								//混一色
const BYTE RIGHT_ZI_YI_SE			= 3;								//字一色
const BYTE RIGHT_PENG_PENG			= 2;								//碰碰胡
const BYTE RIGHT_QING_YI_SE			= 4;								//清一色
//////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbCardIndex[3];						//扑克索引
};

//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//胡牌结果
struct tagChiHuResult
{
	WORD							wChiHuKind;							//吃胡类型
	WORD							wChiHuRight;						//胡牌权位
	BYTE							cbHuCount;							//胡牌胡数	
	BYTE							cbHuTimes;							//胡牌番数
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[4];						//扑克数据
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	BYTE							cbWeaveKind[4];						//组合类型
	BYTE							cbCenterCard[4];					//中心扑克
	BYTE							cbCardData[4][4];					//扑克数据
};


//////////////////////////////////////////////////////////////////////////
//机器人相关

//出牌结果
struct tagOutCardResult
{
	BYTE							cbOperateCode;						//出牌代码
	BYTE							cbOperateCard;						//出牌值
};

//听牌分析结果
struct tagTingCardResult
{
	BYTE bAbandonCount;													//可丢弃牌数
	BYTE bAbandonCard[MAX_COUNT];										//可丢弃牌
	BYTE bTingCardCount[MAX_COUNT];										//听牌数目
	BYTE bTingCard[MAX_COUNT][MAX_COUNT-1];								//听牌
	BYTE bRemainCount[MAX_COUNT];										//听的牌的剩余数
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CArrayTemplate<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//混乱扑克
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//排序,根据牌值排序
	bool SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount );
	//辅助函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(BYTE cbCardIndex[MAX_INDEX]);
	//组合扑克
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);

	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);
	//胡牌等级
	BYTE GetChiHuActionRank(tagChiHuResult & ChiHuResult);

	//动作判断
public:
	//吃牌判断
	BYTE EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	BYTE EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, tagGangCardResult & GangCardResult);
	//吃胡分析
	BYTE AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult,WORD wBankerUser,bool bJugeHuOnly,bool bZimo = false);

	//特殊胡牌
public:
	//清一色牌
	bool IsQingYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);
	//混一色牌
	bool IsHunYiSe(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount);
	//字一色牌
	bool IsZiYiSe(tagAnalyseItem & AnalyseItem);

	//转换函数
public:
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT],BYTE bMaxCount);
	
	//算台函数
public:
	//计算分数
	LONG  CalScore(tagChiHuResult & ChiHuResult,LONG &lTimes);


	//内部函数
private:
	//分析扑克
	bool AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);

	//机器人相关
protected:
	//分析单牌
	BYTE AnalyseSingleCount( BYTE cbCardIndex[MAX_INDEX] );
	//分析废弃牌
	bool AnalyseTrushCard( BYTE cbCardIndex[MAX_INDEX],BYTE bTrushCardIndex[MAX_INDEX] );
public:
	//听牌分析
	bool AnalyseTingCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, WORD wChiHuRight);
	//返回废弃的牌
	bool SearchTingCard(WORD wMeChairId,BYTE (&cbCardIndex)[GAME_PLAYER][MAX_INDEX],tagWeaveItem (&cWeaveItem)[GAME_PLAYER][4],BYTE cbItemCount[4],
		BYTE (&cbDiscardCard)[GAME_PLAYER][55],BYTE cbDiscardCount[GAME_PLAYER],tagTingCardResult &TingCardResult);
	//搜索出牌
	bool SearchOutCard( WORD wMeChairId,WORD wCurrentUser,BYTE (&cbCardIndex)[GAME_PLAYER][MAX_INDEX],tagWeaveItem (&cWeaveItem)[GAME_PLAYER][4],
		BYTE cbItemCount[4],BYTE (&cbDiscardCard)[GAME_PLAYER][55],BYTE cbDiscardCount[GAME_PLAYER],BYTE cbActionMask,BYTE cbActionCard,const BYTE cbEnjoinCard[MAX_COUNT],BYTE cbEnjoinCount,tagOutCardResult &OutCardResult );

};

//////////////////////////////////////////////////////////////////////////

#endif