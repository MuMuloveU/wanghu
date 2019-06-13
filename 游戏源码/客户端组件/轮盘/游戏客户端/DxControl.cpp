#include "StdAfx.h"
#include "DxControl.h"
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib,"dxguid.lib")

//////////////////////////////////////////////////////////////////

CDxControl::CDxControl()
{
	m_pDXInputKB=NULL;
	m_pDXInputMouse=NULL;
	m_pDXInput=NULL;
}

CDxControl::~CDxControl()
{

}

//初始化输入
LRESULT CDxControl::InitDXInput(HINSTANCE hInst,HWND hWnd)
{
	//创建DXInput对象
	if(DI_OK!=DirectInput8Create(hInst,DIRECTINPUT_VERSION,IID_IDirectInput8,(LPVOID*)&m_pDXInput,NULL))
	{		MessageBox(hWnd,"创建DXInput对象失败","ERROR",MB_OK);			}

	//创建键盘设备
	if(DI_OK!=m_pDXInput->CreateDevice(GUID_SysKeyboard,&m_pDXInputKB,NULL))
	{		MessageBox(hWnd,"创建键盘设备失败","ERROR",MB_OK);				}
	//设置键盘数据格式
	if(DI_OK!=m_pDXInputKB->SetDataFormat(&c_dfDIKeyboard))
	{		MessageBox(hWnd,"MISS","设置键盘数据格式失败",MB_OK);			}
	m_pDXInputKB->SetCooperativeLevel(hWnd,DISCL_EXCLUSIVE|DISCL_NOWINKEY);
	m_pDXInputKB->Acquire();

	//创建鼠标设备
	if(DI_OK!=m_pDXInput->CreateDevice(GUID_SysMouse,&m_pDXInputMouse,NULL))
	{		MessageBox(hWnd,"创建鼠标设备失败","ERROR",MB_OK);				}
	//设置鼠标数据格式
	if(DI_OK!=m_pDXInputMouse->SetDataFormat(&c_dfDIMouse))
	{		MessageBox(hWnd,"设置鼠标数据格式失败","ERROR",MB_OK);			}
	m_pDXInputMouse->SetCooperativeLevel(hWnd,DISCL_EXCLUSIVE|DISCL_FOREGROUND);
	m_pDXInputMouse->Acquire();

	return TRUE;
}

//更新输入
LRESULT CDxControl::UpdateInput()
{
	if(m_pDXInputMouse==NULL||m_pDXInputKB==NULL) return DI_NOTATTACHED;

	//获取鼠标
	if(DI_OK!=m_pDXInputMouse->GetDeviceState(sizeof(m_MouseState),&m_MouseState))
	{	
		memset(&m_MouseState,0,sizeof(m_MouseState));	
		m_pDXInputMouse->Acquire();
	}
	//获取键盘
	if(DI_OK!=m_pDXInputKB->GetDeviceState(sizeof(m_strKeyState),m_strKeyState))
	{	
		memset(&m_strKeyState,0,sizeof(m_strKeyState));	
		m_pDXInputKB->Acquire();
	}

	return DI_OK;
}

//释放设备
void CDxControl::Release()
{
	SAFE_RELEASE(m_pDXInputKB);
	SAFE_RELEASE(m_pDXInputMouse);
	SAFE_RELEASE(m_pDXInput);
}