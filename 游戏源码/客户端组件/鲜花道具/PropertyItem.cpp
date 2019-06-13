#include "stdafx.h"
#include "resource.h"
#include "PropertyItem.h"
#include ".\propertyitem.h"
///////////////////////////////////////////////////////////////////////////////////

//定时器I D
#define IDI_PRO_ITEM_UPDATE_VIEW	1								//更新界面
#define TIME_PRO_ITEM_UPDATE_VIEW	200								//更新界面

//宏定义
#define DEF_INSIDEBORDER_COLOR	RGB(176,20,1)						//默认颜色
#define BGROUND_COLOR			RGB(254,250,221)					//背景颜色

///////////////////////////////////////////////////////////////////////////////////

//消息映射
BEGIN_MESSAGE_MAP(CPropertyItem, CSkinDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_TARGET_USER, OnCbnSelchangeTargetUser)
	ON_CBN_SELCHANGE(IDC_PROPERTY_TYPE, OnCbnSelchangePropType)
	ON_EN_CHANGE(IDC_PURCHASE_COUNT, OnEnChangePurchaseCount)
	ON_WM_TIMER()	
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////////

//消息映射
BEGIN_MESSAGE_MAP(CFlowerSetting, CSkinDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()	
	ON_EN_CHANGE(IDC_FLOWER_COUNT, OnEnChangeFlowerCount)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CPropertyItem::CPropertyItem(CWnd* pParent): CSkinDialogEx(IDD_PROPERTY_ITEM, pParent)
{
	m_nPropertyID=PROPERTY_COUNT;
	m_pIPurchaseInfo=NULL;
	m_nPropCount=1;
	m_nPropertyType=0;
}

//析构函数
CPropertyItem::~CPropertyItem()
{
}

//控件绑定
void CPropertyItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_TARGET_USER, m_ComboBoxTargetUser);
	DDX_Control(pDX, IDC_PROPERTY_TYPE, m_ComboBoxPropType);
	DDX_Text(pDX, IDC_NORMAL_USER_PRICE, m_strNormalUserPrice);
	DDX_Text(pDX, IDC_MENBER_USER_PRICE, m_strMemberUserPrice);
	DDX_Text(pDX, IDC_GOLD_DESCRIBE, m_strGoldDescribe);
	DDX_Text(pDX, IDC_RESIDUAL_TIME, m_strProContent);
}

