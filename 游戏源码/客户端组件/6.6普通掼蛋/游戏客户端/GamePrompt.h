#pragma once

#include "..\消息定义\GameLogic.h"

///////////////////////////////////////////////////
//结构定义
struct  CardNumber
{
	BYTE           bCardLogicValue;										//扑克大小
	BYTE           bCardValue;											//扑克值
	BYTE           bCardCount;											//扑克数目
	BYTE           bCardLocation;										//扑克位置
};

struct CardType
{
	short           bCardType;											//扑克类型
	BYTE            bCardCount;											//扑克数目
	BYTE            bCardLocation[15];									//扑克位置
	CardType(){ZeroMemory(bCardLocation,sizeof(bCardLocation));}
};
//////////////////////////////////////////////////
//提示类
class CGamePrompt:public CGameLogic
{
public:
	CGamePrompt(void);
	~CGamePrompt(void);

	//出牌变量
public:
    short							m_bTurnCardType;				    //出牌类型
    BYTE                            m_bCardData[28];					//扑克数据
	BYTE                            m_bCardCount;						//扑克数目
	//辅助变量
protected:
    BYTE                            m_bRound;							//不同类型扑克数
	BYTE                            m_bCardPosition[28];				//扑克位置
	BYTE							m_bHongTaoCount;                    //红桃数目
	BYTE                            m_bHongTaoLocation[2];				//红桃位置

	//变量定义
public:
	CArrayTemplate<CardType>		OutCardList;						//出牌列表
	CardNumber					    CardData[20];						//同牌数目
	CardNumber						desiCardData[20];					//同排数目
	
	//操作方法
public:
    //分析提示
	void   Prompt();

	//辅助方法
protected:
	//分析单条
	void  GetSingle();
	//分析对子
	void  GetDouble();
	//分析santiao
	void  GetSanTiao();
	//分析葫芦
	void  GetHulu();
	//分析两联对
	void  GetLiangLianDui();
	//分析顺子
	void GetShunZhi();
	//分析钢板
	void GetGangBan();
	//分析同花顺
	void GetTongHuaShun(BYTE mark=0);
	//分析四炸弹
	void GetSiZhangBomb();
	//分析五炸弹
	void GetWuZhangBomb();
	//分析六炸弹
	void GetLiuZhangBomb();
	//分析七炸弹
	void GetQiZhangBomb();
	//分析八炸弹
	void GetBaZhangBomb();
	//分析九炸弹
	void GetJiuZhangBomb();
	//分析十炸弹
	void GetShiZhangBomb();
	//分析四王
	void GetSiWang();


};
