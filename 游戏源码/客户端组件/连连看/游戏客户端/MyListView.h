#pragma once

#include "MyEdit.h"
#include "AInfo.h"

// CMyListView 对话框

#include "..\..\..\开发库\Include\clientshare.h"


//按钮消息
#define			IDM_START				300
#define			IDM_EXERCISE			301
#define			IDM_QUIT				302
#define			IDM_CHATCOLOR			303
//道具消息
#define			IDM_GUIDE				401
#define			IDM_RESET				402
#define			IDM_BOM					403
#define			IDM_STOPIT				404
#define			IDM_CLOG				405

struct UserData_List
{
	LPCTSTR m_name;
	LONG	m_id;
	LPCTSTR m_grade;
	LONG	m_score;
};
struct UserChat_List
{
	UserChat_List(){m_type=0;}
	LPCTSTR m_name;
	CString m_message;
	COLORREF m_color;
	int    m_type;
};

//超级链接
struct SupperLink
{
	SupperLink(){m_LinkRect_1 = CRect(0,0,0,0);m_LinkRect_2 = CRect(0,0,0,0);m_LinkRect_3 = CRect(0,0,0,0);}
	CString m_URL;
	CRect m_LinkRect_1;
	CRect m_LinkRect_2;
	CRect m_LinkRect_3;
};

class CMyListView : public CDialog , public IMessageProxy
{
	DECLARE_DYNAMIC(CMyListView)
private:
	enum {  CHAT = 0, DATA};
	enum {  CHATMESSAGE=0, SYSMESSAGE, INOROUT};    //消息类型
	int		m_CurrentType;							//当前类型
	bool	m_CanScroll;							//是否可以滚动
	int		m_XPos;									//基准

	int     m_CurrentSelectUser;

	bool	m_bReDrawBk;
	bool	m_DisplayPropInfo;
	CString m_PropInfoString;

	CDC		memDC;

	HCURSOR					m_hand;
	CLogic					m_Logic;
	CExpression				m_Expression;					//表情窗口
	CBrush					m_Brush;
	IClientKernel			* m_pIClientKernel;				//内核接口

	//button
	CSkinButton				m_btUp;
	CSkinButton				m_btDown;
	CSkinButton				m_btChat;
	CSkinButton				m_btData;

	CSkinButton				m_btAdvise;
	CSkinButton				m_btOrder;
	CSkinButton				m_btQuit2;
	CSkinButton				m_btSet;


	CSkinButton				m_btSend;								//Send
	CSkinButton				m_btColor;								//color;
	CSkinButton				m_btFace;								//face
	CSkinButton				m_btClear;								//clear;

public:
	CSkinButton				m_btExercise;
private:
	CSkinButton				m_btQuit;

	CSkinButton				m_btTeamA;
	CSkinButton				m_btTeamB;
	CSkinButton				m_btTeamFree;

	//道具
	CSkinButton				m_btGuide;
	CSkinButton				m_btReset;
	CSkinButton				m_btBom;
	//CSkinButton				m_btStopIt;
//	CSkinButton				m_btClog;


	CSkinImage				m_ImageListViewBg;
	CSkinImage				m_ImageFace;
	CSkinImage				m_ImageLogo;

	//Edit
	CMyEdit					m_messageEdit;                  //消息框

	//
	CAInfo					m_AInfo;						//道具信息
	int						m_AInfoPos;	

public:
	CMyListView(CWnd* pParent = NULL);					// 标准构造函数
	virtual ~CMyListView();

	CSkinButton				m_btStart;

	//
	CArrayTemplate<UserChat_List> m_Chat;				//聊天信息容器
	CArrayTemplate<UserData_List> m_Data;	

	CArrayTemplate<SupperLink> m_SupperLink;			//超链接区域
	int				m_ChatPos;							//聊天起始位置
	int				m_DataPos;							//用户数据

	int				m_ImplementCount[PROP_MAX_CNT];

	tagUserData*	m_pCurrentUserData;
	
// 对话框数据
	enum { IDD = IDD_LISTINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

public:
	//删除用户数据
	void DeleteUserItem(tagUserData * pUserData);
	//获取用户数据
	void InsertUserItem(const tagUserData * pUserData);
	//插入聊天纪录
	bool InsertChat(LPCTSTR pszSendName, LPCTSTR str, COLORREF color);
	//更新积分
	void UpdateUserItem(tagUserData * pUserData);
	
	//设置内核
	void SetClientKernel(IClientKernel * pIClientKernel);

	//颜色按钮
	void OnBnClickedColorSet();
	//表情按钮
	void OnBnClickedExpression();
	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);
	//发送按钮
	void OnBnClickedSendChat();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnUp(void);
	void OnDown(void);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	void OnMakeChat(void);
	void OnMakeData(void);
public:
	CRect AutoDrawTextAndImage(CDC * pDC, LPRECT lpRect,CString szString);
	bool OutPutData(CDC * pDC, CString data, CPoint *cpos,LPRECT lpRect,bool textorimage);
	void ClearChat(void);
	// 道具请零
	void PropZero(void);

	//设置句柄
	 bool __cdecl SetRichEditHwnd(CSkinRichEdit * pSkinRichEdit);
	//基础接口
public:
	//释放对象
	virtual void __cdecl Release() { if (IsValid()) delete this; return ; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CMyListView))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//设置颜色
	virtual bool __cdecl SetMessageOption(tagMessageOption & MessageOption);

	virtual CSkinRichEdit * __cdecl GetRichEdit(){return NULL;}
	//控制接口
public:
	//清除屏幕
	virtual bool __cdecl CleanScreen();
	//设置滚动
	virtual bool __cdecl SetAutoScroll(bool bScroll);
	//加载消息
	virtual bool __cdecl LoadMessage(LPCTSTR pszFileName);
	//保存信息
	virtual bool __cdecl SaveMessage(LPCTSTR pszFileName);
	// 设置用户信息
	void SetUserDataItem(tagUserData* pUserData);

	//信息接口
public:
	//插入图片
	virtual bool __cdecl InsertImage(CBitmap * pBitmap);
	//插入时间
	virtual bool __cdecl InsertSystemTime(COLORREF crColor);
	//用户名字
	virtual bool __cdecl InsertUserName(LPCTSTR pszUserName);
	//系统消息
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString, UINT uSize);
	//特色消息
	virtual bool __cdecl InsertShtickString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine);
	//普通消息
	virtual bool __cdecl InsertGeneralString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine);
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszString, COLORREF crColor, UINT uSize);
	//用户聊天
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszRecvName, LPCTSTR pszString, COLORREF crColor, UINT uSize);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void OpenUrl(CString strBuffer);
	// 游戏开始
	void OnStart(void);
	// 游戏结束
	void OnQuit(void);
	// 指南针
	void OnGuide(void);
	// 重列
	void OnReset(void);
	// 炸弹
	void OnBom(void);
	// 练习
	void OnExecise(void);
	// 初始控制选项
	void InitCtrl(void);
	// 更新道具按钮显示状态
	void UpdateImplementState(void);
	//void OnStopIt(void);
	// 障碍
	//void OnClog(void);
	void OnSet(void);
	void OnAdvise(void);
	void OnOrder(void);
	void OnQuit2(void);
};
