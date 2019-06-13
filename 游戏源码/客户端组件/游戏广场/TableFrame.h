#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "UserManager.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define NOTE_LEN						64									//桌子备注

//接口说明
class CTableResource;
interface ITableFrameView;

//////////////////////////////////////////////////////////////////////////

//游戏桌子属性
struct tagTableState
{
	bool								bPass;								//密码标志
	bool								bPlaying;							//游戏标志
	WORD								wTableID;							//桌子号码
	WORD								wWatchCount;						//旁观数目
	WORD								wChairCount;						//玩家数目
	DWORD								dwTableMasterID;					//台主玩家
	TCHAR								szTableNote[NOTE_LEN];				//桌子备注
	IUserItem							* pIUserItem[MAX_CHAIR];			//用户信息
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableView INTERFACE_VERSION(1,1)
static const GUID IID_ITableView={0xd10c8db6,0xb9d8,0x47c2,0x9c,0xb9,0x34,0x97,0x59,0xa1,0x87,0x77};

//桌子接口
interface ITableView : IUnknownEx
{
	//初始化函数
	virtual void __cdecl InitTableView(WORD wTableID, WORD wChairCount, ITableFrameView * pITableFrameView)=NULL;
	//设置用户信息
	virtual bool __cdecl SetUserInfo(WORD wChairID, IUserItem * pIUserItem)=NULL;
	//设置鼠标按下
	virtual void __cdecl SetMouseDown(bool bMouseDown)=NULL;
	//设置焦点框架
	virtual void __cdecl SetFocusFrame(bool bFocusFrame)=NULL;
	//设置盘旋位置
	virtual void __cdecl SetHoverChairID(WORD wHoverChairID)=NULL;
	//设置游戏标志 
	virtual void __cdecl SetPlayFlag(bool bPlaying)=NULL;
	//设置密码标志
	virtual void __cdecl SetPassFlag(bool bPass)=NULL;
	//设置备注
	virtual void __cdecl SetTableNote(LPCTSTR pszNote)=NULL;
	//获取用户信息
	virtual IUserItem * __cdecl GetUserInfo(WORD wChairID)=NULL;
	//获取空椅子数
	virtual WORD __cdecl GetNullChairCount(WORD & wNullChairID)=NULL;
	//查询焦点框架
	virtual bool __cdecl QueryFocusFrame()=NULL;
	//查询盘旋位置
	virtual WORD __cdecl QueryHoverChairID()=NULL;
	//查询游戏标志
	virtual bool __cdecl QueryPlayFlag()=NULL;
	//查询密码标志
	virtual bool __cdecl QueryPassFlag()=NULL;
	//获取桌子属性
	virtual const tagTableState * GetTableAttrib()=NULL;
	//绘画函数
	virtual void __cdecl DrawTable(CDC * pDC, bool bHideInfo, CTableResource * pTableResource, bool bShowUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameView INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameView={0x719fbd24,0x5b49,0x4714,0x95,0x16,0xca,0xd5,0xf1,0x52,0xe7,0x40};

//框架接口
interface ITableFrameView : public IUnknownEx
{
	//创建函数
	virtual bool __cdecl CreateTableFrame(CWnd * pParentWnd, UINT uWndID)=NULL;
	//配置函数
	virtual bool __cdecl InitTableFrame(WORD wTableCount, WORD wPlayerCount, bool bHideInfo, const tagGameKind * pGameKind, IUnknownEx * pIUnknownEx)=NULL;
	//销毁函数
	virtual void __cdecl DestroyTableFrame()=NULL;
	//更新桌子
	virtual void __cdecl UpdateTableView(WORD wTableID, bool bMustUpdate)=NULL;
	//获取游戏桌指针
	virtual ITableView * __cdecl GetTableArrayPtr(WORD wTableID)=NULL;
	//设置显示
	virtual bool __cdecl ShowUserInfo(bool bShowUser)=NULL;
	//设置视图桌子
	virtual bool __cdecl EnsureVisibleTable(WORD wTableID)=NULL;
	//闪动椅子
	virtual bool __cdecl FlashTable(WORD wTableID)=NULL;
	//闪动椅子
	virtual bool __cdecl FlashChair(WORD wTableID, WORD wChairID)=NULL;
	//获取桌子数目
	virtual WORD __cdecl GetTableCount()=NULL;
	//获取椅子数目
	virtual WORD __cdecl GetChairCount()=NULL;
	//设置用户信息
	virtual bool __cdecl SetUserInfo(WORD wTableID, WORD wChairID, IUserItem * pIUserItem)=NULL;
	//设置游戏标志 
	virtual void __cdecl SetPlayFlag(WORD wTableID, bool bPlaying)=NULL;
	//设置密码标志
	virtual void __cdecl SetPassFlag(WORD wTableID, bool bPass)=NULL;
	//获取用户信息
	virtual IUserItem * __cdecl GetUserInfo(WORD wTableID, WORD wChairID)=NULL;
	//获取空椅子数
	virtual WORD __cdecl GetNullChairCount(WORD wTableID, WORD & wNullChairID)=NULL;
	//查询游戏标志
	virtual bool __cdecl QueryPlayFlag(WORD wTableID)=NULL;
	//查询密码标志
	virtual bool __cdecl QueryPassFlag(WORD wTableID)=NULL;
	//获取桌子属性
	virtual const tagTableState * GetTableAttrib(WORD wTableID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameSink={0xc06ff71d,0x2c2,0x4e51,0x9d,0xe6,0x22,0x43,0x85,0x7c,0x16,0xda};

//框架回调接口
interface ITableFrameSink : public IUnknownEx
{
	//鼠标左键按下
	virtual void __cdecl OnLButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)=NULL;
	//鼠标右键按下
	virtual void __cdecl OnRButtonHitTable(WORD wTableID, WORD wChairID, bool bMirror)=NULL;
	//鼠标双击
	virtual void __cdecl OnDoubleHitTable(WORD wTableID, bool bMirror)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子资源类
class CTableResource
{
	friend class CTableFrame;

	//变量定义
public:
	WORD								m_wWidth;							//桌子宽度
	WORD								m_wHeight;							//桌子高度
	COLORREF							m_crBackGround;						//背景颜色

	//位置变量
public:
	CPoint								m_ptLock;							//锁位置
	CRect								m_rcNote;							//备注位置
	CRect								m_rcTableID;						//桌号位置
	CPoint								m_ptReady[MAX_CHAIR];				//准备位置
	CRect								m_rcName[MAX_CHAIR];				//名字位置
	CRect								m_rcChair[MAX_CHAIR];				//椅子位置

	//颜色参数
public:
	COLORREF							m_crName;							//名字颜色
	COLORREF							m_crNote;							//备注颜色
	COLORREF							m_crTableID;						//号码颜色
	int									m_nDrawStyle[MAX_CHAIR];			//输出格式

	//标志变量
public:
	bool								m_bDZShowHand;						//德州扑克

	//位图资源
public:
	CSkinImage							m_ImageLock;						//桌锁位图
	CSkinImage							m_ImagePlay;						//游戏位图
	CSkinImage							m_ImageReady;						//准备位图
	CSkinImage							m_ImageTable;						//桌子位图
	CSkinImage							m_ImagePeople;						//用户位图

	//函数定义
protected:
	//构造函数
	CTableResource();
	//析构函数
	virtual ~CTableResource();

	//功能函数
protected:
	//加载资源
	bool LoadFromFiles(LPCTSTR pszGameName);
	//卸载资源
	void DestroyResource();
};

//////////////////////////////////////////////////////////////////////////

//游戏桌基础类
class CTableView : public ITableView
{
	//变量定义
protected:
	bool								m_bMouseDown;						//是否按下
	bool								m_bFocusFrame;						//框架标志
	WORD								m_wHoverChairID;					//盘旋椅子
	tagTableState						m_TableState;						//桌子属性
	ITableFrameView						* m_pITableFrameView;				//框架接口

	//函数定义
public:
	//构造函数
	CTableView();
	//析构函数
	virtual ~CTableView();

	//基础函数
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//设置接口
public:
	//初始化函数
	virtual void __cdecl InitTableView(WORD wTableID, WORD wChairCount, ITableFrameView * pITableFrameView);
	//设置用户信息
	virtual bool __cdecl SetUserInfo(WORD wChairID, IUserItem * pIUserItem);
	//设置鼠标按下
	virtual void __cdecl SetMouseDown(bool bMouseDown);
	//设置焦点框架
	virtual void __cdecl SetFocusFrame(bool bFocusFrame);
	//设置盘旋位置
	virtual void __cdecl SetHoverChairID(WORD wHoverChairID);
	//设置游戏标志 
	virtual void __cdecl SetPlayFlag(bool bPlaying);
	//设置密码标志
	virtual void __cdecl SetPassFlag(bool bPass);
	//设置备注
	virtual void __cdecl SetTableNote(LPCTSTR pszNote);

	//查询接口
public:
	//获取用户信息
	virtual IUserItem * __cdecl GetUserInfo(WORD wChairID);
	//获取空椅子数
	virtual WORD __cdecl GetNullChairCount(WORD & wNullChairID);
	//查询焦点框架
	virtual bool __cdecl QueryFocusFrame() { return m_bFocusFrame; }
	//查询盘旋位置
	virtual WORD __cdecl QueryHoverChairID() { return m_wHoverChairID; }
	//查询游戏标志
	virtual bool __cdecl QueryPlayFlag() { return m_TableState.bPlaying; }
	//查询密码标志
	virtual bool __cdecl QueryPassFlag() { return m_TableState.bPass; }
	//获取桌子属性
	virtual const tagTableState * GetTableAttrib() { return &m_TableState; }

	//绘画接口
public:
	//绘画函数
	virtual void __cdecl DrawTable(CDC * pDC, bool bHideInfo, CTableResource * pTableResource, bool bShowUser);
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子框架
class CTableFrame : public CWnd, public ITableFrameView
{
	//状态信息
protected:
	WORD								m_wDownChairID;						//位置号码
	WORD								m_wDownTableID;						//按下号码
	WORD								m_wFrameTableID;					//边框号码
	bool								m_bLeftMouseDown;					//鼠标按下
	bool								m_bRightMouseDown;					//鼠标按下

	//设置变量
protected:
	bool								m_bShowUser;						//是否显示
	bool								m_bHideInfo;						//隐藏信息
	WORD								m_wTableCount;						//游戏桌数
	WORD								m_wChairCount;						//椅子数目

	//滚动信息
protected:
	int									m_iXExcursion;						//横行偏移
	int									m_iCurrentPos;						//当前位置
	int									m_iMaxScrollPos;					//最大位置
	int									m_iCountRow;						//桌子列数
	int									m_iCountAllLine;					//桌子总行
	int									m_iCountVisibleLine;				//可见行数

	//控件变量
protected:
	CTableResource						m_TableResource;					//桌子资源
	CTableView							* m_pTableViewArray;				//桌子数组
	ITableFrameSink						* m_pITableFrameSink;			//回调接口

	//配置参数
protected:
	static int							m_iScrollPos;						//滚动象素
	static int							m_iWhellTimes;						//滚轮倍数

	//函数定义
public:
	//构造函数
	CTableFrame();
	//析构函数
	virtual ~CTableFrame();

	//重载消息
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { delete this; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//配置接口
public:
	//创建函数
	virtual bool __cdecl CreateTableFrame(CWnd * pParentWnd, UINT uWndID);
	//配置函数
	virtual bool __cdecl InitTableFrame(WORD wTableCount, WORD wChairCount, bool bHideInfo, const tagGameKind * pGameKind, IUnknownEx * pIUnknownEx);
	//销毁函数
	virtual void __cdecl DestroyTableFrame();

	//功能接口
public:
	//更新桌子
	virtual void __cdecl UpdateTableView(WORD wTableID, bool bMustUpdate);
	//获取游戏桌指针
	virtual ITableView * __cdecl GetTableArrayPtr(WORD wTableID);
	//设置显示
	virtual bool __cdecl ShowUserInfo(bool bShowUser);
	//设置视图桌子
	virtual bool __cdecl EnsureVisibleTable(WORD wTableID);
	//闪动椅子
	virtual bool __cdecl FlashTable(WORD wTableID);
	//闪动椅子
	virtual bool __cdecl FlashChair(WORD wTableID, WORD wChairID);

	//信息接口
public:
	//获取桌子数目
	virtual WORD __cdecl GetTableCount() { return m_wTableCount; }
	//获取椅子数目
	virtual WORD __cdecl GetChairCount() { return m_wChairCount; }
	//设置用户信息
	virtual bool __cdecl SetUserInfo(WORD wTableID, WORD wChairID, IUserItem * pIUserItem);
	//设置游戏标志 
	virtual void __cdecl SetPlayFlag(WORD wTableID, bool bPlaying);
	//设置密码标志
	virtual void __cdecl SetPassFlag(WORD wTableID, bool bPass);
	//获取用户信息
	virtual IUserItem * __cdecl GetUserInfo(WORD wTableID, WORD wChairID);
	//获取空椅子数
	virtual WORD __cdecl GetNullChairCount(WORD wTableID, WORD & wNullChairID);
	//查询游戏标志
	virtual bool __cdecl QueryPlayFlag(WORD wTableID);
	//查询密码标志
	virtual bool __cdecl QueryPassFlag(WORD wTableID);
	//获取桌子属性
	virtual const tagTableState * GetTableAttrib(WORD wTableID);

    //辅助函数
protected:
	//桌子测试
	WORD TableHitTest(POINT MousePoint);
	//椅子测试
	WORD ChairHitTest(POINT MousePoint);
	//资源目录
	void GetGameResDirectory(const tagGameKind * pGameKind, TCHAR szResDirectory[], WORD wBufferCount);

	//消息函数
protected:
	//重画消息
	afx_msg void OnPaint();
	//位置消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//滚动消息
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	//鼠标消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif