#ifndef FLOWER_EFFECT_CONTROL_HEAD
#define FLOWER_EFFECT_CONTROL_HEAD

#pragma once

#include "PropertyModule.h"

///////////////////////////////////////////////////////////////////////////////////

//Gif结构
typedef struct tagGifImage{
	WORD lWidth;
	WORD lHeight;
	WORD width;
	WORD height;
	struct flag{
		BYTE d:3;
		BYTE c:1;
		BYTE b:3;
        BYTE a:1;
	}Flag;
}GifImage,*PGifImage;

///////////////////////////////////////////////////////////////////////////////////

//鲜花线程
class PROPERTY_MODULE_CLASS CFlowerEffectThread : public CServiceThread
{
	//变量定义
protected:
	HWND							m_hFlowerEffectControl;					//效果窗体
	WORD							m_wFlowerID;							//鲜花索引
	CSkinImage						m_ImageBuffer;							//缓冲位图
	CSkinImage						m_ImageBufferScreen;					//屏幕缓冲

	//基础函数
public:
	//构造函数
	CFlowerEffectThread(void);
	//析构函数
	~CFlowerEffectThread(void);

	//设置函数
public:
	//初始函数
	void InitThread(HWND hWnd);
	//启动动画
	bool StartFlowerEffect(WORD wFlowerID);

	//事件函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
	//开始事件   
	virtual bool OnEventThreadStrat();
	//停止事件
	virtual bool OnEventThreadConclude();

	//内部函数
private:
	//运行函数
	bool RepetitionRunBMP();
	//运行函数
	bool RepetitionRunPNG();
};

///////////////////////////////////////////////////////////////////////////////////

//鲜花控件
class PROPERTY_MODULE_CLASS CFlowerEffectControl : public CDialog
{
	//变量定义
public:
	CFlowerEffectThread				*m_pFlowerEffectThread;					//动画线程

	//函数定义
public:
	//启动动画
	bool StartFlowerEffect(UINT uFlowerEffectID);
	//创建窗体
	BOOL CreateControl(CWnd *pParent);

	//基础函数
public:
	//构造函数
	CFlowerEffectControl(); 
	//析构函数
	virtual ~CFlowerEffectControl();

protected:
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);

	//继承函数
public:
	//初始函数
	virtual BOOL OnInitDialog();

	//消息函数
public:
	//绘画函数
	afx_msg void OnPaint();
	
	//消息映射
	DECLARE_MESSAGE_MAP()
	
};
///////////////////////////////////////////////////////////////////////////////////

#endif