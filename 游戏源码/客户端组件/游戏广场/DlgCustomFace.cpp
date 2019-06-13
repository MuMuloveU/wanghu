#include "stdafx.h"
#include"resource.h"
#include "DlgCustomFace.h"
#include "GameFrame.h"
#include "GlobalUnits.h"
#include "Zip.h"

////////////////////////////////////////////////////////////////////////////////////
//宏定义
#define DEF_INSIDEBORDER_COLOR	RGB(176,20,1)						//默认颜色

#define BGROUND_COLOR			RGB(254,250,221)					//背景颜色
#define FRAME_TL_COLOR1			RGB(128,128,128)					//边框颜色
#define FRAME_TL_COLOR2			RGB(64,64,64)						//边框颜色
#define FRAME_RB_COLOR2			RGB(212,208,200)					//边框颜色
#define FRAME_RB_COLOR1			RGB(212,208,200)					//边框颜色

#define SMALL_FACE_WIDTH		32									//图片大小
#define SMALL_FACE_HEIGHT		32									//图片大小

#define LARGE_FACE_WIDTH		100									//图片大小
#define LARGE_FACE_HEIGHT		100									//图片大小

#define SMALL_FRAME_LEFT		235									//边框位置
#define SMALL_FRAME_RIGHT		(SMALL_FRAME_LEFT+SMALL_FACE_WIDTH)	//边框位置
#define SMALL_FRAME_TOP			105									//边框位置
#define SMALL_FRAME_BOTTOM		(SMALL_FRAME_TOP+SMALL_FACE_HEIGHT)	//边框位置

#define LARGE_FRAME_LEFT		50									//边框位置
#define LARGE_FRAME_RIGHT		(LARGE_FRAME_LEFT+LARGE_FACE_WIDTH)	//边框位置
#define LARGE_FRAME_TOP			75									//边框位置
#define LARGE_FRAME_BOTTOM		(LARGE_FRAME_TOP+LARGE_FACE_HEIGHT)	//边框位置

#define FRAME_SPACE				2									//边框宽度

////////////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CDlgCustomFace, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BROWSER,OnBnClickedBrowser)
	ON_BN_CLICKED(IDC_UPLOAD,OnBnClickedUpload) 
	ON_BN_CLICKED(IDC_DELETE,OnBnClickedDelete) 
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgCustomFace::CDlgCustomFace(CWnd* pParent): CDialog(IDD_CUSTOM_FACE, pParent), CSkinWndObject(this)
{
	//设置变量
	m_pIClientSocket = NULL;
	m_enOperateStatus=enOperateStatus_NULL;
}

//虚构函数
CDlgCustomFace::~CDlgCustomFace()
{
	//释放资源
	if ( !m_Image.IsNull() ) m_Image.Destroy();
	if (m_brBkground.m_hObject != NULL ) m_brBkground.DeleteObject();
}

//交换函数
void CDlgCustomFace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BROWSER, m_btBrowser);
	DDX_Control(pDX, IDC_UPLOAD, m_btUpload);
	DDX_Control(pDX, IDC_DELETE, m_btDelete);
}

//初始函数
BOOL CDlgCustomFace::OnInitDialog()
{
	__super::OnInitDialog();

	//创建刷子
	m_brBkground.CreateSolidBrush(BGROUND_COLOR);

	//设置标题
	SetWindowText(TEXT("自定义头像"));

	//更新控件
	UpdateControls();

	//初始化设置
	__super::InitSkinObject();

	return TRUE;  
}

