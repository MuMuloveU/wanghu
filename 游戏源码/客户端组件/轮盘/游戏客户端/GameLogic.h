#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE
#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//注意：这里的基准点坐标，是相对于图片正中心的。如CPoint(-118,-67)，表示距离正中心点左边118，上方67个象素的位置
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//位置宏定义
#define BET_IN_ERROR		255
#define BET_IN_LINE1		37	
#define BET_IN_LINE2		38
#define BET_IN_LINE3		39
#define BET_IN_1TO12		40
#define BET_IN_13TO24		41
#define BET_IN_25TO36		42
#define BET_IN_1TO18		43
#define BET_IN_19TO36		44
#define	BET_IN_SINGLE		45
#define	BET_IN_DOUBLE		46
#define BET_IN_RED			47
#define BET_IN_BLACK		48

//数字颜色定义
#define COLOR_0				0
#define COLOR_RED			1
#define COLOR_BLACK			2
//////////////////////////////////////////////////////////////////////////
//平行四边形顶点结构体
struct RegionInfo
{
	CPoint BasePoint;
	CPoint VertexTop;
	CPoint VertexButtom;
	CPoint VertexRight;
};

//////////////////////////////////////////////////////////////////////////
//轮盘上的数字顺序
const static BYTE numberArrayIndex[37]={0,26,3,35,12,28,7,29,18,22,9,31,14,20,1,33,16,24,5,10,23,8,30,11,36,13,27,6,34,17,25,2,21,4,19,15,32};
//选择筹码区域
const static RegionInfo ChooseChipArea[6]=
{
	CPoint(-162,55),CPoint(21,-12),CPoint(22,15),CPoint(39,0),
	CPoint(-125,82),CPoint(21,-12),CPoint(22,15),CPoint(39,0),
	CPoint(-86,111),CPoint(21,-12),CPoint(22,15),CPoint(39,0),
	CPoint(-50,136),CPoint(21,-12),CPoint(22,15),CPoint(39,0),
	CPoint(-13,165),CPoint(21,-12),CPoint(22,15),CPoint(39,0),
	CPoint(27,194),CPoint(21,-12),CPoint(22,15),CPoint(39,0),
};

