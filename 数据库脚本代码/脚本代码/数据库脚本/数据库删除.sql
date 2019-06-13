
USE master
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPGameUserDB')
DROP DATABASE [QPGameUserDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPTreasureDB')
DROP DATABASE [QPTreasureDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPServerInfoDB')
DROP DATABASE [QPServerInfoDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPGameScoreDB')
DROP DATABASE [QPGameScoreDB]
GO

IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'QPEducateDB')
DROP DATABASE [QPEducateDB]
GO
