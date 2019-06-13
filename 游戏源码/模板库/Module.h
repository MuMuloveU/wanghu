#ifndef MODULE_HEAD_FILE
#define MODULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//模块接口

#define VER_IUnknownEx INTERFACE_VERSION(1,1)
static const GUID IID_IUnknownEx={0x5feec21e,0xdbf3,0x46f0,0x9f,0x57,0xd1,0xcd,0x71,0x1c,0x46,0xde};

//基础接口
interface IUnknownEx
{
	//释放对象
	virtual VOID __cdecl Release()=NULL;
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//版本比较宏

//产品版本号
#define PRODUCT_VER					8									//产品版本

//接口版本
#define INTERFACE_VERSION(cbMainVer,cbSubVer)							\
		(DWORD)(														\
		(((BYTE)(PRODUCT_VER))<<24)+									\
		(((BYTE)(cbMainVer))<<16)+										\
		((BYTE)(cbSubVer)<<8))

//模块版本
#define PROCESS_VERSION(cbMainVer,cbSubVer,cbBuildVer)					\
		(DWORD)(														\
		(((BYTE)(PRODUCT_VER))<<24)+									\
		(((BYTE)(cbMainVer))<<16)+										\
		((BYTE)(cbSubVer)<<8)+											\
		(BYTE)(cbBuildVer))

//产品版本
inline BYTE GetProductVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[3];
}

//主要版本
inline BYTE GetMainVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[2];
}

//次要版本
inline BYTE GetSubVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[1];
}

//编译版本
inline BYTE GetBuildVer(DWORD dwVersion)
{
	return ((BYTE *)&dwVersion)[0];
}

//版本比较
inline bool InterfaceVersionCompare(DWORD dwQueryVer, DWORD dwInterfaceVer)
{
	if (GetProductVer(dwQueryVer)!=GetProductVer(dwInterfaceVer)) return false;
	if (GetMainVer(dwQueryVer)!=GetMainVer(dwInterfaceVer)) return false;
	if (GetSubVer(dwQueryVer)>GetSubVer(dwInterfaceVer)) return false;
	return true;
};

//////////////////////////////////////////////////////////////////////////
//内部接口查询

