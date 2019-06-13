#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//静态变量
RegionInfo BetNumberArea[3][12]={};
RegionInfo CrossBetArea[3][12]={};
RegionInfo WideSideCrossArea[3][12]={};
RegionInfo HeightSideCrossArea[3][12]={};
RegionInfo Cross0Area[6]={};
//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	m_nScreenWidth=0;
	m_nScreenHeight=0;
	calcPoint();
}

//析构函数
CGameLogic::~CGameLogic()
{

}
//取得数字颜色
BYTE CGameLogic::GetNumberColor(BYTE bNumber)
{
	if(bNumber==0) return COLOR_0;
	if(bNumber==1||bNumber==3||bNumber==5||bNumber==7||bNumber==9||bNumber==12||bNumber==14||bNumber==16||bNumber==18||bNumber==19||bNumber==21
		||bNumber==23||bNumber==25||bNumber==27||bNumber==30||bNumber==32||bNumber==34||bNumber==36)
		return COLOR_RED;
	return COLOR_BLACK;
}

//计算得分
long CGameLogic::CountScore(long ChipsInfo[256],int index[256],int count)
{
	//累计得分
	long lAllScore=0;
	for(int i=0;i<count;i++)
	{
		int areaIndex=index[i];
		if(areaIndex>=255) continue;
		//单一数字区
		if(areaIndex>=1&&areaIndex<=36)							lAllScore+=ChipsInfo[areaIndex]*36;
		//列区域
		if(areaIndex>=37&&areaIndex<=39) 						lAllScore+=ChipsInfo[areaIndex]*3;
		//打区域
		if(areaIndex>=40&&areaIndex<=42) 						lAllScore+=ChipsInfo[areaIndex]*3;
		//1~18或19~36
		if(areaIndex>=43&&areaIndex<=44) 						lAllScore+=ChipsInfo[areaIndex]*2;
		//单双，红黑
		if(areaIndex>=45&&areaIndex<=48) 						lAllScore+=ChipsInfo[areaIndex]*2;
		//十字交叉最左边
		if(areaIndex>=53&&areaIndex<=83&&(areaIndex-53)%3==0)	lAllScore+=ChipsInfo[areaIndex]*6;
		//十字交叉
		if(areaIndex>=53&&areaIndex<=85&&(areaIndex-53)%3!=0)	lAllScore+=ChipsInfo[areaIndex]*9;
		//竖相交边最左边
		if(areaIndex>=90&&areaIndex<=123&&areaIndex%3==0)		lAllScore+=ChipsInfo[areaIndex]*12;
		//竖相交边
		if(areaIndex>=90&&areaIndex<=125&&areaIndex%3!=0)		lAllScore+=ChipsInfo[areaIndex]*18;
		//横相交边
		if(areaIndex>=130&&areaIndex<=162)						lAllScore+=ChipsInfo[areaIndex]*18;
		//零区域
		if(areaIndex==200)										lAllScore+=ChipsInfo[areaIndex]*36;
		if(areaIndex==201)										lAllScore+=ChipsInfo[areaIndex]*9;
		if(areaIndex==203||areaIndex==205)						lAllScore+=ChipsInfo[areaIndex]*12;
		if(areaIndex==202||areaIndex==204||areaIndex==206)		lAllScore+=ChipsInfo[areaIndex]*18;
	}
	return lAllScore;
}
//计算得分
long CGameLogic::ComputeScore(long ChipsInfo[256],BYTE WinNumber)
{
	switch(WinNumber)
	{
	case 0: {int temp[]={200,201,202,203,204,205,206};return CountScore(ChipsInfo,temp,7);}
	case 1: {int temp[]={201,202,203,90,1,91,53,130,54,40,43,37,47,45};return CountScore(ChipsInfo,temp,14);}
	case 2: {int temp[]={201,203,204,205,91,2,92,54,131,55,90,53,38,43,40,46,48};return CountScore(ChipsInfo,temp,17);}
	case 3: {int temp[]={201,205,206,90,92,3,53,55,132,40,43,39,45,47};return CountScore(ChipsInfo,temp,15);}
	case 4:	{int temp[]={53,130,54,93,4,94,56,133,57,40,43,37,46,48};return CountScore(ChipsInfo,temp,14);}
	case 5:	{int temp[]={53,54,131,55,93,94,5,95,56,57,134,58,40,43,38,47,45};return CountScore(ChipsInfo,temp,17);}
	case 6:	{int temp[]={53,55,132,93,95,6,56,58,135,39,40,43,46,48};return CountScore(ChipsInfo,temp,14);}
	case 7:	{int temp[]={56,133,57,96,7,97,59,136,60,40,43,37,45,47};return CountScore(ChipsInfo,temp,14);}
	case 8:	{int temp[]={56,57,134,58,96,97,8,98,59,60,137,61,40,43,38,48,46};return CountScore(ChipsInfo,temp,17);}
	case 9:	{int temp[]={56,58,135,96,98,9,59,61,138,40,43,45,47,39};return CountScore(ChipsInfo,temp,14);}
	case 10:{int temp[]={59,136,60,99,10,100,62,139,63,40,43,46,37,48};return CountScore(ChipsInfo,temp,14);}

	case 11:{int temp[]={59,60,137,61,99,100,11,101,62,63,140,64,38,40,43,45,48};return CountScore(ChipsInfo,temp,17);}
	case 12:{int temp[]={59,61,138,99,101,12,62,64,141,39,46,47,40,43};return CountScore(ChipsInfo,temp,14);}
	case 13:{int temp[]={62,139,63,102,13,103,65,142,66,37,41,43,45,48};return CountScore(ChipsInfo,temp,14);}
	case 14:{int temp[]={62,63,140,64,102,103,14,104,65,66,143,67,38,41,47,43,46};return CountScore(ChipsInfo,temp,17);}
	case 15:{int temp[]={62,64,141,102,104,15,65,67,144,39,41,43,45,48};return CountScore(ChipsInfo,temp,14);}
	case 16:{int temp[]={65,142,66,105,16,106,68,145,69,37,41,46,43,47};return CountScore(ChipsInfo,temp,14);}
	case 17:{int temp[]={65,66,143,67,105,106,107,17,68,69,70,146,38,41,43,45,48};return CountScore(ChipsInfo,temp,17);}
	case 18:{int temp[]={65,67,144,105,107,18,68,70,147,39,41,47,43,46};return CountScore(ChipsInfo,temp,14);}
	case 19:{int temp[]={68,145,69,108,19,109,71,148,72,37,41,45,47,44};return CountScore(ChipsInfo,temp,14);}
	case 20:{int temp[]={68,69,146,70,108,109,110,20,71,72,149,73,38,41,48,44,46};return CountScore(ChipsInfo,temp,17);}

	case 21:{int temp[]={68,70,147,108,110,21,71,73,150,39,41,44,45,47};return CountScore(ChipsInfo,temp,14);}
	case 22:{int temp[]={71,148,72,111,22,112,74,151,75,37,41,44,46,48};return CountScore(ChipsInfo,temp,14);}
	case 23:{int temp[]={71,72,149,73,111,112,23,113,74,75,152,76,38,41,44,45,47};return CountScore(ChipsInfo,temp,17);}
	case 24:{int temp[]={71,73,150,111,113,24,74,76,153,39,41,44,46,48};return CountScore(ChipsInfo,temp,14);}
	case 25:{int temp[]={74,151,75,114,25,115,77,154,78,37,42,44,45,47};return CountScore(ChipsInfo,temp,14);}
	case 26:{int temp[]={74,75,152,76,114,115,26,116,77,78,155,79,38,42,44,46,48};return CountScore(ChipsInfo,temp,17);}
	case 27:{int temp[]={74,76,153,114,116,27,77,79,156,39,42,44,45,47};return CountScore(ChipsInfo,temp,14);}
	case 28:{int temp[]={77,154,78,117,28,118,80,157,81,37,42,44,46,48};return CountScore(ChipsInfo,temp,14);}
	case 29:{int temp[]={77,78,155,79,117,118,29,119,80,81,158,82,38,42,44,45,48};return CountScore(ChipsInfo,temp,17);}
	case 30:{int temp[]={77,79,156,117,119,30,80,82,159,39,42,44,46,47};return CountScore(ChipsInfo,temp,14);}

	case 31:{int temp[]={80,157,81,120,31,121,83,160,84,37,42,44,45,48};return CountScore(ChipsInfo,temp,14);}
	case 32:{int temp[]={80,81,158,82,120,121,32,122,83,84,161,85,38,42,44,46,47};return CountScore(ChipsInfo,temp,17);}
	case 33:{int temp[]={80,82,159,120,122,33,83,85,162,39,42,44,45,48};return CountScore(ChipsInfo,temp,14);}
	case 34:{int temp[]={83,160,84,123,34,124,37,42,44,46,47};return CountScore(ChipsInfo,temp,11);}
	case 35:{int temp[]={83,84,161,85,123,124,35,125,38,42,44,45,48};return CountScore(ChipsInfo,temp,13);}
	case 36:{int temp[]={83,85,162,123,125,36,39,42,44,46,47};return CountScore(ChipsInfo,temp,11);}
	default:return 0;
	}
	return 0;
}

