#include "StdAfx.h"
#include "GameClient.h"
#include "DlgChessManual.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_MANUAL_PALY				100									//棋谱播放

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgChessManual, CDialog)

	//系统消息
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()

	//控制按钮
	ON_BN_CLICKED(IDC_HIDE, OnBnClickedHide)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_LOAD, OnBnClickedLoad)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_NOTE, OnBnClickedNote)

	//棋谱按钮
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_HEAD, OnBnClickedHead)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_PLAY, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_MANUAL_VIEW_BT_TAIL, OnBnClickedTail)

	//控件消息
	ON_LBN_SELCHANGE(IDC_MANUAL_VIEW_LIST, OnLbnSelchangeManualList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CChessManualFile::CChessManualFile()
{
	//设置变量
	m_wBankerUser=INVALID_CHAIR;
	ZeroMemory(m_szUserAccounts,sizeof(m_szUserAccounts));
	ZeroMemory(&m_ManualDateTime,sizeof(m_ManualDateTime));

	return;
}

//析构函数
CChessManualFile::~CChessManualFile()
{
}

//获取用户
TCHAR * CChessManualFile::GetUserAccounts(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return NULL;

	return m_szUserAccounts[wChairID];
}

//设置用户
bool CChessManualFile::SetUserAccounts(WORD wChairID, TCHAR * pszAccounts)
{
	//效验参数
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//设置变量
	lstrcpyn(m_szUserAccounts[wChairID],pszAccounts,CountArray(m_szUserAccounts[wChairID]));

	return true;
}

//加载棋谱
bool CChessManualFile::LoadChessManual(LPCTSTR pszFileName, CChessManualArray & ChessManualArray)
{
	//打开文件
	CFile FileManual;
	if (FileManual.Open(pszFileName,CFile::modeRead)==FALSE) return false;

	//判断文件
	DWORD dwFileLength=(DWORD)FileManual.GetLength();
	if (dwFileLength<sizeof(tagChessManualHead)) return false;

	//读取数据
	tagChessManualHead ChessManualHead;
	FileManual.Read(&ChessManualHead,sizeof(ChessManualHead));

	//版本效验
	ASSERT(ChessManualHead.wVersion==VER_MANUAL_FILE);
	if (ChessManualHead.wVersion!=VER_MANUAL_FILE) return false;

	//长度效验
	ASSERT((ChessManualHead.wItemCount*sizeof(tagChessManual)+sizeof(tagChessManualHead))==dwFileLength);
	if ((ChessManualHead.wItemCount*sizeof(tagChessManual)+sizeof(tagChessManualHead))!=dwFileLength) return false;

	//设置变量
	m_wBankerUser=ChessManualHead.wBankerUser;
	m_ManualDateTime=ChessManualHead.ManualDateTime;
	lstrcpyn(m_szUserAccounts[0],ChessManualHead.szUserAccounts[0],CountArray(m_szUserAccounts[0]));
	lstrcpyn(m_szUserAccounts[1],ChessManualHead.szUserAccounts[1],CountArray(m_szUserAccounts[1]));

	//读取棋谱
	ChessManualArray.RemoveAll();
	ChessManualArray.SetSize(ChessManualHead.wItemCount);
	FileManual.Read(ChessManualArray.GetData(),ChessManualHead.wItemCount*sizeof(tagChessManual));

	return true;
}

//保存棋谱
bool CChessManualFile::SaveChessManual(LPCTSTR pszFileName, CChessManualArray & ChessManualArray)
{
	//效验参数
	ASSERT(m_wBankerUser<GAME_PLAYER);
	if (m_wBankerUser>=GAME_PLAYER) return false;

	//打开文件
	CFile FileManual;
	if (FileManual.Open(pszFileName,CFile::modeWrite|CFile::modeCreate)==FALSE) return false;

	//获取时间
	SYSTEMTIME SystemTime;
	tagChessManualHead ChessManualHead;

	//设置变量
	GetLocalTime(&SystemTime);
	ZeroMemory(&ChessManualHead,sizeof(ChessManualHead));

	//棋谱信息
	ChessManualHead.wVersion=VER_MANUAL_FILE;
	ChessManualHead.ManualDateTime=SystemTime;
	ChessManualHead.wItemCount=(WORD)ChessManualArray.GetCount();

	//游戏信息
	ChessManualHead.wBankerUser=m_wBankerUser;
	lstrcpyn(ChessManualHead.szUserAccounts[0],m_szUserAccounts[0],CountArray(ChessManualHead.szUserAccounts[0]));
	lstrcpyn(ChessManualHead.szUserAccounts[1],m_szUserAccounts[1],CountArray(ChessManualHead.szUserAccounts[1]));

	//写入文件头
	FileManual.SeekToBegin();
	FileManual.Write(&ChessManualHead,sizeof(ChessManualHead));

	//棋谱数据
	tagChessManual * pManualBuffer=ChessManualArray.GetData();
	FileManual.Write(pManualBuffer,ChessManualHead.wItemCount*sizeof(tagChessManual));

	//设置长度
	FileManual.SetLength(sizeof(ChessManualHead)+ChessManualHead.wItemCount*sizeof(tagChessManual));

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgChessManual::CDlgChessManual() : CDialog(IDD_CHESS_MANUAL)
{
	//设置棋盘
	m_wStepCount=0;
	ZeroMemory(m_ChessItemView,sizeof(m_ChessItemView));
	ZeroMemory(m_ChessBoradView,sizeof(m_ChessBoradView));

	//设置组件
	m_ChessBorad.SetChessBoradMode(true);

	//加载资源
	m_ImageViewBack.SetLoadInfo(IDB_MANUAL_VIEW_BACK,AfxGetInstanceHandle());

	return;
}

//析构函数
CDlgChessManual::~CDlgChessManual()
{
}

//控件绑定
VOID CDlgChessManual::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HIDE, m_btHide);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_MANUAL_VIEW_LIST, m_ManualList);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_LOAD, m_btManualLoad);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_SAVE, m_btManualSave);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_NOTE, m_btManualNote);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_HEAD, m_btManualHead);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_LAST, m_btManualLast);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_PLAY, m_btManualPlay);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_NEXT, m_btManualNext);
	DDX_Control(pDX, IDC_MANUAL_VIEW_BT_TAIL, m_btManualTail);
}

