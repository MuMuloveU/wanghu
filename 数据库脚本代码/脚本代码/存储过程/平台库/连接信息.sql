
----------------------------------------------------------------------------------------------------

USE QPServerInfoDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_LoadDataBaseInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_LoadDataBaseInfo]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- 连接信息
CREATE PROCEDURE GSP_LoadDataBaseInfo
	@strDBAddr NVARCHAR(15)						-- 数据库地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 变量定义
DECLARE @DBPort INT
DECLARE @DBAddr NVARCHAR(15)
DECLARE @DBUser NVARCHAR(512)
DECLARE @DBPassword NVARCHAR(512)
DECLARE @ErrorDescribe NVARCHAR(128)

-- 执行逻辑
BEGIN

	-- 查询信息
	SELECT @DBAddr=DBAddr, @DBPort=DBPort, @DBUser=DBUser, @DBPassword=DBPassword 
		FROM DataBaseInfo(NOLOCK) WHERE DBAddr=@strDBAddr
	
	-- 存在判断
	IF @DBAddr IS NULL
	BEGIN
		SET @ErrorDescribe=N'数据库 [ '+@strDBAddr+N' ] 连接信息不存在，请检查 QPServerInfoDB 数据库的 SQLDBConnectInfo 表数据'
		SELECT [ErrorDescribe]=@ErrorDescribe
		RETURN 1
	END
	
	-- 输出变量
	SELECT @DBAddr AS DBAddr, @DBPort AS DBPort, @DBUser AS DBUser, @DBPassword AS DBPassword
	
END

RETURN 0

----------------------------------------------------------------------------------------------------

GO