//绘画函数
void CDlgCustomFace::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);
	int nXExcursion=GetXExcursionPos();
	int nYExcursion=GetYExcursionPos();

	//绘画内框
	CPen BorderPen(PS_SOLID,1,m_SkinAttribute.m_crInsideBorder);
	CPen * pOldPen=dc.SelectObject(&BorderPen);
	CRect rcDrawRect(nXExcursion,nYExcursion,rcRect.Width()-2*nXExcursion,rcRect.Height());
	dc.SelectObject(m_SkinAttribute.m_brBackGround);
	if (m_bMaxSize==true) rcDrawRect.DeflateRect(3,3,3,2);
	else rcDrawRect.DeflateRect(3,4,3,4);
	dc.RoundRect(&rcDrawRect,CPoint(8,8));
	dc.SelectObject(pOldPen);
	BorderPen.DeleteObject();

	//绘画边框
	CRect rcSmallFace(SMALL_FRAME_LEFT-FRAME_SPACE, SMALL_FRAME_TOP-FRAME_SPACE, SMALL_FRAME_RIGHT+FRAME_SPACE, SMALL_FRAME_BOTTOM+FRAME_SPACE);
	dc.Draw3dRect(rcSmallFace, FRAME_TL_COLOR1, FRAME_RB_COLOR1);
	rcSmallFace.DeflateRect(1,1,1,1);
	dc.Draw3dRect(rcSmallFace, FRAME_TL_COLOR2, FRAME_RB_COLOR2);

	//绘画边框
	CRect rcLargeFace(LARGE_FRAME_LEFT-FRAME_SPACE, LARGE_FRAME_TOP-FRAME_SPACE, LARGE_FRAME_RIGHT+FRAME_SPACE, LARGE_FRAME_BOTTOM+FRAME_SPACE);
	dc.Draw3dRect(rcLargeFace, FRAME_TL_COLOR1, FRAME_RB_COLOR1);
	rcLargeFace.DeflateRect(1,1,1,1);
	dc.Draw3dRect(rcLargeFace, FRAME_TL_COLOR2, FRAME_RB_COLOR2);

	//绘画头像
	tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
	if ( !m_Image.IsNull() )
	{
		CDC *pDC = GetDC() ;

		//窗体大小
		CRect rcSmallFace(SMALL_FRAME_LEFT, SMALL_FRAME_TOP, SMALL_FRAME_RIGHT, SMALL_FRAME_BOTTOM);
		CRect rcLargeFace(LARGE_FRAME_LEFT, LARGE_FRAME_TOP, LARGE_FRAME_RIGHT, LARGE_FRAME_BOTTOM);

		//设置模式
		dc.SetStretchBltMode(HALFTONE);

		//绘画图片
		m_Image.StretchBlt(dc.GetSafeHdc(), rcSmallFace);
		m_Image.StretchBlt(dc.GetSafeHdc(), rcLargeFace);

		//释放资源
		ReleaseDC(pDC);
	}
	else if(GlobalUserData.dwUserID!=0 && GlobalUserData.dwCustomFaceVer!=0)
	{
		CDC *pDC = GetDC() ;
		g_GlobalUnits.m_UserFaceRes->DrawNormalFace(pDC,SMALL_FRAME_LEFT,SMALL_FRAME_TOP,
			GlobalUserData.wFaceID,GlobalUserData.dwUserID,GlobalUserData.dwCustomFaceVer);

		g_GlobalUnits.m_UserFaceRes->DrawCustomBigFace(pDC,LARGE_FRAME_LEFT,LARGE_FRAME_TOP,
			GlobalUserData.dwUserID,GlobalUserData.dwCustomFaceVer);

		//释放资源
		ReleaseDC(pDC);
	}

	//绘画标题
	__super::DrawSkinView(&dc);
}

//浏览图片
void CDlgCustomFace::OnBnClickedBrowser()
{
	//选择文件
	LPCTSTR pszFilter=TEXT("支持格式(*.bmp;*.jpg;*.gif;*.png)|*.bmp; *.jpg; *.jpeg; *.jpe;*.jfif;*.gif;*.png|所有文件 (*.*)|*.*||");
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,pszFilter);

	if(IDOK == FileDlg.DoModal())
	{
		//加载图片
		CString strName = FileDlg.GetPathName() ;

		//释放判断
		if ( !m_Image.IsNull() ) m_Image.Destroy();

		HRESULT hResult = m_Image.Load(strName);

		//成功判断
		if ( SUCCEEDED(hResult) )
		{
			//更新界面
			CRect rcSmallFace(SMALL_FRAME_LEFT, SMALL_FRAME_TOP, SMALL_FRAME_RIGHT, SMALL_FRAME_BOTTOM);
			CRect rcLargeFace(LARGE_FRAME_LEFT, LARGE_FRAME_TOP, LARGE_FRAME_RIGHT, LARGE_FRAME_BOTTOM);
			InvalidateRect(rcSmallFace, FALSE);
			InvalidateRect(rcLargeFace, FALSE);
		}
		else
		{
			ShowMessage(TEXT("加载图片失败，请重新选择！"));
			return ;
		}
	}

	//更新控件
	UpdateControls();
}

