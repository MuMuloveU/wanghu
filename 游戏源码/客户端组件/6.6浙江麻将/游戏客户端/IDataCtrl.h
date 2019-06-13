#ifndef _IDATACTRL_H_
#define _IDATACTRL_H_

class IDataCtrl
{
public:
	virtual BYTE	GetGameStatus()						= 0;		//获取游戏状态
	virtual BYTE	GetKingCardData()					= 0;		//获取王牌
	virtual BYTE	GetKingBrotherData()				= 0;		//获取财神的弟弟
	virtual WORD	GetBankerUser()						= 0;		//获取庄家
	virtual bool	IsUserReady(WORD wChairID)			= 0;		//是否准备
	virtual bool	IsConsign()							= 0;		//是否托管
	virtual LONG    GetScoreLeft(WORD wChairID)			= 0;		//获取剩余财富
	virtual LONG	GetCellScore()						= 0;		//获取底注

	virtual bool	IsLookOn()							= 0;		//是否旁观者
	virtual bool	IsAllowLookOn()						= 0;		//是否允许旁观
	virtual bool	IsEnableLookOn()					= 0;		//是否可以旁观
	virtual bool	IsEnableSound()						= 0;		//是否允许声效

};
#endif