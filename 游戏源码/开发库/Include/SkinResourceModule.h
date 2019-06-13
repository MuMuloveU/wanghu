#ifndef SKIN_RESOURCE_MODULE_HEAD_FILE
#define SKIN_RESOURCE_MODULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台头文件
//#include "Template.h"
#include "GlobalProperty.h"
#include "SkinControls.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#define RESOURCE_DEFAULT			0									//蓝色资源
#define RESOURCE_BLACK				1									//黑色资源
#define RESOURCE_RED				2									//红色资源

//////////////////////////////////////////////////////////////////////////

//模块定义
#ifndef _DEBUG
	#define SKIN_RESOURCE_DLL_NAME	TEXT("SkinResource.dll")			//组件 DLL 名字
#else
	#define SKIN_RESOURCE_DLL_NAME	TEXT("SkinResourceD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构定义

//框架资源
struct tagPlatformFrameImage
{
	//框架资源
	UINT							uFrameML;							//资源标识
	UINT							uFrameMR;							//资源标识
	UINT							uFrameTL;							//资源标识
	UINT							uFrameTM;							//资源标识
	UINT							uFrameTR;							//资源标识
	UINT							uFrameBL;							//资源标识
	UINT							uFrameBM;							//资源标识
	UINT							uFrameBR;							//资源标识

	//列表资源
	UINT							uServerML;							//资源标识
	UINT							uServerMR;							//资源标识
	UINT							uServerTL;							//资源标识
	UINT							uServerTM;							//资源标识
	UINT							uServerTR;							//资源标识
	UINT							uServerBL;							//资源标识
	UINT							uServerBM;							//资源标识
	UINT							uServerBR;							//资源标识

	//框架按钮
	UINT							uBtFrameMin;						//资源标识
	UINT							uBtFrameClose;						//资源标识
	UINT							uBtFrameMax;						//资源标识
	UINT							uBtFrameResore;						//资源标识

	UINT							uBtFrameSwitch;						//资源标识
	UINT							uBtFrameSelfSet;					//资源标识
	UINT							uBtFrameOption;						//资源标识

	UINT							uFrameServiceBtShow1;				//资源标识
	UINT							uFrameServiceBtHide1;				//资源标识
	UINT							uFrameServiceBtShow2;				//资源标识
	UINT							uFrameServiceBtHide2;				//资源标识

	//切换按钮
	UINT							uBtShowGameList;					//资源标识
	UINT							uBtHideGameList;					//资源标识
	UINT							uBtFrameItemNormal;					//资源标识
	UINT							uBtFrameItemActive;					//资源标识

	//导航按钮
	UINT							uFrameNavigation[5];				//资源标识
};

//广场资源
struct tagPlazaViewImage
{
	//浏览资源
	UINT							uBrowseML;							//资源标识
	UINT							uBrowseMR;							//资源标识
	UINT							uBrowseTL;							//资源标识
	UINT							uBrowseTM;							//资源标识
	UINT							uBrowseTR;							//资源标识
	UINT							uBrowseBL;							//资源标识
	UINT							uBrowseBM;							//资源标识
	UINT							uBrowseBR;							//资源标识

	//按钮资源
	UINT							uBtWebLast;							//资源标识
	UINT							uBtWebNext;							//资源标识
	UINT							uBtWebStop;							//资源标识
	UINT							uBtWebHome;							//资源标识
	UINT							uBtWebReload;						//资源标识
};

//房间资源
struct tagServerViewImage
{
	//聊天资源
	UINT							uChatML;							//资源标识
	UINT							uChatMR;							//资源标识
	UINT							uChatTL;							//资源标识
	UINT							uChatTM;							//资源标识
	UINT							uChatTR;							//资源标识
	UINT							uChatBL;							//资源标识
	UINT							uChatBM;							//资源标识
	UINT							uChatBR;							//资源标识

