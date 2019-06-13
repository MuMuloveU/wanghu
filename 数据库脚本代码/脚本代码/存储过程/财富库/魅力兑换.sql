

----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ExchangeLoveliness]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ExchangeLoveliness]
GO

----------------------------------------------------------------------------------------------------
-- 魅力值兑换
CREATE PROC GSP_GR_ExchangeLoveliness
	@dwUserID INT,
	@lLoveliness BIGINT,
	@lInsureScore BIGINT,
	@dwKindID INT,
	@wServerID INT,
	@strClientIP NVARCHAR(15)
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @CharmValue BIGINT

-- 执行逻辑
BEGIN
	-- 插入记录
	INSERT RecordExchangeLoveliness (UserID,KindID,ServerID,Loveliness,InsureScore,ClientIP)
	VALUES (@dwUserID,@dwKindID,@wServerID,@lLoveliness,@lInsureScore,@strClientIP)	

END

RETURN 0

GO


----------------------------------------------------------------------------------------------------