//设置道具
void CPropertyItem::SetPropertyID(int nPropertyID)
{
	//合法判断
	ASSERT(nPropertyID <PROPERTY_COUNT );
	if ( PROPERTY_COUNT <= nPropertyID ) return;

	//设置I D
	m_bPlazaRoom = true;
	m_nPropertyID = nPropertyID;

	//删除信息
	while ( 0 < m_ComboBoxTargetUser.GetCount() ) m_ComboBoxTargetUser.DeleteString(0);
	while ( 0 < m_ComboBoxPropType.GetCount() ) m_ComboBoxPropType.DeleteString(0);

	//插入玩家
	WORD wMeItem = 0;
	for ( WORD wChairID = 0; wChairID < MAX_CHAIR; ++wChairID )
	{
		const tagUserData *pUserData = m_pIPurchaseInfo->GetUserInfo(wChairID);
		if ( pUserData == NULL ) continue;
		if(m_bPlazaRoom)m_bPlazaRoom = false;

		int nIndex = m_ComboBoxTargetUser.AddString(pUserData->szName);

		if ( pUserData->dwUserID == m_pIPurchaseInfo->GetMeUserInfo()->dwUserID ) wMeItem = nIndex;
		m_ComboBoxTargetUser.SetItemData(nIndex, pUserData->dwUserID);
	}

	//控件判断
	GetDlgItem(IDC_PRO_USER_ID)->ShowWindow((!m_bPlazaRoom)?SW_HIDE:SW_SHOW);
	GetDlgItem(IDC_TARGET_USER)->ShowWindow((m_bPlazaRoom)?SW_HIDE:SW_SHOW);
	if(m_bPlazaRoom)
	{
		GetDlgItem(IDC_PRO_USER_ID)->SetWindowText(TEXT("请输入赠送对象的用户名"));
		GetDlgItem(IDC_PRO_USER_ID)->SetFocus();
		((CEdit*)GetDlgItem(IDC_PRO_USER_ID))->SetSel(0,-1);
	}

	if(!m_bPlazaRoom)
	{
		//插入旁观
		WORD wIndex = 0;
		tagUserData *pLookonUserdata = NULL;
		do
		{
			pLookonUserdata = m_pIPurchaseInfo->EnumLookonUser(wIndex++);
			if ( pLookonUserdata == NULL ) break;

			int nItem = m_ComboBoxTargetUser.AddString(pLookonUserdata->szName);

			if ( pLookonUserdata->dwUserID == m_pIPurchaseInfo->GetMeUserInfo()->dwUserID ) wMeItem = nItem;
			m_ComboBoxTargetUser.SetItemData(nItem, pLookonUserdata->dwUserID);

		}while(pLookonUserdata != NULL);

		m_ComboBoxTargetUser.SetCurSel(wMeItem);
	}

	//设置变量
	tagPropertyInfo const &PropertyInfo = CPropertyBar::m_PropertyInfo[m_nPropertyID];
	DWORD dwPropertyCount[] = {PropertyInfo.dwPropCount1, PropertyInfo.dwPropCount2, PropertyInfo.dwPropCount3, PropertyInfo.dwPropCount4,
		PropertyInfo.dwPropCount5, PropertyInfo.dwPropCount6, PropertyInfo.dwPropCount7, PropertyInfo.dwPropCount8, PropertyInfo.dwPropCount9, PropertyInfo.dwPropCount10};

	LONG lPropertyPrice[] = {PropertyInfo.lPrice1, PropertyInfo.lPrice2, PropertyInfo.lPrice3, PropertyInfo.lPrice4,PropertyInfo.lPrice5,
		PropertyInfo.lPrice6, PropertyInfo.lPrice7, PropertyInfo.lPrice8, PropertyInfo.lPrice9, PropertyInfo.lPrice10};


	//设置类型
	CString strPropType;
	m_ComboBoxPropType.EnableWindow(TRUE);
	if ( m_nPropertyID != PROP_FLEE && m_nPropertyID != PROP_NEGAGIVE && m_nPropertyID != PROP_BUGLE )
	{
		//设置控件		
		DWORD dwDays=0, dwHours=0, dwMinutes=0, dwSeconds=0;
		for (int nIndex = 0; nIndex < CountArray(dwPropertyCount); ++nIndex )
		{
			if ( 0 < lPropertyPrice[nIndex] && 0 < dwPropertyCount[nIndex] )
			{
				dwDays = CalcDays(dwPropertyCount[nIndex]);
				dwHours = CalcHours(dwPropertyCount[nIndex]);
				dwMinutes = CalcMinutes(dwPropertyCount[nIndex]);
				dwSeconds = CalcSeconds(dwPropertyCount[nIndex]);
				if ( 0 != dwDays ) strPropType.Format(TEXT("%ld天"), dwDays);
				else if ( 0 != dwHours ) strPropType.Format(TEXT("%ld小时"), dwHours);
				else if ( 0 != dwMinutes ) strPropType.Format(TEXT("%ld分钟"), dwMinutes);
				else if ( 0 != dwSeconds ) strPropType.Format(TEXT("%ld秒"), dwSeconds);
				else
				{
					ASSERT(FALSE);
					return;
				}

				//插入信息
				int nItem = m_ComboBoxPropType.InsertString(-1, strPropType);
				m_ComboBoxPropType.SetItemData(nItem, dwPropertyCount[nIndex]);
			}
		}
		m_ComboBoxPropType.SetCurSel(0);
	}
	else if ( m_nPropertyID == PROP_BUGLE )
	{
		for (int nIndex = 0; nIndex < CountArray(dwPropertyCount); ++nIndex )
		{
			if ( 0 < lPropertyPrice[nIndex] && 0 < dwPropertyCount[nIndex] )
			{
				strPropType.Format(TEXT("%ld条"), dwPropertyCount[nIndex]);

				//插入信息
				int nItem = m_ComboBoxPropType.InsertString(-1, strPropType);
				m_ComboBoxPropType.SetItemData(nItem, dwPropertyCount[nIndex]);
			}
		}
		m_ComboBoxPropType.SetCurSel(0);
	}
	else m_ComboBoxPropType.EnableWindow(FALSE); //负分逃跑率

	//道具类型
	if(m_ComboBoxPropType.IsWindowEnabled()==TRUE)OnCbnSelchangePropType();

	//设置数目
	m_nPropCount = 1;
	GetDlgItem(IDC_PURCHASE_COUNT)->SetWindowText(TEXT("1"));

	//更新界面
	CRect Rect(10,38,427,151);
	InvalidateRect(Rect);

	return;
}

