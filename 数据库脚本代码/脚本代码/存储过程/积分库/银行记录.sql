

----------------------------------------------------------------------------------------------------

USE QPGameScoreDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SavingGameGold]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SavingGameGold]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DrawoutGameGold]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DrawoutGameGold]
GO

----------------------------------------------------------------------------------------------------

-- 存储金币
CREATE PROC GSP_GR_SavingGameGold
	@dwUserID INT,
	@lSwapScore BIGINT,
	@dwKindID INT,
	@dwServerID INT,
	@strClientIP NVARCHAR(15)
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @SourceGameID INT
DECLARE @SourceAccounts NVARCHAR(31)
DECLARE @InsureScore BIGINT
DECLARE @TradeType TINYINT

-- 执行逻辑
BEGIN
	SET @TradeType=1
	SELECT @InsureScore=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

--	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score-@lSwapScore,InsureScore=InsureScore+@lSwapScore
--	WHERE UserID=@dwUserID

	SELECT @SourceGameID=GameID,@SourceAccounts=Accounts FROM QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo
	WHERE UserID=@dwUserID

	INSERT QPTreasureDBLink.QPTreasureDB.dbo.RecordInsure(KindID,ServerID,SourceUserID,SourceGameID,SourceAccounts,InsureScore,SwapScore,TradeType,ClientIP)
	VALUES (@dwKindID,@dwServerID,@dwUserID,@SourceGameID,@SourceAccounts,@InsureScore,@lSwapScore,@TradeType,@strClientIP)
	
END
RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 取出金币
CREATE PROC GSP_GR_DrawoutGameGold
	@dwUserID INT,
	@lSwapScore BIGINT,
	@dwKindID INT,
	@dwServerID INT,
	@strClientIP NVARCHAR(15)
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @SourceGameID INT
DECLARE @SourceAccounts NVARCHAR(31)
DECLARE @InsureScore BIGINT
DECLARE @TradeType TINYINT

-- 执行逻辑
BEGIN
	SET @TradeType=2
	SELECT @InsureScore=InsureScore FROM QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

--	UPDATE QPTreasureDBLink.QPTreasureDB.dbo.GameScoreInfo SET Score=Score+@lSwapScore,InsureScore=InsureScore-@lSwapScore
--	WHERE UserID=@dwUserID

	SELECT @SourceGameID=GameID,@SourceAccounts=Accounts FROM QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo
	WHERE UserID=@dwUserID

	INSERT QPTreasureDBLink.QPTreasureDB.dbo.RecordInsure(KindID,ServerID,SourceUserID,SourceGameID,SourceAccounts,InsureScore,SwapScore,TradeType,ClientIP)
	VALUES (@dwKindID,@dwServerID,@dwUserID,@SourceGameID,@SourceAccounts,@InsureScore,@lSwapScore,@TradeType,@strClientIP)
END

RETURN 0

GO
