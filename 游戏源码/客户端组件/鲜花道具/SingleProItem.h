#ifndef SINGLE_PRO_ITEM_CONTROL_HEAD
#define SINGLE_PRO_ITEM_CONTROL_HEAD

#pragma once

#include "..\..\客户端组件\游戏框架\GameFrame.h"
#include "PropertyModule.h"

//////////////////////////////////////////////////////////////////////////
//啦八道具
class PROPERTY_MODULE_CLASS CBugleDlg : public CSkinDialogEx
{
	//变量定义
protected:
	tagUserData						*m_pMeUserData;					//用户信息
	COLORREF						m_crChatTX;						//字体颜色
	CString							m_strInuptChat;					//输入信息
	int								m_nMaxChar;						//最大个数
	CString							m_strPropertyInfo;				//道具信息

	//控件变量
protected:
	CExpression						m_Expression;					//表情窗口
	CSkinButton						m_btOK;							//确定按钮
	CSkinButton						m_btCancel;						//关闭按钮
	CSkinButton						m_btExpression;					//表情按钮
	CSkinButton						m_btColorSet;					//颜色设置
	CEdit							m_InputChat;					//聊天框

	//组件变量
protected:
	IClientKernel					* m_pIClientKernel;				//内核接口
	ITCPSocket						* m_pClientSocket;				//网络接口

public:
	//构造函数
	CBugleDlg(CWnd* pParent = NULL); 
	//析构函数
	virtual ~CBugleDlg();

protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);    

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource();
	//设置组件
	void SetSendInfo(IClientKernel *pClientKernel,ITCPSocket *pClientSocket,tagUserData *pMeUserData);
	//发送信息
	void SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize);

	//消息处理
protected:
	//颜色按钮
	afx_msg void OnBnClickedColorSet();
	//表情消息
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);
	//表情按钮
	afx_msg void OnBnClickedExpression();
	//初始框架
	virtual BOOL OnInitDialog();
	//确认消息
	afx_msg void OnBnClickedOk();
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//绘画函数
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//啦八道具
extern "C" PROPERTY_MODULE_CLASS VOID __cdecl ShowBugleDlg(IClientKernel *pIClientKernel,ITCPSocket *pClientSocket,tagUserData *pMeUserData);

//////////////////////////////////////////////////////////////////////////

#endif
