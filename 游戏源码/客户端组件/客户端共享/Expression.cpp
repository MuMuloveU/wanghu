#include "Stdafx.h"
#include "Resource.h"
#include "Expression.h"
#include ".\expression.h"

//图像大小
#define PIC_SPACE					3
#define EP_PIC_WHIDE				24								
#define EP_PIC_HIGHT				24		

//颜色定义
#define COLOR_BORAD					RGB(0,75,151)						//边框颜色
#define COLOR_FRAME					RGB(0,75,151)						//边框颜色
#define COLOR_CHAR_BK				RGB(230,249,255)					//聊天背景
#define COLOR_BACK_GROUND			RGB(250,250,250)					//背景颜色

WORD					CExpression::m_wMaxRow=11;							//最大列数目
WORD					CExpression::m_wMaxLine=11;							//最大行数目
CExpressionItem			CExpression::m_ExpreesionItem[MAX_EXPRESS_ITEM];	//表情子项

BEGIN_MESSAGE_MAP(CExpression, CDialog)
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CExpressionItem::CExpressionItem() 
{ 
	m_wTranceLength=0;
	m_szTrancelate[0]=0;
	m_szChineseName[0]=0;
};

//析构函数
CExpressionItem::~CExpressionItem() 
{
}

//输入解释
void CExpressionItem::InitExpressItem(LPCTSTR pszChineseName, LPCTSTR pszTrancelate)
{
	//设置变量
	lstrcpyn(m_szTrancelate,pszTrancelate,CountArray(m_szTrancelate));
	lstrcpyn(m_szChineseName,pszChineseName,CountArray(m_szChineseName));
	m_wTranceLength=lstrlen(m_szTrancelate);

	return;
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CExpression::CExpression() : CDialog(IDD_EXPRESSION)
{
	m_pParentWnd=NULL;
	m_wSelectIndex=MAX_EXPRESS_ITEM;
	m_cbPageIndex=0;
	InitExpressItem();
}

//析构函数
CExpression::~CExpression()
{
}

//初始化资源
bool CExpression::InitExpressItem()
{
	static bool bInit=false;
	if (bInit==false)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(CLIENT_SHARE_DLL_NAME));

		//设置信息
		m_ExpreesionItem[0].InitExpressItem(TEXT(""),TEXT("/:01"));
		m_ExpreesionItem[1].InitExpressItem(TEXT(""),TEXT("/:02"));
		m_ExpreesionItem[2].InitExpressItem(TEXT(""),TEXT("/:03"));
		m_ExpreesionItem[3].InitExpressItem(TEXT(""),TEXT("/:04"));
		m_ExpreesionItem[4].InitExpressItem(TEXT(""),TEXT("/:05"));
		m_ExpreesionItem[5].InitExpressItem(TEXT(""),TEXT("/:06"));
		m_ExpreesionItem[6].InitExpressItem(TEXT(""),TEXT("/:07"));
		m_ExpreesionItem[7].InitExpressItem(TEXT(""),TEXT("/:08"));
		m_ExpreesionItem[8].InitExpressItem(TEXT(""),TEXT("/:09"));
		m_ExpreesionItem[9].InitExpressItem(TEXT(""),TEXT("/:10"));

		m_ExpreesionItem[10].InitExpressItem(TEXT(""),TEXT("/:11"));
		m_ExpreesionItem[11].InitExpressItem(TEXT(""),TEXT("/:12"));
		m_ExpreesionItem[12].InitExpressItem(TEXT(""),TEXT("/:13"));
		m_ExpreesionItem[13].InitExpressItem(TEXT(""),TEXT("/:14"));
		m_ExpreesionItem[14].InitExpressItem(TEXT(""),TEXT("/:15"));
		m_ExpreesionItem[15].InitExpressItem(TEXT(""),TEXT("/:16"));
		m_ExpreesionItem[16].InitExpressItem(TEXT(""),TEXT("/:17"));
		m_ExpreesionItem[17].InitExpressItem(TEXT(""),TEXT("/:18"));
		m_ExpreesionItem[18].InitExpressItem(TEXT(""),TEXT("/:19"));
		m_ExpreesionItem[19].InitExpressItem(TEXT(""),TEXT("/:20"));

		m_ExpreesionItem[20].InitExpressItem(TEXT(""),TEXT("/:21"));
		m_ExpreesionItem[21].InitExpressItem(TEXT(""),TEXT("/:22"));
		m_ExpreesionItem[22].InitExpressItem(TEXT(""),TEXT("/:23"));
		m_ExpreesionItem[23].InitExpressItem(TEXT(""),TEXT("/:24"));
		m_ExpreesionItem[24].InitExpressItem(TEXT(""),TEXT("/:25"));
		m_ExpreesionItem[25].InitExpressItem(TEXT(""),TEXT("/:26"));
		m_ExpreesionItem[26].InitExpressItem(TEXT(""),TEXT("/:27"));
		m_ExpreesionItem[27].InitExpressItem(TEXT(""),TEXT("/:28"));
		m_ExpreesionItem[28].InitExpressItem(TEXT(""),TEXT("/:29"));
		m_ExpreesionItem[29].InitExpressItem(TEXT(""),TEXT("/:30"));

		m_ExpreesionItem[30].InitExpressItem(TEXT(""),TEXT("/:31"));
		m_ExpreesionItem[31].InitExpressItem(TEXT(""),TEXT("/:32"));
		m_ExpreesionItem[32].InitExpressItem(TEXT(""),TEXT("/:33"));
		m_ExpreesionItem[33].InitExpressItem(TEXT(""),TEXT("/:34"));
		m_ExpreesionItem[34].InitExpressItem(TEXT(""),TEXT("/:35"));
		m_ExpreesionItem[35].InitExpressItem(TEXT(""),TEXT("/:36"));
		m_ExpreesionItem[36].InitExpressItem(TEXT(""),TEXT("/:37"));
		m_ExpreesionItem[37].InitExpressItem(TEXT(""),TEXT("/:38"));
		m_ExpreesionItem[38].InitExpressItem(TEXT(""),TEXT("/:39"));
		m_ExpreesionItem[39].InitExpressItem(TEXT(""),TEXT("/:40"));

		m_ExpreesionItem[40].InitExpressItem(TEXT(""),TEXT("/:41"));
		m_ExpreesionItem[41].InitExpressItem(TEXT(""),TEXT("/:42"));
		m_ExpreesionItem[42].InitExpressItem(TEXT(""),TEXT("/:43"));
		m_ExpreesionItem[43].InitExpressItem(TEXT(""),TEXT("/:44"));
		m_ExpreesionItem[44].InitExpressItem(TEXT(""),TEXT("/:45"));
		m_ExpreesionItem[45].InitExpressItem(TEXT(""),TEXT("/:46"));
		m_ExpreesionItem[46].InitExpressItem(TEXT(""),TEXT("/:47"));
		m_ExpreesionItem[47].InitExpressItem(TEXT(""),TEXT("/:48"));
		m_ExpreesionItem[48].InitExpressItem(TEXT(""),TEXT("/:49"));
		m_ExpreesionItem[49].InitExpressItem(TEXT(""),TEXT("/:50"));

		m_ExpreesionItem[50].InitExpressItem(TEXT(""),TEXT("/:51"));
		m_ExpreesionItem[51].InitExpressItem(TEXT(""),TEXT("/:52"));
		m_ExpreesionItem[52].InitExpressItem(TEXT(""),TEXT("/:53"));
		m_ExpreesionItem[53].InitExpressItem(TEXT(""),TEXT("/:54"));
		m_ExpreesionItem[54].InitExpressItem(TEXT(""),TEXT("/:55"));
		m_ExpreesionItem[55].InitExpressItem(TEXT(""),TEXT("/:56"));
		m_ExpreesionItem[56].InitExpressItem(TEXT(""),TEXT("/:57"));
		m_ExpreesionItem[57].InitExpressItem(TEXT(""),TEXT("/:58"));
		m_ExpreesionItem[58].InitExpressItem(TEXT(""),TEXT("/:59"));
		m_ExpreesionItem[59].InitExpressItem(TEXT(""),TEXT("/:60"));

		m_ExpreesionItem[60].InitExpressItem(TEXT(""),TEXT("/:61"));
		m_ExpreesionItem[61].InitExpressItem(TEXT(""),TEXT("/:62"));
		m_ExpreesionItem[62].InitExpressItem(TEXT(""),TEXT("/:63"));
		m_ExpreesionItem[63].InitExpressItem(TEXT(""),TEXT("/:64"));
		m_ExpreesionItem[64].InitExpressItem(TEXT(""),TEXT("/:65"));
		m_ExpreesionItem[65].InitExpressItem(TEXT(""),TEXT("/:66"));
		m_ExpreesionItem[66].InitExpressItem(TEXT(""),TEXT("/:67"));
		m_ExpreesionItem[67].InitExpressItem(TEXT(""),TEXT("/:68"));
		m_ExpreesionItem[68].InitExpressItem(TEXT(""),TEXT("/:69"));
		m_ExpreesionItem[69].InitExpressItem(TEXT(""),TEXT("/:70"));

		m_ExpreesionItem[70].InitExpressItem(TEXT(""),TEXT("/:71"));
		m_ExpreesionItem[71].InitExpressItem(TEXT(""),TEXT("/:72"));
		m_ExpreesionItem[72].InitExpressItem(TEXT(""),TEXT("/:73"));
		m_ExpreesionItem[73].InitExpressItem(TEXT(""),TEXT("/:74"));
		m_ExpreesionItem[74].InitExpressItem(TEXT(""),TEXT("/:75"));
		m_ExpreesionItem[75].InitExpressItem(TEXT(""),TEXT("/:76"));
		m_ExpreesionItem[76].InitExpressItem(TEXT(""),TEXT("/:77"));
		m_ExpreesionItem[77].InitExpressItem(TEXT(""),TEXT("/:78"));
		m_ExpreesionItem[78].InitExpressItem(TEXT(""),TEXT("/:79"));
		m_ExpreesionItem[79].InitExpressItem(TEXT(""),TEXT("/:80"));

		m_ExpreesionItem[80].InitExpressItem(TEXT(""),TEXT("/:81"));
		m_ExpreesionItem[81].InitExpressItem(TEXT(""),TEXT("/:82"));
		m_ExpreesionItem[82].InitExpressItem(TEXT(""),TEXT("/:83"));
		m_ExpreesionItem[83].InitExpressItem(TEXT(""),TEXT("/:84"));
		m_ExpreesionItem[84].InitExpressItem(TEXT(""),TEXT("/:85"));
		m_ExpreesionItem[85].InitExpressItem(TEXT(""),TEXT("/:86"));
		m_ExpreesionItem[86].InitExpressItem(TEXT(""),TEXT("/:87"));
		m_ExpreesionItem[87].InitExpressItem(TEXT(""),TEXT("/:88"));
		m_ExpreesionItem[88].InitExpressItem(TEXT(""),TEXT("/:89"));
		m_ExpreesionItem[89].InitExpressItem(TEXT(""),TEXT("/:90"));

		m_ExpreesionItem[90].InitExpressItem(TEXT(""),TEXT("/:91"));
		m_ExpreesionItem[91].InitExpressItem(TEXT(""),TEXT("/:92"));
		m_ExpreesionItem[92].InitExpressItem(TEXT(""),TEXT("/:93"));
		m_ExpreesionItem[93].InitExpressItem(TEXT(""),TEXT("/:94"));
		m_ExpreesionItem[94].InitExpressItem(TEXT(""),TEXT("/:95"));
		m_ExpreesionItem[95].InitExpressItem(TEXT(""),TEXT("/:96"));
		m_ExpreesionItem[96].InitExpressItem(TEXT(""),TEXT("/:97"));
		m_ExpreesionItem[97].InitExpressItem(TEXT(""),TEXT("/:98"));
		m_ExpreesionItem[98].InitExpressItem(TEXT(""),TEXT("/:99"));
		m_ExpreesionItem[99].InitExpressItem(TEXT(""),TEXT("/:100"));

		m_ExpreesionItem[100].InitExpressItem(TEXT(""),TEXT("/:101"));
		m_ExpreesionItem[101].InitExpressItem(TEXT(""),TEXT("/:102"));
		m_ExpreesionItem[102].InitExpressItem(TEXT(""),TEXT("/:103"));
		m_ExpreesionItem[103].InitExpressItem(TEXT(""),TEXT("/:104"));
		m_ExpreesionItem[104].InitExpressItem(TEXT(""),TEXT("/:105"));
		m_ExpreesionItem[105].InitExpressItem(TEXT(""),TEXT("/:106"));
		m_ExpreesionItem[106].InitExpressItem(TEXT(""),TEXT("/:107"));
		m_ExpreesionItem[107].InitExpressItem(TEXT(""),TEXT("/:108"));
		m_ExpreesionItem[108].InitExpressItem(TEXT(""),TEXT("/:109"));
		m_ExpreesionItem[109].InitExpressItem(TEXT(""),TEXT("/:110"));

		m_ExpreesionItem[110].InitExpressItem(TEXT(""),TEXT("/:111"));
		m_ExpreesionItem[111].InitExpressItem(TEXT(""),TEXT("/:112"));
		m_ExpreesionItem[112].InitExpressItem(TEXT(""),TEXT("/:113"));
		m_ExpreesionItem[113].InitExpressItem(TEXT(""),TEXT("/:114"));
		m_ExpreesionItem[114].InitExpressItem(TEXT(""),TEXT("/:115"));
		m_ExpreesionItem[115].InitExpressItem(TEXT(""),TEXT("/:116"));
		m_ExpreesionItem[116].InitExpressItem(TEXT(""),TEXT("/:117"));
		m_ExpreesionItem[117].InitExpressItem(TEXT(""),TEXT("/:118"));
		m_ExpreesionItem[118].InitExpressItem(TEXT(""),TEXT("/:119"));
		m_ExpreesionItem[119].InitExpressItem(TEXT(""),TEXT("/:120"));

		m_ExpreesionItem[120].InitExpressItem(TEXT(""),TEXT("/:121"));

		//设置资源
		AfxSetResourceHandle(GetModuleHandle(NULL));

		bInit=true;
	}
	return bInit;
}