//上传图片
void CDlgCustomFace::OnBnClickedUpload()
{
	if ( m_Image.IsNull() )
	{
		ShowMessage(TEXT("请选择图片！"));
		return ;
	}

	//设置状态
	m_enOperateStatus = enOperateStatus_Upload;

	//链接网络
	if ( ConnectServer() == false )
	{
		//设置状态
		m_enOperateStatus = enOperateStatus_NULL;
		return;
	}

	//获取DC
	CDC *pDC = GetDC();

	LONG lBmpSize = LARGE_FACE_HEIGHT * (LARGE_FACE_WIDTH*3+LARGE_FACE_WIDTH%4) ;
	BYTE *pByteBmpData = (BYTE *)VirtualAlloc(NULL,lBmpSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	//构造位图
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;

	bfh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bfh.bfReserved1=0;
	bfh.bfReserved2=0;
	bfh.bfType = 0x4D42;
	bfh.bfSize=lBmpSize+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) ;

	bih.biBitCount=24;
	bih.biClrImportant=0;
	bih.biClrUsed=0;
	bih.biCompression=BI_RGB;
	bih.biHeight=LARGE_FACE_HEIGHT;
	bih.biPlanes=1;
	bih.biSize=sizeof(bih);
	bih.biSizeImage=0;
	bih.biWidth=LARGE_FACE_WIDTH;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;	

	//设置像素
	WORD wWidth = (LARGE_FACE_WIDTH*3+LARGE_FACE_WIDTH%4);
	for ( int nCol = 0, nRealCol = LARGE_FACE_HEIGHT-1; nCol < LARGE_FACE_HEIGHT; ++nCol, --nRealCol )
		for ( WORD wRow = 0, wByteIdex = 0; wRow < LARGE_FACE_WIDTH; wRow++, wByteIdex+=3)
		{
			COLORREF clr = pDC->GetPixel(LARGE_FRAME_LEFT+wRow, LARGE_FRAME_TOP+nCol);
			pByteBmpData[nRealCol*wWidth+wByteIdex] = GetBValue(clr);
			pByteBmpData[nRealCol*wWidth+wByteIdex+1] = GetGValue(clr);
			pByteBmpData[nRealCol*wWidth+wByteIdex+2] = GetRValue(clr);
		}

	//创建目录
	CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
	CreateDirectory(strDirName, NULL) ;
	CString strFileName;
	strFileName = TEXT("\\MyFace.bmp");

	//写入文件
	CFile fileMyFace;
	try
	{
		if ( fileMyFace.Open(strDirName + strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary) )
		{
			//写入位图
			fileMyFace.Write(&bfh, sizeof(bfh));
			fileMyFace.Write(&bih, sizeof(bih));
			fileMyFace.Write(pByteBmpData, lBmpSize);

			//关闭文件
			fileMyFace.Close();
		}

		//压缩文件
		CZip oZip( strDirName + strFileName );
		oZip.SwapSize( strDirName+TEXT("\\MyFace.zip") );
	}
	catch(...){}			

	//释放资源
	ReleaseDC(pDC);
	VirtualFree(pByteBmpData, lBmpSize , MEM_DECOMMIT) ;

	//更新控件
	UpdateControls();
}

//删除图片
void CDlgCustomFace::OnBnClickedDelete()
{
	//设置状态
	m_enOperateStatus = enOperateStatus_Delete;

	//链接网络
	if ( ConnectServer() == false )
	{
		//设置状态
		m_enOperateStatus = enOperateStatus_NULL;
		return;
	}

	//释放资源
	if ( ! m_Image.IsNull() ) m_Image.Destroy();

	//更新控件
	UpdateControls();

	//更新界面
	InvalidateRect(NULL);
}