	//桌子资源
	UINT							uTableML;							//资源标识
	UINT							uTableMR;							//资源标识
	UINT							uTableTL;							//资源标识
	UINT							uTableTM;							//资源标识
	UINT							uTableTR;							//资源标识
	UINT							uTableBL;							//资源标识
	UINT							uTableBM;							//资源标识
	UINT							uTableBR;							//资源标识

	//列表资源
	UINT							uUserListML;						//资源标识
	UINT							uUserListMR;						//资源标识
	UINT							uUserListTL;						//资源标识
	UINT							uUserListTM;						//资源标识
	UINT							uUserListTR;						//资源标识
	UINT							uUserListBL;						//资源标识
	UINT							uUserListBM;						//资源标识
	UINT							uUserListBR;						//资源标识

	//聊天按钮
	UINT							uBtColorSet;						//资源标识
	UINT							uBtSendChat;						//资源标识
	UINT							uBtExpression;						//资源标识
	UINT							uBtBugle;							//资源标识
	UINT							uBtCleanScreen;						//资源标识
	//UINT							uBtPhrase;							//资源标识
	//UINT							uBtSaveChat;						//资源标识
	//UINT							uBtChatShort;						//资源标识
	//UINT							uBtMoreFunction;					//资源标识

	//桌子按钮
	UINT							uBtAutoJoin;						//资源标识
	UINT							uBtBank;							//资源标识
	UINT							uBtSearchUser;						//资源标识
	UINT							uBtQuitRoom;						//资源标识
	//UINT							uBtViewMode;						//资源标识
	//UINT							uBtCallMaster;						//资源标识
	//UINT							uBtGoldManager;						//资源标识
	//UINT							uBtServerManage;					//资源标识

	//切换按钮
	UINT							uBtShowUserList;					//资源标识
	UINT							uBtHideUserList;					//资源标识
	//UINT							uBtCustomUserNormal;				//资源标识
	//UINT							uBtNormalUserNormal;				//资源标识
	//UINT							uBtCustomUserActive;				//资源标识
	//UINT							uBtNormalUserActive;				//资源标识
};

//私聊资源
struct tagWhisperViewImage
{
	//聊天资源
	UINT							uChatML;							//资源标识
	UINT							uChatMR;							//资源标识
	UINT							uChatTL;							//资源标识
	UINT							uChatTM;							//资源标识
	UINT							uChatTR;							//资源标识
	UINT							uChatBL;							//资源标识
	UINT							uChatBM;							//资源标识
	UINT							uChatBR;							//资源标识

	//用户资源
	UINT							uUserML;							//资源标识
	UINT							uUserMR;							//资源标识
	UINT							uUserTL;							//资源标识
	UINT							uUserTM;							//资源标识
	UINT							uUserTR;							//资源标识
	UINT							uUserBL;							//资源标识
	UINT							uUserBM;							//资源标识
	UINT							uUserBR;							//资源标识

	//输入资源
	UINT							uInputML;							//资源标识
	UINT							uInputMR;							//资源标识
	UINT							uInputTL;							//资源标识
	UINT							uInputTM;							//资源标识
	UINT							uInputTR;							//资源标识
	UINT							uInputBL;							//资源标识
	UINT							uInputBM;							//资源标识
	UINT							uInputBR;							//资源标识

	//按钮资源
	UINT							uBtAddUser;							//资源标识
	UINT							uBtColorSet;						//资源标识
	UINT							uBtLeaveChat;						//资源标识
	UINT							uBtExpression;						//资源标识
	UINT							uBtCleanScreen;						//资源标识
	UINT							uBtMoreFunction;					//资源标识

	//控制按钮
	UINT							uBtWhisper;							//资源标识
	UINT							uBtSendControl;						//资源标识

	//其他资源
	UINT							uChatSplitter;						//资源标识
};

//控件资源
struct tagControlViewImage
{
	//用户信息
	UINT							uUserInfoL;							//资源标识
	UINT							uUserInfoM;							//资源标识
	UINT							uUserInfoR;							//资源标识
	//UINT							uBtUserInfoMenu;					//资源标识