//建立函数
bool CExpression::CreateExpression(CWnd * pParentWnd)
{
	//设置变量
	m_pParentWnd=pParentWnd;

	//创建窗口
	AfxSetResourceHandle(GetModuleHandle(CLIENT_SHARE_DLL_NAME));
	Create(IDD_EXPRESSION,pParentWnd);
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//设置窗口
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	BringWindowToTop();

	return true;
}

//获取适合大小
bool CExpression::GetFixSize(CSize & Size)
{
	Size.cx=m_wMaxRow*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE;
	Size.cy=m_wMaxLine*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE;
	return true;
}

//获取表情
CExpressionItem * CExpression::GetExpressItem(WORD wIndex)
{
	if (wIndex<MAX_EXPRESS_ITEM) return &m_ExpreesionItem[wIndex];
	return NULL;
}

//失去焦点消息
void CExpression::OnKillFocus(CWnd * pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
	DestroyWindow();
}

//绘画头像
void CExpression::DrawItem(CDC * pDC, int iXPos, int iYPos, bool bSelectec)
{
	CExpressionItem * pExpressionItem=GetExpressItem(iYPos*m_wMaxRow+iXPos);
	int nFaceIndex = iYPos*m_wMaxRow+iXPos;
	if (pExpressionItem!=NULL)
	{
		if (bSelectec)
		{
			pDC->FillSolidRect(iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE/2,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE/2,
				EP_PIC_WHIDE+PIC_SPACE,EP_PIC_HIGHT+PIC_SPACE,RGB(255,255,255));
			pDC->Draw3dRect(iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE/2,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE/2,
				EP_PIC_WHIDE+PIC_SPACE,EP_PIC_HIGHT+PIC_SPACE,RGB(28,57,208),RGB(28,57,208));
		}
		else
		{
			pDC->FillSolidRect(iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE/2,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE/2,
				EP_PIC_WHIDE+PIC_SPACE,EP_PIC_HIGHT+PIC_SPACE,RGB(255,255,255));

		}

		//装载图片
		TCHAR szFileName[MAX_PATH]=TEXT("");
		_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\Face\\%d.gif"),m_szPath, nFaceIndex);
		CImage ImageFace;
		if ( SUCCEEDED(ImageFace.Load(szFileName) ) )
		{
			//背景判断
			COLORREF rfColor = ImageFace.GetPixel(0,0);
			TransparentBlt(pDC->GetSafeHdc(), iXPos*(EP_PIC_WHIDE+PIC_SPACE)+PIC_SPACE,iYPos*(EP_PIC_HIGHT+PIC_SPACE)+PIC_SPACE, 
				EP_PIC_WHIDE, EP_PIC_HIGHT, ImageFace.GetDC(), 0, 0, EP_PIC_WHIDE, EP_PIC_HIGHT, rfColor);
			ImageFace.ReleaseDC();
		}
	}
	return;
}

