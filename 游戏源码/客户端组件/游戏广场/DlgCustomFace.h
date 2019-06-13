#ifndef CUSTOM_FACE_HEAD_FILE
#define CUSTOM_FACE_HEAD_FILE

#pragma once
////////////////////////////////////////////////////////////////////////////////////

//操作状态
enum enOperateStatus
{
	enOperateStatus_NULL,
	enOperateStatus_Upload,
	enOperateStatus_Delete
};

////////////////////////////////////////////////////////////////////////////////////

//自定义头像
class CDlgCustomFace : public CDialog, CSkinWndObject
{
	//变量定义
protected:
	CImage						m_Image;							//图片变量
	enOperateStatus				m_enOperateStatus;					//状态变量
	CBrush						m_brBkground;						//背景刷子

	//控件变量
protected:
	CSkinButton					m_btBrowser;						//浏览按钮
	CSkinButton					m_btUpload;							//上传按钮
	CSkinButton					m_btDelete;							//删除按钮
	ITCPSocket					* m_pIClientSocket;					//网络组件
	//tagUserData					* m_pMeUserData;					//用户信息

	//函数定义
public:
	//构造函数
	CDlgCustomFace(CWnd* pParent = NULL);
	//虚构函数
	virtual ~CDlgCustomFace();

	//函数定义
public:
	//发送数据
	bool SendData();

	//函数定义
protected:
	//交换函数
	virtual void DoDataExchange(CDataExchange* pDX);
	//初始函数
	virtual BOOL OnInitDialog();

	//辅助函数
public:
	//显示消息
	void ShowMessage(TCHAR *pMessage);
	//网络链接
	bool ConnectServer();
	//更新控件
	void UpdateControls();
	//设置状态
	void SetStatus(enOperateStatus enOperateStatus){m_enOperateStatus = enOperateStatus;}

	//消息函数
protected:
	//浏览图片
	afx_msg void OnBnClickedBrowser();
	//上传图片
	afx_msg void OnBnClickedUpload();
	//删除图片
	afx_msg void OnBnClickedDelete();
	//绘画函数
	afx_msg void OnPaint();
	//颜色处理
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//显示消息
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//销毁消息
	afx_msg void OnClose();
	//鼠标信息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标信息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标信息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////////

#endif