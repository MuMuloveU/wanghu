#include "StdAfx.h"
#include "../include/PisaList.h"


void CPisaList::CopyAllPaiToIntPtrList(CPtrList& to)
{
	for(POSITION pos = m_PisaList.GetHeadPosition(); pos != NULL; )
	{
		Node* nd=(Node*)m_PisaList.GetNext(pos);
		if(!nd)continue;
		int *pp=new int;
		*pp=nd->mPisa.m_iPs;
		to.AddTail(pp);

	}
}

int CPisaList::ClickAPisa(CPoint& pt,int& index)
{
	index=-1;
	if(m_iShow_DX <0 )return -1;
	int ps=-1; int ii = -1;
	int pslen=(GetCount() -1) *( m_iShow_DX) + PISA_W;
	int x=m_rcPisaShow.left  + m_rcPisaShow.Width () /2 - pslen / 2 ,
		y=m_rcPisaShow.bottom  - PISA_H  ;

	if(m_iShow_Aligin_type == ALIGIN_TYPE_RIGHT)x=m_rcPisaShow.right - pslen;
	if(m_iShow_Aligin_type == ALIGIN_TYPE_LEFT)x=m_rcPisaShow.left ;

	for(POSITION pos = m_PisaList.GetHeadPosition(); pos != NULL; )
	{
		ii++;
		index=ii;
		Node* ps = (Node*)m_PisaList.GetNext(pos);
		if(!ps)continue;
		int l=x + (ii) * m_iShow_DX;
		int t=y;
		int r=l+m_iShow_DX;
		int b=m_rcPisaShow.top + PISA_H;
		if(ps->IsSelect)
		{
			t -=  m_rcPisaShow.Height () - PISA_H ;
			b -=  m_rcPisaShow.Height () - PISA_H ;
			if(pt.x>=l && pt.y>=t && pt.x <= r && pt.y <= b)return ps->mPisa.m_iPs  ;

		}
		if(pt.x>=l && pt.y>=t && pt.x <= r && pt.y <= b)return ps->mPisa.m_iPs  ;
	}
	return -1;
}



void CPisaList::ShowAllPisa(CWnd* pWnd)
{
	CDC *pdc=pWnd->GetDC ();
	ShowAllPisa(pdc );
    pWnd->ReleaseDC (pdc);
}



void CPisaList::ShowAllPisa1(CDC* pDC)
{
    int pslen=(GetCount() ) *( PISA_H) ;
	int x=m_rcPisaShow.left  + m_rcPisaShow.Width () /2 - pslen / 2 ,
		y=m_rcPisaShow.bottom  - PISA_H  ;

	if(m_iShow_Aligin_type == ALIGIN_TYPE_TOP)
	{
		x=m_rcPisaShow.left;
		y=m_rcPisaShow.top;
	}
	if(m_iShow_Aligin_type == ALIGIN_TYPE_BOTTOM)
	{
		x=m_rcPisaShow.left;
		y=m_rcPisaShow.bottom - pslen;
	}

	

	int in=0;
	for(POSITION pos = m_PisaList.GetHeadPosition();pos != NULL;)
	{
		Node* ps = (Node*)m_PisaList.GetNext(pos);
		if(!ps)continue;
		in ++;

		CPoint p;
		p.x=x;p.y=y;
		
		bool bIfShowFront=true;
		if(	m_iShowType==SHOW_TYPE_ALL_BACK ||
			(m_iShowType==SHOW_TYPE_FIRST_BACK && in == 1 )
			)bIfShowFront=false;
		ps->mPisa.ShowAPisa (pDC,p,bIfShowFront);

		y+=m_iShow_DX;
	}

}



void CPisaList::ShowAllPisa(CDC* pDC)
{
	if(m_iShow_Aligin_type == ALIGIN_TYPE_TOP||
		m_iShow_Aligin_type == ALIGIN_TYPE_BOTTOM)
	{
		ShowAllPisa1(pDC);
		return;
	}
    int pslen=(GetCount() -1) *( m_iShow_DX) + PISA_W;
	int x=m_rcPisaShow.left  + m_rcPisaShow.Width () /2 - pslen / 2 ,
		y=m_rcPisaShow.bottom  - PISA_H  ;

	if(m_iShow_Aligin_type == ALIGIN_TYPE_RIGHT)x=m_rcPisaShow.right - pslen;
	if(m_iShow_Aligin_type == ALIGIN_TYPE_LEFT)x=m_rcPisaShow.left ;

	
	int in=0;

	for(POSITION pos = m_PisaList.GetHeadPosition();pos != NULL;)
	{
		Node* ps = (Node*)m_PisaList.GetNext(pos);
		if(!ps)continue;
		in ++;
		CPoint p;
		p.x=x;p.y=y;
		if(ps->IsSelect )p.y -= m_rcPisaShow.Height () - PISA_H ;
		bool bIfShowFront=true;
		if(	m_iShowType==SHOW_TYPE_ALL_BACK ||
			(m_iShowType==SHOW_TYPE_FIRST_BACK && in == 1 )
			)bIfShowFront=false;
		ps->mPisa.ShowAPisa (pDC,p,bIfShowFront);
		x+=m_iShow_DX;
	}

}


