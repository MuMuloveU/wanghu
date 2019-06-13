
----------------------------------------------------------------------------------------------------

USE QPServerInfoDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameKindItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 游戏类型
CREATE  PROCEDURE dbo.GSP_LoadGameKindItem WITH ENCRYPTION AS

-- 设置属性
SET NOCOUNT ON

-- 游戏类型
SELECT * FROM GameKindItem(NOLOCK) WHERE (GameKindItem.Nullity = 0) ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------