//绘画函数
void CPropertyItem::OnPaint()
{
	CPaintDC dc(this); 

	//绘画标题
	DrawCaption(&dc);

	//获取位置
	CRect ClientRect;
	GetClientRect(&ClientRect);
	int nXExcursion=GetXExcursionPos();
	int nYExcursion=GetYExcursionPos();

	//绘画背景
	COLORREF Color = RGB(161,212,226);
	dc.FillSolidRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,4,Color);
	dc.FillSolidRect(nXExcursion,ClientRect.Height()-8,ClientRect.Width()-2*nXExcursion,7,Color);
	dc.FillSolidRect(nXExcursion,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,Color);
	dc.FillSolidRect(ClientRect.Width()-nXExcursion-12,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,Color);

	//绘画边框
	DrawBorder(&dc);

	//道具图片
	CBitmap Bitmap;

	//加载图片
	AfxSetResourceHandle(CPropertyBar::m_pPropertyBar->m_ReshInstance);
	if ( Bitmap.LoadBitmap(CPropertyBar::m_pPropertyBar->m_PropertyViewImage.uPropertyFormer[m_nPropertyID]) )
	{
		//创建资源
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap *pOldBitmap = dcMem.SelectObject(&Bitmap);

		//绘画图片
		BITMAP bm;
		Bitmap.GetBitmap(&bm);
		dc.TransparentBlt(10, 28+5, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, bm.bmWidth, bm.bmHeight,RGB(255,255,255)); 
		
		//是否资源
		dcMem.SelectObject(pOldBitmap);
		Bitmap.DeleteObject();
		dcMem.DeleteDC();
	}
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//获取位置
	CRect rcStaticName, rcStaticDes;
	GetDlgItem(IDC_PROPERTY_NAME)->GetWindowRect(rcStaticName);
	GetDlgItem(IDC_PROPERTY_DESCRIBE)->GetWindowRect(rcStaticDes);
	ScreenToClient(&rcStaticName);
	ScreenToClient(&rcStaticDes);

	//设置内容
	CRect rcPropertyName(rcStaticName.right+3, rcStaticName.top, rcStaticName.right+200, rcStaticName.bottom+100);
	CRect rcPropertyDes(rcStaticDes.right+3, rcStaticDes.top, rcStaticDes.right+200, rcStaticDes.bottom+70);
	if(m_nPropertyID>=PROP_MEMBER_1)rcPropertyDes.right+=30;

	//创建字体
	CFont font;
	VERIFY(font.CreateFont(
		15,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial"));                 // lpszFacename

	//设置属性
	dc.SetBkMode(TRANSPARENT);
	CFont* def_font = dc.SelectObject(&font);
	
	//描述信息
	UINT nFormat = DT_LEFT | DT_TOP | DT_WORDBREAK;
	dc.DrawText(g_PropertyDescribe[m_nPropertyID].szName, rcPropertyName, nFormat);
	dc.DrawText(g_PropertyDescribe[m_nPropertyID].szDescribe, rcPropertyDes, nFormat);

	//删除资源
	dc.SelectObject(def_font);
	font.DeleteObject();
}

//初始函数
BOOL CPropertyItem::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("道具"));

	//定时更新
	SetTimer(IDI_PRO_ITEM_UPDATE_VIEW,TIME_PRO_ITEM_UPDATE_VIEW,NULL);

	return TRUE; 
}

//设置组件
void CPropertyItem::SetPurchaseInfoSink(IPurchaseInfo *pIPurchaseInfo)
{
	m_pIPurchaseInfo = pIPurchaseInfo;

	return;
}

//确认道具
void CPropertyItem::OnBnClickedOk()
{
	//合法判断
	if ( m_nPropCount <= 0 || m_nPropCount > MAX_PROPERTY_COUNT )
	{
		CString strError;
		strError.Format(TEXT("赠送数目必须大于0且小于等于%d"), MAX_PROPERTY_COUNT);
		ShowInformationEx(strError,0,MB_ICONINFORMATION,TEXT("系统消息"));
		GetDlgItem(IDC_PURCHASE_COUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_PURCHASE_COUNT))->SetSel(0,-1);

		return ;
	}

	//查找用户
	DWORD dwTargetUserID=0;
	tagUserData *pTargetUserData=NULL;
	CString strTargetName;
	if(!m_bPlazaRoom)
	{
		int nSelectItem = m_ComboBoxTargetUser.GetCurSel();
		if ( CB_ERR == nSelectItem ) return;

		//获取信息
		dwTargetUserID = DWORD(m_ComboBoxTargetUser.GetItemData(nSelectItem));
		m_ComboBoxTargetUser.GetLBText(nSelectItem, strTargetName);
		pTargetUserData = m_pIPurchaseInfo->SearchUserItem(dwTargetUserID);
	}
	else
	{
		//获取信息
		GetDlgItem(IDC_PRO_USER_ID)->GetWindowText(strTargetName);
		pTargetUserData = m_pIPurchaseInfo->SearchUserItem(strTargetName);
		if (pTargetUserData!=NULL)dwTargetUserID=pTargetUserData->dwUserID;
	}

	//判断在线
	if ( pTargetUserData == NULL )
	{
		CString strMessage;
		strMessage.Format(TEXT("[ % ]已经离开，请重新选择！"), strTargetName);
		ShowInformation(strMessage,0,MB_ICONINFORMATION);
		return;
	}

	//类型判断
	WORD wGameGenre= m_pIPurchaseInfo->GetGameGenre();

	if ( wGameGenre == GAME_GENRE_GOLD && ( m_nPropertyID==PROP_NEGAGIVE || m_nPropertyID == PROP_DOUBLE || m_nPropertyID == PROP_FOURDOLD ))
	{
		ShowInformation(TEXT("游戏币房间不可以使用此功能！"),0,MB_ICONINFORMATION);
		return ;
	}
	if ( wGameGenre == GAME_GENRE_MATCH && ( m_nPropertyID==PROP_NEGAGIVE || m_nPropertyID == PROP_DOUBLE ||
		m_nPropertyID == PROP_FOURDOLD || m_nPropertyID == PROP_BUGLE ))
	{
		ShowInformation(TEXT("比赛房间不可以使用此功能！"),0,MB_ICONINFORMATION);
		return ;
	}

	//获取玩家
	const tagUserData *pMeUserData = m_pIPurchaseInfo->GetMeUserInfo();

	//获取价格
	LONG lNormalPrice = 0, lMemberPrice = 0;
	GetPropertyPrice(lNormalPrice, lMemberPrice);
	
	//判断金额
	LONG lMePrice = pMeUserData->cbMemberOrder ? lMemberPrice : lNormalPrice;
	LONG lMeScore = pMeUserData->lInsureScore;
	if ( lMeScore < lMePrice )
	{
		if ( IDYES == ShowInformation(TEXT("您的游戏币不足，是否充值？"),0,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) )	
		{
			ShellExecute(NULL,TEXT("open"),TEXT("http://www.dc173.com"),NULL,NULL,SW_SHOWDEFAULT);
		}
		
		return;
	}

	//设置数据
	CMD_GF_Property PropertyHeadInfo;
	ZeroMemory(&PropertyHeadInfo, sizeof(PropertyHeadInfo));
	PropertyHeadInfo.cbSendLocation = m_pIPurchaseInfo->GetLocation();
	PropertyHeadInfo.nPropertyID = m_nPropertyID;
	PropertyHeadInfo.dwSourceUserID = pMeUserData->dwUserID;
	PropertyHeadInfo.dwTargetUserID = dwTargetUserID;
	PropertyHeadInfo.dwPachurseCount = m_nPropCount;
	PropertyHeadInfo.dwOnceCount = (DWORD)m_nPropertyType;

	//发送数据
	m_pIPurchaseInfo->SendData(MDM_GF_PRESENT, SUB_GF_PROPERTY, &PropertyHeadInfo, sizeof(PropertyHeadInfo));

	OnOK();
	return;
}

