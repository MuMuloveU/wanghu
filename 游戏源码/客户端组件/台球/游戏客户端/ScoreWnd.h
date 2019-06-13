//***********************************************
//File		: ScoreWnd.h 
//Time		: 2005-02-25
//Author	: Luwenguang
//Comment	: 显示得分情况类
//***********************************************


#pragma once



class CScoreWnd : public CWnd
{
	DECLARE_DYNAMIC(CScoreWnd)

//===============================================
//	成员函数申明
//===============================================
public:
	CScoreWnd();
	virtual ~CScoreWnd();
	void	Init(void);
	void	GetSize(CSize & size);

protected:
	DECLARE_MESSAGE_MAP()


	//===========================================
	//	重载虚函数与消息响应函数
	//===========================================
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	

	//===========================================
	//	其他成员函数
	//===========================================
protected:
	bool	LoadBitmap_(void);
	void	DrawScore(CDC *pDC, CRect &rect);

public:
	void	SetName(int desk, TCHAR *pName);
	void	SetFactScore(int desk, LONG score);


//===============================================
//	成员变量申明
//===============================================
protected:
	TCHAR		m_name[GAME_PLAYER][32];	//用户姓名
	LONG		m_factscore[GAME_PLAYER];	//实际得分
		
	CSkinImage	m_bkg;

	CRect		m_Rct_Play;
};


//===============================================
//					the end
//===============================================