//重画消息
void CExpression::OnPaint()
{
	CPaintDC dc(this);

	//绘画背景
	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillSolidRect(&rcClient,RGB(255,255,255));
	dc.Draw3dRect(&rcClient,RGB(0,75,151),RGB(0,75,151));

	//绘画表情头像
	for (WORD i=0;i<m_wMaxLine*m_wMaxRow;i++) DrawItem(&dc,i%m_wMaxRow,i/m_wMaxRow,(m_wSelectIndex==i));

	return;
}

//鼠标移动消息
void CExpression::OnMouseMove(UINT nFlags, CPoint point)
{
	WORD wSelectIndex=HitItemTest(point);
	if (wSelectIndex!=m_wSelectIndex)
	{
		CDC * pDC=GetDC();
		if (m_wSelectIndex!=MAX_EXPRESS_ITEM)
		{
			//清除旧选择
			DrawItem(pDC,m_wSelectIndex%m_wMaxRow,m_wSelectIndex/m_wMaxRow,false);
			m_wSelectIndex=MAX_EXPRESS_ITEM;
		}
		m_wSelectIndex=wSelectIndex;
		if (m_wSelectIndex!=MAX_EXPRESS_ITEM)
		{
			//显示新选择
			CExpressionItem * pExpressionItem=GetExpressItem(m_wSelectIndex);
			if (pExpressionItem!=NULL) DrawItem(pDC,m_wSelectIndex%m_wMaxRow,m_wSelectIndex/m_wMaxRow,true);
		}
		ReleaseDC(pDC);
	}

	return;
}

