
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadAndroidUser]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadAndroidUser]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 加载机器人
CREATE PROC GSP_GR_LoadAndroidUser
	@wKindID INT,								-- 游戏 I D
	@wServerID INT								-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN

	-- 加载信息
	SELECT UserID FROM AndroidUserInfo WHERE KindID=@wKindID AND ServerID=@wServerID AND Nullity=0 ORDER BY NEWID()

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------