
#pragma once

#include "pisa.h"

class CPisaList
{
	CPtrList m_PisaList;
	struct Node
	{
		CPisa mPisa;
		bool IsSelect;
		Node(){IsSelect=false;}
		Node(int ps){ IsSelect=false; mPisa.m_iPs = ps;}
	};
	Node* HitPoint(CPoint& pt);
	void Delete(CPisaList& pl);
	void ShowAllPisa1(CDC* pDC);

public:
	int FindCount(int ps);

	CPisaList(void);
	~CPisaList(void);
	void AddTailPS(int ps);
	void InsertBeforePS(int ps,POSITION pos);
	int ClickAPisa(CPoint& pt,int& index);
	int DeleteHead();
	int GetTailPs();  int GetHeadPs();

	void CopyAllPaiToIntPtrList(CPtrList& to);



	bool Find(int ps);
	int GetSelectedCount(CPtrList& pl);
	bool FindHuaSe(int hs);
	int FindFromIndex(int index);
	int GetPs(POSITION &pos)
	{
		if(pos == NULL)return -1;
		Node *ps=(Node *)m_PisaList.GetAt (pos);
		return ps->mPisa.m_iPs;
	}

	CPtrList & GetList(){return m_PisaList;}
	void ClearAll();
    void RemoveChuPai(CPisaList& ChuPai);

	////////////////////
	CRect  m_rcPisaShow;
	int m_iShow_DX;
	int m_iShow_Aligin_type;

	/////////////////
	
	int m_iSortType;
	bool Select(CPoint& pt,bool IsChuPai);
	void Init(int nFuPai);
	int GetCount(){return m_PisaList.GetCount ();}
	void Delete(int ps);
	void Insert(int ps);
	void Insert(CPisaList& pl);
	void ShowAllPisa(CDC* pDC);
	void ShowAllPisa(CWnd* pWnd);


	enum SORT_TYPE
	{
		SORT_TYPE_NONE = -1,
		SORT_TYPE_PAIDIAN=0,
		SORT_TYPE_HUASE=1
	};
	

	enum ALIGIN_TYPE
	{
		ALIGIN_TYPE_LEFT=0,
		ALIGIN_TYPE_RIGHT=1,
		ALIGIN_TYPE_TOP,
		ALIGIN_TYPE_BOTTOM,

		ALIGIN_TYPE_CENTER
	};

	
	enum SHOW_TYPE
	{
		SHOW_TYPE_ALL_FRONT = 0,
		SHOW_TYPE_ALL_BACK=1,
		SHOW_TYPE_FIRST_BACK=2
	};

	int m_iShowType;

	
	
};

