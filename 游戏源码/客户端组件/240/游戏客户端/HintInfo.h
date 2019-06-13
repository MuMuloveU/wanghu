#pragma once

#include "Resource.h"
// CHintInfo 对话框

class CHintInfo : public CDialog
{
	DECLARE_DYNAMIC(CHintInfo)

public:
	CHintInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHintInfo();

// 对话框数据
	enum { IDD = IDD_HINT_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	void	SetShowInfo(CString& str);

	DECLARE_MESSAGE_MAP()
};
