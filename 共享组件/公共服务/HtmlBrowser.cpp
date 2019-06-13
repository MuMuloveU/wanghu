#include "Stdafx.h"
#include "HtmlBrowser.h"

BEGIN_MESSAGE_MAP(CHtmlBrower, CHtmlView)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CHtmlBrower::CHtmlBrower()
{
	m_bEnableBrowser=false;
}

//析构函数
CHtmlBrower::~CHtmlBrower()
{
}

//转向网页
void CHtmlBrower::Navigate(LPCTSTR pszURL)
{
	ASSERT(pszURL!=NULL);
	m_strNavigateUrl=pszURL;
	if (m_bEnableBrowser) __super::Navigate(m_strNavigateUrl,NULL,NULL,NULL,NULL);
	return;
}

//启用网页
bool CHtmlBrower::EnableBrowser(bool bEnableBrowser)
{
	if (m_bEnableBrowser!=bEnableBrowser)
	{
		m_bEnableBrowser=bEnableBrowser;
		if (m_bEnableBrowser==true)
		{
			if ((GetSafeHwnd()!=NULL)&&(m_strNavigateUrl.IsEmpty()==FALSE))
			{
				__super::Navigate(m_strNavigateUrl,NULL,NULL,NULL,NULL);
			}
		}
		else m_strNavigateUrl.Empty();
	}
	return m_bEnableBrowser;
}

//////////////////////////////////////////////////////////////////////////