bool CPisaList::FindHuaSe(int hs)
{
	for(POSITION pos = m_PisaList.GetHeadPosition(); pos != NULL; )
	{
		Node* nd=(Node*)m_PisaList.GetNext(pos);
		if(!nd)continue;
		int pi=nd->mPisa.GetHuaSe();

		if(pi == hs) return true;
	}
	return false;
}


int CPisaList::GetSelectedCount(CPtrList& pl)
{
	int ret=0;
	for(POSITION pos = m_PisaList.GetHeadPosition(); pos != NULL; )
	{
		Node* nd=(Node*)m_PisaList.GetNext(pos);
		if(!nd)continue;
		if(nd->IsSelect)
		{
			int * pi=new int;
			*pi=nd->mPisa.m_iPs ;
			pl.AddTail (pi);
			ret++;
		}
	}
	return ret;
}


void CPisaList::RemoveChuPai(CPisaList& ChuPai)
{
	for(POSITION pos = m_PisaList.GetHeadPosition(); pos != NULL; )
	{
		POSITION temp=pos;
		Node* nd=(Node*)m_PisaList.GetNext(pos);
		if(!nd)continue;
		if(nd->IsSelect)
		{
			ChuPai.AddTailPS (nd->mPisa.m_iPs );
			m_PisaList.RemoveAt (temp);
			delete nd;
		}
	}
}

bool CPisaList::Select(CPoint& pt,bool IsChuPai )
{
	Node* nd=HitPoint(pt);
	if(!nd)
	{
		return false;
	}
	if(IsChuPai)
	{
		if(nd->IsSelect)return true;
		return false;
	}
	
	if(nd->IsSelect)
	{
		nd->IsSelect=false;
		return true;
	}
	else 
	{
		nd->IsSelect=true;
		return true;
	}
}


CPisaList::Node* CPisaList::HitPoint(CPoint& pt)
{
	if(m_iShow_DX <0 )return NULL;
	int ps=-1; int ii = -1;

	int dd=m_rcPisaShow.bottom -PISA_H;

	 int pslen=(GetCount() -1) *( m_iShow_DX) + PISA_W;
	int x=m_rcPisaShow.left  + m_rcPisaShow.Width() /2 - pslen / 2 ,
		y=m_rcPisaShow.bottom -PISA_H ;

	for(POSITION pos = m_PisaList.GetHeadPosition(); pos != NULL; )
	{
		ii++;
		Node* ps = (Node*)m_PisaList.GetNext(pos);
		if(!ps)continue;
		int l=x + (ii) * m_iShow_DX;
		int t=y;
		int r=l+m_iShow_DX;
		int b=m_rcPisaShow.top + PISA_H;
		if(ps->IsSelect)
		{
			if(!pos)////最后一张
		   {
			   if(pt.x>=l && pt.y>=t && pt.x <= r + PISA_W && pt.y <= b)return ps;
		   }

			t -=  m_rcPisaShow.Height () - PISA_H ;
			b -= m_rcPisaShow.Height () - PISA_H ;
			if(pt.x>=l && pt.y>=t && pt.x <= r && pt.y <= b)return ps;

		}
		else
		{
		   if(pt.x>=l && pt.y>=t && pt.x <= r && pt.y <= b)return ps;
		   if(!pos)////最后一张
		   {
			   if(pt.x>=l && pt.y>=t && pt.x <= r + PISA_W && pt.y <= b)return ps;
		   }
		}
	}
	return NULL;
}


void CPisaList::Init(int nFuPai)
{
	if(nFuPai == 0)
	{
		ClearAll();
		return;
	}
	for(int i=0;i<nFuPai;i++)
	{
		for(int j=0;j<A_FU_PISA_NUM;j++)
		  Insert(stcArr_A_Pisa[j]);
	}
	
}


void CPisaList::InsertBeforePS(int ps,POSITION pos)
{
	if(pos == NULL)return;
	Node *nd1=new Node(ps);
	CPisa* ps1=&nd1->mPisa ;
	if(!ps1->IsValidatePisa ())
	{
		delete nd1;
		return;
	}
	m_PisaList.InsertBefore(pos,nd1);
	
}


void CPisaList::AddTailPS(int ps)
{
	Node *nd1=new Node(ps);
	CPisa* ps1=&nd1->mPisa ;
	if(!ps1->IsValidatePisa ())
	{
		delete nd1;
		return;
	}
	m_PisaList.AddTail(nd1);
	
}