//显示信息
void CPropertyItem::ShowMessage(TCHAR *pszMessage)
{
	ShowInformationEx(pszMessage,0,MB_ICONINFORMATION,TEXT("道具"));

	return;
}

//设置颜色
HBRUSH CPropertyItem::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( nCtlColor == CTLCOLOR_EDIT )
	{
		pDC->SetTextColor(RGB(0,0,0));
	}
	return hbr;
}

//更新控件
void CPropertyItem::UpdateView()
{
	//更新变量
	UpdateData(TRUE);

	if ( PROPERTY_COUNT <= m_nPropertyID ) return;

	//获取容量
	const tagUserData *pMeUserData = m_pIPurchaseInfo->GetMeUserInfo();
	DWORD dwResidualTime=pMeUserData->dwPropResidualTime[m_nPropertyID];

	//显示容量
	CString strResidualTime;
	CString strText=(m_nPropertyID==PROP_BUGLE)?(TEXT("购买数：   ")):(TEXT("有效期：   "));
	if ( dwResidualTime > 0 )
	{
		if ( m_nPropertyID != PROP_BUGLE )
		{
			DWORD dwDays = CalcDays(dwResidualTime);
			DWORD dwHours = CalcHours(dwResidualTime);
			DWORD dwMinutes = CalcMinutes(dwResidualTime);
			DWORD dwSeconds = CalcSeconds(dwResidualTime);
			CString strTmp;
			if ( dwDays != 0 )
			{
				strTmp.Format(TEXT("%ld天"), dwDays);
				strResidualTime += strTmp;
			}
			if ( dwHours != 0 )
			{
				strTmp.Format(TEXT("%ld小时"), dwHours);
				strResidualTime += strTmp;
			}
			if ( dwMinutes != 0 )
			{
				strTmp.Format(TEXT("%ld分钟"), dwMinutes);
				strResidualTime += strTmp;
			}
			if ( dwSeconds != 0 )
			{
				strTmp.Format(TEXT("%ld秒"), dwSeconds);
				strResidualTime += strTmp;
			}
		}
		else strResidualTime.Format(TEXT("%ld"), dwResidualTime);
	}
	else strResidualTime = TEXT("未购买");
	strText+=strResidualTime;
	m_strProContent.Format(strText);

	//变量定义
	LONG lNormalPrice = 0;
	LONG lMemberPrice = 0;

	//获取价格
	GetPropertyPrice(lNormalPrice, lMemberPrice);

	m_strNormalUserPrice.Format(TEXT("普通用户需支付游戏币：%ld"), lNormalPrice);
	m_strMemberUserPrice.Format(TEXT("会员用户需支付游戏币：%ld"), lMemberPrice);	

	LONG lMePrice = pMeUserData->cbMemberOrder ? lMemberPrice : lNormalPrice;
	LONG lMeScore = pMeUserData->lInsureScore;
	if ( lMeScore < lMePrice )
		m_strGoldDescribe.Format(TEXT("您的银行存款：%ld，请先存款后再购买！"), lMeScore);
	else
        m_strGoldDescribe.Format(TEXT("您的银行存款：%ld，购买道具后，存款剩余：%ld"), lMeScore, lMeScore-lMePrice);

	//更新变量
	UpdateData(FALSE);

	return;
}

