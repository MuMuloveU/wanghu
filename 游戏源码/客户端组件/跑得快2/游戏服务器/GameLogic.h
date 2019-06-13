#pragma once

//////////////////////////////////////////////////////////////////////////
#include "..\消息定义\CMD_QuickRun.h"

//用于比较大小的数组，跑得快中从大到小排序2，1，K，Q，J，10，....
//                   0, A, 2, 3, 4, 5, 6, 7, 8, 9,10, J, Q, K,King
const int compare[]={0,12,13, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,14};
//用于还原大小的数组
const int uncompare[]={0, 3, 4, 5, 6, 7, 8, 9,10,11,12,13, 1, 2};

//游戏逻辑类
class CGameLogic
{
	//变量定义
private:
	//每个人的牌
	CArrayTemplate<struct tagPisa,struct tagPisa&> m_PisaArray[GAME_PLAYER];
	//保存最后一次出牌的结果
	struct tagSetOutPisa m_LastPisa;
	//是否第一次出牌
	BOOL m_bFirst;
	//出错信息
	TCHAR m_chErrorMsg[100];

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//清除最后的出牌记录
	void EmptyLastCardInfo();
	//是否当前玩家就是最后一次出牌的玩家，即已经轮了一圈了
	BOOL IsUserBegin(int index);
	//当前牌是否比最后出的牌要大
	BOOL IsBiggerThanLastCard(const struct tagSetOutPisa *pisa);
	//当前牌是否合符出牌规则
	BOOL IsRegular(const struct tagSetOutPisa *pisa);
	//获取比较大小的错误信息
	char *GetCompareError();
	//以当前牌更新最后出的牌，返回值为该玩家是否还有牌
	BOOL SaveLastCard(const struct tagSetOutPisa *pisa);
	//开始一个新游戏，返回值是方片3所在的数据索引
	int StartANewGame();
	//获取每个玩家的牌
	void GetInitPisa(int nIndex,struct tagSetInitPisa *pPisa);
	//获取玩家当前剩牌数应乘的倍率
	int GetPlayerLeftCard(int nIndex);
	//内部函数
private:
	//判断牌中是否为炸弹，返回的Count是张数，返回的PisaCount是最大牌值
	BOOL IsBomb(const struct tagSetOutPisa *pisa,int *Count,struct tagPisa *PisaPoint);
	//判断牌中是否为连对，返回的PisaCount是最大牌值
	BOOL IsContinue2(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint);
	//判断牌中是否为连三同张，返回的PisaCount是最大牌值
	BOOL IsContinue3(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint);
	//判断牌中是否为顺子，返回的PisaCount是最大牌值，bSameColor表示是否为同花顺
	BOOL IsStraight(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint,BOOL *bSameColor);
	//判断牌中是否为三连二，返回的PisaCount是最大牌值
	BOOL Is3And2(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint);
	//判断牌中是否为蝴蝶牌型，返回的PisaCount是最大牌值
	BOOL IsButterfly(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint);
	//公用函数
	BOOL _IsContinue(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint,int offest);
	//是否同时含有3和K
	BOOL IsBothHave3AndK(const struct tagSetOutPisa *pisa);
	//获取最大的牌面值，bEnd表示是否通过最后回环的
	void GetMaxPoint(const struct tagSetOutPisa *pisa,struct tagPisa *PisaPoint);
	//判断两个值是否相邻
	BOOL IsNeighbor(int index,int index2);
};