void CPisaList::Insert(int ps)
{
	
	POSITION pos,temp;
	Node *nd1=new Node(ps);
	CPisa* ps1=&nd1->mPisa ;
	if(!ps1->IsValidatePisa ())
	{
		delete nd1;
		return;
	}
	if(m_PisaList.GetCount() == 0)
	{			
			m_PisaList.AddHead(nd1);
			return;
	}
	for(pos = m_PisaList.GetHeadPosition();pos != NULL;)
	{
		temp = pos;
		CPisa* ps2=NULL;
		Node *nd2=(Node*)m_PisaList.GetNext(pos);
		if(nd2)ps2=&nd2->mPisa ;
		if(!ps2  || !ps2->IsValidatePisa ())continue;
		int hs1=ps1->GetHuaSe (),pd1=ps1->GetPaiDian (),
			hs2=ps2->GetHuaSe (),pd2=ps2->GetPaiDian ();

		if(m_iSortType == SORT_TYPE_HUASE)
		{
			if(hs1>hs2 ||(hs1 == hs2 && pd1 >= pd2 ))///insert
			{
				m_PisaList.InsertBefore (temp,nd1);
				return;
			}
		}
		else
		{
			if(pd1>pd2 ||(pd1 == pd2 && hs1 >= hs2 ))///insert
			{
				m_PisaList.InsertBefore (temp,nd1);
				return;
			}
		}

		if(pos == NULL)
		{
			m_PisaList.InsertAfter (temp,nd1);
			return;
		}
	}
}


	
void CPisaList::Insert(CPisaList& pl)
{
	POSITION pos;
	for(pos = pl.m_PisaList.GetHeadPosition(); pos != NULL;)
	{
		Node* pS = (Node*)pl.m_PisaList.GetNext(pos);
		if(!pS)continue;
		Insert(pS->mPisa.m_iPs);
	}
}


CPisaList::CPisaList(void)
{
	m_iSortType=SORT_TYPE_HUASE;
	m_iShow_DX=-1;
	m_iShowType=SHOW_TYPE_ALL_FRONT;
}


void CPisaList::ClearAll()
{
	
	while(!m_PisaList.IsEmpty())
	{
		Node* pdesk = (Node*)m_PisaList.RemoveHead();
		delete pdesk;
	}
	m_PisaList.RemoveAll();
}

CPisaList::~CPisaList(void)
{
	ClearAll();
	
}




int CPisaList::FindFromIndex(int index)
{
	int in = 0;
	for(POSITION pos=m_PisaList.GetHeadPosition();pos!=NULL;in++)
	{
		Node* s=(Node*)m_PisaList.GetNext(pos);
		if(!s)continue;
		if( in == index)return s->mPisa.m_iPs;
	}
	return -1;
	
}


int CPisaList::FindCount(int ps)
{
	int cot=0;
	for(POSITION pos=m_PisaList.GetHeadPosition();pos!=NULL;m_PisaList.GetNext(pos))
	{
		Node* s=(Node*)m_PisaList.GetAt(pos);
		if(!s)continue;
		if(s->mPisa.m_iPs  == ps)cot++;
	}
	return cot;
	
}


bool CPisaList::Find(int ps)
{
	for(POSITION pos=m_PisaList.GetHeadPosition();pos!=NULL;m_PisaList.GetNext(pos))
	{
		Node* s=(Node*)m_PisaList.GetAt(pos);
		if(!s)continue;
		if(s->mPisa.m_iPs  == ps)return true;
	}
	return false;
	
}




void CPisaList::Delete(int ps)
{
	
	POSITION pos,temp;
	for(pos = m_PisaList.GetHeadPosition(); pos != NULL;)
	{
		temp = pos;
		Node* pS = (Node*)m_PisaList .GetNext(pos);
		if(!pS)continue;
		if (pS->mPisa.m_iPs  == ps)
		{
			m_PisaList.RemoveAt(temp);
			delete pS;
			return;
		}
	}
}



int CPisaList::GetHeadPs()
{
	if(m_PisaList.GetCount () <= 0)return -1;
	Node* ps = (Node*)m_PisaList.GetHead ();
	if(!ps)return -1;
	int ret=ps->mPisa.m_iPs;
	return ret;
}

int CPisaList::GetTailPs()
{
	if(m_PisaList.GetCount () <= 0)return -1;
	Node* ps = (Node*)m_PisaList.GetTail ();
	if(!ps)return -1;
	int ret=ps->mPisa.m_iPs;
	return ret;
}

int CPisaList::DeleteHead()
{
	Node* ps = (Node*)m_PisaList.RemoveHead();
	if(!ps)return -1;
	int ret=ps->mPisa.m_iPs;
	delete ps;
	return ret;
}

void CPisaList::Delete(CPisaList& pl)
{
	POSITION pos;
	for(pos = pl.m_PisaList.GetHeadPosition(); pos != NULL;)
	{
		Node* pS = (Node*)pl.m_PisaList.GetNext(pos);
		if(!pS)continue;
		Delete(pS->mPisa.m_iPs);
	}
}


	
