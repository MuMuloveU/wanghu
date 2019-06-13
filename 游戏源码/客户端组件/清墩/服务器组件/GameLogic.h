#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_VALUE					2									//数值排序
#define ST_COLOR					3									//花色排序
#define ST_CUSTOM					4									//自定排序
#define ST_QINGDUN					5									//清敦排序
#define ST_TONGHUA					6									//同花排序

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[12];					//扑克数目
	BYTE							cbCardData[12][MAX_COUNT];			//扑克数据
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//分布信息
struct tagDistributing
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbDistributing[15][6];				//分布信息
};

struct tagMagicResult
{
	BYTE							cbMagicCardCount;					//魔术牌数目
	BYTE							cbNormalCardCount;					//常规牌数目
	BYTE							cbMagicCard[2];						//魔术牌数据
	BYTE							cbNormalCard[MAX_COUNT];			//常规牌数据
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//逻辑变量
protected:
	BYTE							m_cbMainValue;						//主牌数值
	bool m_bFirst2;

	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//信息函数
public:
	//主牌数值
	BYTE GetMainValue() { return m_cbMainValue; }
	//主牌数值
	VOID SetMainValue(BYTE cbMainValue) { m_cbMainValue=cbMainValue; }

	//属性函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取花色
	BYTE GetCardColor(const BYTE cbCardData[], BYTE cbCardCount);
	//获取积分
	WORD GetCardScore(const BYTE cbCardData[], BYTE cbCardCount);

	//控制函数
public:
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	void SetFirst2(bool bFirst2);
	//逻辑函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//对比扑克
	bool CompareCard( const BYTE cbFirstCard,const BYTE cbNextCard );
	//对比点树
	bool CompareCardMaxValue( const BYTE cbFirstCard[],const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount );
	//功能函数
public:
	//分析扑克
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//分析分布
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);
	//变幻扑克
	bool MagicCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT], BYTE cbCardType);
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult);

	//内部函数
public:
	//构造扑克
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//是否连牌
	bool IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount);
	//对比扑克
	bool CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);

	//搜索牌型
protected:
	//搜索墩牌
	bool SearchDong( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult );
	//搜索排炮
	bool SearchPaiPao( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult );
	//搜索八无墩
	bool SearchBomb8( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult );
	//搜索连墩
	bool SearchLianDun( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult );
	//搜索兄弟墩
	bool SearchXiongdiDun( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[], BYTE cbTurnCardCount,tagOutCardResult &OutCardResult );
	//搜索同花顺
	bool SearchTongHuaShun( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult );
	//搜索连牌
	bool SearchLinkCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,BYTE cbCardType,tagOutCardResult &OutCardResult ,BYTE TurnCardCount);
	//搜索葫芦
	bool SearchHuLu( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult ,BYTE TurnCardCount );
	//搜索蝴蝶
	bool SearchHuDie( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult ,BYTE TurnCardCount );
	//搜索天王
	bool SearchTianWang( const BYTE cbHandCardData[],BYTE cbHandCardCount,tagOutCardResult &OutCardResult );
	//按花色分析扑克
	//VOID AnalyseColorData( const BYTE cbCardData[],BYTE cbCardCount,tagColorResult &ColorResult );
	//按变幻牌分析扑克
	VOID AnalyseMagicData( const BYTE cbHandCardData[],BYTE cbHandCardCount,tagMagicResult &MagicResult );
	//变幻搜索出牌
	bool SearchMagicOutCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbTurnCardData[],BYTE cbTurnCardCount,tagOutCardResult &OutCardResult );
	//变幻搜索相同牌
	bool SearchMagicSameCard( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,BYTE cbLessSameCount,tagOutCardResult &OutCardResult );
	//变幻搜索连牌
	bool SearchMagicLink( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,BYTE cbCardType,tagOutCardResult &OutCardResult );
	//变幻搜索同花顺
	bool SearchMagicTongHuaShun( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult );
	//变幻搜索葫芦
	bool SearchMagicHuLu( const BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbReferCard,tagOutCardResult &OutCardResult );
	
};

//////////////////////////////////////////////////////////////////////////

#endif