//显示消息
void CDlgCustomFace::ShowMessage(TCHAR *pMessage)
{
	ShowInformationEx(pMessage,0,MB_ICONQUESTION,TEXT("自定义头像"));

	return;
}

//网络链接
bool CDlgCustomFace::ConnectServer()
{
	if ( m_pIClientSocket != NULL && m_pIClientSocket->GetSocketStatus() == SOCKET_STATUS_CONNECT && m_enOperateStatus != enOperateStatus_NULL )
	{
		ShowMessage(TEXT("操作正在进行，请稍后！"));
		return false;
	}

	if ( m_pIClientSocket != NULL && m_pIClientSocket->GetSocketStatus() == SOCKET_STATUS_CONNECT )//?GetSocketStatus
	{
		SendData();
		return true;
	}

	//获取大厅
	CGameFrame *pGameFrame = (CGameFrame *)AfxGetMainWnd() ;
	CPlazaViewItem * pPlazaViewItem = &(pGameFrame->m_DlgGamePlaza) ;
	if ( m_pIClientSocket == NULL )
	{

		//合法判断
		if ( pPlazaViewItem->m_ClientSocket.GetInterface() == NULL ) 
		{
			ShowMessage(TEXT("网络组件还没有创建，请重新登录！"));
			return false;
		}

		//设置变量
		m_pIClientSocket = (ITCPSocket*)pPlazaViewItem->m_ClientSocket.GetInterface();
	}

	//地址解释
	CRegKey RegServer;
	LPCTSTR pszServerIP=pPlazaViewItem->m_DlgLogon.GetLogonServer();
	TCHAR szRegServer[256]=TEXT(""),szServerAddr[64]=TEXT("");
	_snprintf(szRegServer,sizeof(szRegServer),TEXT("%s\\%s"),REG_LOGON_SERVER,pszServerIP);

	if (RegServer.Open(HKEY_CURRENT_USER,szRegServer,KEY_READ)==ERROR_SUCCESS)
	{
		TCHAR szReadData[1024]=TEXT("");
		DWORD dwReadData=0L,dwDataType=0L,dwDataSize=sizeof(szReadData);
		LONG lErrorCode=RegServer.QueryValue(TEXT("ServerAddr"),&dwDataType,szReadData,&dwDataSize);
		if (lErrorCode==ERROR_SUCCESS) 
		{
			CXOREncrypt::CrevasseData(szReadData,szServerAddr,sizeof(szServerAddr));
			pszServerIP=szServerAddr;
		}
	}

	//连接服务器
	try
	{
		//连接服务器
		m_pIClientSocket->CloseSocket();
		if (m_pIClientSocket->Connect(pszServerIP,PORT_LOGON_SERVER)!=ERROR_SUCCESS)
		{
			throw TEXT("服务器连接错误，可能是你的系统还没有成功连接上网络！");
		}
	}
	catch (LPCTSTR pszError)
	{
		ShowMessage((TCHAR*)pszError);
		return false;
	}

	return true;
}

