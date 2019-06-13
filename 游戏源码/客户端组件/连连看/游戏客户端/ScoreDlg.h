#pragma once
#include"Resource.h"

// CScoreDlg 对话框

class CScoreDlg : public CDialog
{
	DECLARE_DYNAMIC(CScoreDlg)

public:
	CScoreDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScoreDlg();
	virtual BOOL OnInitDialog();

    // 对话框数据
	enum { IDD = IDD_SCORE };

	//变量定义
protected:
	LONG							m_lTax;								//游戏税收
	LONG							m_lScore[MAX_CHAIR];				//游戏积分
	float							m_lPer[MAX_CHAIR];			//游戏得分
	TCHAR							m_szUserName[MAX_CHAIR][NAME_LEN];	//用户名字

	bool							m_isExer;							//是否时练习模式

	//资源变量
protected:
	CSkinImage						m_ImageBack;						//背景图案
	CSkinImage						m_ImageBackExer;					//背景图案

	CSkinImage						m_ImageRate_bg;						//升级进度
	CSkinImage						m_ImageRate;				


	//功能函数
public:
	//重置积分
	void ResetScore();
	//设置税收
	void SetTax(LONG lTax);
	//设置积分
	void SetGameScore(WORD wChairID, LPCTSTR pszUserName, float per, int lScore);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	void SetCurrMode(bool Exer);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