//设置窗口大小
void CGameLogic::SetDxWindowSize(int nWidth,int nHeight)
{
	m_nScreenWidth=nWidth;
	m_nScreenHeight=nHeight;
}

//返回点击位置
WORD CGameLogic::GetMouseClickRegion(CPoint point)
{	
	//选择筹码区
	for(int i=0;i<6;i++)
	{
		if(IsInRegion(ChooseChipArea[i].BasePoint,ChooseChipArea[i].VertexTop,ChooseChipArea[i].VertexButtom,ChooseChipArea[i].VertexRight,point)) 
		{
			return 900+i;
		}
	}

	//0区
	if(IsInRegion(Bet0Area.BasePoint,Bet0Area.VertexTop,Bet0Area.VertexButtom,Bet0Area.VertexRight,point,1))
	{
		return 200;
	}
	for(int i=0;i<6;i++)
	{
		if(IsInRegion(Cross0Area[i].BasePoint,Cross0Area[i].VertexTop,Cross0Area[i].VertexButtom,Cross0Area[i].VertexRight,point))
		{
			return i+201;
		}
	}
	//单数区
	if(IsInRegion(BetSingleArea.BasePoint,BetSingleArea.VertexTop,BetSingleArea.VertexButtom,BetSingleArea.VertexRight,point))		return BET_IN_SINGLE;
	//双数区
	if(IsInRegion(BetDoubleArea.BasePoint,BetDoubleArea.VertexTop,BetDoubleArea.VertexButtom,BetDoubleArea.VertexRight,point))		return BET_IN_DOUBLE;
	//红色区
	if(IsInRegion(BetRedArea.BasePoint,BetRedArea.VertexTop,BetRedArea.VertexButtom,BetRedArea.VertexRight,point))					return BET_IN_RED;
	//黑色区
	if(IsInRegion(BetBlackArea.BasePoint,BetBlackArea.VertexTop,BetBlackArea.VertexButtom,BetBlackArea.VertexRight,point))			return BET_IN_BLACK;
	//1~18区
	if(IsInRegion(Bet1to18Area.BasePoint,Bet1to18Area.VertexTop,Bet1to18Area.VertexButtom,Bet1to18Area.VertexRight,point))			return BET_IN_1TO18;
	//19~36区
	if(IsInRegion(Bet19to36Area.BasePoint,Bet19to36Area.VertexTop,Bet19to36Area.VertexButtom,Bet19to36Area.VertexRight,point))		return BET_IN_19TO36;
	//右下的2倍区
	for(int i=0;i<3;i++)
	{
		if(IsInRegion(BetLineArea[i].BasePoint,BetLineArea[i].VertexTop,BetLineArea[i].VertexButtom,BetLineArea[i].VertexRight,point))		
		{
			if(i==0) return BET_IN_LINE1;
			if(i==1) return BET_IN_LINE2;
			if(i==2) return BET_IN_LINE3;
		}
	}
	//左边3个打区
	for(int i=0;i<3;i++)
	{
		if(IsInRegion(BetDozenArea[i].BasePoint,BetDozenArea[i].VertexTop,BetDozenArea[i].VertexButtom,BetDozenArea[i].VertexRight,point)) 
		{
			if(i==0) return BET_IN_1TO12;
			if(i==1) return BET_IN_13TO24;
			if(i==2) return BET_IN_25TO36;
		}
	}
	//复合区域
	for(int i=0;i<3;i++)
	for(int j=0;j<12;j++)
	{
		if(IsInRegion(CrossBetArea[i][j].BasePoint,CrossBetArea[i][j].VertexTop,CrossBetArea[i][j].VertexButtom,CrossBetArea[i][j].VertexRight,point)) 
		{
			return (j*3+i)+50;
		}
	}
	//横相交区域
	for(int i=0;i<3;i++)
	for(int j=0;j<12;j++)
	{
		//第一列
		if(IsInRegion(WideSideCrossArea[i][j].BasePoint,WideSideCrossArea[i][j].VertexTop,WideSideCrossArea[i][j].VertexButtom,WideSideCrossArea[i][j].VertexRight,point))
		{
			return (j*3+i)+90;
		}
	}
	//竖相交区域
	for(int i=0;i<3;i++)
	for(int j=0;j<11;j++)
	{
		if(IsInRegion(HeightSideCrossArea[i][j].BasePoint,HeightSideCrossArea[i][j].VertexTop,HeightSideCrossArea[i][j].VertexButtom,HeightSideCrossArea[i][j].VertexRight,point))
		{
			return (j*3+i)+130;
		}
	}
	//数字区
	for(int x=0;x<3;x++)
	for(int y=0;y<12;y++)
	{
		if(IsInRegion(BetNumberArea[x][y].BasePoint,BetNumberArea[x][y].VertexTop,BetNumberArea[x][y].VertexButtom,BetNumberArea[x][y].VertexRight,point)) 
		{
			return (y*3+x)+1;//返回数字
		}
	}
	return BET_IN_ERROR;
}