int g_nUploadCount = 0;
//发送数据
bool CDlgCustomFace::SendData()
{
	g_nUploadCount = 0;
	//状态判断
	ASSERT(m_enOperateStatus != enOperateStatus_NULL);
	if ( m_enOperateStatus == enOperateStatus_NULL ) return false;

	if ( m_pIClientSocket == NULL || m_pIClientSocket->GetSocketStatus() != SOCKET_STATUS_CONNECT ) return false;

	if ( m_enOperateStatus == enOperateStatus_Upload )
	{
		//读取文件
		CString strDirName = CString(g_GlobalUnits.GetWorkDirectory()) + TEXT("\\CustomFace");
		CString strFileName;
		strFileName = TEXT("\\MyFace.zip");

		CFile fileMyFace;
		try
		{
			//变量定义
			CMD_GP_UploadCustomFace UploadCustomFace;
			UploadCustomFace.dwUserID = g_GlobalUnits.GetGolbalUserData().dwUserID;
			UploadCustomFace.bFirstPacket=true;

			if ( fileMyFace.Open(strDirName + strFileName, CFile::modeRead|CFile::typeBinary) )
			{				
				UINT nReadSize = 0;
				do
				{
					//读取数据
					nReadSize=fileMyFace.Read(UploadCustomFace.bFaceData, CountArray(UploadCustomFace.bFaceData));
					UploadCustomFace.wCurrentSize=nReadSize;

					//完成判断
					if ( nReadSize != CountArray(UploadCustomFace.bFaceData) ) UploadCustomFace.bLastPacket=true;
					else UploadCustomFace.bLastPacket=false;

					//连接判断
					if ( m_pIClientSocket->GetSocketStatus() != SOCKET_STATUS_CONNECT ) break;

					//发送数据
					WORD wSendSize = sizeof(UploadCustomFace) - sizeof(UploadCustomFace.bFaceData) + nReadSize;
					m_pIClientSocket->SendData(MDM_GP_USER, SUB_GP_USER_UPLOAD_FACE, &UploadCustomFace, wSendSize) ;

					Sleep(500);

					//设置标识
					UploadCustomFace.bFirstPacket=false;

				} while(nReadSize==CountArray(UploadCustomFace.bFaceData));

				//关闭文件
				fileMyFace.Close();

				//删除文件
				CFile::Remove(strDirName + strFileName);
			}
			else
			{
				ShowMessage(TEXT("文件操作失败"));
			}
		}
		catch(...){}
	}
	else if ( m_enOperateStatus == enOperateStatus_Delete )
	{
		CMD_GP_CustomFaceDelete CustomFaceDelete;
		ZeroMemory(&CustomFaceDelete, sizeof(CustomFaceDelete));

		tagGlobalUserData &GlobalUserData = g_GlobalUnits.GetGolbalUserData();
		CustomFaceDelete.dwUserID = GlobalUserData.dwUserID;
		CustomFaceDelete.dwFaceVer = GlobalUserData.dwCustomFaceVer;

		//投递消息
		m_pIClientSocket->SendData(MDM_GP_USER, SUB_GP_CUSTOM_FACE_DELETE, &CustomFaceDelete, sizeof(CustomFaceDelete));        

		//预先删除
		g_GlobalUnits.GetGolbalUserData().dwCustomFaceVer=0;

		//更新界面
		InvalidateRect(NULL);
	}

	//设置状态
	m_enOperateStatus = enOperateStatus_NULL ;

	return true;
}

//更新控件
void CDlgCustomFace::UpdateControls()
{
	//变量定义
	tagGlobalUserData & GlobalUserInfo=g_GlobalUnits.GetGolbalUserData();

	if ( GlobalUserInfo.dwCustomFaceVer!=0 ) (GetDlgItem(IDC_DELETE))->EnableWindow(TRUE);
	else (GetDlgItem(IDC_DELETE))->EnableWindow(FALSE);

	if ( m_Image.IsNull() ) (GetDlgItem(IDC_UPLOAD))->EnableWindow(FALSE);
	else (GetDlgItem(IDC_UPLOAD))->EnableWindow(TRUE);
}

//颜色处理
HBRUSH CDlgCustomFace::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			if(pWnd->m_hWnd==GetDlgItem(IDC_WARIMG_MSG)->m_hWnd)
			{
				pDC->SetTextColor(RGB(0,0,255));
			}
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(BGROUND_COLOR);
			return m_SkinAttribute.m_brBackGround;
		}
	}
	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//显示消息
void CDlgCustomFace::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	//更新控件
	if ( bShow ) UpdateControls();
}

//销毁消息
void CDlgCustomFace::OnClose()
{
	//释放资源
	if ( ! m_Image.IsNull() ) m_Image.Destroy();

	m_enOperateStatus = enOperateStatus_NULL;

	CDialog::OnClose();
}

//鼠标信息
void CDlgCustomFace::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
	OnLMouseDownEvent(point);
}

//鼠标信息
void CDlgCustomFace::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);
	OnLMouseUpEvent(point);
}

//鼠标信息
void CDlgCustomFace::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);
	OnMouseMoveEvent(point);
}
////////////////////////////////////////////////////////////////////////////////////