//消息解释
BOOL CDlgChessManual::PreTranslateMessage(MSG * pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	//按键过虑
	if (pMsg->message==WM_KEYDOWN)
	{
		if ((pMsg->wParam==VK_ESCAPE)||(pMsg->wParam==VK_RETURN)) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgChessManual::OnInitDialog()
{
	__super::OnInitDialog();

	//设置窗口
	SetWindowText(TEXT("象棋棋谱"));

	//创建区域
	CRgn RgnImage;
	CImageHandle HandleViewBack(&m_ImageViewBack);
	m_ImageViewBack.CreateImageRegion(RgnImage,RGB(255,0,255));

	//设置窗口
	SetWindowRgn(RgnImage,TRUE);
	SetWindowPos(NULL,0,0,m_ImageViewBack.GetWidth(),m_ImageViewBack.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);

	//设置按钮
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btHide.SetButtonImage(IDB_MANUAL_VIEW_BT_HIDE,hResInstance,false);
	m_btCancel.SetButtonImage(IDB_MANUAL_VIEW_BT_CLOSE,hResInstance,false);
	m_btManualLoad.SetButtonImage(IDB_MANUAL_VIEW_BT_LOAD,hResInstance,false);
	m_btManualSave.SetButtonImage(IDB_MANUAL_VIEW_BT_SAVE,hResInstance,false);
	m_btManualNote.SetButtonImage(IDB_MANUAL_VIEW_BT_NOTE,hResInstance,false);
	m_btManualHead.SetButtonImage(IDB_MANUAL_VIEW_BT_HEAD,hResInstance,false);
	m_btManualLast.SetButtonImage(IDB_MANUAL_VIEW_BT_LAST,hResInstance,false);
	m_btManualPlay.SetButtonImage(IDB_MANUAL_VIEW_BT_PLAY,hResInstance,false);
	m_btManualNext.SetButtonImage(IDB_MANUAL_VIEW_BT_NEXT,hResInstance,false);
	m_btManualTail.SetButtonImage(IDB_MANUAL_VIEW_BT_TAIL,hResInstance,false);

	//设置棋盘
	m_wStepCount=0;
	m_GameLogic.ResetChessBorad();
	m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);

	//建立提示
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);

	//棋谱提示
	m_ToolTipCtrl.AddTool(&m_btManualHead,TEXT("最前"));
	m_ToolTipCtrl.AddTool(&m_btManualLast,TEXT("前一步"));
	m_ToolTipCtrl.AddTool(&m_btManualPlay,TEXT("播放棋谱"));
	m_ToolTipCtrl.AddTool(&m_btManualNext,TEXT("下一步"));
	m_ToolTipCtrl.AddTool(&m_btManualTail,TEXT("最后"));

	//控制提示
	m_ToolTipCtrl.AddTool(&m_btHide,TEXT("最小化"));
	m_ToolTipCtrl.AddTool(&m_btCancel,TEXT("关闭"));
	m_ToolTipCtrl.AddTool(&m_btManualLoad,TEXT("打开棋谱"));
	m_ToolTipCtrl.AddTool(&m_btManualSave,TEXT("保存棋谱"));
	m_ToolTipCtrl.AddTool(&m_btManualNote,TEXT("编辑棋谱解说"));

	//调整控件
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyGameView(rcClient.Width(),rcClient.Height());

	return FALSE;
}

