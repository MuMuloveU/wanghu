

----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_PurchaseProp]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_PurchaseProp]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadUserProp]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadUserProp]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WriteUserProp]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WriteUserProp]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadFlowerCate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadFlowerCate]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadGameShopCate]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadGameShopCate]
GO

----------------------------------------------------------------------------------------------------
-- 购买道具
CREATE PROC GSP_GR_PurchaseProp
	@dwSendUserID INT,				--赠送者ID
	@dwRcvUserID INT,				--接受者ID		
	@dwKindID INT,					--类型标识
	@dwServerID INT,				--房间标识
	@dwCateID INT,					--道具类别
	@dwCurCount BIGINT,				--当前数目
	@dwOnceCount BIGINT,			--单次数目
	@dwPachurseCount BIGINT,		--购买数目
	@bwSpendScore BIGINT=0,			--购买价格
	@strClientIP	NVARCHAR(15) 	--购买地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 会员资料
DECLARE @CurMemberOrder INT
DECLARE @MaxMemberOrder INT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberCount INT
DECLARE @UpMemberOverDate DATETIME

-- 辅助属性
DECLARE @EffectRows INT

-- 执行逻辑
BEGIN
	INSERT RecordPurchaseProp(SendUserID,RcvUserID,KindID,ServerID,CateID,CurCount,OnceCount,PachurseCount,SpendScore,ClientIP)
    VALUES(@dwSendUserID,@dwRcvUserID,	@dwKindID,@dwServerID,@dwCateID,@dwCurCount,@dwOnceCount,@dwPachurseCount,@bwSpendScore,@strClientIP)

	-- 赠送会员
	IF @dwCateID>=7
	BEGIN
		---------------------------== 为用户绑定会员信息 begin ==-----------------------------
		SET @CurMemberOrder=@dwCateID-7+1

		--------------------------------------------------------------
		-- 期限推算
		SELECT @MemberCount=COUNT(UserID)
		FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@dwRcvUserID
		
		-- 首次购买
		IF @MemberCount IS NULL OR @MemberCount<=0
		BEGIN
			INSERT QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo (UserID,MemberOrder,MemberOverDate)
			VALUES (@dwRcvUserID,@CurMemberOrder,DATEADD(ss, @dwOnceCount*@dwPachurseCount, GETDATE()))
		END
		ELSE 
		BEGIN	
			-- 上级会员
			SELECT @UpMemberOverDate=MAX(MemberOverDate)
			FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@dwRcvUserID AND MemberOrder>=@CurMemberOrder		

			-- 无上级会员
			IF @UpMemberOverDate IS NULL SET @UpMemberOverDate=GETDATE()	 
			IF @MemberOverDate IS NULL SET @MemberOverDate=GETDATE()
			
			-- 上级会员过期时间小
			IF @UpMemberOverDate<@MemberOverDate SET @UpMemberOverDate=@MemberOverDate
			
			-- 更新同级别会员
			UPDATE QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo SET MemberOverDate=DATEADD(ss, @dwOnceCount*@dwPachurseCount, @UpMemberOverDate)
			WHERE UserID=@dwRcvUserID AND MemberOrder=@CurMemberOrder

			SELECT @EffectRows=@@ROWCOUNT
			IF @EffectRows=0
			BEGIN
				INSERT QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo (UserID,MemberOrder,MemberOverDate)
				VALUES (@dwRcvUserID,@CurMemberOrder,DATEADD(ss, @dwOnceCount*@dwPachurseCount, @UpMemberOverDate))
			END		

			-- 更新下级会员
			UPDATE QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo SET MemberOverDate=DATEADD(ss, @dwOnceCount*@dwPachurseCount, MemberOverDate)
			WHERE UserID=@dwRcvUserID AND MemberOrder<@CurMemberOrder
		END		
		--------------------------------------------------------------

		-- 绑定会员,(会员期限与切换时间)
		SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate)
		FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@dwRcvUserID	

		-- 附加会员信息
		UPDATE QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo
		SET MemberOrder=@MaxMemberOrder,MemberOverDate=@MemberOverDate
		WHERE UserID=@dwRcvUserID
		---------------------------== 为用户绑定会员信息 end ==-------------------------------
	END
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- 用户道具
CREATE PROC GSP_GR_LoadUserProp
	@dwUserID INT,				-- 玩家标识	
	@dwKindID INT				-- 类型标识
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 会员过期
DECLARE @MemberOrder INT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberOverSec INT
DECLARE @PropCount BIGINT

DECLARE @MemberCount INT
DECLARE @MaxMemberOrder INT
DECLARE @MaxMemberOverDate DATETIME
DECLARE @CurMemberOverDate DATETIME

