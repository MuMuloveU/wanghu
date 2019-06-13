#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//#define MJ_DESPERATE 
//#define _AUTOPALY

//////////////////////////////////////////////////////////////////////////
//状态定义
#define GS_FREE_FAKE		200			//伪free状态 （冲击麻将)

//数值定义
#define MAX_INDEX					35									//最大索引		//LIANG
#define MAX_COUNT					14									//最大数目
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//组合类型
#define ACK_NULL					0x00								//没有类型
#define ACK_LEFT					0x01								//左吃类型
#define ACK_CENTER					0x02								//中吃类型
#define ACK_RIGHT					0x04								//右吃类型
#define ACK_PENG					0x08								//碰牌类型
#define ACK_GANG					0x10								//杠牌类型
#define ACK_CHIHU					0x20								//吃胡类型
//分数定义
#ifdef MJ_DESPERATE

const LONG SCORE_ADD_COMMON				= 6;							//加分 平  胡
const LONG SCORE_ADD_3A2				= 0;							//加分 3 ＋ 2
const LONG SCORE_ADD_7TS				= 5;							//加分 七小对
const LONG SCORE_ADD_13A				= 5;							//加分 13不靠
const LONG SCORE_ADD_W7T				= 7;							//加分 清七对

const LONG SCORE_TIME_ZIMO				= 1;							//加番 自  摸
const LONG SCORE_ADD_TIANDI				= 10;							//加分 天地胡
const LONG SCORE_ADD_KING_WAIT			= 5;							//加分 抛花麻
const LONG SCORE_ADD_GANG_FLOWER		= 5;							//加分 杠上花
const LONG SCORE_ADD_WHOLE_COLOR		= 5;							//加分 清一色
const LONG SCORE_ADD_ALL_ASK			= 5;							//加分 全求人
const LONG SCORE_ADD_7WINDS				= 1;							//加分 七风齐

const LONG SCORE_ADD_NO_KING			= 2;							//加分 无财神
const LONG SCORE_ADD_PER_KING			= 1;							//加分 有财神
const LONG_SCORE_ADD_PER_BANKER			= 2;							//加分 连  庄

const LONG_AFFORD_ADD_ACCEPT3			= 0;							//加分 求  三
const LONG_AFFORD_TIME_ACCEPT3			= 0;							//加番 求  三
const LONG_AFFORD_ADD_GIVE3				= 0;							//加分 供  三
const LONG_AFFORD_TIME_GIVE3			= 0;							//加番 供  三
const LONG_AFFORD_ADD_GANG_APPEND		= 5;							//加分 杠上花
const LONG_AFFORD_TIME_GANG_APPEND		= 1;							//加番 杠上花
const LONG_AFFORD_ADD_HIDE_WIND			= 5;							//加分 藏  风
const LONG_AFFORD_TIME_HIDE_WIND		= 0;							//加番 藏  风

#else

const LONG SCORE_ADD_COMMON				= 1;							//加分 平  胡
const LONG SCORE_ADD_3A2				= 0;							//加分 3 ＋ 2
const LONG SCORE_ADD_7TS				= 2;							//加分 七小对
const LONG SCORE_ADD_13A				= 2;							//加分 13不靠
const LONG SCORE_ADD_W7T				= 4;							//加分 清七对

const LONG SCORE_TIME_ZIMO				= 1;							//加番 自  摸
const LONG SCORE_ADD_TIANDI				= 4;							//加分 天地胡
const LONG SCORE_ADD_KING_WAIT			= 2;							//加分 抛花麻
const LONG SCORE_ADD_GANG_FLOWER		= 2;							//加分 杠上花
const LONG SCORE_ADD_WHOLE_COLOR		= 4;							//加分 清一色
const LONG SCORE_ADD_ALL_ASK			= 2;							//加分 全求人
const LONG SCORE_ADD_7WINDS				= 2;							//加分 七风齐

const LONG SCORE_ADD_NO_KING			= 2;							//加分 无财神
const LONG SCORE_ADD_PER_KING			= 1;							//加分 有财神
const LONG_SCORE_ADD_PER_BANKER			= 2;							//加分 连  庄

const LONG_AFFORD_ADD_ACCEPT3			= 0;							//加分 求  三
const LONG_AFFORD_TIME_ACCEPT3			= 0;							//加番 求  三
const LONG_AFFORD_ADD_GIVE3				= 0;							//加分 供  三
const LONG_AFFORD_TIME_GIVE3			= 1;							//加番 供  三
const LONG_AFFORD_ADD_GANG_APPEND		= 0;							//加分 杠上花
const LONG_AFFORD_TIME_GANG_APPEND		= 0;							//加番 杠上花
const LONG_AFFORD_ADD_HIDE_WIND			= 0;							//加分 藏  风
const LONG_AFFORD_TIME_HIDE_WIND		= 0;							//加番 藏  风