	//设置资源
	UINT							uCollocateBarActive;				//资源标识
	UINT							uCollocateBarBackground;			//资源标识
	UINT							uCollocateBarHot;					//资源标识
	UINT							uCollocateBarNormal;				//资源标识
};

//道具资源
struct tagPropertyViewImage
{
	//按钮资源
	UINT							uBtGift[FLOWER_COUNT];				//资源标识
	UINT							uBtProperty[PROPERTY_COUNT];		//资源标识

	//版图资源
	UINT							uPropertyFormer[PROPERTY_COUNT];	//资源标识

	//兑换资源
	LPCTSTR							pszExChagneArrowhead;				//资源标识

	//拉八资源
	UINT							uBtBugleColorSet;						//资源标识
	UINT							uBtBugleExpression;						//资源标识

	//框架资源
	UINT							uPropertyFrameL;						//资源标识
	UINT							uPropertyFrameM;						//资源标识
	UINT							uPropertyFrameR;						//资源标识
	UINT							uBtPropertyToLeft;						//资源标识
	UINT							uBtPropertyToRight;						//资源标识
};

//表情资源
struct tagChatExpViewImage
{
	//聊框资源
	UINT							uChatBugle;							//资源标识
	UINT							uChatGift[FLOWER_COUNT];			//资源标识
};

//游戏框架
struct tagGameFrameImage
{
	//聊天资源
	UINT							uChatML;							//资源标识
	UINT							uChatMR;							//资源标识
	UINT							uChatTL;							//资源标识
	UINT							uChatTM;							//资源标识
	UINT							uChatTR;							//资源标识
	UINT							uChatBL;							//资源标识
	UINT							uChatBM;							//资源标识
	UINT							uChatBR;							//资源标识

	//列表资源
	UINT							uUserListML;						//资源标识
	UINT							uUserListMR;						//资源标识
	UINT							uUserListTL;						//资源标识
	UINT							uUserListTM;						//资源标识
	UINT							uUserListTR;						//资源标识
	UINT							uUserListBL;						//资源标识
	UINT							uUserListBM;						//资源标识
	UINT							uUserListBR;						//资源标识

	//用户资源
	UINT							uBtExchangeCharm;					//资源标识
	UINT							uFemaleFace;						//资源标识
	UINT							uMaleFace;							//资源标识

	//聊天按钮
	UINT							uBtColorSet;						//资源标识
	UINT							uBtSendChat;						//资源标识
	UINT							uBtExpression;						//资源标识
	UINT							uBtBugle;							//资源标识
	UINT							uBtCleanScreen;						//资源标识

	//切换按钮
	UINT							uBtShowUserList;					//资源标识
	UINT							uBtHideUserList;					//资源标识

	//设置按钮
	UINT							uBtGameOption;						//资源标识
	UINT							uBtGameRule;						//资源标识
	UINT							uBtGameLookOn;						//资源标识
	UINT							uBtGameUnLookOn;					//资源标识
	UINT							uBtGameSound;						//资源标识
	UINT							uBtGameUnSound;						//资源标识
};

//视频资源
struct tagVideoFrameImage 
{
	//功能按钮
	UINT							uBtBigFlower  ; 					//资源标识
	UINT							uBtBigMore    ; 					//资源标识
	UINT							uBtBigNosee   ; 					//资源标识
	UINT							uBtBigNotalk  ; 					//资源标识
	UINT							uBtBigResore  ; 					//资源标识
	UINT							uBtBigSee     ; 					//资源标识
	UINT							uBtBigTalk    ; 					//资源标识
	UINT							uBtSmallFlower; 					//资源标识
	UINT							uBtSmallMore  ; 					//资源标识
	UINT							uBtSmallNosee ; 					//资源标识
	UINT							uBtSmallNotalk; 					//资源标识
	UINT							uBtSmallResore; 					//资源标识
	UINT							uBtSmallSee   ; 					//资源标识
	UINT							uBtSmallTalk  ; 					//资源标识