//选择用户
void CPropertyItem::SetUserDataInfo(tagUserData *pUserData)
{
	if(pUserData==NULL)return;

	//更新变量
	UpdateData(TRUE);

	if(m_ComboBoxTargetUser.IsWindowVisible()==TRUE)
	{
		//查找玩家
		INT nFineIndex=CB_ERR;
		for(INT i=0;i<m_ComboBoxTargetUser.GetCount();i++)
		{
			CString csUserName;
			m_ComboBoxTargetUser.GetLBText(i,csUserName);
			if(csUserName==pUserData->szName)
			{
				nFineIndex = i;
				break;
			}
		}

		//插入玩家
		if(nFineIndex==CB_ERR)
		{
			nFineIndex = m_ComboBoxTargetUser.AddString(pUserData->szName);
			m_ComboBoxTargetUser.SetItemData(nFineIndex, pUserData->dwUserID);
		}

		//当前选择
		if(nFineIndex == m_ComboBoxTargetUser.GetCurSel()) return;

		//更新玩家
		m_ComboBoxTargetUser.SetCurSel(nFineIndex);
	}
	else
	{
		GetDlgItem(IDC_PRO_USER_ID)->SetWindowText(pUserData->szName);
	}

	//更新变量
	UpdateData(FALSE);
	return;
}

//选择消息
void CPropertyItem::OnCbnSelchangeTargetUser()
{
	//当前选择
	int nSelectItem = m_ComboBoxTargetUser.GetCurSel();
	if ( CB_ERR == nSelectItem ) return;

	//获取玩家
	DWORD dwTargetUserID = DWORD(m_ComboBoxTargetUser.GetItemData(nSelectItem));
	tagUserData *pTargetUserData = m_pIPurchaseInfo->SearchUserItem(dwTargetUserID);

	//离开判断
	if ( pTargetUserData == NULL )
	{
		//删除玩家
		m_ComboBoxTargetUser.DeleteString(nSelectItem);

		//显示信息
		CString strMessage, strTargetName;
		m_ComboBoxTargetUser.GetLBText(nSelectItem, strTargetName);
		strMessage.Format(TEXT("[ % ]已经离开，请重新选择！"), strTargetName);
		ShowInformation(strMessage,0,MB_ICONINFORMATION);
		return;
	}

	return;
}

//按钮信息
void CPropertyItem::OnCbnSelchangePropType()
{
	//道具类型
	int nItem = m_ComboBoxPropType.GetCurSel();
	m_nPropertyType = (int)m_ComboBoxPropType.GetItemData(nItem);

	return;
}

//输入信息
void CPropertyItem::OnEnChangePurchaseCount()
{
	CString strCount;
	GetDlgItem(IDC_PURCHASE_COUNT)->GetWindowText(strCount);
	m_nPropCount=_tstoi(strCount);

	return;
}

//时间消息
void CPropertyItem::OnTimer(UINT nIDEvent)
{
	//时间消息
	switch(nIDEvent)
	{
	case IDI_PRO_ITEM_UPDATE_VIEW:		//更新界面
		{
			//更新界面
			UpdateView();

			return;
		}
	}

	CSkinDialogEx::OnTimer(nIDEvent);
}