-- 执行逻辑
BEGIN
	SELECT @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate
	FROM QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 会员等级
	IF GETDATE()<@MemberOverDate
	BEGIN		
		DECLARE @tbMember TABLE (UserID INT, CateID INT, KINDID INT, PropCount BIGINT)

		SELECT @MemberCount=COUNT(UserID),@MaxMemberOrder=MAX(MemberOrder) 
		FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@dwUserID	
		IF @MemberCount IS NULL SET @MemberCount=0
		SET @MaxMemberOverDate=GETDATE()

		WHILE (@MemberCount>0)
		BEGIN
			SELECT @CurMemberOverDate=MemberOverDate
			FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo
			WHERE UserID=@dwUserID AND MemberOrder=@MaxMemberOrder

			-- 会员标识从7 开始 @MemberOrder+6
			INSERT @tbMember 
			SELECT @dwUserID,@MaxMemberOrder+6,0,DATEDIFF(ss,@MaxMemberOverDate,@CurMemberOverDate)			

			SELECT @MaxMemberOrder=MAX(MemberOrder) 
			FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@dwUserID AND MemberOrder<@MaxMemberOrder
			SET @MaxMemberOverDate=@CurMemberOverDate

			SET @MemberCount=@MemberCount-1
		END

		DELETE FROM @tbMember WHERE PropCount<=0
	END
	
	-- 全部道具
	IF @dwKindID=0
		-- 会员等级
		IF GETDATE()<@MemberOverDate
		BEGIN
			SELECT UserID,CateID,KindID,PropCount FROM GameUserShelf WHERE UserID=@dwUserID AND UsedFlag=0
			UNION
			SELECT UserID,CateID,KindID,PropCount FROM @tbMember
		END
		ELSE
		BEGIN
			SELECT UserID,CateID,KindID,PropCount FROM GameUserShelf WHERE UserID=@dwUserID AND UsedFlag=0
		END
	ELSE
		-- 会员等级
		IF GETDATE()<@MemberOverDate
		BEGIN
			SELECT UserID,CateID,KindID,PropCount FROM GameUserShelf WHERE UserID=@dwUserID AND UsedFlag=0 AND (KindID=@dwKindID OR KindID=0)
			UNION
			SELECT UserID,CateID,KindID,PropCount FROM @tbMember
		END
		ELSE
		BEGIN
			SELECT UserID,CateID,KindID,PropCount FROM GameUserShelf WHERE UserID=@dwUserID AND UsedFlag=0 AND (KindID=@dwKindID OR KindID=0)
		END
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- 写入道具
CREATE PROC GSP_GR_WriteUserProp	
	@dwUserID INT,					-- 玩家标识	
	@dwKindID INT,					-- 类型标识
	@dwCateID INT,					-- 道具类别
	@dwPropCount BIGINT,			-- 使用数目
	@strClientIP NVARCHAR(15) 		-- 玩家地址
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 执行逻辑
BEGIN
	--
	IF @dwPropCount<=0
	BEGIN
		DELETE FROM GameUserShelf WHERE UserID=@dwUserID AND KindID=@dwKindID AND CateID=@dwCateID
	END
	ELSE
	BEGIN
		-- 道具数目可用
		UPDATE GameUserShelf SET PropCount=@dwPropCount,UsedFlag=0,ClientIP=@strClientIP WHERE UserID=@dwUserID AND KindID=@dwKindID AND CateID=@dwCateID

		IF @@ROWCOUNT=0
		BEGIN
			INSERT GameUserShelf(UserID,CateID,KindID,PropCount,ClientIP) 
			VALUES(@dwUserID,@dwCateID,@dwKindID,@dwPropCount,@strClientIP)
		END	
	END

	-- 写记录
	INSERT RecordUserProp (UserID,KindID,CateID,PropCount,ClientIP)
	VALUES (@dwUserID,@dwKindID,@dwCateID,@dwPropCount,@strClientIP)
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------
-- 加载鲜花
CREATE PROC GSP_GR_LoadFlowerCate
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @FlowerCount INT

-- 执行逻辑
BEGIN
	SELECT [ID],[Name],Price,SendUserCharm,RcvUserCharm,Discount FROM FlowerCate WHERE Nullity=0

	SELECT @FlowerCount=@@ROWCOUNT
END

RETURN @FlowerCount
GO

----------------------------------------------------------------------------------------------------
-- 加载道具
CREATE PROC GSP_GR_LoadGameShopCate	
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

DECLARE @PropCount INT

-- 执行逻辑
BEGIN
	SELECT [CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10]
      ,[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[CateNote]
  FROM [GameShopCate] WHERE  Nullity=0

	SELECT @PropCount=@@ROWCOUNT
END

RETURN @PropCount
GO