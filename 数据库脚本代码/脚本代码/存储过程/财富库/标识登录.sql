
----------------------------------------------------------------------------------------------------
-- 增加道具统计，消费金币，用户魅力
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------

USE QPTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_EfficacyUserID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_EfficacyUserID]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- I D 登录
CREATE PROC GSP_GR_EfficacyUserID
	@dwUserID INT,								-- 用户 I D
	@strPassword NCHAR(32),						-- 用户密码
	@strClientIP NVARCHAR(15),					-- 连接地址
	@strMachineSerial NCHAR(32),				-- 机器标识
	@wKindID INT,								-- 游戏 I D
	@wServerID INT								-- 房间 I D
WITH ENCRYPTION AS

-- 属性设置
SET NOCOUNT ON

-- 基本信息
DECLARE @UserID INT
DECLARE @FaceID INT
DECLARE @Accounts NVARCHAR(31)
DECLARE @UnderWrite NVARCHAR(63)

-- 扩展信息
DECLARE @GameID INT
DECLARE @GroupID INT
DECLARE @UserRight INT
DECLARE @Gender TINYINT
DECLARE @Loveliness INT
DECLARE @MasterRight INT
DECLARE @MasterOrder INT
DECLARE @MemberOrder INT
DECLARE @MemberOverDate DATETIME
DECLARE @GroupName NVARCHAR(31)
DECLARE @CustomFaceVer TINYINT

-- 积分变量
DECLARE @GameGold BIGINT
DECLARE @InsureScore BIGINT
DECLARE @Score BIGINT
DECLARE @WinCount INT
DECLARE @LostCount INT
DECLARE @DrawCount INT
DECLARE @FleeCount INT
DECLARE @Experience INT

-- 道具信息
DECLARE @PropCount INT

-- 辅助变量
DECLARE @EnjoinLogon BIGINT
DECLARE @ErrorDescribe AS NVARCHAR(128)

