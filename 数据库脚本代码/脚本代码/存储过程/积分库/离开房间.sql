
----------------------------------------------------------------------------------------------------
-- 增加用户魅力(修改后的)，保留金币,@lScore(改变值)
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LeaveGameServer]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LeaveGameServer]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 离开房间
CREATE PROC GSP_GR_LeaveGameServer
	@dwUserID INT,								-- 用户 I D
	@lScore BIGINT,								-- 用户分数
	@lGameGold BIGINT,							-- 游戏金币
	@lInsureScore BIGINT,						-- 游戏金币
	@lLoveliness BIGINT,						-- 玩家魅力	
	@lRevenue BIGINT,							-- 用户税收
	@lWinCount INT,								-- 胜利盘数
	@lLostCount INT,							-- 失败盘数
	@lDrawCount INT,							-- 和局盘数
	@lFleeCount INT,							-- 断线数目
	@lExperience INT,							-- 用户经验
	@dwPlayTimeCount INT,						-- 游戏时间
	@dwOnLineTimeCount INT,						-- 在线时间
	@wKindID INT,								-- 游戏 I D
	@wServerID INT,								-- 房间 I D
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

	-- 离开房间
	INSERT RecordUserLeave (UserID, Score, KindID, ServerID, PlayTimeCount, OnLineTimeCount) 
	VALUES (@dwUserID, @lScore, @wKindID, @wServerID, @dwPlayTimeCount, @dwOnLineTimeCount)

	-- 金币操作 ------------------------------------------------------------------------------------
	-- 锁定解除
	DELETE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreLocker WHERE UserID=@dwUserID

	-- 累加金币
	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@lGameGold,InsureScore=InsureScore+@lInsureScore WHERE UserID=@dwUserID

	IF @@ROWCOUNT=0
	BEGIN
		-- 赠送金币
		INSERT QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo(UserID, RegisterIP,LastLogonIP)
		VALUES (@dwUserID, @strClientIP, @strClientIP)

		UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@lGameGold,InsureScore=InsureScore+@lInsureScore WHERE UserID=@dwUserID
	END

	-- 用户魅力
	INSERT QPTreasureDBLink.QPTreasureDB.dbo.RecordUserLoveliness (UserID, KindID, ServerID,Loveliness,ClientIP)
	VALUES (@dwUserID,@wKindID,@wServerID,@lLoveliness,@strClientIP)

	-- 金币操作 ------------------------------------------------------------------------------------

	-- 用户经验
	UPDATE QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo SET Experience=Experience+@lExperience,Loveliness=@lLoveliness WHERE UserID=@dwUserID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------