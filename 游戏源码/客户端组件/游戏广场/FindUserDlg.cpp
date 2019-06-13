#include "StdAfx.h"
#include "FindUserDlg.h"
#include "RoomViewItem.h"

BEGIN_MESSAGE_MAP(CFindUserDlg, CSkinDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)
END_MESSAGE_MAP()

//构造函数
CFindUserDlg::CFindUserDlg(CWnd * pParentWnd, UINT uGameType) : CSkinDialogEx(IDD_FIND_USER)
{
	m_uGameType=uGameType;
	m_pParentWnd=pParentWnd;
}

//析够函数
CFindUserDlg::~CFindUserDlg()
{
}

// DDX/DDV 支持
void CFindUserDlg::DoDataExchange(CDataExchange * pDX)
{
	CSkinDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_BtFind);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDX_Control(pDX, IDC_LIST, m_UserList);
	DDX_Control(pDX, IDC_NAME, m_UserName);
	DDX_Control(pDX, IDC_ACCID, m_UserAcc);
}

//查找用户
void CFindUserDlg::OnOK()
{
	if ((m_UserName.GetWindowTextLength()==0)&&(m_UserAcc.GetWindowTextLength()==0))
	{
		ShowInformation(TEXT("请输入要查找的 用户名字 或者游戏 ID 号码！"),0,MB_ICONQUESTION);
		m_UserName.SetFocus();
		return;
	}
	if (m_pParentWnd!=NULL)
	{
		CString strFindInput;
		CPtrArray UserResultArray;
		m_UserList.DeleteAllItems();

		//搜索用户名字
		CRoomViewItem * pGameRoom=(CRoomViewItem *)m_pParentWnd;
		m_UserName.GetWindowText(strFindInput);
		if (strFindInput.IsEmpty()==false)
		{
			strFindInput.MakeLower();
			WORD wIndex=0;
			CString strBuffer;
			while (true)
			{
				IUserItem * pIUserItem=pGameRoom->m_ClientUserManager.EnumUserItem(wIndex++);
				if (pIUserItem==NULL) break;
				strBuffer=pIUserItem->GetUserData()->szName;
				strBuffer.MakeLower();
				if (strBuffer.Find(strFindInput)!=-1) UserResultArray.Add(pIUserItem);
			}
		}

		//搜索 ACC ID
		m_UserAcc.GetWindowText(strFindInput);
		if (strFindInput.IsEmpty()==false)
		{
			DWORD dwAccID=atol(strFindInput);
			if (dwAccID>0L) 
			{
				IUserItem * pIUserItem=pGameRoom->m_ClientUserManager.SearchUserByGameID(dwAccID);
				if (pIUserItem!=NULL) UserResultArray.Add(pIUserItem);
			}
		}

		if (UserResultArray.GetCount()>0L) 
		{
			for (INT_PTR i=0;i<UserResultArray.GetCount();i++)
			{
				IUserItem * pFindUserItem=(IUserItem *)UserResultArray[i];
				m_UserList.InsertUserItem(pFindUserItem);
			}
		}
		else 
		{
			ShowInformation(TEXT("找不到适合条件的用户信息！"),0,MB_ICONQUESTION);
			m_UserName.SetFocus();
		}
	}
	return;
}

//初始化函数
BOOL CFindUserDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText(TEXT("查找玩家："));

	//设置控件
	CRoomViewItem * pGameRoom=(CRoomViewItem *)m_pParentWnd;
	m_UserList.InitUserListView();
	m_UserName.LimitText(NAME_LEN);

	return TRUE;
}

//单击鼠标右键
void CFindUserDlg::OnNMRclickList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	if ((pListNotify->iItem!=-1)&&(m_pParentWnd!=NULL))
	{
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		CRoomViewItem * pGameRoom=(CRoomViewItem *)m_pParentWnd;
		IUserItem * pUserItem=(IUserItem *)m_UserList.GetItemData(pListNotify->iItem);
		if (pUserItem==NULL) return;
		pGameRoom->ShowUserInfoMenu(pUserItem,MousePoint);
	}
	*pResult = 0;
}

//更新用户信息
void CFindUserDlg::UpdateUserItem(IUserItem * pUserItem)
{
	if (m_UserList.GetSafeHwnd()!=NULL) m_UserList.UpdateUserItem(pUserItem);
	return;
}

//删除用户信息
void CFindUserDlg::DeleteUserItem(IUserItem * pUserItem)
{
	if (m_UserList.GetSafeHwnd()!=NULL) m_UserList.DeleteUserItem(pUserItem);
	return;
}
