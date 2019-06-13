
----------------------------------------------------------------------------------------------------

USE QPServerInfoDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadGameTypeItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadGameTypeItem]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------


--加载种类
CREATE  PROCEDURE dbo.GSP_LoadGameTypeItem WITH ENCRYPTION AS

--设置属性
SET NOCOUNT ON

--查询种类
SELECT * FROM GameTypeItem(NOLOCK) WHERE Nullity=0 ORDER BY SortID

RETURN 0

GO

----------------------------------------------------------------------------------------------------