//确定消息
VOID CDlgChessManual::OnOK()
{
	//销毁窗口
	DestroyWindow();
	
	return;
}

//确定消息
VOID CDlgChessManual::OnCancel()
{
	//销毁窗口
	DestroyWindow();

	return;
}

//设置棋谱
bool CDlgChessManual::SetChessManual(CChessManualArray & ChessManualArray, CChessManualFile & ChessManualFile)
{
	//停止播放
	KillTimer(IDI_MANUAL_PALY);
	m_ManualList.EnableWindow(TRUE);

	//还原控制
	m_wStepCount=0;
	m_ManualList.ResetContent();
	m_GameLogic.ResetChessBorad();

	//设置棋盘
	m_ChessBorad.SetChessBorad(m_GameLogic.m_ChessBorad);
	m_ChessBorad.SelectChessItem(INVALID_POS,INVALID_POS);
	m_ChessBorad.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);

	//设置棋谱
	for (WORD i=0;i<ChessManualArray.GetCount();i++) 
	{
		//获取棋谱
		tagChessManual * pChessManualItem=&ChessManualArray[i];

		//变量定义
		BYTE cbXSourcePos=pChessManualItem->cbXSourceChessPos;
		BYTE cbYSourcePos=pChessManualItem->cbYSourceChessPos;
		BYTE cbXTargetPos=pChessManualItem->cbXTargetChessPos;
		BYTE cbYTargetPos=pChessManualItem->cbYTargetChessPos;

		//设置棋子
		TCHAR szManualName[32]=TEXT("");
		tagMoveChessResult MoveChessResult;
		bool bSuccess=m_GameLogic.MoveChess(cbXSourcePos,cbYSourcePos,cbXTargetPos,cbYTargetPos,MoveChessResult,szManualName);

		//棋谱界面
		if (bSuccess==true) m_ManualList.AddString(szManualName);
	}

	//设置变量
	if (&m_ChessManualFile!=&ChessManualFile)
	{
		m_ChessManualFile.SetBankerUser(ChessManualFile.GetBankerUser());
		m_ChessManualFile.SetUserAccounts(0,ChessManualFile.GetUserAccounts(0));
		m_ChessManualFile.SetUserAccounts(1,ChessManualFile.GetUserAccounts(1));
		m_ChessManualFile.SetManualDateTime(ChessManualFile.GetManualDateTime());
	}

	//播放按钮
	m_btManualPlay.EnableWindow((m_GameLogic.m_ChessManualArray.GetCount()>0)?TRUE:FALSE);

	//更新控制
	UpdateManualControl();

	//更新界面
	Invalidate(FALSE);

	return true;
}

