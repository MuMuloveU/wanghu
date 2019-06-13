
----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_SetUserRight]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_SetUserRight]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

CREATE PROCEDURE GSP_GR_SetUserRight
  	@dwUserID INT,
	@dwUserRight INT,
	@dwMasterUserID INT,
	@strClientIP VARCHAR(15)
WITH ENCRYPTION AS

SET NOCOUNT ON

BEGIN

	-- …Ë÷√»®œﬁ
	UPDATE GameScoreInfo SET UserRight=@dwUserRight WHERE UserID=@dwUserID

END

RETURN 0
----------------------------------------------------------------------------------------------------

GO

SET QUOTED_IDENTIFIER OFF 
GO

SET ANSI_NULLS ON 
GO