#endif

//包牌
const int MAX_AFFORD_COUNT				= 1;

//////////////////////////////////////////////////////////////////////////


//胡牌牌型
enum EN_HU_CARD_QUEUE_TYPE
{
	EN_HU_CARD_QUEUE_NULL				= 0X0000,							//没有胡
	EN_HU_CARD_QUEUE_3A2				= 0X0001,							//胡 3＋2
	EN_HU_CARD_QUEUE_7TS				= 0X0002,							//胡 7对
	EN_HU_CARD_QUEUE_13A				= 0X0004,							//胡 13离
	EN_HU_CARD_QUEUE_W7T				= 0X0008,							//胡 清7对
	EN_HU_CARD_QUEUE_WW					= 0X0010							//全风向
};
//附加胡
enum EN_HU_ATTACHED_TYPE
{
	EN_HU_ATTACHED_NULL					= 0X0000,						//没有附加胡
	EN_HU_ATTACHED_ZIMO					= 0X0001,						//附加 自摸
	EN_HU_ATTACHED_TIANDI				= 0X0002,						//附加 天地胡
	EN_HU_ATTACHED_KING_WAIT			= 0X0004,						//附加 王牌单吊（抛花麻）
	EN_HU_ATTACHED_GANG_FLOWER			= 0X0008,						//附加 杠上花
	EN_HU_ATTACHED_WHOLE_COLOR			= 0X0010,						//附加 清一色
	EN_HU_ATTACHED_ALL_ASK				= 0X0020,						//附加 全求人
	EN_HU_ATTACHED_7WINDS				= 0X0040,						//附加 七风齐
};

enum EN_AFFORD_TYPE
{
	EN_AFFORD_NULL						= 0,							//无包牌
	EN_AFFORD_ACCEPT3,													//求3包牌
	EN_AFFORD_GIVE3,													//供3包牌
	EN_AFFORD_GANG_APPEND,												//杠补包牌
	EN_AFFORD_HIDE_WIND													//藏风包牌
};
//包牌子项
struct tagAffordItem
{
	BYTE			cbAffordType;
	WORD			wChair;
	tagAffordItem()
	{
		cbAffordType = EN_AFFORD_NULL;
		wChair = INVALID_CHAIR;
	}
};

struct tagHuCourt
{
	BYTE			cbHuCardData;										//胡牌	
	BYTE			cbCardEye;											//牌眼
	BYTE			cbKingCount;										//王牌数目						
	WORD			wHuCardQueueType;									//基本胡牌型
	WORD			wHuAttachedType;									//附加胡
	BYTE			cbBankerCount;										//连庄数目
	WORD			wWinChair;											//赢  者
	WORD			wFuseChair;											//点炮者
	tagAffordItem	AffordItem[MAX_AFFORD_COUNT];						//包牌

	tagHuCourt()
	{
		Reset();
	}
	void Reset()
	{
		cbHuCardData		= 0;
		cbCardEye			= 0;
		cbKingCount			= 0;
		wHuCardQueueType	= EN_HU_CARD_QUEUE_NULL;
		wHuAttachedType		= EN_HU_ATTACHED_NULL;
		cbBankerCount		= 0;
		wWinChair			= INVALID_CHAIR;
		wFuseChair			= INVALID_CHAIR;
		AffordItem[0].wChair = INVALID_CHAIR;
		AffordItem[0].cbAffordType = EN_AFFORD_NULL;
	}
	bool IsHu()
	{
		return (wHuCardQueueType != EN_HU_CARD_QUEUE_NULL);
	}
	bool IsZimo()
	{
		return (IsHu() && (wHuAttachedType & EN_HU_ATTACHED_ZIMO));
	}
};


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
	WORD							wProvideUser;						//供应用户
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	BYTE							cbWeaveKind[4];						//组合类型
	BYTE							cbCenterCard[4];					//中心扑克
};

//杠牌信息
struct tagGangInfo
{
	BYTE							cbGangCardData;						//杠牌
	WORD							wPerformUser;						//杠牌者
	WORD							wProvideUser;						//提供者
	BYTE							cbSendData;							//补牌
	BYTE							cbCurrentOutCount;					//当时出牌数目