//查询接口
#define QUERYINTERFACE(Interface,Guid,dwQueryVer)															\
	if ((Guid==IID_##Interface)&&(InterfaceVersionCompare(dwQueryVer,VER_##Interface)))						\
		return static_cast<Interface *>(this);											

//查询接口
#define QUERYINTERFACE_IUNKNOWNEX(BaseInterface,Guid,dwQueryVer)											\
	if ((Guid==IID_IUnknownEx)&&(InterfaceVersionCompare(dwQueryVer,VER_IUnknownEx)))						\
		return static_cast<IUnknownEx *>(static_cast<BaseInterface *>(this));			

//////////////////////////////////////////////////////////////////////////
//外部接口查询

//查询接口
#define QUERY_ME_INTERFACE(Interface)																		\
	((Interface *)QueryInterface(IID_##Interface,VER_##Interface))

//查询接口
#define QUERY_OBJECT_INTERFACE(Object,Interface)															\
	((Interface *)Object.QueryInterface(IID_##Interface,VER_##Interface))

//查询接口
#define QUERY_OBJECT_PTR_INTERFACE(pObject,Interface)														\
	((pObject==NULL)?NULL:((Interface *)pObject->QueryInterface(IID_##Interface,VER_##Interface)))

//////////////////////////////////////////////////////////////////////////
//组件模板辅助模板

//组件创建函数
typedef VOID * (__cdecl ModuleCreateProc)(REFGUID Gudi, DWORD dwInterfaceVer);

//组件辅助类模板
template <typename IModeluInterface> class CTempldateHelper
{
	//接口属性
public:
	REFGUID							m_Guid;								//接口标识
	const DWORD						m_dwVersion;						//接口版本

	//组件属性
public:
	CHAR							m_szCreateProc[32];					//创建函数
	TCHAR							m_szModuleDllName[MAX_PATH];		//组件名字

	//内核变量
public:
	HINSTANCE						m_hDllInstance;						//DLL 句柄
	IModeluInterface *				m_pIModeluInterface;				//模块接口

	//辅助变量
public:
	TCHAR							m_szDescribe[128];					//错误描述

	//函数定义
public:
	//构造函数
	CTempldateHelper(REFGUID Guid, DWORD dwVersion);
	//构造函数
	CTempldateHelper(REFGUID Guid, DWORD dwVersion, LPCTSTR pszModuleDll, LPCSTR pszCreateProc);
	//析构函数
	virtual ~CTempldateHelper();

	//管理函数
public:
	//创建函数
	bool CreateInstance();
	//释放组件
	bool CloseInstance();

	//配置函数
public:
	//创建信息
	VOID SetModuleCreateInfo(LPCTSTR pszModuleDllName, LPCSTR pszCreateProc);

	//辅助函数
public:
	//获取错误
	inline LPCTSTR GetErrorDescribe() const;
	//指针重载
	inline IModeluInterface * operator->() const;
	//获取接口
	inline IModeluInterface * GetInterface() const;
};

//////////////////////////////////////////////////////////////////////////
// CTempldateHelper<IModeluInterface> 外联函数

//构造函数
template <typename IModeluInterface>
CTempldateHelper<IModeluInterface>::CTempldateHelper(REFGUID Guid, DWORD dwVersion) : m_dwVersion(dwVersion), m_Guid(Guid)
{
	//辅助变量
	m_szDescribe[0]=0;

	//内核信息
	m_hDllInstance=NULL;
	m_pIModeluInterface=NULL;

	//组件属性
	ZeroMemory(m_szCreateProc,sizeof(m_szCreateProc));
	ZeroMemory(m_szModuleDllName,sizeof(m_szModuleDllName));

	return;
}

//构造函数
template <typename IModeluInterface>
CTempldateHelper<IModeluInterface>::CTempldateHelper(REFGUID Guid, DWORD dwVersion, LPCTSTR pszModuleDll, LPCSTR pszCreateProc) : m_dwVersion(dwVersion), m_Guid(Guid)
{
	//辅助变量
	m_szDescribe[0]=0;

	//内核信息
	m_hDllInstance=NULL;
	m_pIModeluInterface=NULL;

	//组件属性
	lstrcpynA(m_szCreateProc,pszCreateProc,CountArray(m_szCreateProc));
	lstrcpyn(m_szModuleDllName,pszModuleDll,CountArray(m_szModuleDllName));

	return;
}

//析构函数
template <typename IModeluInterface>
CTempldateHelper<IModeluInterface>::~CTempldateHelper()
{
	CloseInstance();
}

//创建组件
template <typename IModeluInterface>
bool CTempldateHelper<IModeluInterface>::CreateInstance()
{
	//释放组件
	CloseInstance();

	//创建组件
	try
	{
		//效验参数
		ASSERT(m_szCreateProc[0]!=0);
		ASSERT(m_szModuleDllName[0]!=0);

		//加载模块
		m_hDllInstance=AfxLoadLibrary(m_szModuleDllName);
		if (m_hDllInstance==NULL) 
		{
			_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("“%s”模块加载失败"),m_szModuleDllName);
			return false;
		}

		//寻找函数
		ModuleCreateProc * CreateProc=(ModuleCreateProc *)GetProcAddress(m_hDllInstance,m_szCreateProc);
		if (CreateProc==NULL) 
		{
			_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("找不到组件创建函数“%s”"),m_szCreateProc);
			return false;
		}

		//创建组件
		m_pIModeluInterface=(IModeluInterface *)CreateProc(m_Guid,m_dwVersion);
		if (m_pIModeluInterface==NULL) 
		{
			_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("调用函数“%s”生成对象失败"),m_szCreateProc);
			return false;
		}
	}
	catch (LPCTSTR pszError)
	{
		_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("由于“%s”，组件创建失败"),pszError);
		return false;
	}
	catch (...)	
	{ 
		_sntprintf(m_szDescribe,CountArray(m_szDescribe),TEXT("组件创建函数“%s”产生未知异常错误，组件创建失败"),m_szCreateProc);
		return false;
	}

	return true;
}

//释放组件
template <typename IModeluInterface>
bool CTempldateHelper<IModeluInterface>::CloseInstance()
{
	//设置变量
	m_szDescribe[0]=0;

	//销毁对象
	if (m_pIModeluInterface!=NULL)
	{
		m_pIModeluInterface->Release();
		m_pIModeluInterface=NULL;
	}

	//释放 DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}

	return true;
}

//创建信息
template <typename IModeluInterface>
VOID CTempldateHelper<IModeluInterface>::SetModuleCreateInfo(LPCTSTR pszModuleDllName, LPCSTR pszCreateProc)
{
	//设置信息
	lstrcpynA(m_szCreateProc,pszCreateProc,CountArray(m_szCreateProc));
	lstrcpyn(m_szModuleDllName,pszModuleDllName,CountArray(m_szModuleDllName));

	return;
}

//////////////////////////////////////////////////////////////////////////
// CTempldateHelper<IModeluInterface> 内联函数

//获取描述
template <typename IModeluInterface>
inline LPCTSTR CTempldateHelper<IModeluInterface>::GetErrorDescribe() const
{ 
	return m_szDescribe; 
}

//指针重载
template <typename IModeluInterface>
inline IModeluInterface * CTempldateHelper<IModeluInterface>::operator->() const
{ 
	return GetInterface(); 
}

//获取接口
template <typename IModeluInterface>
inline IModeluInterface * CTempldateHelper<IModeluInterface>::GetInterface() const
{ 
	return m_pIModeluInterface; 
}

//////////////////////////////////////////////////////////////////////////
//组件辅助宏

//组件创建函数
#define DECLARE_CREATE_MODULE(OBJECT_NAME)																	\
extern "C" __declspec(dllexport) VOID * __cdecl Create##OBJECT_NAME(REFGUID Guid, DWORD dwInterfaceVer)		\
{																											\
	C##OBJECT_NAME * p##OBJECT_NAME=NULL;																	\
	try																										\
	{																										\
		p##OBJECT_NAME=new C##OBJECT_NAME();																\
		if (p##OBJECT_NAME==NULL) throw TEXT("创建失败");													\
		VOID * pObject=p##OBJECT_NAME->QueryInterface(Guid,dwInterfaceVer);									\
		if (pObject==NULL) throw TEXT("接口查询失败");														\
		return pObject;																						\
	}																										\
	catch (...) {}																							\
	SafeDelete(p##OBJECT_NAME);																				\
	return NULL;																							\
}

//组件辅助类宏
#define DECLARE_MODULE_DYNAMIC(OBJECT_NAME)																	\
class C##OBJECT_NAME##Helper : public CTempldateHelper<I##OBJECT_NAME>										\
{																											\
public:																										\
	C##OBJECT_NAME##Helper() : CTempldateHelper<I##OBJECT_NAME>(IID_I##OBJECT_NAME,VER_I##OBJECT_NAME) { }	\
};

//组件辅助类宏
#define DECLARE_MODULE_HELPER(OBJECT_NAME,MODULE_DLL_NAME,CREATE_FUNCTION_NAME)								\
class C##OBJECT_NAME##Helper : public CTempldateHelper<I##OBJECT_NAME>										\
{																											\
public:																										\
	C##OBJECT_NAME##Helper() : CTempldateHelper<I##OBJECT_NAME>(IID_I##OBJECT_NAME,							\
		VER_I##OBJECT_NAME,MODULE_DLL_NAME,CREATE_FUNCTION_NAME) { }										\
};

//////////////////////////////////////////////////////////////////////////

#endif