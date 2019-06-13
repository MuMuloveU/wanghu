

----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_RecordFlowerGrant]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_RecordFlowerGrant]
GO

----------------------------------------------------------------------------------------------------

-- 鲜花赠送
CREATE PROC GSP_GR_RecordFlowerGrant
	@dwSendUserID INT,			-- 赠送者
	@dwRcvUserID INT,			-- 接受者
	@dwSendUserLoveliness INT,	-- 魅力值
	@dwRcvUserLoveliness INT,	-- 魅力值
	@dwFlowerCount INT,			-- 鲜花数目
	@strFlowerName NVARCHAR(32),-- 赠送名称
	@lFlowerPay BIGINT,			-- 赠送付费
	@wKindID INT,				-- 类型标识
	@wServerID INT,				-- 房间标识
	@strGrantIP	VARCHAR(15)		-- 赠送地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	INSERT RecordFlowerGrant(SendUserID,RcvUserID,KindID,ServerID,FlowerName,FlowerCount,FlowerPay,SendUserLoveliness,RcvUserLoveliness,GrantIP)
    VALUES(@dwSendUserID,@dwRcvUserID,@wKindID,@wServerID,@strFlowerName,@dwFlowerCount,@lFlowerPay,@dwSendUserLoveliness,@dwRcvUserLoveliness,@strGrantIP)
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------