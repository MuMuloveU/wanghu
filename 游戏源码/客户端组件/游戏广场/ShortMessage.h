#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "Afxcmn.h"

//发送短信息类
class CShortMessage : public CSkinDialogEx
{
	//变量定义
public:
	CWnd								* m_pParentWnd;						//处理窗口
	
	//信息变量
	long int							m_dwTargetUserID;					//对话用户
	__int64								m_dwTargerAccID;					//ACC 号码
	CString								m_strTargerName;					//用户名字
	CString								m_strRoomName;						//房间名字
	IUserItem							* m_pTargetUserItem;				//目标用户
	CString								m_strMsgFileName;					//文件名字

	//控件变量
public:
	CEdit								m_SendMessage;						//发送窗口
	CSkinRichEdit						m_RecvMessage;						//接受窗口
	CSkinButton							m_BtSend;							//发送按钮
	CSkinButton							m_BtExpression;						//表情按钮
	CSkinButton							m_BtClose;							//退出按钮
	CMessageProxyHelper					m_MessageProxyHelper;				//消息代理
	CExpression							m_ExpressWnd;						//表情窗口
	CSkinButton							m_BtOpenChatMsg;					//打开记录

	//函数定义
public:
	//构造函数
	CShortMessage(CWnd * pParentWnd);
	//析构函数
	virtual ~CShortMessage();
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
protected:
	//控件绑定函数
	virtual void DoDataExchange(CDataExchange * pDX); 
	//确定按钮
	virtual void OnOK();
	//取消按钮
	virtual void OnCancel();
	//消息解释函数
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//功能函数 
public:
	//设置信息
	void SetTalkInfo(IUserItem * pTargerUserItem, TCHAR * szRoomName);
	//收到消息
	void OnRecvMessage(TCHAR * szSendName, TCHAR * szMessage, COLORREF crColor, bool bMeMessage);

	//按钮函数
public:
	//表情按钮
	void OnBnClickedExpression();

	//消息函数
public:
	//发送按钮
	afx_msg void OnBnClickedSendShort();
	//退出按钮
	afx_msg void OnBnClickedClose();
	//获取位置
	afx_msg void OnGetMinMaxInfo(MINMAXINFO * lpMMI);
	//打开记录
	afx_msg void OnBnClickedOpenChatMsg();
	//表情消息
	LRESULT OnExpression(WPARAM wparam, LPARAM lparam);
	//绘画函数
	afx_msg void OnPaint();
	
	DECLARE_MESSAGE_MAP()
};
