#ifndef COMMONALITY_HEAD_FILE
#define COMMONALITY_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//数据库对象定义

//数据库请求标识
#define	DBR_GP_LOGON_BY_ACCOUNTS	1									//帐号登录
#define	DBR_GP_LOGON_BY_USERID		2									//I D 登录
#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号
#define DBR_GP_USER_UPLOAD_FACE		4									//上传头像
#define DBR_GP_USER_LEAVE			5									//玩家离开
#define DBR_GP_DOWNLOAD_FACE		6									//下载头像
#define DBR_GP_DELETE_FACE			7									//删除头像
#define DBR_GP_MODIFY_INDIVIDUAL	8									//修改资料


//数据库输出标识
#define DBR_GP_LOGON_SUCCESS		100									//登录成功
#define DBR_GP_LOGON_ERROR			101									//登录失败
#define DBR_GP_DOWNLOADFACE_SUCCESS	102									//下载成功
#define DBR_GP_UPLOAD_FACE_RESULT	103									//上传结果
#define DBR_GP_DELETE_FACE_RESULT	104									//删除结果
#define DBR_GP_MODIFY_RESULT		105									//修改结果

//帐号登录
struct DBR_GP_LogonByAccounts
{
	DWORD							dwClientIP;							//连接地址
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//ID 登录
struct DBR_GP_LogonByUserID
{
	DWORD							dwClientIP;							//连接地址
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	WORD							wFaceID;							//注册头像
	BYTE							cbGender;							//用户性别
	DWORD							dwClientIP;							//连接地址
	TCHAR							szSpreader[NAME_LEN];				//推荐人名
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szPassWord[PASS_LEN];				//登录密码
	TCHAR							szComputerID[COMPUTER_ID_LEN];		//机器序列
};

//登录成功
struct DBR_GP_LogonSuccess
{
	//属性资料
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwExperience;						//经验数值
	TCHAR							szPassword[PASS_LEN];				//登录密码
	TCHAR							szAccounts[NAME_LEN];				//登录帐号
	TCHAR							szGroupName[GROUP_LEN];				//社团名字

	//用户资料
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomFaceVer;					//头像版本
	BYTE							cbMember;							//会员等级
	TCHAR							szUnderWrite[UNDER_WRITE_LEN];		//个性签名

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
};

//登录失败
struct DBR_GP_LogonError
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//上传头像
struct DBR_GP_UploadCustomFace
{
	DWORD							dwUserID;							//玩家 ID
};

//上传结果
struct DBR_GP_UploadFaceResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//删除结果
struct DBR_GP_DeleteFaceResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	DWORD							dwFaceVer;							//头像版本
	bool							bOperateSuccess;					//成功标识
};

//下载头像
struct DBR_GP_DownloadCustomFace
{
	DWORD							dwUserID;							//玩家 ID
};

//下载成功
struct DBR_GP_DownloadFaceSuccess
{
	DWORD							dwToltalSize;						//总共大小
	DWORD							dwCurrentSize;						//当前大小
	DWORD							dwUserID;							//玩家 ID
	BYTE							bFaceData[2048];					//头像数据
};

//删除消息
struct DBR_GP_CustomFaceDelete
{
	DWORD							dwUserID;							//玩家 ID
	DWORD							dwFaceVer;							//头像版本
};

//个人资料
struct DBR_GP_ModifyIndividual
{
	DWORD							dwUserID;							//玩家 ID
	TCHAR							szNickname[NAME_LEN];				//玩家昵称
	int								nGender;							//玩家性别
	int								nAge;								//玩家年龄
	TCHAR							szAddress[64];						//玩家地址
	TCHAR							szUnderWrite[32];					//个性签名
	TCHAR							szPassword[33];						//玩家密码
};

//修改结果
struct DBR_GP_ModifyIndividualResult
{
	TCHAR							szDescribeMsg[128];					//描述信息
	bool							bOperateSuccess;					//成功标识
};

//////////////////////////////////////////////////////////////////////////

#endif