//判断鼠标所在区域
bool CGameLogic::RespondMouseInRegionX(CPoint point,bool Light[3][12],bool& ZeroLight,LPTSTR TipsMessage)
{
	//选择筹码区
	for(int i=0;i<6;i++)
	{
		if(IsInRegion(ChooseChipArea[i].BasePoint,ChooseChipArea[i].VertexTop,ChooseChipArea[i].VertexButtom,ChooseChipArea[i].VertexRight,point)) 
		{
			lstrcpyn(TipsMessage,TEXT("更换筹码面额"),sizeof(TCHAR)*64);
			return true;
		}
	}

	//0区
	if(IsInRegion(Bet0Area.BasePoint,Bet0Area.VertexTop,Bet0Area.VertexButtom,Bet0Area.VertexRight,point,1))
	{
		ZeroLight=true;
		lstrcpyn(TipsMessage,TEXT("下注 0  ( 1 赔 35 )"),sizeof(TCHAR)*64);
		return true;
	}
	for(int i=0;i<6;i++)
	{
		if(IsInRegion(Cross0Area[i].BasePoint,Cross0Area[i].VertexTop,Cross0Area[i].VertexButtom,Cross0Area[i].VertexRight,point))
		{
			if(i==0) {ZeroLight=true;Light[0][0]=true;Light[1][0]=true;Light[2][0]=true;lstrcpyn(TipsMessage,TEXT("下注 0,1,2,3  ( 1 赔 8 )"),sizeof(TCHAR)*64);return true;}
			if(i==1) {ZeroLight=true;Light[0][0]=true;lstrcpyn(TipsMessage,TEXT("下注 0,1  ( 1 赔 17 )"),sizeof(TCHAR)*64);return true;}
			if(i==2) {ZeroLight=true;Light[0][0]=true;Light[1][0]=true;lstrcpyn(TipsMessage,TEXT("下注 0,1,2  ( 1 赔 11 )"),sizeof(TCHAR)*64);return true;}
			if(i==3) {ZeroLight=true;Light[1][0]=true;lstrcpyn(TipsMessage,TEXT("下注 0,2  ( 1 赔 17 )"),sizeof(TCHAR)*64);return true;}
			if(i==4) {ZeroLight=true;Light[1][0]=true;Light[2][0]=true;lstrcpyn(TipsMessage,TEXT("下注 0,2,3  ( 1 赔 11 )"),sizeof(TCHAR)*64);return true;}
			if(i==5) {ZeroLight=true;Light[2][0]=true;lstrcpyn(TipsMessage,TEXT("下注 0,3  ( 1 赔 17 )"),sizeof(TCHAR)*64);return true;}
			return true;
		}
	}
	//单数区
	if(IsInRegion(BetSingleArea.BasePoint,BetSingleArea.VertexTop,BetSingleArea.VertexButtom,BetSingleArea.VertexRight,point))
	{
		bool bTemp[36]={1,0,1,0,1,0,1,0,1,0,1,0,    0,1,0,1,0,1,0,1,0,1,0,1,    1,0,1,0,1,0,1,0,1,0,1,0,};
		CopyMemory(Light,bTemp,sizeof(bTemp));
		lstrcpyn(TipsMessage,TEXT("下注所有单数  ( 1 赔 1 )"),sizeof(TCHAR)*64);
		return true;
	}
	//双数区
	if(IsInRegion(BetDoubleArea.BasePoint,BetDoubleArea.VertexTop,BetDoubleArea.VertexButtom,BetDoubleArea.VertexRight,point))
	{
		bool bTemp[36]={0,1,0,1,0,1,0,1,0,1,0,1,     1,0,1,0,1,0,1,0,1,0,1,0,    0,1,0,1,0,1,0,1,0,1,0,1,};
		CopyMemory(Light,bTemp,sizeof(bTemp));
		lstrcpyn(TipsMessage,TEXT("下注所有双数  ( 1 赔 1 )"),sizeof(TCHAR)*64);
		return true; 
	}
	//红色区
	if(IsInRegion(BetRedArea.BasePoint,BetRedArea.VertexTop,BetRedArea.VertexButtom,BetRedArea.VertexRight,point))
	{
		bool bTemp[36]={1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,1,0,1,0,1,1,0,1,1,0,1,1,0,1};
		CopyMemory(Light,bTemp,sizeof(bTemp));	
		lstrcpyn(TipsMessage,TEXT("下注所有红色数字  ( 1 赔 1 )"),sizeof(TCHAR)*64);
		return true;
	}
	//黑色区
	if(IsInRegion(BetBlackArea.BasePoint,BetBlackArea.VertexTop,BetBlackArea.VertexButtom,BetBlackArea.VertexRight,point))
	{
		bool bTemp[36]={0,1,0,1,1,0,0,1,0,1,1,0,1,0,1,1,0,1,1,0,1,1,0,1,0,1,0,0,1,0,0,1,0,0,1,0};
		CopyMemory(Light,bTemp,sizeof(bTemp));	
		lstrcpyn(TipsMessage,TEXT("下注所有黑色数字  ( 1 赔 1 )"),sizeof(TCHAR)*64);
		return true;
	}
	//1~18区
	if(IsInRegion(Bet1to18Area.BasePoint,Bet1to18Area.VertexTop,Bet1to18Area.VertexButtom,Bet1to18Area.VertexRight,point)) 
	{
		bool bTemp[36]={1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,};
		CopyMemory(Light,bTemp,sizeof(bTemp));
		lstrcpyn(TipsMessage,TEXT("下注数字 1~18  ( 1 赔 1 )"),sizeof(TCHAR)*64);
		return true;
	}
	//19~36区
	if(IsInRegion(Bet19to36Area.BasePoint,Bet19to36Area.VertexTop,Bet19to36Area.VertexButtom,Bet19to36Area.VertexRight,point)) 
	{
		bool bTemp[36]={0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,};
		CopyMemory(Light,bTemp,sizeof(bTemp));
		lstrcpyn(TipsMessage,TEXT("下注数字 19~36  ( 1 赔 1 )"),sizeof(TCHAR)*64);
		return true;
	}
	//右下的2倍区
	for(int i=0;i<3;i++)
	{
		if(IsInRegion(BetLineArea[i].BasePoint,BetLineArea[i].VertexTop,BetLineArea[i].VertexButtom,BetLineArea[i].VertexRight,point)) 
		{
			for(int j=0;j<12;j++) Light[i][j]=true;	
			if(i==0) lstrcpyn(TipsMessage,TEXT("下注数字 第一列  ( 1 赔 2 )"),sizeof(TCHAR)*64);
			if(i==1) lstrcpyn(TipsMessage,TEXT("下注数字 第二列  ( 1 赔 2 )"),sizeof(TCHAR)*64);
			if(i==2) lstrcpyn(TipsMessage,TEXT("下注数字 第三列  ( 1 赔 2 )"),sizeof(TCHAR)*64);
			return true;
		}
	}
	//左边3个打区
	for(int i=0;i<3;i++)
	{
		if(IsInRegion(BetDozenArea[i].BasePoint,BetDozenArea[i].VertexTop,BetDozenArea[i].VertexButtom,BetDozenArea[i].VertexRight,point)) 
		{
			for(int j=4*i;j<4*i+4;j++) {Light[0][j]=true;	Light[1][j]=true;Light[2][j]=true;}
			if(i==0) lstrcpyn(TipsMessage,TEXT("下注数字 1~12  ( 1 赔 2 )"),sizeof(TCHAR)*64);
			if(i==1) lstrcpyn(TipsMessage,TEXT("下注数字 13~24  ( 1 赔 2 )"),sizeof(TCHAR)*64);
			if(i==2) lstrcpyn(TipsMessage,TEXT("下注数字 25~36  ( 1 赔 2 )"),sizeof(TCHAR)*64);
			return true;
		}
	}
	//复合区域
	for(int j=1;j<12;j++)
	{
		//第一列
		if(IsInRegion(CrossBetArea[0][j].BasePoint,CrossBetArea[0][j].VertexTop,CrossBetArea[0][j].VertexButtom,CrossBetArea[0][j].VertexRight,point)) 
		{
			Light[0][j]=true;	Light[1][j]=true;	Light[2][j]=true;
			Light[0][j-1]=true;	Light[1][j-1]=true;	Light[2][j-1]=true;
						
			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d,%d,%d, %d,%d,%d ( 1 赔 5 )"),(j-1)*3+1,(j-1)*3+2,(j-1)*3+3,j*3+1,j*3+2,j*3+3 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
		//第二列
		if(IsInRegion(CrossBetArea[1][j].BasePoint,CrossBetArea[1][j].VertexTop,CrossBetArea[1][j].VertexButtom,CrossBetArea[1][j].VertexRight,point)) 
		{
			Light[1][j]=true;		Light[1][j-1]=true;
			Light[0][j]=true;		Light[0][j-1]=true;

			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d, %d, %d, %d ( 1 赔 8 )"),(j-1)*3+1,(j-1)*3+2,j*3+1,j*3+2 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
		//第三列
		if(IsInRegion(CrossBetArea[2][j].BasePoint,CrossBetArea[2][j].VertexTop,CrossBetArea[2][j].VertexButtom,CrossBetArea[2][j].VertexRight,point)) 
		{
			Light[2][j]=true;		Light[2][j-1]=true;
			Light[1][j]=true;		Light[1][j-1]=true;

			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d, %d, %d, %d ( 1 赔 8 )"),(j-1)*3+2,(j-1)*3+3,j*3+2,j*3+3 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
	}
	//横相交区域
	for(int j=0;j<12;j++)
	{
		//第一列
		if(IsInRegion(WideSideCrossArea[0][j].BasePoint,WideSideCrossArea[0][j].VertexTop,WideSideCrossArea[0][j].VertexButtom,WideSideCrossArea[0][j].VertexRight,point))
		{
			Light[0][j]=true;Light[1][j]=true;Light[2][j]=true;
			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d, %d, %d ( 1 赔 11 )"),j*3+1,j*3+2,j*3+3 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
		//第二列
		if(IsInRegion(WideSideCrossArea[1][j].BasePoint,WideSideCrossArea[1][j].VertexTop,WideSideCrossArea[1][j].VertexButtom,WideSideCrossArea[1][j].VertexRight,point))
		{
			Light[0][j]=true;Light[1][j]=true;
			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d, %d ( 1 赔 17 )"),j*3+1,j*3+2 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
		//第三列
		if(IsInRegion(WideSideCrossArea[2][j].BasePoint,WideSideCrossArea[2][j].VertexTop,WideSideCrossArea[2][j].VertexButtom,WideSideCrossArea[2][j].VertexRight,point))
		{
			Light[2][j]=true;Light[1][j]=true;
			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d, %d ( 1 赔 17 )"),j*3+2,j*3+3 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
	}
	//竖相交区域
	for(int i=0;i<3;i++)
	for(int j=0;j<11;j++)
	{
		if(IsInRegion(HeightSideCrossArea[i][j].BasePoint,HeightSideCrossArea[i][j].VertexTop,HeightSideCrossArea[i][j].VertexButtom,HeightSideCrossArea[i][j].VertexRight,point))
		{
			Light[i][j]=true;Light[i][j+1]=true;
			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d, %d ( 1 赔 17 )"),j*3+i+1,j*3+i+4 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
			return true;
		}
	}
	//循环检测数字区
	bool InNumberArea=false;
	for(int x=0;x<3;x++)
	for(int y=0;y<12;y++)
	{
		if(IsInRegion(BetNumberArea[x][y].BasePoint,BetNumberArea[x][y].VertexTop,BetNumberArea[x][y].VertexButtom,BetNumberArea[x][y].VertexRight,point)) 
		{
			Light[x][y]=InNumberArea=true;
			TCHAR szBuffer[64];
			_snprintf(szBuffer,sizeof(szBuffer),TEXT("下注数字 %d ( 1 赔 35 )"),y*3+x+1 );
			lstrcpyn(TipsMessage,szBuffer,sizeof(TCHAR)*64);
		}
	}

	return InNumberArea;
}

bool CGameLogic::IsInRegion(CPoint BasePoint,CPoint VertexTop,CPoint VertexButtom,CPoint VertexRight,CPoint point,bool bReverseSlop4)
{
	//基准点坐标是该点在窗口的坐标；
	//三个顶点的坐标都是相对于基准点的坐标；
	//鼠标位置也转换为相对与基准点的坐标；
	//
	//							（顶点一）
	//								↓
	//								*
	//                slope1    *		*	  slope3
	//						*				*
	//					*						*
	//（基准点）→  *								*  ←（顶点三）
	//					*						*
	//						*				*
	//			      slope2	*		*	  slope4
	//								*
	//								↑
	//							（顶点二）

	//转换基准点坐标为屏幕绝对坐标
	BasePoint.x=m_nScreenWidth/2+BasePoint.x;
	BasePoint.y=m_nScreenHeight/2+BasePoint.y;

	//转换鼠标坐标为相对于基准点坐标
	MousePoint.x=point.x-BasePoint.x;
	MousePoint.y=point.y-BasePoint.y;

	//计算四条边的斜率
	slope1=(float)(VertexTop.y)/(VertexTop.x);
	slope2=(float)(VertexButtom.y)/(VertexButtom.x);
	slope3=(float)(VertexRight.y-VertexTop.y)/(VertexRight.x-VertexTop.x);
	slope4=(float)(VertexRight.y-VertexButtom.y)/(VertexRight.x-VertexButtom.x);

	//计算右边两条边的截轴
	float b3=(VertexRight.y)-slope3*VertexRight.x;
	float b4=(VertexRight.y)-slope4*VertexRight.x;

	//0号位置的特殊四边形,第4条边反转方向
	if(bReverseSlop4)
	{
		if( MousePoint.x>0 && MousePoint.y > MousePoint.x*slope1)	//y在slope1右边  
		if( MousePoint.x>0 && MousePoint.y < MousePoint.x*slope2)	//y在slope2右边
		if( MousePoint.x<(VertexButtom.x)&&MousePoint.y>(MousePoint.x*slope3+b3))//y在slope3左边
		if( MousePoint.x<(VertexButtom.x)&&MousePoint.y>(MousePoint.x*slope4+b4))//y在slope4左边
		{
			return true;
		}
	}

	//判断鼠标点是否在此四边形区域内
	if( MousePoint.x>0 && MousePoint.y > MousePoint.x*slope1)	//y在slope1右边  
	if( MousePoint.x>0 && MousePoint.y < MousePoint.x*slope2)	//y在slope2右边
	if( MousePoint.x<(VertexRight.x)&&MousePoint.y>(MousePoint.x*slope3+b3))//y在slope3左边
	if( MousePoint.x<(VertexRight.x)&&MousePoint.y<(MousePoint.x*slope4+b4))//y在slope4左边
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
