
----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteGameScore]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏写分
CREATE PROC GSP_GR_WriteGameScore
	@dwUserID INT,								-- 用户 I D
	@lScore BIGINT,								-- 用户分数
	@lRevenue BIGINT,							-- 游戏税收
	@lWinCount INT,								-- 胜利盘数
	@lLostCount INT,							-- 失败盘数
	@lDrawCount INT,							-- 和局盘数
	@lFleeCount INT,							-- 断线数目
	@lExperience INT,							-- 用户经验
	@dwPlayTimeCount INT,						-- 游戏时间
	@dwOnLineTimeCount INT,						-- 在线时间
	@wKindID INT,							-- 游戏 I D
	@wServerID INT,						-- 房间 I D
	@strClientIP NVARCHAR(15)					-- 连接地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 用户积分
	UPDATE GameScoreInfo SET Score=Score+@lScore, WinCount=WinCount+@lWinCount, LostCount=LostCount+@lLostCount,
		DrawCount=DrawCount+@lDrawCount, FleeCount=FleeCount+@lFleeCount, PlayTimeCount=PlayTimeCount+@dwPlayTimeCount, 
		OnLineTimeCount=OnLineTimeCount+@dwOnLineTimeCount
	WHERE UserID=@dwUserID

	-- 用户经验
	UPDATE QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo SET Experience=Experience+@lExperience WHERE UserID=@dwUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------