//更新控制
VOID CDlgChessManual::UpdateManualControl()
{
	//变量定义
	WORD wManualCount=(WORD)m_GameLogic.m_ChessManualArray.GetCount();

	//设置按钮
	m_btManualNote.EnableWindow(m_wStepCount>0);
	m_btManualHead.EnableWindow(m_wStepCount>0);
	m_btManualLast.EnableWindow(m_wStepCount>0);
	m_btManualNext.EnableWindow(m_wStepCount<wManualCount);
	m_btManualTail.EnableWindow(m_wStepCount<wManualCount);

	//设置列表
	m_ManualList.SetCurSel((m_wStepCount>0)?(m_wStepCount-1):LB_ERR);

	return;
}

//棋谱定位
VOID CDlgChessManual::OrientationManual(WORD wStepCount)
{
	//显示棋谱
	if (m_wStepCount!=wStepCount)
	{
		//设置变量
		m_wStepCount=wStepCount;

		//棋谱状态
		ZeroMemory(m_ChessItemView,sizeof(m_ChessItemView));
		ZeroMemory(m_ChessBoradView,sizeof(m_ChessBoradView));
		m_GameLogic.GetHistoryStatus(m_ChessItemView,m_ChessBoradView,wStepCount);

		//设置界面
		UpdateManualControl();
		m_ChessBorad.SetChessBorad(m_ChessBoradView);
		m_ChessBorad.SelectChessItem(INVALID_POS,INVALID_POS);

		//设置框架
		if (m_wStepCount>0)
		{
			tagChessManual * pChessManual=&m_GameLogic.m_ChessManualArray[wStepCount-1];
			m_ChessBorad.SetMoveChessTrace(pChessManual->cbXSourceChessPos,pChessManual->cbYSourceChessPos,
				pChessManual->cbXTargetChessPos,pChessManual->cbYTargetChessPos);
		}
		else
		{
			m_ChessBorad.SetMoveChessTrace(INVALID_POS,INVALID_POS,INVALID_POS,INVALID_POS);
		}

		//更新控制
		UpdateManualControl();

		//更新界面
		Invalidate(FALSE);
	}

	return;
}

