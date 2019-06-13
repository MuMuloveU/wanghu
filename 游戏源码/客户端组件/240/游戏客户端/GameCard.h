

#pragma once


//消息定义
#define		IDM_LEFT_HITCARD		(WM_USER+70)		//左击扑克
#define		IDM_RIGHT_HITCARD		(WM_USER+75)		//右击扑克

#define IDM_REVERT_CARD		WM_USER+136					//看上一轮的时候还原扑克

//操作掩码
#define		UG_HUA_MASK				0xF0			//花色掩码
#define		UG_VALUE_MASK			0x0F			//数字掩码

#define		HEIGHT_CARD				95
#define		WIDTH_CARD				70


//
//扑克类
//
class  CGameCard : public CWnd
{
	//枚举定义
public:
	enum XShowMode { SWX_MODE_LEFT, SWX_MODE_CENTER, SWX_MODE_RIGHT };
	enum YShowMode { SWY_MODE_TOP,  SWY_MODE_CENTER, SWY_MODE_BOTTOM }; 

	//变量定义
protected:
	int								m_iCardHight;					//扑克高度
	int								m_iCardWidth;					//扑克宽度
	int								m_iCardHSpace;					//扑克间隔
	int								m_iCardVSpace;					//扑克间隔
	int								m_iCardUpSpace;					//跳起高度
	CRgn							m_CardViewRgn;					//扑克区域
	CWnd							* m_pHandleWnd;					//处理窗口

	//信息变量
protected:
	bool							m_bShowCard;					//显示扑克
	bool							m_bHorzLook;					//是否横放
	bool							m_bUseMouse;					//响应鼠标
	BYTE							m_bHotHitCard;					//焦点扑克
	BYTE							m_bCardCount;					//扑克数目
	CPoint							m_PtBasePoint;					//基准坐标
	XShowMode						m_XShowMode;					//显示模式
	YShowMode						m_YShowMode;					//显示模式
	CByteArray						m_CardArray;					//扑克数组
	CByteArray						m_CardUpArray;					//扑克数组

	//图片变量
protected:
	CSkinImage						m_CardPic[2];					//扑克控件

	//函数定义
public:
	//构造函数
	CGameCard();
	//析构函数
	virtual ~CGameCard();

	//设置函数
public:
	//初始化函数
	bool InitGameCard(CWnd * pHandleWnd);
	//设置扑克参数
	void SetCardParameter(int iHSpace, int iVSpace, int iUpSpace);
	//设置基准点函数
	void SetBasePoint(CPoint & BasePoint, XShowMode XShowMode, YShowMode YShowMode);
	//设置显示扑克
	void SetShowCard(bool bShow);
	//设置使用鼠标
	void SetUseMouse(bool bUserMouse);
	//设置显示方式
	void SetLookMode(bool bHorzLook);

	//功能函数
public:
	//设置扑克
	bool SetCard(BYTE bCardList[], BYTE bUpList[], BYTE bCardCount);
	//删除升起的扑克
	BYTE RemoveUpCard();
	//获取升起的扑克
	BYTE GetUpCard(BYTE bCardList[]);
	//获取扑克信息
	BYTE GetCard(BYTE bCardList[], BYTE bUpList[]);

	//内联函数
public:
	//获取扑克数目
	inline BYTE GetCardCount() { return m_bCardCount; }
	//获取扑克数值
	inline BYTE GetCardValoe(BYTE bCard) { 
		BYTE bkk=(bCard&UG_VALUE_MASK)+1;
		if (bkk==14) bkk=1;
		return bkk;  }
	//获取扑克花色
	inline BYTE GetHuaKind(BYTE bCard) { return bCard&UG_HUA_MASK; }
	//获取扑克位置
	inline BYTE GetCardXPos(BYTE bCard) { return GetCardValoe(bCard)-1; };
	//获取扑克位置
	inline BYTE GetCardYPos(BYTE bCard) { return GetHuaKind(bCard)>>4; };

	//内部函数
protected:
	//按键测试
	BYTE HitCardTest(CPoint & Point);
	//调整窗口位置
	void FixCardControl();
	//获取大小函数
	bool GetCardSize(CSize & Size);

	//消息函数
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//重画函数
	afx_msg void OnPaint();
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