	//框架资源
	UINT							uBigFemale		; 					//资源标识
	UINT							uBigFrame		 ; 					//资源标识 
	UINT							uBigMale		 ; 					//资源标识 
	UINT							uBigPhoto		; 					//资源标识
	UINT							uBigVolume		; 					//资源标识
	UINT							uBigVolumeEx	 ; 					//资源标识 
	UINT							uSmallFemale	 ; 					//资源标识   
	UINT							uSmallFrame	; 					//资源标识  
	UINT							uSmallMale		; 					//资源标识
	UINT							uSmallPhoto	; 					//资源标识  
	UINT							uSmallVolume	; 					//资源标识  
	UINT							uSmallVolumeEx; 					//资源标识 
};

//道具菜单
struct tagPropertyMenuImage
{
	UINT							uMenuGift[FLOWER_COUNT];			//资源标识	
};

//动画资源
struct tagGiftFlashImage
{
	//移动资源
	LPCTSTR							pszGiftMove[FLOWER_COUNT];			//资源标识	
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPlatformResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlatformResource={0x91931013,0x9969,0x44f4,0x0096,0xb6,0x88,0x61,0x49,0xba,0x52,0xa0};
#else
	#define VER_IPlatformResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlatformResource={0xd280055a,0x12cb,0x429e,0x00ab,0x09,0x96,0xf4,0x27,0xa5,0xb0,0xc5};
#endif

//游戏资源
interface IPlatformResource : public IUnknownEx
{
	//内核接口
public:
	//资源句柄
	virtual HINSTANCE __cdecl GetResInstance()=NULL;
	//获取预览
	virtual bool __cdecl GetSkinPreview(CImage & ImagePreview)=NULL;

	//资源类型
public:
	//资源类型
	virtual void __cdecl SetResourceType(BYTE cbResourceType)=NULL;

	//界面资源
public:
	//界面资源
	virtual VOID * __cdecl GetSkinResource(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//大厅资源
public:
	//广场资源
	virtual bool __cdecl GetPlazaViewImage(tagPlazaViewImage * PlazaViewImage)=NULL;
	//房间资源
	virtual bool __cdecl GetServerViewImage(tagServerViewImage * ServerViewImage)=NULL;
	//框架资源
	virtual bool __cdecl GetPlatformFrameImage(tagPlatformFrameImage * PlatformFrameImage)=NULL;

	//附加资源
public:
	//私聊资源
	virtual bool __cdecl GetWhisperViewImage(tagWhisperViewImage * WhisperViewImage)=NULL;
	//控件资源
	virtual bool __cdecl GetControlViewImage(tagControlViewImage * ControlViewImage)=NULL;
	//道具资源
	virtual bool __cdecl GetPropertyViewImage(tagPropertyViewImage * PropertyViewImage)=NULL;
	//表情资源
	virtual bool __cdecl GetChatExpViewImage(tagChatExpViewImage * ChatExpViewImage)=NULL;
	//游戏框架
	virtual bool __cdecl GetGameFrameImage(tagGameFrameImage * GameFrameImage)=NULL;
	//视频资源
	virtual bool __cdecl GetVideoFrameImage(tagVideoFrameImage * VideoFrameImage)=NULL;
	//道具菜单
	virtual bool __cdecl GetPropertyMenuImage(tagPropertyMenuImage * PropertyMenuImage)=NULL;
	//动画资源
	virtual bool __cdecl GetGiftFlashImage(tagGiftFlashImage * GiftFlashImage)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//组件辅助类

DECLARE_MODULE_HELPER(SkinResource,SKIN_RESOURCE_DLL_NAME,"CreateSkinResource")
DECLARE_MODULE_HELPER(PlatformResource,SKIN_RESOURCE_DLL_NAME,"CreatePlatformResource")

//////////////////////////////////////////////////////////////////////////

#endif