#include "StdAfx.h"
#include "Resource.h"
#include "RecordGameList.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameRecord, CSkinDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameRecord::CGameRecord() : CSkinDialogEx(IDD_DLG_GAME_RECORD)
{
}

//析构函数
CGameRecord::~CGameRecord()
{
}

//控件绑定
void CGameRecord::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECORD_LIST, m_RecordGameList);
}

//初始化函数
BOOL CGameRecord::OnInitDialog()
{
	__super::OnInitDialog();

	//设置颜色
	m_RecordGameList.SetTextColor(RGB(52,87,130));
	m_RecordGameList.SetBkColor(RGB(255,255,255));
	m_RecordGameList.SetTextBkColor(RGB(255,255,255));

	//插入列表
	m_RecordGameList.InsertColumn(0,TEXT("局数"),LVCFMT_LEFT,50);
	m_RecordGameList.InsertColumn(1,TEXT("赢家"),LVCFMT_LEFT,50);
	m_RecordGameList.InsertColumn(2,TEXT("本局成绩"),LVCFMT_LEFT,70);
	m_RecordGameList.InsertColumn(3,TEXT("本局下注"),LVCFMT_LEFT,70);
	m_RecordGameList.InsertColumn(4,TEXT("买闲总注"),LVCFMT_LEFT,80);
	m_RecordGameList.InsertColumn(5,TEXT("买平总注"),LVCFMT_LEFT,80);
	m_RecordGameList.InsertColumn(6,TEXT("买庄总注"),LVCFMT_LEFT,80);
	m_RecordGameList.InsertColumn(7,TEXT("闲家牌点"),LVCFMT_LEFT,70);
	m_RecordGameList.InsertColumn(8,TEXT("庄家牌点"),LVCFMT_LEFT,70);

	return FALSE;
}

//插入列表
void CGameRecord::FillGameRecourd(tagGameRecord & GameRecord)
{
	//变量定义
	WORD wListIndex=0;
	WORD wColumnCount=1;
	TCHAR szBuffer[128]=TEXT("");

	//游戏局数
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%d"),GameRecord.wDrawCount);
	INT nItemIndex=m_RecordGameList.InsertItem(m_RecordGameList.GetItemCount(),szBuffer);

	//游戏赢家
	if (GameRecord.cbWinerSide==0) m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,TEXT("闲家"),0,0,0,0);
	else if (GameRecord.cbWinerSide==1) m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,TEXT("平家"),0,0,0,0);
	else m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,TEXT("庄家"),0,0,0,0);

	//本局成绩
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.lGameScore);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//本局下注
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.lMyAddGold);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//闲家下注
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.lDrawPlayerScore);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//平家下注
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.lDrawTieScore);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//庄家下注
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.lDrawBankerScore);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//闲家牌点
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.cbPlayerPoint);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//庄家牌点
	_snprintf(szBuffer,CountArray(szBuffer),TEXT("%ld"),GameRecord.cbBankerPoint);
	m_RecordGameList.SetItem(nItemIndex,wColumnCount++,LVIF_TEXT,szBuffer,0,0,0,0);

	//移动列表
	int nCount = m_RecordGameList.GetItemCount();
	if (nCount > 0)
		m_RecordGameList.EnsureVisible(nCount-1, FALSE);

	return;
}

//位置变化
void CGameRecord::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//列表控件
	if (m_RecordGameList.m_hWnd!=NULL)
	{
		m_RecordGameList.MoveWindow(0,0,cx,cy);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