-- 执行逻辑
BEGIN
	-- 效验地址
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT [ErrorDescribe]=N'抱歉地通知您，系统禁止了您所在的 IP 地址的游戏登录权限，请联系客户服务中心了解详细情况！'
		RETURN 4
	END
	
	-- 效验机器
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineSerial AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT [ErrorDescribe]=N'抱歉地通知您，系统禁止了您的机器的游戏登录权限，请联系客户服务中心了解详细情况！'
		RETURN 7
	END
 
	-- 查询用户
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE	@MachineSerial NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @UnderWrite=UnderWrite, @LogonPass=LogonPass, @FaceID=FaceID, 
		@Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @Experience=Experience, @UserRight=UserRight, @MasterRight=MasterRight,
		@MasterOrder=MasterOrder, @MemberOrder=MemberOrder,  @MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine, @MachineSerial=MachineSerial, 
		@Loveliness=Loveliness,@CustomFaceVer=CustomFaceVer
	FROM QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- 查询用户
	IF @UserID IS NULL
	BEGIN
		SELECT [ErrorDescribe]=N'您的帐号不存在或者密码输入有误，请查证后再次尝试登录！'
		RETURN 1
	END	

	-- 帐号禁止
	IF @Nullity<>0
	BEGIN
		SELECT [ErrorDescribe]=N'您的帐号暂时处于冻结状态，请联系客户服务中心了解详细情况！'
		RETURN 2
	END	

	-- 帐号关闭
	IF @StunDown<>0
	BEGIN
		SELECT [ErrorDescribe]=N'您的帐号使用了安全关闭功能，必须到重新开通后才能继续使用！'
		RETURN 2
	END	
	
	-- 固定机器
	IF @MoorMachine=1
	BEGIN
		IF @MachineSerial<>@strMachineSerial
		BEGIN
			SELECT [ErrorDescribe]=N'您的帐号使用固定机器登录功能，您现所使用的机器不是所指定的机器！'
			RETURN 1
		END
	END

	-- 密码判断
	IF @LogonPass<>@strPassword AND @strClientIP<>N'0.0.0.0' AND @strPassword<>N''
	BEGIN
		SELECT [ErrorDescribe]=N'您的帐号不存在或者密码输入有误，请查证后再次尝试！'
		RETURN 3
	END

	-- 固定机器
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SELECT [ErrorDescribe]=N'您的帐号成功使用了固定机器登录功能！'
		UPDATE AccountsInfo SET MoorMachine=@MoorMachine, MachineSerial=@strMachineSerial WHERE UserID=@UserID
	END

	-- 房间锁定
	IF EXISTS (SELECT UserID FROM GameScoreLocker WHERE UserID=@dwUserID)
	BEGIN
		SELECT [ErrorDescribe]=N'您已经在充值游戏房间了，不能同时在进入此游戏房间了！'
		RETURN 4
	END
	INSERT GameScoreLocker (UserID,KindID,ServerID) VALUES (@dwUserID,@wKindID,@wServerID)

	-- 游戏信息
	DECLARE @GameUserRight INT
	DECLARE @GameMasterRight INT
	DECLARE @GameMasterOrder INT
	SELECT @Score=Score,@InsureScore=InsureScore, @WinCount=WinCount, @LostCount=LostCount, @DrawCount=DrawCount,
		@DrawCount=DrawCount, @FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterRight=MasterRight, @GameMasterOrder=MasterOrder
	FROM GameScoreInfo WHERE UserID=@dwUserID

	-- 信息判断
	IF @Score IS NULL
	BEGIN
		-- 插入资料
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, RegisterIP)	VALUES (@dwUserID,@strClientIP,@strClientIP)

		-- 游戏信息
		SELECT @Score=Score, @InsureScore=InsureScore, @WinCount=WinCount, @LostCount=LostCount,@DrawCount=DrawCount, @DrawCount=DrawCount, 
			@FleeCount=FleeCount, @GameUserRight=UserRight, @GameMasterOrder=MasterOrder, @MasterRight=MasterRight
		FROM GameScoreInfo WHERE UserID=@dwUserID
	END

	-- 更新信息
	UPDATE GameScoreInfo SET AllLogonTimes=AllLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP WHERE UserID=@dwUserID

	-- 金币
	SET @GameGold=@Score

	IF @InsureScore IS NULL SET @InsureScore=0

	-- 社团信息
	SET @GroupID=0
	SET @GroupName=''

	-- 权限标志
	SET @UserRight=@UserRight|@GameUserRight
	SET @MasterRight=@MasterRight|@GameMasterRight

	-- 权限等级
	IF @MasterOrder<>0 OR @GameMasterOrder<>0
	BEGIN
		IF @GameMasterOrder>@MasterOrder SET @MasterOrder=@GameMasterOrder
	END
	ELSE SET @MasterRight=0

	-- 进入记录
	INSERT RecordUserEnter (UserID, Score, KindID, ServerID, ClientIP) VALUES (@UserID, @Score, @wKindID, @wServerID, @strClientIP)

	-- 道具统计
	SELECT @PropCount=COUNT(UserID) FROM GameUserShelf WHERE UserID=@UserID AND UsedFlag=0 AND (KindID=@wKindID OR KindID=0)
	
	-- 会员等级
	IF GETDATE()>=@MemberOverDate 
	BEGIN 
		SET @MemberOrder=0

		-- 删除过期会员身份
		DELETE FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@UserID
	END
	ELSE 
	BEGIN
		DECLARE @MemberCurDate DATETIME

		-- 当前会员时间
		SELECT @MemberCurDate=MIN(MemberOverDate) 
		FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@UserID

		-- 删除过期会员
		IF GETDATE()>=@MemberCurDate
		BEGIN
			-- 删除会员期限过期的所有会员身份
			DELETE FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

			-- 切换到下一级别会员身份
			SELECT @MemberOrder=MAX(MemberOrder) 
			FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@UserID
			IF @MemberOrder IS NOT NULL
			BEGIN
				UPDATE QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo 
				SET MemberOrder=@MemberOrder
				WHERE UserID=@UserID
			END
			ELSE SET @MemberOrder=0
		END
	END

	-- 更新信息
	UPDATE QPGameUserDBLink.QPGameUserDB.dbo.AccountsInfo SET MemberOrder=@MemberOrder WHERE UserID=@UserID

	-- 会员道具
	SELECT @PropCount=@PropCount+COUNT(UserID) FROM QPGameUserDBLink.QPGameUserDB.dbo.MemberInfo WHERE UserID=@UserID

	-- 登录统计
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET LogonCount=LogonCount+1 WHERE DateID=@DateID AND KindID=@wKindID AND ServerID=@wServerID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, KindID, ServerID, LogonCount) VALUES (@DateID, @wKindID, @wServerID, 1)

	-- 输出变量
	SELECT @UserID AS UserID, @GameID AS GameID, @GroupID AS GroupID, @Accounts AS Accounts, @UnderWrite AS UnderWrite, @FaceID AS FaceID, 
		@Gender AS Gender, @GroupName AS GroupName, @MasterOrder AS MemberOrder, @UserRight AS UserRight, @MasterRight AS MasterRight, 
		@MasterOrder AS MasterOrder, @MemberOrder AS MemberOrder, @WinCount AS WinCount, @LostCount AS LostCount, @Loveliness AS Loveliness,
		@PropCount AS PropCount, @GameGold AS GameGold, @InsureScore AS InsureScore, @Loveliness AS Loveliness,	@CustomFaceVer AS CustomFaceVer,
		@DrawCount AS DrawCount, @FleeCount AS FleeCount, @Score AS Score, @Experience AS Experience, @ErrorDescribe AS ErrorDescribe

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------