	tagGangInfo()
	{
		Reset();
	}
	void Reset()
	{
		cbGangCardData			= 0;
		wPerformUser			= INVALID_CHAIR;
		wProvideUser			= INVALID_CHAIR;
		cbSendData				= 0;	
		cbCurrentOutCount		= 0XFF;
	}
};

//胡牌输入
struct tagHuInput
{
	BYTE*							pCardIndexSeq;						//牌列
	bool							bInsertCurrentCard;					//是否插入判断	
	bool							bJudgeHuOnly;						//只判断胡
	bool							bJudgeKingWait;						//判断抛花麻
	tagWeaveItem*					pWeaveItemSeq;						//组合列
	BYTE							cbWeaveItemCount;					//组合数目
	BYTE							cbCurrentCard;						//当前牌
	tagHuInput()
	{
		pCardIndexSeq		= 0;
		cbWeaveItemCount	= 0;
		bJudgeHuOnly		= true;
		bJudgeKingWait		= false;
		bInsertCurrentCard	= false;
	}
	bool IsValid()
	{
		return (pCardIndexSeq != 0 && pWeaveItemSeq != 0);
	}
};
//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CArrayTemplate<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//常量定义
const BYTE CARD_KING_INDEX			=		MAX_INDEX-1;					//王牌索引
const BYTE CARD_KING_DATA_MASK		=		0x80;							//王牌数据蒙板

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[136];					//扑克数据
	BYTE							m_cbKingCardData;						//王牌

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//混乱扑克
	void RandCardData(BYTE cbCardData[], BYTE bMaxCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE bRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE bCardCount, BYTE cbRemoveCard[], BYTE bRemoveCount);
	//设置王牌
	void SetKingCardData(BYTE cbKingCardData);
	//获取王牌
	BYTE GetKingCardData() { return m_cbKingCardData; }

	//辅助函数
public:
	//有效判断
	bool IsValidCommonCard(BYTE cbCardData);
	//判断王牌
	bool IsKingCardData(BYTE cbCardData);
	bool IsKingCardIndex(BYTE cbCardIndex);
	//扑克数目
	BYTE GetCardCount(BYTE cbCardIndex[MAX_INDEX]);
	//获取组合
	BYTE GetWeaveCardData(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//是否全3
	bool IsNeat3(BYTE cbCardIndex[MAX_INDEX]);
	//是否全2
	bool IsNeat2(BYTE cbCardIndex[MAX_INDEX]);
	//是否全离
	bool IsNeatAlone(BYTE cbCardIndex[MAX_INDEX]);
	//是否风牌
	bool IsWindCard(BYTE cbCardData) { return IsValidCommonCard(cbCardData) && ((cbCardData & MASK_COLOR) == 0X30);}
	
	//被动函数
public:
	//吃牌类型
	BYTE GetEatCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌类型
	BYTE GetPengCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌类型
	BYTE GetGangCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//胡牌信息
	bool GetHuInfo(tagHuInput* pInput, tagHuCourt& HuCourt);
	//计算分数
	bool CGameLogic::CalScore(tagHuCourt& HuCourt, LONG& lGain, LONG& lTimes);

	//主动函数
public:
	//听牌状态
	bool IsTingPaiStatus(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE bItemCount);
	//杠牌信息
	BYTE GetGangCardInfo(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE bItemCount, BYTE cbGangCard[4]);

	//转换函数
public:
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[], BYTE bMaxCount);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE bCardCount, BYTE cbCardIndex[MAX_INDEX]);
	//计算财神
	BYTE GetKingFromBrother(BYTE cbBrotherData);
	//计算财神的弟弟
	BYTE GetBrotherFromKing(BYTE cbKingData);

private:
	//判断胡
	bool _GetHuInfo			(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye);
	bool _GetHuInfo3A2or7TS	(tagHuInput* pInput, int iInserKingtStart, bool bHaveType[4], tagHuCourt& HuCourt);
	bool __GetHuInfo3A2or7TS(tagHuInput* pInput, tagHuCourt& HuCourt);
	//十三不搭
	bool _GetHuInfo13A		(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye);
	//全风向
	bool _GetHuInfoWW		(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye);
	//判断附加胡
	bool _GetHuInfoAttached	(tagHuInput* pInput, tagHuCourt& HuCourt);
	//胡牌详情调整
	void _AdjustHuInfo		(tagHuInput* pInput, tagHuCourt& HuCourt);

};

//////////////////////////////////////////////////////////////////////////

#endif