//3列数字区域的四个角
const static RegionInfo CornerLine1 = { CPoint(-118,-67),CPoint(-77,-93),CPoint(247,175),CPoint(285,140) };
const static RegionInfo CornerLine2 = { CPoint(-74,-94),CPoint(-36,-119),CPoint(287,138),CPoint(324,103) };
const static RegionInfo CornerLine3 = { CPoint(-34,-120),CPoint(4,-144),CPoint(327,102),CPoint(362,71)	 };
//0区域
const static RegionInfo Bet0Area		={	CPoint(-119,-69),CPoint(15,-40),CPoint(119,-75),CPoint(75,-74)};
//右边2比1区
const static RegionInfo BetLineArea[3]	={	CPoint(247,176),CPoint(40,-37),CPoint(36,24),CPoint(75,-14),    CPoint(287,138),CPoint(39,-35),CPoint(35,23),CPoint(74,-14),    CPoint(330,103),CPoint(35,-33),CPoint(33,21),CPoint(68,-12)};
//左边打区
const static RegionInfo BetDozenArea[3]	={	CPoint(-170,-39),CPoint(42,-27),CPoint(107,74),CPoint(140,45),  CPoint(-61,36),CPoint(42,-29),CPoint(119,84),CPoint(153,52),    CPoint(60,121),CPoint(43,-33),CPoint(134,92),CPoint(167,56)};
//1至18
const static RegionInfo Bet1to18Area	={	CPoint(-206,-10),CPoint(42,-26),CPoint(49,37),CPoint(92,10) };
//19~36
const static RegionInfo Bet19to36Area	={	CPoint(90,204),	CPoint(43,-37),CPoint(67,49),CPoint(110,10) };
//单数
const static RegionInfo BetSingleArea	={	CPoint(23,156),	CPoint(44,-34),CPoint(64,46),CPoint(105,11) };
//双数
const static RegionInfo BetDoubleArea	={	CPoint(-154,29),CPoint(43,-30),CPoint(53,39),CPoint(96,8)	};
//红
const static RegionInfo BetRedArea		={  CPoint(-39,111),CPoint(43,-32),CPoint(59,43),CPoint(102,10) };
//黑
const static RegionInfo BetBlackArea	={	CPoint(-99,68),	CPoint(45,-30),CPoint(57,43),CPoint(99,10)  };
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//单个数字区域坐标,由上面3个矩形计算得到
extern RegionInfo BetNumberArea[3][12];
//十字形交叉多选区域矩形坐标，由数字区域坐标计算得到，可以选择4个或6个数字
extern RegionInfo CrossBetArea[3][12];
//两个相交边区域矩形坐标(横向相交)，由数字区域坐标计算得到，可以选择2个或3个数字
extern RegionInfo WideSideCrossArea[3][12];
//两个相交边区域矩形坐标(竖向相交)，由数字区域坐标计算得到，可以选择2个数字
extern RegionInfo HeightSideCrossArea[3][12];
//与零相交的区域
extern RegionInfo Cross0Area[6];
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//从3个矩形12个点计算出36*4个数字区域点和各种相交区域的点，大概有36*4*4这么多个吧。
static void calcPoint()
{
	//计算数字区域顶点坐标
	//基准点偏移
	int tempx[12]={0,2,4,6,  9,14,17,24,   30,38,47,55};
	int tempy[12]={0,4,6,9,  14,18,23,29,  36,42,50,58};
	//上顶点偏移
	int topx[12]={};
	int topy[12]={6,7,8,8, 8,9,11,12, 13,13,14,15,};
	//右顶点偏移
	int rightx[12]={2,1,1,1,  1,1,1,1,  -1,-1,-1,-1,};
	int righty[12]={7,6,5,6,  4,4,4,4,  3,3,3,3,};
	for(BYTE i=0;i<12;i++)
	{
		//基准点坐标                   =							线段长度 /12							  +增量		 + 初始坐标
		BetNumberArea[0][i].BasePoint.x= (i*(CornerLine1.VertexButtom.x-CornerLine1.BasePoint.x - 66*1 )/12)  + tempx[i] + CornerLine1.BasePoint.x;	
		BetNumberArea[0][i].BasePoint.y= (i*(CornerLine1.VertexButtom.y-CornerLine1.BasePoint.y - 66*1 )/12)  + tempy[i] + CornerLine1.BasePoint.y;
		BetNumberArea[1][i].BasePoint.x= (i*(CornerLine2.VertexButtom.x-CornerLine2.BasePoint.x - 66*1 )/12)  + tempx[i] + CornerLine2.BasePoint.x;	
		BetNumberArea[1][i].BasePoint.y= (i*(CornerLine2.VertexButtom.y-CornerLine2.BasePoint.y - 66*1 )/12)  + tempy[i] + CornerLine2.BasePoint.y;
		BetNumberArea[2][i].BasePoint.x= (i*(CornerLine3.VertexButtom.x-CornerLine3.BasePoint.x - 66*1 )/12)  + tempx[i] + CornerLine3.BasePoint.x;	
		BetNumberArea[2][i].BasePoint.y= (i*(CornerLine3.VertexButtom.y-CornerLine3.BasePoint.y - 66*1 )/12)  + tempy[i] + CornerLine3.BasePoint.y;

		//上方点坐标
		BetNumberArea[0][i].VertexTop.x= (CornerLine1.VertexTop.x-CornerLine1.BasePoint.x) + topx[i];			
		BetNumberArea[0][i].VertexTop.y= -(CornerLine1.VertexRight.y-CornerLine1.VertexTop.y)/12 - topy[i];
		BetNumberArea[1][i].VertexTop.x= (CornerLine2.VertexTop.x-CornerLine2.BasePoint.x) + topx[i];			
		BetNumberArea[1][i].VertexTop.y= -(CornerLine2.VertexRight.y-CornerLine2.VertexTop.y)/12 - topy[i];
		BetNumberArea[2][i].VertexTop.x= (CornerLine3.VertexTop.x-CornerLine3.BasePoint.x) + topx[i];			
		BetNumberArea[2][i].VertexTop.y= -(CornerLine3.VertexRight.y-CornerLine3.VertexTop.y)/12 - topy[i];

		//下方点坐标
		BetNumberArea[0][i].VertexButtom.x= (CornerLine1.VertexButtom.x-CornerLine1.BasePoint.x)/12 ;		
		BetNumberArea[0][i].VertexButtom.y= (CornerLine1.VertexButtom.y-CornerLine1.BasePoint.y)/12 ;
		BetNumberArea[1][i].VertexButtom.x= (CornerLine2.VertexButtom.x-CornerLine2.BasePoint.x)/12 ;		
		BetNumberArea[1][i].VertexButtom.y= (CornerLine2.VertexButtom.y-CornerLine2.BasePoint.y)/12 ;
		BetNumberArea[2][i].VertexButtom.x= (CornerLine3.VertexButtom.x-CornerLine3.BasePoint.x)/12 ;		
		BetNumberArea[2][i].VertexButtom.y= (CornerLine3.VertexButtom.y-CornerLine3.BasePoint.y)/12 ;

		//右边点坐标
		BetNumberArea[0][i].VertexRight.x=  (CornerLine1.VertexRight.x-CornerLine1.VertexTop.x)/12 +BetNumberArea[0][i].VertexTop.x -rightx[i];	
		BetNumberArea[0][i].VertexRight.y=  -(CornerLine1.VertexRight.y-CornerLine1.VertexTop.y)/12 + righty[i];
		BetNumberArea[1][i].VertexRight.x=  (CornerLine2.VertexRight.x-CornerLine2.VertexTop.x)/12 +BetNumberArea[0][i].VertexTop.x -rightx[i];	
		BetNumberArea[1][i].VertexRight.y=  -(CornerLine2.VertexRight.y-CornerLine2.VertexTop.y)/12 + righty[i];
		BetNumberArea[2][i].VertexRight.x=  (CornerLine3.VertexRight.x-CornerLine3.VertexTop.x)/12 +BetNumberArea[0][i].VertexTop.x -rightx[i];	
		BetNumberArea[2][i].VertexRight.y=  -(CornerLine3.VertexRight.y-CornerLine3.VertexTop.y)/12 + righty[i];
	}
	//计算复合区域顶点坐标
	for(BYTE i=0;i<3;i++)
	for(BYTE j=0;j<12;j++)
	{
		CrossBetArea[i][j].BasePoint.x=BetNumberArea[i][j].BasePoint.x-8;
		CrossBetArea[i][j].BasePoint.y=BetNumberArea[i][j].BasePoint.y;

		CrossBetArea[i][j].VertexTop.x=8;
		CrossBetArea[i][j].VertexTop.y=-8;

		CrossBetArea[i][j].VertexButtom.x=8;
		CrossBetArea[i][j].VertexButtom.y=8;

		CrossBetArea[i][j].VertexRight.x=16;
		CrossBetArea[i][j].VertexRight.y=0;
	}
	//计算横向相交顶点坐标
	for(BYTE i=0;i<3;i++)
	for(BYTE j=0;j<12;j++)
	{
		WideSideCrossArea[i][j].BasePoint.x=BetNumberArea[i][j].BasePoint.x;
		WideSideCrossArea[i][j].BasePoint.y=BetNumberArea[i][j].BasePoint.y+6;
		
		WideSideCrossArea[i][j].VertexTop.x=6;
		WideSideCrossArea[i][j].VertexTop.y=-4;

		WideSideCrossArea[i][j].VertexButtom.x=BetNumberArea[i][j].VertexButtom.x-6;
		WideSideCrossArea[i][j].VertexButtom.y=BetNumberArea[i][j].VertexButtom.y-4;

		WideSideCrossArea[i][j].VertexRight.x=BetNumberArea[i][j].VertexButtom.x;
		WideSideCrossArea[i][j].VertexRight.y=BetNumberArea[i][j].VertexButtom.y-10;
	}
	//计算竖向相交顶点坐标
	for(BYTE i=0;i<3;i++)
	for(BYTE j=0;j<4;j++)
	{
		HeightSideCrossArea[i][j].BasePoint.x=BetNumberArea[i][j].BasePoint.x+BetNumberArea[i][j].VertexButtom.x+1;
		HeightSideCrossArea[i][j].BasePoint.y=BetNumberArea[i][j].BasePoint.y+BetNumberArea[i][j].VertexButtom.y-10;
		
		HeightSideCrossArea[i][j].VertexTop.x=20;
		HeightSideCrossArea[i][j].VertexTop.y=-15;

		HeightSideCrossArea[i][j].VertexButtom.x=5;
		HeightSideCrossArea[i][j].VertexButtom.y=8;

		HeightSideCrossArea[i][j].VertexRight.x=36;
		HeightSideCrossArea[i][j].VertexRight.y=-10;
	}
	for(BYTE i=0;i<3;i++)
	for(BYTE j=4;j<8;j++)
	{
		HeightSideCrossArea[i][j].BasePoint.x=BetNumberArea[i][j].BasePoint.x+BetNumberArea[i][j].VertexButtom.x+1;
		HeightSideCrossArea[i][j].BasePoint.y=BetNumberArea[i][j].BasePoint.y+BetNumberArea[i][j].VertexButtom.y-7;
		
		HeightSideCrossArea[i][j].VertexTop.x=28;
		HeightSideCrossArea[i][j].VertexTop.y=-20;

		HeightSideCrossArea[i][j].VertexButtom.x=12;
		HeightSideCrossArea[i][j].VertexButtom.y=8;

		HeightSideCrossArea[i][j].VertexRight.x=42;
		HeightSideCrossArea[i][j].VertexRight.y=-12;
	}
	for(BYTE i=0;i<3;i++)
	for(BYTE j=8;j<12;j++)
	{
		HeightSideCrossArea[i][j].BasePoint.x=BetNumberArea[i][j].BasePoint.x+BetNumberArea[i][j].VertexButtom.x+1;
		HeightSideCrossArea[i][j].BasePoint.y=BetNumberArea[i][j].BasePoint.y+BetNumberArea[i][j].VertexButtom.y-7;
		
		HeightSideCrossArea[i][j].VertexTop.x=25;
		HeightSideCrossArea[i][j].VertexTop.y=-18;

		HeightSideCrossArea[i][j].VertexButtom.x=12;
		HeightSideCrossArea[i][j].VertexButtom.y=8;

		HeightSideCrossArea[i][j].VertexRight.x=50;
		HeightSideCrossArea[i][j].VertexRight.y=-10;
	}
	//与0相交区域
	Cross0Area[0].BasePoint.x=Bet0Area.BasePoint.x-6;
	Cross0Area[0].BasePoint.y=Bet0Area.BasePoint.y;
	Cross0Area[0].VertexTop.x=6;
	Cross0Area[0].VertexTop.y=-6;
	Cross0Area[0].VertexButtom.x=6;
	Cross0Area[0].VertexButtom.y=6;
	Cross0Area[0].VertexRight.x=12;
	Cross0Area[0].VertexRight.y=0;

	Cross0Area[1].BasePoint.x=BetNumberArea[0][0].BasePoint.x;
	Cross0Area[1].BasePoint.y=BetNumberArea[0][0].BasePoint.y-6;
	Cross0Area[1].VertexTop.x=34;
	Cross0Area[1].VertexTop.y=-22;
	Cross0Area[1].VertexButtom.x=5;
	Cross0Area[1].VertexButtom.y=5;
	Cross0Area[1].VertexRight.x=39;
	Cross0Area[1].VertexRight.y=-18;

	Cross0Area[2].BasePoint.x=BetNumberArea[1][0].BasePoint.x-10;
	Cross0Area[2].BasePoint.y=BetNumberArea[1][0].BasePoint.y;
	Cross0Area[2].VertexTop.x=9;
	Cross0Area[2].VertexTop.y=-7;
	Cross0Area[2].VertexButtom.x=5;
	Cross0Area[2].VertexButtom.y=5;
	Cross0Area[2].VertexRight.x=16;
	Cross0Area[2].VertexRight.y=0;

	Cross0Area[3].BasePoint.x=BetNumberArea[1][0].BasePoint.x;
	Cross0Area[3].BasePoint.y=BetNumberArea[1][0].BasePoint.y-6;
	Cross0Area[3].VertexTop.x=33;
	Cross0Area[3].VertexTop.y=-21;
	Cross0Area[3].VertexButtom.x=7;
	Cross0Area[3].VertexButtom.y=6;
	Cross0Area[3].VertexRight.x=38;
	Cross0Area[3].VertexRight.y=-16;

	Cross0Area[4].BasePoint.x=BetNumberArea[2][0].BasePoint.x-9;
	Cross0Area[4].BasePoint.y=BetNumberArea[2][0].BasePoint.y;
	Cross0Area[4].VertexTop.x=9;
	Cross0Area[4].VertexTop.y=-7;
	Cross0Area[4].VertexButtom.x=6;
	Cross0Area[4].VertexButtom.y=6;
	Cross0Area[4].VertexRight.x=16;
	Cross0Area[4].VertexRight.y=0;

	Cross0Area[5].BasePoint.x=BetNumberArea[2][0].BasePoint.x;
	Cross0Area[5].BasePoint.y=BetNumberArea[2][0].BasePoint.y-6;
	Cross0Area[5].VertexTop.x=30;
	Cross0Area[5].VertexTop.y=-19;
	Cross0Area[5].VertexButtom.x=7;
	Cross0Area[5].VertexButtom.y=6;
	Cross0Area[5].VertexRight.x=34;
	Cross0Area[5].VertexRight.y=-16;
};



//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogic
{
	//运行变量
protected:
	//鼠标位置控制
	CPoint MousePoint;
	//平行四边形四条边的斜率
	float slope1,slope2,slope3,slope4;
	//窗口宽高
	int m_nScreenWidth,m_nScreenHeight;		

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//功能函数
public:
	//得到鼠标所在区域,同时设置鼠标所影响数字区域的灯光效果
	bool RespondMouseInRegionX(CPoint point,bool Light[3][12],bool& ZeroLight,LPTSTR TipsMessage);
	//响应点击
	WORD GetMouseClickRegion(CPoint point);
	//设置窗口大小
	void SetDxWindowSize(int nWidth,int nHeight);
	//计算得分
	long ComputeScore(long ChipsInfo[256],BYTE WinNumber);

	//取得数字颜色
	BYTE GetNumberColor(BYTE bNumber);
	//内部函数
private:
	bool IsInRegion(CPoint BasePoin,CPoint VertexTop,CPoint VertexButtom,CPoint VertexRight,CPoint MousePoint,bool bReverseSlop4=false);
	//计算得分
	long CountScore(long ChipsInfo[256],int index[256],int count);
};

//////////////////////////////////////////////////////////////////////////

#endif