//鼠标按下消息
void CExpression::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((m_wSelectIndex!=MAX_EXPRESS_ITEM)&&(m_pParentWnd!=NULL))
	{
		m_pParentWnd->SendMessage(WM_HIT_EXPMESSTION,(WPARAM)m_wSelectIndex,(LPARAM)m_wSelectIndex);
		DestroyWindow();
	}
	return;
}

//按键测试
WORD CExpression::HitItemTest(CPoint MousePoint)
{
	WORD wXPos=(WORD)(MousePoint.x/(PIC_SPACE+EP_PIC_WHIDE));
	WORD wYPos=(WORD)(MousePoint.y/(PIC_SPACE+EP_PIC_HIGHT));
	wXPos=__min(m_wMaxRow-1,wXPos);
	wYPos=__min(m_wMaxLine-1,wYPos);
	return wYPos*m_wMaxRow+wXPos;
}

//控件绑定
void CExpression::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LAST_PAGE, m_btLastPage);
	//DDX_Control(pDX, IDC_NEXT_PAGE, m_btNextPage);
}

//////////////////////////////////////////////////////////////////////////

BOOL CExpression::OnInitDialog()
{
	CDialog::OnInitDialog();

	//获取目录
	GetCurrentDirectory(sizeof(m_szPath),m_szPath);

	//HINSTANCE hInstance = GetModuleHandle(CLIENT_SHARE_DLL_NAME);
	//m_btNextPage.SetButtonImage(IDB_BT_NEXT_PAGE, hInstance, false);
	//m_btLastPage.SetButtonImage(IDB_BT_LAST_PAGE, hInstance, false);

	//CRect rcClient, rcButton;
	//m_btNextPage.GetClientRect(rcButton);
	//GetClientRect(&rcClient);
	//m_btLastPage.MoveWindow(rcClient.right - rcButton.Width() - 5, rcClient.bottom - 5 - rcButton.Height(), rcButton.Width(), rcButton.Height());
	//m_btNextPage.MoveWindow(rcClient.right - rcButton.Width()*2 - 10, rcClient.bottom - 5 - rcButton.Height(), rcButton.Width(), rcButton.Height());

	return TRUE;  
}
