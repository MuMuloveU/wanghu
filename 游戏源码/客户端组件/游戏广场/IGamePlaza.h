#ifndef IGAME_PLAZA_HEAD_FILE
#define IGAME_PLAZA_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

#define VER_IViewItem INTERFACE_VERSION(1,1)
static const GUID IID_IViewItem={0xb15641a1,0xb3b0,0x4d8c,0xb4,0xaf,0xa7,0x3a,0x5f,0xc,0xac,0x54};

//视图项接口
interface IViewItem : public IUnknownEx
{
	//获取句柄
	virtual HWND __cdecl GetWindowHandle()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IPlazaViewItem INTERFACE_VERSION(1,1)
static const GUID IID_IPlazaViewItem={0x9538925b,0x19a8,0x433b,0xa4,0x2a,0x9d,0x23,0x5f,0x72,0x79,0x81};

//广场视图项接口
interface IPlazaViewItem : public IViewItem
{
	//登录消息
	virtual bool __cdecl SendLogonMessage()=NULL;
	//连接消息
	virtual bool __cdecl SendConnectMessage()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IRoomViewItem INTERFACE_VERSION(2,1)
static const GUID IID_IRoomViewItem={0xa3af02ff,0x89f0,0x4b30,0x82,0x6,0xda,0x1d,0x19,0x11,0x58,0x29};

//房间视图项接口
interface IRoomViewItem : public IViewItem
{
	//关闭询问
	virtual bool __cdecl QueryCloseRoom()=NULL;
	//关闭房间
	virtual void __cdecl CloseRoomViewItem()=NULL;;
	//类型信息
	virtual tagGameKind * __cdecl GetKindInfo()=NULL;
	//房间信息
	virtual tagGameServer * __cdecl GetServerInfo()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IUserItem={0x2850679d,0xb76c,0x435d,0x81,0x3b,0xd3,0x8,0x9e,0x91,0x16,0x1f};

//用户信息
interface IUserItem : public IUnknownEx
{
	//对象接口
public:
	//访问判断
	virtual bool __cdecl IsActive()=NULL;

	//属性接口
public:
	//用户 I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//用户名字
	virtual LPCTSTR __cdecl GetUserName()=NULL;
	//游戏局数
	virtual LONG __cdecl GetUserPlayCount()=NULL;
	//获取用户
	virtual tagUserData * __cdecl GetUserData()=NULL;

	//设置接口
public:
	//设置积分
	virtual void __cdecl SetUserScore(const tagUserScore * pUserScore)=NULL;
	//设置状态
	virtual void __cdecl SetUserStatus(const tagUserStatus * pUserStatus)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IScoreParser INTERFACE_VERSION(1,1)
static const GUID IID_IScoreParser={0xa7a3b158,0x7d57,0x4c67,0x8d,0x5b,0x0,0xab,0xe5,0x50,0x7f,0x42};

//分数解释
interface IScoreParser : public IUnknownEx
{
	//设置分数解释
	virtual bool __cdecl SetScoreDescribe(WORD wDataDescribe[], WORD wDescribeCount)=NULL;
	//更新用户分数
	virtual bool __cdecl UpdateUserScore(IUserItem * pIUserItem, BYTE cbScoreBuffer[], WORD wDataSize)=NULL;
	//是否存在分数
	virtual bool __cdecl ExistPoint()=NULL;
	//是否存在金币
	virtual bool __cdecl ExistGold()=NULL;
	//是否存在胜局
	virtual bool __cdecl ExistWinCount()=NULL;
	//是否存在输局
	virtual bool __cdecl ExistLostCount()=NULL;
	//是否存在和局
	virtual bool __cdecl ExistDrawCount()=NULL;
	//是否存在逃局
	virtual bool __cdecl ExistFleeCount()=NULL;
	//是否存在经验
	virtual bool __cdecl ExistExperience()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientUserManager INTERFACE_VERSION(1,1)
static const GUID IID_IClientUserManager={0xe55db51d,0x46a8,0x443e,0xa8,0x93,0x28,0xdc,0xc4,0xad,0x52,0x20};

//用户管理
interface IClientUserManager : public IUnknownEx
{
	//管理接口
public:
	//设置回调接口
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx)=NULL;
	//增加用户
	virtual IUserItem * __cdecl ActiveUserItem(tagUserData & UserData)=NULL;
	//删除用户
	virtual bool __cdecl DeleteUserItem(IUserItem * pIUserItem)=NULL;
	//更新积分
	virtual bool __cdecl UpdateUserItemScore(IUserItem * pIUserItem, const tagUserScore * pUserScore)=NULL;
	//更新状态
	virtual bool __cdecl UpdateUserItemStatus(IUserItem * pIUserItem, const tagUserStatus * pUserStatus)=NULL;

	//信息接口
public:
	//获取人数
	virtual DWORD __cdecl GetOnLineCount()=NULL;

	//查找接口
public:
	//枚举用户
	virtual IUserItem * __cdecl EnumUserItem(WORD wEnumIndex)=NULL;
	//查找用户
	virtual IUserItem * __cdecl SearchUserByUserID(DWORD dwUserID)=NULL;
	//查找用户
	virtual IUserItem * __cdecl SearchUserByGameID(DWORD dwUserID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IUserManagerSink INTERFACE_VERSION(1,1)
static const GUID IID_IUserManagerSink={0xd46b0de0,0xb1fc,0x4835,0x8e,0x85,0x37,0x9a,0x7e,0xf3,0xb3,0xa0};

//用户回调
interface IUserManagerSink : public IUnknownEx
{
	//用户激活
	virtual void __cdecl OnUserItemAcitve(IUserItem * pIUserItem)=NULL;
	//用户更新
	virtual void __cdecl OnUserItemUpdate(IUserItem * pIUserItem)=NULL;
	//用户删除
	virtual void __cdecl OnUserItemDelete(IUserItem * pIUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#endif