//命令信息
BOOL CPropertyItem::PreTranslateMessage(MSG* pMsg)
{
	//过滤字母
	if(GetFocus()==GetDlgItem(IDC_PURCHASE_COUNT) && pMsg->message==WM_CHAR)
	{
		BYTE bMesValue = (BYTE)(pMsg->wParam);
		BYTE bTemp = bMesValue-'0';
		if((bTemp<0 || bTemp>9) && bMesValue!=VK_BACK) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//计算天数
DWORD CPropertyItem::CalcDays(DWORD dwTime)
{
	DWORD dwDays = 0;
	dwDays = dwTime / 86400;

	return dwDays;	
}

//计算小时数
DWORD CPropertyItem::CalcHours(DWORD dwTime)
{
	DWORD dwDays = 0, dwHours = 0;
	dwDays = dwTime / 86400;
	dwHours = (dwTime - dwDays * 86400 ) / 3600;
	 
	return dwHours;	
}

//计算分钟数
DWORD CPropertyItem::CalcMinutes(DWORD dwTime)
{
	DWORD dwDays = 0, dwHours = 0, dwMinutes = 0;
	dwDays = dwTime / 86400;
	dwHours = (dwTime - dwDays * 86400 ) / 3600;
	dwMinutes = (dwTime - dwDays * 86400 - dwHours * 3600 ) / 60;
	 
	return dwMinutes;
}

//计算秒数
DWORD CPropertyItem::CalcSeconds(DWORD dwTime)
{
	DWORD dwDays = 0, dwHours = 0, dwMinutes = 0, dwSeconds = 0;
	dwDays = dwTime / 86400;
	dwHours = (dwTime - dwDays * 86400 ) / 3600;
	dwMinutes = (dwTime - dwDays * 86400 - dwHours * 3600 ) / 60;
	dwSeconds = dwTime - dwDays * 86400 - dwHours * 3600 - dwMinutes * 60;
	 
	return dwSeconds;
}

//获取价格
void CPropertyItem::GetPropertyPrice(LONG &lNormalPrice, LONG &lMemberPrice)
{
	//获取玩家
	const tagUserData *pMeUserData = m_pIPurchaseInfo->GetMeUserInfo();

	//设置变量
	tagPropertyInfo const &PropertyInfo = CPropertyBar::m_PropertyInfo[m_nPropertyID];
	DWORD dwPropertyCount[] = {PropertyInfo.dwPropCount1, PropertyInfo.dwPropCount2, PropertyInfo.dwPropCount3, PropertyInfo.dwPropCount4,
		PropertyInfo.dwPropCount5, PropertyInfo.dwPropCount6, PropertyInfo.dwPropCount7, PropertyInfo.dwPropCount8, PropertyInfo.dwPropCount9, PropertyInfo.dwPropCount10};

	LONG lPropertyPrice[] = {PropertyInfo.lPrice1, PropertyInfo.lPrice2, PropertyInfo.lPrice3, PropertyInfo.lPrice4,PropertyInfo.lPrice5,
		PropertyInfo.lPrice6, PropertyInfo.lPrice7, PropertyInfo.lPrice8, PropertyInfo.lPrice9, PropertyInfo.lPrice10};

	//初始变量
	lNormalPrice = lPropertyPrice[0];
	lMemberPrice = lPropertyPrice[0];

	//逃跑负分
	if ( m_nPropertyID != PROP_FLEE && m_nPropertyID != PROP_NEGAGIVE )
	{
		for ( int nIndex = 0; nIndex < CountArray(dwPropertyCount); ++nIndex )
		{
			if ( dwPropertyCount[nIndex] == m_nPropertyType )
			{
				lNormalPrice = m_nPropCount * lPropertyPrice[nIndex];
				lMemberPrice = lNormalPrice * CPropertyBar::m_PropertyInfo[m_nPropertyID].cbDiscount / 100;
				break;
			}
		}
	}
	else
	{
		lNormalPrice = lPropertyPrice[0] * m_nPropCount;
		lMemberPrice = lNormalPrice * CPropertyBar::m_PropertyInfo[m_nPropertyID].cbDiscount / 100;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CFlowerSetting::CFlowerSetting(CWnd* pParent): CSkinDialogEx(IDD_PROPERTY_ITEM, pParent)
{
	m_nFlowerID=FLOWER_COUNT;
	m_pIPurchaseInfo=NULL;
	m_nFlowerCount=1;
	//dwTargetUserID=0;
}

//析构函数
CFlowerSetting::~CFlowerSetting()
{
}

//绑定函数
void CFlowerSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TARGET_USER, m_ComboBoxTargetUser);
	DDX_Text(pDX, IDC_NORMAL_USER_PRICE, m_strNormalUserPrice);
	DDX_Text(pDX, IDC_MENBER_USER_PRICE, m_strMemberUserPrice);
	DDX_Text(pDX, IDC_GOLD_DESCRIBE, m_strGoldDescribe);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

//设置组件
void CFlowerSetting::SetPurchaseInfoSink(IPurchaseInfo *pIPurchaseInfo)
{
	m_pIPurchaseInfo = pIPurchaseInfo;

	return;
}

//设置ID
void CFlowerSetting::SetFlowerID(int nFlowerID) 
{
	//合法判断
	ASSERT(nFlowerID <FLOWER_COUNT );
	if ( FLOWER_COUNT <= nFlowerID ) return;

	//设置I D
	m_bPlazaRoom = true;
	m_nFlowerID = nFlowerID;

	//删除信息
	while ( 0 < m_ComboBoxTargetUser.GetCount() ) m_ComboBoxTargetUser.DeleteString(0);

	//设置标题
	CString strTitle;
	strTitle.Format(TEXT("赠送[ %s ]"), g_FlowerDescribe[m_nFlowerID].szName);
	SetWindowText(strTitle);

	//接口判断
	ASSERT( m_pIPurchaseInfo != NULL );
	if ( m_pIPurchaseInfo == NULL ) return ;

	//插入玩家
	int nTargetItem = 0;
	for ( WORD wChairID = 0; wChairID < MAX_CHAIR; ++wChairID )
	{
		const tagUserData *pUserData = m_pIPurchaseInfo->GetUserInfo(wChairID);
		if ( pUserData == NULL ) continue;
		if(m_bPlazaRoom)m_bPlazaRoom = false;

		int nIndex = m_ComboBoxTargetUser.AddString(pUserData->szName);
		m_ComboBoxTargetUser.SetItemData(nIndex, pUserData->dwUserID);
		if ( pUserData->dwUserID == m_pIPurchaseInfo->GetMeUserInfo()->dwUserID ) nTargetItem = nIndex;
	}

	if(!m_bPlazaRoom)
	{
		//插入旁观
		WORD wIndex = 0;
		tagUserData *pLookonUserdata = NULL;
		do
		{
			pLookonUserdata = m_pIPurchaseInfo->EnumLookonUser(wIndex++);
			if ( pLookonUserdata == NULL ) break;

			int nItem = m_ComboBoxTargetUser.AddString(pLookonUserdata->szName);

			if ( pLookonUserdata->dwUserID == m_pIPurchaseInfo->GetMeUserInfo()->dwUserID ) nTargetItem = nItem;
			m_ComboBoxTargetUser.SetItemData(nItem, pLookonUserdata->dwUserID);

		}while(pLookonUserdata != NULL);
		m_ComboBoxTargetUser.SetCurSel(nTargetItem);
	}

	GetDlgItem(IDC_FLOWER_COUNT)->SetFocus();

	//控件判断
	GetDlgItem(IDC_PRO_USER_ID)->ShowWindow((!m_bPlazaRoom)?SW_HIDE:SW_SHOW);
	GetDlgItem(IDC_TARGET_USER)->ShowWindow((m_bPlazaRoom)?SW_HIDE:SW_SHOW);
	if(m_bPlazaRoom)
	{
		GetDlgItem(IDC_PRO_USER_ID)->SetWindowText(TEXT("请输入赠送对象的用户名"));
		GetDlgItem(IDC_PRO_USER_ID)->SetFocus();
		((CEdit*)GetDlgItem(IDC_PRO_USER_ID))->SetSel(0,-1);
	}

	//设置数目
	m_nFlowerCount=1;
	GetDlgItem(IDC_FLOWER_COUNT)->SetWindowText(TEXT("1"));

	//更新标题
	CRect Rect(0,0,150,30);
	InvalidateRect(Rect);

	return;
}

//按钮消息
void CFlowerSetting::OnBnClickedOk()
{
	//合法判断
	if ( m_nFlowerCount <= 0 || m_nFlowerCount > MAX_FLOWER_COUNT )
	{
		CString strError;
		strError.Format(TEXT("赠送数目必须大于0且小于等于%d"), MAX_FLOWER_COUNT);
		ShowInformation(strError,0,MB_ICONINFORMATION);
		GetDlgItem(IDC_FLOWER_COUNT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_FLOWER_COUNT))->SetSel(0,-1);
		return ;
	}

	//查找用户
	DWORD dwTargetUserID=0;
	tagUserData *pSelectedUserData =NULL;
	if(!m_bPlazaRoom)
	{
		int nSelectedItem = m_ComboBoxTargetUser.GetCurSel();
		if ( CB_ERR != nSelectedItem )
		{
			dwTargetUserID = (DWORD)m_ComboBoxTargetUser.GetItemData(nSelectedItem);
			tagUserData *pTargetUserData = m_pIPurchaseInfo->SearchUserItem(dwTargetUserID);
			if ( pTargetUserData == NULL )
			{
				m_ComboBoxTargetUser.DeleteString(nSelectedItem);
				ShowInformation(TEXT("您要赠送的玩家已经离开，请重新选择赠送玩家"),0,MB_ICONINFORMATION);
				m_ComboBoxTargetUser.SetFocus();
				return;
			}
		}
		else
		{
			ShowInformation(TEXT("请选择赠送玩家"),0,MB_ICONINFORMATION);
			m_ComboBoxTargetUser.SetFocus();
		}
		pSelectedUserData = m_pIPurchaseInfo->SearchUserItem(dwTargetUserID) ;
	}
	else
	{
		//获取信息
		CString strTargetName;
		GetDlgItem(IDC_PRO_USER_ID)->GetWindowText(strTargetName);
		pSelectedUserData = m_pIPurchaseInfo->SearchUserItem(strTargetName);
		if (pSelectedUserData!=NULL)dwTargetUserID=pSelectedUserData->dwUserID;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//存在判断
	if ( pSelectedUserData == NULL )
	{
		ShowInformation(TEXT("对方已经离开，赠送失败！"),0,MB_ICONINFORMATION);
		return ;
	}

	//获取玩家
	const tagUserData *pMeUserData = m_pIPurchaseInfo->GetMeUserInfo();
	
	//设置信息
	LONG lNormalPrice = CPropertyBar::m_FlowerInfo[m_nFlowerID].lPrice * m_nFlowerCount;
	LONG lMemberPrice = lNormalPrice * CPropertyBar::m_FlowerInfo[m_nFlowerID].cbDiscount / 100;

	LONG lMePrice = pMeUserData->cbMemberOrder ? lMemberPrice : lNormalPrice;
	LONG lMeScore = pMeUserData->lInsureScore;
	if ( lMeScore < lMePrice )
	{
		if ( IDYES == ShowInformation(TEXT("您的游戏币不足，是否充值？"),0,MB_ICONQUESTION |MB_YESNO|MB_DEFBUTTON2))	
		{
			ShellExecute(NULL,TEXT("open"),TEXT("http://www.dc173.com"),NULL,NULL,SW_SHOWDEFAULT);
		}

		return;
	}

	//构造数据
	CMD_GF_Gift Gift;
	ZeroMemory( &Gift, sizeof( Gift ) );
	Gift.cbSendLocation = m_pIPurchaseInfo->GetLocation();
	Gift.dwRcvUserID = pSelectedUserData->dwUserID;
	Gift.dwSendUserID = m_pIPurchaseInfo->GetMeUserInfo()->dwUserID;
	Gift.wFlowerCount = m_nFlowerCount;
	Gift.wGiftID = m_nFlowerID;

	//发送消息
	m_pIPurchaseInfo->SendData( MDM_GF_PRESENT,SUB_GF_FLOWER,&Gift, sizeof(Gift));

	OnOK();
	return;
}

//关闭消息
void CFlowerSetting::OnBnClickedCancel()
{
	OnCancel();
	return;
}

//初始函数
BOOL CFlowerSetting::OnInitDialog()
{
	CSkinDialogEx::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("道具"));

	//定时更新
	SetTimer(IDI_PRO_ITEM_UPDATE_VIEW,TIME_PRO_ITEM_UPDATE_VIEW,NULL);

	return TRUE; 
}

//设置颜色
HBRUSH CFlowerSetting::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( nCtlColor == CTLCOLOR_EDIT )
	{
		pDC->SetTextColor(RGB(0,0,0));
	}
	return hbr;
}

//更新界面
void CFlowerSetting::UpdateView()
{
	//更新变量
	UpdateData(TRUE);

	//合法判断
	if ( FLOWER_COUNT <= m_nFlowerID ) return ;

	//获取玩家
	const tagUserData *pMeUserData = m_pIPurchaseInfo->GetMeUserInfo();
	
	//设置信息
	LONG lNormalPrice = CPropertyBar::m_FlowerInfo[m_nFlowerID].lPrice * m_nFlowerCount;
	LONG lMemberPrice = lNormalPrice * CPropertyBar::m_FlowerInfo[m_nFlowerID].cbDiscount / 100;
	m_strNormalUserPrice.Format(TEXT("普通用户需支付游戏币：%ld"), lNormalPrice);
	m_strMemberUserPrice.Format(TEXT("会员用户需支付游戏币：%ld"), lMemberPrice);	
	
	LONG lMePrice = pMeUserData->cbMemberOrder ? lMemberPrice : lNormalPrice;
	LONG lMeScore = pMeUserData->lInsureScore;
	if ( lMeScore < lMePrice )
		m_strGoldDescribe.Format(TEXT("您的银行存款：%ld，请先存款后再购买！"), lMeScore);
	else
		m_strGoldDescribe.Format(TEXT("您的银行存款：%ld，购买道具后，存款剩余：%ld"), lMeScore, lMeScore-lMePrice);

	//更新变量
	UpdateData(FALSE);
}

//选择用户
void CFlowerSetting::SetUserDataInfo(tagUserData *pUserData)
{
	if(pUserData==NULL)return;

	//更新变量
	UpdateData(TRUE);

	if(m_ComboBoxTargetUser.IsWindowVisible()==TRUE)
	{
		//查找玩家
		INT nFineIndex=CB_ERR;
		for(INT i=0;i<m_ComboBoxTargetUser.GetCount();i++)
		{
			CString csUserName;
			m_ComboBoxTargetUser.GetLBText(i,csUserName);
			if(csUserName==pUserData->szName)
			{
				nFineIndex = i;
				break;
			}
		}

		//插入玩家
		if(nFineIndex==CB_ERR)
		{
			nFineIndex = m_ComboBoxTargetUser.AddString(pUserData->szName);
			m_ComboBoxTargetUser.SetItemData(nFineIndex, pUserData->dwUserID);
		}

		//当前选择
		if(nFineIndex == m_ComboBoxTargetUser.GetCurSel()) return;

		//更新玩家
		m_ComboBoxTargetUser.SetCurSel(nFineIndex);
	}
	else
	{
		GetDlgItem(IDC_PRO_USER_ID)->SetWindowText(pUserData->szName);
	}

	//更新变量
	UpdateData(FALSE);
	return;
}

//绘画函数
void CFlowerSetting::OnPaint()
{
	CPaintDC dc(this); 

	//绘画标题
	DrawCaption(&dc);

	CRect ClientRect;
	GetClientRect(&ClientRect);
	int nXExcursion=GetXExcursionPos();
	int nYExcursion=GetYExcursionPos();

	//绘画背景
	COLORREF Color = RGB(161,212,226);
	dc.FillSolidRect(nXExcursion,nYExcursion,ClientRect.Width()-2*nXExcursion,4,Color);
	dc.FillSolidRect(nXExcursion,ClientRect.Height()-8,ClientRect.Width()-2*nXExcursion,7,Color);
	dc.FillSolidRect(nXExcursion,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,Color);
	dc.FillSolidRect(ClientRect.Width()-nXExcursion-12,nYExcursion+4,12,ClientRect.Height()-nYExcursion-9,Color);

	//绘画边框
	DrawBorder(&dc);
}

//时间消息
void CFlowerSetting::OnTimer(UINT nIDEvent)
{
	//时间消息
	switch(nIDEvent)
	{
	case IDI_PRO_ITEM_UPDATE_VIEW:		//更新界面
		{
			//更新界面
			UpdateView();

			return;
		}
	}

	CSkinDialogEx::OnTimer(nIDEvent);
}

//命令信息
BOOL CFlowerSetting::PreTranslateMessage(MSG* pMsg)
{
	//过滤字母
	if(GetFocus()==GetDlgItem(IDC_FLOWER_COUNT) && pMsg->message==WM_CHAR)
	{
		BYTE bMesValue = (BYTE)(pMsg->wParam);
		BYTE bTemp = bMesValue-'0';
		if((bTemp<0 || bTemp>9) && bMesValue!=VK_BACK) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//输入信息
void CFlowerSetting::OnEnChangeFlowerCount()
{
	CString strCount;
	GetDlgItem(IDC_FLOWER_COUNT)->GetWindowText(strCount);
	m_nFlowerCount=_tstoi(strCount);

	return;
}

//////////////////////////////////////////////////////////////////////////
