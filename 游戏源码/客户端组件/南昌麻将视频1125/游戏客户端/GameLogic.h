#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//常量定义

#define MAX_WEAVE					4									//最大组合
#define MAX_INDEX					34									//最大索引			
#define MAX_REPERTORY				136									//最大库存

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作标志
#define WIK_NULL					0x0000								//没有类型
#define WIK_LEFT					0x0001								//左吃类型
#define WIK_CENTER					0x0002								//中吃类型
#define WIK_RIGHT					0x0004								//右吃类型
#define WIK_PENG					0x0008								//碰牌类型
#define WIK_GANG					0x0010								//杠牌类型
#define WIK_CHI_HU					0x0020								//吃胡类型
#define WIK_DNBL					0x0040								//东南北左
#define WIK_DNBC					0x0080								//东南北中
#define WIK_DNBR					0x0100								//东南北右
#define WIK_DXBL					0x0200								//东西北左	
#define WIK_DXBC					0x0400								//东西北中	
#define WIK_DXBR					0x0800								//东西北右	
//////////////////////////////////////////////////////////////////////////
//胡牌定义

//非胡类型
#define CHK_NULL					0x0000								//非胡类型

//胡牌类型
#define CHK_JI_HU					0x0001								//基本胡
#define CHK_QI_DUI					0x0002								//七对
#define CHK_THIRTEEN				0x0004								//13乱
#define CHK_SERVEN					0x0008								//七星
#define CHK_GERMAN_SERVEN			0x0010								//德国七星
#define CHK_PENG_PENG				0x0020								//大七对（碰碰胡）
//胡牌权位
#define CHR_ZI_MO					0x0001								//自摸权位
#define CHR_QIANG_GANG				0x0002								//抢杠权位
#define CHR_GANG_FLOWER				0x0004								//杠上开花
#define CHR_KING_WAIT				0x0008								//精吊权位
#define CHR_GERMAN					0x0010								//德国权位
#define CHR_GERMAN_GERMAN			0x0020								//德中德	
#define CHR_TIAN					0x0040								//天胡权位	
#define CHR_DI						0x0080								//地胡权位
//////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	WORD							wWeaveKind;							//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbCardIndex[3];						//扑克索引
};

//组合子项
struct tagWeaveItem
{
	WORD							wWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
};

//胡牌结果
struct tagChiHuResult
{
	WORD							wChiHuKind;							//吃胡类型
	WORD							wChiHuRight;						//胡牌权位
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
	BYTE							cbEyeKingReplace;					//精牌替换
	BYTE							cbWeaveKingReplace;					//精牌替换	
	BYTE							cbCardEye;							//牌眼扑克
	WORD							wWeaveKind[MAX_WEAVE];				//组合类型
	BYTE							cbCenterCard[MAX_WEAVE];			//中心扑克
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CArrayTemplate<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;


//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据
	BYTE							m_cbKingCardIndex[2];				//王牌

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
	//设置精牌
	void SetKingCardIndex(BYTE cbKingCardIndex[2]);
	//获取精牌
	BYTE *GetKingCardIndex(BYTE cbKingCardIndex[2]);

	//辅助函数
public:
	//判断正精
	bool IsKingCardData(BYTE cbCardData);
	//判断正精
	bool IsKingCardIndex(BYTE cbCardIndex);
	//判断副精
	bool IsViceKingCardData(BYTE cbCardData);
	//判断副精
	bool IsViceKingCardIndex(BYTE cbCardIndex);
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(BYTE cbCardIndex[MAX_INDEX]);
	//组合扑克
	BYTE GetWeaveCard(WORD wWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//是否精吊
	bool IsKingWait(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbCurrentCard,bool bZimo);

	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(WORD wUserAction);
	//胡牌等级
	BYTE GetChiHuActionRank(tagChiHuResult & ChiHuResult);

	//动作判断
public:
	//吃牌判断
	WORD EstimateEatCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	WORD EstimatePengCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	WORD EstimateGangCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	WORD AnalyseGangCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//吃胡分析
	WORD AnalyseChiHuCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, WORD wChiHuRight, tagChiHuResult & ChiHuResult,bool bZimo=false);

	//转换函数
public:
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT],BYTE bMaxCount);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
	//计算积分
	LONG CalScore(tagChiHuResult & ChihuResult,LONG &lScore,LONG &lTimes);
	//计算副精
	BYTE GetKingFromBrother(BYTE cbBrotherData);

	//内部函数
private:
	//分析扑克
	bool AnalyseCard(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray,BYTE cbCurrentCard,bool bZimo);
	//7星
	bool ServenStar(BYTE cbCardIndex[MAX_INDEX],BYTE &cbGerman,BYTE cbCurrentCard,bool bZimo);
	//七小对牌
	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbWeaveCount,const BYTE cbCurrentCard,BYTE &cbGerman,bool bZimo);
	//3顺 3刻
	bool IsNeat3(const BYTE cbCardIndex[MAX_INDEX],BYTE &cbGerman);
	// 对子 豪华对子
	bool IsNeat2(BYTE cbCardIndex[MAX_INDEX],tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//全分离
	bool IsNeatAlone(BYTE cbCardIndex[MAX_INDEX],BYTE &cbGerman,BYTE cbCurrentCard,bool bZimo);

};

//////////////////////////////////////////////////////////////////////////

#endif