USE QPTreasureDB
GO

-- 鲜花
TRUNCATE TABLE [FlowerCate]
GO

INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 0,'掌声',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 1,'香吻',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 2,'鲜花',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 3,'啤酒',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 4,'香烟',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 5,'钻戒',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 6,'轿车',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 7,'别墅',100,0,1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 8,'臭蛋',100,0,-1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 9,'砖头',100,0,-1,90,0)
INSERT [FlowerCate] ([ID],[Name],[Price],[SendUserCharm],[RcvUserCharm],[Discount],[Nullity]) VALUES ( 10,'炸弹',100,0,-1,90,0)

GO

-- 道具

Truncate TABLE GameShopCate
GO

INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 0,'双倍积分卡',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 1,'四倍积分卡',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 2,'负分清零',1,0,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 3,'清逃跑率',1,0,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 4,'小喇叭',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 5,'防踢卡',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 6,'护身符',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 7,'红钻会员卡',2592000,5184000,0,0,0,0,0,0,0,0,1000,2000,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 8,'蓝钻会员卡',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 9,'黄钻会员卡',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)
INSERT [GameShopCate] ([CateID],[CateName],[PropCount1],[PropCount2],[PropCount3],[PropCount4],[PropCount5],[PropCount6],[PropCount7],[PropCount8],[PropCount9],[PropCount10],[Price1],[Price2],[Price3],[Price4],[Price5],[Price6],[Price7],[Price8],[Price9],[Price10],[Discount],[Nullity]) VALUES ( 10,'紫钻会员卡',60,120,0,0,0,0,0,0,0,0,100,200,0,0,0,0,0,0,0,0,90,0)

GO