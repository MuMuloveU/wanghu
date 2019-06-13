#ifndef EXPRESSION_HEAD_FILE
#define EXPRESSION_HEAD_FILE

#pragma once

#include "ClientShare.h"

//宏定义
#define MAX_EXPRESS_ITEM			121										//表情数目
#define WM_HIT_EXPMESSTION			WM_USER+100								//消息定义

//////////////////////////////////////////////////////////////////////////

//表情子项
class CLIENT_SHARE_CLASS CExpressionItem
{
	//变量定义
public:
	WORD							m_wTranceLength;						//解释长度
	TCHAR							m_szTrancelate[12];						//解释字符
	TCHAR							m_szChineseName[12];					//中文解释

	//函数定义
public:
	//构造函数
	CExpressionItem();
	//析构函数
	~CExpressionItem();

	//功能函数
public:
	//表情配置
	void InitExpressItem(LPCTSTR pszChineseName, LPCTSTR pszTrancelate);
};

//////////////////////////////////////////////////////////////////////////

//表情动作窗口类
class CLIENT_SHARE_CLASS CExpression : public CDialog
{
	//变量定义
protected:
	WORD							m_wSelectIndex;							//选择索引
	BYTE							m_cbPageIndex;							//翻页索引
	TCHAR							m_szPath[MAX_PATH];						//当前路径

	//全局变量
protected:
	static WORD						m_wMaxRow;								//最大列数目
	static WORD						m_wMaxLine;								//最大行数目

	//控件变量
protected:
	CSkinButton						m_btLastPage;							//上页按钮
	CSkinButton						m_btNextPage;							//下页按钮

public:
	static CExpressionItem			m_ExpreesionItem[MAX_EXPRESS_ITEM];		//表情子项

	//函数定义
public:
	//构造函数
	CExpression();
	//析构函数
	virtual ~CExpression();

	//功能函数
public:
	//创建函数
	bool CreateExpression(CWnd * pParentWnd);
	//获取大小
	bool GetFixSize(CSize & Size);
	//获取表情
	static CExpressionItem * GetExpressItem(WORD wIndex);

	//辅助函数
protected:
	//初始化资源
	static bool InitExpressItem();
	//绘画头像
	void DrawItem(CDC * pDC, int iXPos, int iYPos, bool bSelected);
	//按键测试
	WORD HitItemTest(CPoint MousePoint);

	//重载函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange * pDX);

	//消息函数
public:
	//重画消息
	afx_msg void OnPaint();
	//失去焦点消息
	afx_msg void OnKillFocus(CWnd * pNewWnd);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};

//////////////////////////////////////////////////////////////////////////

#endif