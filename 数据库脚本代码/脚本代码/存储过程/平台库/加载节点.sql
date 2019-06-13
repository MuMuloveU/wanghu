
----------------------------------------------------------------------------------------------------

USE QPServerInfoDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameNodeItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameNodeItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

--加载节点
CREATE  PROCEDURE dbo.GSP_LoadGameNodeItem WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--查询节点
SELECT * FROM GameNodeItem(NOLOCK) WHERE Nullity=0 ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------
