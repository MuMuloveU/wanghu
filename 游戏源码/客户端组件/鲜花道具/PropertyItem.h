#ifndef PROPERTY_ITEM_HEAD_FILE
#define PROPERTY_ITEM_HEAD_FILE

#pragma once

#include "PropertyBar.h"
#include "PropertyModule.h"
//////////////////////////////////////////////////////////////////////////

//道具项
class PROPERTY_MODULE_CLASS CPropertyItem : public CSkinDialogEx
{
	//变量定义
protected:
	bool								m_bPlazaRoom;						//大厅房间
	int									m_nPropertyType;					//道具类型
	int									m_nPropertyID;						//道具ID

	//绑定变量
protected:
	CComboBox							m_ComboBoxTargetUser;				//目标玩家
	CComboBox							m_ComboBoxPropType;					//道具类型
	int									m_nPropCount;						//道具数目
	CString								m_strNormalUserPrice;				//普通价格
	CString								m_strMemberUserPrice;				//会员价格
	CString								m_strGoldDescribe;					//游戏币情况
	CString								m_strProContent;					//道具现量
	CSkinButton							m_btOK;								//确定按钮
	CSkinButton							m_btCancel;							//关闭按钮

	//组件变量
protected:
	IPurchaseInfo						* m_pIPurchaseInfo;					//信息接口

	//函数定义
public:
	//构造函数
	CPropertyItem(CWnd* pParent = NULL);
	//析构函数
	virtual ~CPropertyItem();

protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);

	//功能函数
public:
	//设置道具
	void SetPropertyID(int nPropertyID);
	//设置接口
	void SetPurchaseInfoSink(IPurchaseInfo *pIPurchaseInfo);
	//显示信息
	void ShowMessage(TCHAR *pszMessage);
	//更新界面
	void UpdateView();
	//选择用户
	void SetUserDataInfo(tagUserData *pUserData);

	//内部函数
private:
	//计算天数
	DWORD CalcDays(DWORD dwTime);
	//计算小时数
	DWORD CalcHours(DWORD dwTime);
	//计算分钟数
	DWORD CalcMinutes(DWORD dwTime);
	//计算秒数
	DWORD CalcSeconds(DWORD dwTime);
	//获取价格
	void GetPropertyPrice(LONG &lNormalPrice, LONG &lMemberPrice);

	//消息响应
private:
	//绘画函数
	afx_msg void OnPaint();
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定信息
	afx_msg void OnBnClickedOk();
	//控件颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//按钮信息
	afx_msg void OnCbnSelchangeTargetUser();
	//按钮信息
	afx_msg void OnCbnSelchangePropType();
	//输入信息
	afx_msg void OnEnChangePurchaseCount();
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//命令信息
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//消息映射
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////////

//礼物项
class PROPERTY_MODULE_CLASS CFlowerSetting :public CSkinDialogEx
{
	//变量定义
public:
	bool							m_bPlazaRoom;						//大厅房间
	int								m_nFlowerCount;						//鲜花数目
	int								m_nFlowerID;						//鲜花ID

	//绑定变量
public:
	CComboBox						m_ComboBoxTargetUser;				//目标玩家
	CString							m_strDescribe;						//普通价格
	CString							m_strNormalUserPrice;				//普通价格
	CString							m_strMemberUserPrice;				//会员价格
	CString							m_strGoldDescribe;					//游戏币情况

	//控件变量
protected:
	CSkinButton						m_btOK;								//确定按钮
	CSkinButton						m_btCancel;							//取消按钮

	//组件接口
protected:
	IPurchaseInfo					* m_pIPurchaseInfo;					//信息接口

	//函数定义
public:
	//构造函数
	CFlowerSetting(CWnd* pParent = NULL);
	//析构函数
	virtual ~CFlowerSetting();

	//辅助函数
public:
	//设置接口
	void SetPurchaseInfoSink(IPurchaseInfo *pIPurchaseInfo);
	//设置ID
	void SetFlowerID(int nFlowerID) ;
	//更新界面
	void UpdateView();
	//设置目标
	//void SetTargetUserID(DWORD dwTargetUserID){m_dwTargetUserID = dwTargetUserID;}
	//选择用户
	void SetUserDataInfo(tagUserData *pUserData);

	//继承函数
protected:
	//绑定函数
	virtual void DoDataExchange(CDataExchange* pDX);

	//消息函数
public:
	//绘画函数
	afx_msg void OnPaint();
	//初始化函数
	virtual BOOL OnInitDialog();
	//按钮消息
	afx_msg void OnBnClickedOk();
	//关闭消息
	afx_msg void OnBnClickedCancel();
	//设置颜色
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//时间消息
	afx_msg void OnTimer(UINT nIDEvent);
	//命令信息
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//输入信息
	afx_msg void OnEnChangeFlowerCount();

	//消息映射
	DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////////////////////////////
#endif