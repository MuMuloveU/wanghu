#ifndef HINT_MSG_HEAD_FILE
#define HINT_MSG_HEAD_FILE

#pragma once

#include "ClientShare.h"

//////////////////////////////////////////////////////////////////////////

//消息窗口
class CLIENT_SHARE_CLASS CHintMsgBox : public CSkinDialogEx
{
	//属性变量
protected:
	UINT							m_uType;							//消息类型
	CString							m_strString;						//提示消息
	CString							m_strCaption;						//信息标题

	//辅助变量
protected:
	UINT							m_nElapse;							//剩余时间
	UINT							m_nBtCount;							//按钮数目
	CRect							m_rcString;							//字符位置
	HINSTANCE						m_hResInstance;						//资源句柄

	//控件变量
protected:
	CSkinButton						m_btButton[3];						//按钮控件

	//函数定义
public:
	//构造函数
	CHintMsgBox(CWnd * pParentWnd=NULL);
	//析构函数
	virtual ~CHintMsgBox();

	//重载函数
protected:
	//初始化消息
	virtual BOOL OnInitDialog();
	//命令消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//显示消息
	INT ShowMessageBox(LPCTSTR pszString, UINT uType=MB_OK, UINT nElapse=0);
	//显示消息
	INT ShowMessageBox(LPCTSTR pszCaption, LPCTSTR pszString, UINT uType=MB_OK, UINT nElapse=0);
	//边框宽度
	INT GetXBorder(){return 2;}
	//边框高度
	INT GetYBorder(){return 2;}


	//内部函数
private:
	//创建按钮
	VOID CreateBoxButton();

	//消息映射
protected:
	//绘画消息
	VOID OnPaint();
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//显示消息
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformation(LPCTSTR pszString, UINT nElapse, UINT uType);
extern "C" CLIENT_SHARE_CLASS INT __cdecl ShowInformationEx(LPCTSTR pszString, UINT nElapse, UINT uType ,LPCTSTR pszCaption);


//////////////////////////////////////////////////////////////////////////

#endif