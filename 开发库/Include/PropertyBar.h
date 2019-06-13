#ifndef PROPERTY_BAR_HEAD_FILE
#define PROPERTY_BAR_HEAD_FILE

#pragma once

#include "PropertyModule.h"
///////////////////////////////////////////////////////////////////////////////////

//类说明
class CPropertyItem;
class CFlowerSetting;

//道具控件
class PROPERTY_MODULE_CLASS CPropertyBar : public CDialog
{
	//资源变量
public:
	HINSTANCE							m_ReshInstance;						//资源句柄
	tagPropertyViewImage				m_PropertyViewImage;				//道具资源

	//位置变量
protected:
	int									m_nXoriginal;						//起始位置
	int									m_nButtonCount;						//按钮数目
	int									m_nTopButtonHead;					//起始按钮
	int									m_nBtmButtonHead;					//起始按钮

	//道具变量
public:
	int									m_nPropertyCount;					//道具数目
	int									m_nFlowerCount;						//鲜花数目

	//状态变量
protected:
	bool								m_bInitDialog;						//初始变量

	//静态变量
public:
	static tagPropertyInfo				m_PropertyInfo[PROPERTY_COUNT];		//道具信息
	static tagFlowerInfo				m_FlowerInfo[FLOWER_COUNT];			//鲜花信息
	static CPropertyBar					* m_pPropertyBar;					//控制对象

	//资源变量
protected:
	CSkinImage							m_ImageL;							//左边图片
	CSkinImage							m_ImageM;							//中间图片
	CSkinImage							m_ImageR;							//右边图片

	//道具鲜花
protected:
	CSkinButton							m_BtProperty[PROPERTY_COUNT];		//道具按钮
	CSkinButton							m_BtFlower[FLOWER_COUNT];			//鲜花按钮

	//控制按钮
protected:
	CSkinButton							m_btTopMoveToL;						//左移按钮
	CSkinButton							m_btTopMoveToR;						//右移按钮
	CSkinButton							m_btBtmMoveToL;						//左移按钮
	CSkinButton							m_btBtmMoveToR;						//右移按钮
	CToolTipCtrl						m_ToolTipCtrl;						//提示控件

	//组件变量
protected:
	CPropertyItem						*m_pPropertyItem;					//道具对话框
	CFlowerSetting						*m_pFlowerSetting;					//礼物对话框
	IPurchaseInfo						*m_pIPurchaseInfo;					//信息接口

	//函数定义
public:
	//构造函数
	CPropertyBar(CWnd* pParent = NULL);
	//析构函数
	virtual ~CPropertyBar();

	//控制函数
public:
	//创建函数
	void CreateBar(CWnd *pParent);
	//销毁函数
	void DestroyBar();

	//辅助函数
public:
	//获取高度
	int GetHeight();
	//设置资源
	void SetSkinResource(HINSTANCE ReshInstance,tagPropertyViewImage *pPropertyViewImage);
	//更新资源
	bool UpdateSkinResource();
	//更新控件
	void UpdateTopButton(INT nTopButtonHead);
	//更新控件
	void UpdateBtmButton(INT nBtmButtonHead);
	//设置组件
	void SetPurchaseInfoSink(IPurchaseInfo *pIPurchaseInfo);
	//初始函数
	void SetPropertyInfo(int nPropID, tagPropertyInfo const & PropertyInfo);
	//初始函数
	void SetFlowerInfo (int nFlowerID, tagFlowerInfo const & FlowerInfo);
	//选择用户
	void SetUserDataInfo(tagUserData *pUserData);

	//静态函数
public:
	static CPropertyBar * GetInstance() { return m_pPropertyBar; }

	//内部函数
protected:
	//调整控件
	void RectifyControl(int nWidth, int nHeight);
	//鲜花事件
	void FlowerEvent(UINT nButtonID);
	//道具事件
	void PropertyEvent(int nPropertyID);

	//继承函数
protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);

	//消息函数
public:
	//绘画函数
	afx_msg void OnPaint();
	//初始函数
	virtual BOOL OnInitDialog();
	//窗口更改
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//重写函数
protected:
	//命令信息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//移动消息
private:
	afx_msg void OnBnClickedTopMoveToR();
	afx_msg void OnBnClickedTopMoveToL();
	afx_msg void OnBnClickedBtmMoveToR();
	afx_msg void OnBnClickedBtmMoveToL();

	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//消息映射
	DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////////////////////////////
#endif