//调整控件
VOID CDlgChessManual::RectifyGameView(INT nWidth, INT nHeight)
{
	//棋盘控件
	m_ChessBorad.SetBenchmarkPos(16,38);
	m_ManualList.SetWindowPos(NULL,461,175,160,288,SWP_NOZORDER);

	//关闭按钮
	CRect rcHide;
	m_btHide.GetWindowRect(&rcHide);
	m_btHide.SetWindowPos(NULL,nWidth-rcHide.Width()*2-20,6,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btCancel.SetWindowPos(NULL,nWidth-rcHide.Width()-15,6,0,0,SWP_NOSIZE|SWP_NOZORDER);

	//加载按钮
	m_btManualLoad.SetWindowPos(NULL,471,511,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btManualSave.SetWindowPos(NULL,543,511,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btManualNote.SetWindowPos(NULL,370,522,0,0,SWP_NOSIZE|SWP_NOZORDER);

	//棋谱按钮
	m_btManualHead.SetWindowPos(NULL,464,481,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btManualLast.SetWindowPos(NULL,464+31,481,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btManualPlay.SetWindowPos(NULL,464+62,481,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btManualNext.SetWindowPos(NULL,464+93,481,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btManualTail.SetWindowPos(NULL,464+124,481,0,0,SWP_NOSIZE|SWP_NOZORDER);

	return;
}

//棋谱按钮
VOID CDlgChessManual::OnBnClickedHead()
{
	//棋盘定位
	OrientationManual(0);

	return;
}

//棋谱按钮
VOID CDlgChessManual::OnBnClickedLast()
{
	//获取选择
	INT nCurrentSel=m_ManualList.GetCurSel();

	//棋盘定位
	if (nCurrentSel!=LB_ERR) OrientationManual(nCurrentSel);

	return;
}

//棋谱按钮
VOID CDlgChessManual::OnBnClickedPlay()
{
	if (m_ManualList.IsWindowEnabled())
	{
		//定位棋谱
		if (m_wStepCount==m_GameLogic.m_ChessManualArray.GetCount()) OrientationManual(0);

		//播放棋谱
		m_ManualList.EnableWindow(FALSE);
		SetTimer(IDI_MANUAL_PALY,1000,NULL);
	}
	else
	{
		//停止播放
		KillTimer(IDI_MANUAL_PALY);
		m_ManualList.EnableWindow(TRUE);
	}

	return;
}

//棋谱按钮
VOID CDlgChessManual::OnBnClickedNext()
{
	//获取选择
	INT nCurrentSel=m_ManualList.GetCurSel();

	//棋盘定位
	OrientationManual((nCurrentSel==LB_ERR)?1:(nCurrentSel+2));

	return;
}

//棋谱按钮
VOID CDlgChessManual::OnBnClickedTail()
{
	//棋盘定位
	OrientationManual(m_ManualList.GetCount());

	return;
}

//隐藏按钮
VOID CDlgChessManual::OnBnClickedHide()
{
	//隐藏窗口
	ShowWindow(SW_MINIMIZE);

	return;
}

//打开按钮
VOID CDlgChessManual::OnBnClickedLoad()
{
	//获取目录
	TCHAR szFilePath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szFilePath),szFilePath);

	//选择文件
	LPCTSTR pszFilter=TEXT("中国象棋棋谱 （*.CML）|*.CML||");
	CFileDialog DlgLoadManual(TRUE,TEXT("CML"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter,this);
	DlgLoadManual.m_pOFN->lpstrInitialDir=szFilePath;

	//选择文件
	if (DlgLoadManual.DoModal()==IDOK)
	{
		//打开棋谱
		CChessManualArray ChessManualArray;
		bool bSuccess=m_ChessManualFile.LoadChessManual(DlgLoadManual.GetPathName(),ChessManualArray);

		//结果处理
		if (bSuccess==false)
		{
			//CInformation Information(this);
			//Information.ShowMessageBox(TEXT("错误"),TEXT("中国象棋“棋谱”文件加载失败！"),MB_OK|MB_ICONERROR);
		}
		else
		{
			m_btManualSave.EnableWindow(TRUE);
			SetChessManual(ChessManualArray,m_ChessManualFile);
		}
	}

	return;
}

//保存按钮
VOID CDlgChessManual::OnBnClickedSave()
{
	//获取目录
	TCHAR szFilePath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szFilePath),szFilePath);

	//选择文件
	LPCTSTR pszFilter=TEXT("中国象棋棋谱 （*.CML）|*.CML||");
	CFileDialog DlgSaveManual(FALSE,TEXT("CML"),NULL,OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter,this);
	DlgSaveManual.m_pOFN->lpstrInitialDir=szFilePath;

	//选择文件
	if (DlgSaveManual.DoModal()==IDOK)
	{
		//保存棋谱
		bool bSuccess=m_ChessManualFile.SaveChessManual(DlgSaveManual.GetPathName(),m_GameLogic.m_ChessManualArray);

		//结果处理
		if (bSuccess==false)
		{
			//CInformation Information(this);
			//Information.ShowMessageBox(TEXT("错误"),TEXT("中国象棋“棋谱”文件保存失败！"),MB_OK|MB_ICONERROR);
		}
	}

	return;
}

//解说按钮
VOID CDlgChessManual::OnBnClickedNote()
{
	//提示信息
	//CInformation Information(this);
	//Information.ShowMessageBox(TEXT("提示"),TEXT("这里是打开“棋谱解说编辑器”的按钮，我们将在未来的升级版本中提供此功能！"));

	return;
}

//重画函数
VOID CDlgChessManual::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SetTextColor(RGB(64,0,0));
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画背景
	CImageHandle HandleViewBack(&m_ImageViewBack);
	m_ImageViewBack.BitBlt(DCBuffer,0,0);

	//绘画棋盘
	m_ChessBorad.DrawChessBorad(&DCBuffer);

	//绘画信息
	if (m_ChessManualFile.GetBankerUser()!=INVALID_CHAIR)
	{
		//构造位置
		CRect rcString;
		rcString.SetRect(470,55,617,67);

		//棋谱时间
		TCHAR szManualDate[64]=TEXT("");
		TCHAR szManualTime[64]=TEXT("");
		SYSTEMTIME SystemTime=m_ChessManualFile.GetManualDateTime();
		_sntprintf(szManualDate,CountChar(szManualDate),TEXT("日期：%02d 月 %02d 日"),SystemTime.wMonth,SystemTime.wDay);
		_sntprintf(szManualTime,CountChar(szManualTime),TEXT("时间：%02d 时 %02d 分"),SystemTime.wHour,SystemTime.wMinute);

		//对局用户
		TCHAR szAccounts[2][LEN_ACCOUNTS];
		WORD wBankerUser=m_ChessManualFile.GetBankerUser();
		CopyMemory(szAccounts[0],m_ChessManualFile.GetUserAccounts(wBankerUser),LEN_ACCOUNTS);
		CopyMemory(szAccounts[1],m_ChessManualFile.GetUserAccounts((wBankerUser+1)%GAME_PLAYER),LEN_ACCOUNTS);
		//CW2CT strAccounts1(m_ChessManualFile.GetUserAccounts(wBankerUser));
		//CW2CT strAccounts2(m_ChessManualFile.GetUserAccounts((wBankerUser+1)%GAME_PLAYER));
		//_sntprintf(szAccounts[0],CountChar(szAccounts[0]),TEXT("红棋：%s"),(LPCTSTR)strAccounts1);
		//_sntprintf(szAccounts[1],CountChar(szAccounts[1]),TEXT("黑棋：%s"),(LPCTSTR)strAccounts2);

		//对局日期
		DCBuffer.DrawText(szManualDate,lstrlen(szManualDate),rcString,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//对局时间
		rcString.OffsetRect(0,25);
		DCBuffer.DrawText(szManualTime,lstrlen(szManualTime),rcString,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//对局用户
		rcString.OffsetRect(0,25);
		DCBuffer.DrawText(szAccounts[0],lstrlen(szAccounts[0]),rcString,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//对局用户
		rcString.OffsetRect(0,25);
		DCBuffer.DrawText(szAccounts[1],lstrlen(szAccounts[1]),rcString,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CDlgChessManual::OnEraseBkgnd(CDC * pDC)
{
	//更新窗口
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//时间消息
VOID CDlgChessManual::OnTimer(UINT nIDEvent)
{
	//播放棋谱
	if (nIDEvent==IDI_MANUAL_PALY)
	{
		//获取数目
		WORD wManualCount=(WORD)m_GameLogic.m_ChessManualArray.GetCount();

		//播放控制
		if (m_wStepCount>=wManualCount)
		{
			KillTimer(IDI_MANUAL_PALY);
			m_ManualList.EnableWindow(TRUE);
		}
		else OrientationManual(m_wStepCount+1);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//选择改变
VOID CDlgChessManual::OnLbnSelchangeManualList()
{
	//获取选择
	INT nCurrentSel=m_ManualList.GetCurSel();

	//棋盘定位
	OrientationManual(nCurrentSel+1);

	return;
}

//鼠标消息
VOID CDlgChessManual::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//消息模拟
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(Point.x,Point.y));

	return;
}

//////////////////////////////////////////////////////////////////////////
