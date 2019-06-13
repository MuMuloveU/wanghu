
----------------------------------------------------------------------------------------------------

USE QPServerInfoDB
GO

----------------------------------------------------------------------------------------------------

-- 删除数据
DELETE GameTypeItem
DELETE GameKindItem
DELETE GameNodeItem
GO

----------------------------------------------------------------------------------------------------

-- 类型数据
INSERT GameTypeItem (TypeID, TypeName, SortID, Nullity) VALUES ( 1, '财富游戏',100, 0)
INSERT GameTypeItem (TypeID, TypeName, SortID, Nullity) VALUES ( 2, '视频游戏',200, 0)
INSERT GameTypeItem (TypeID, TypeName, SortID, Nullity) VALUES ( 3, '牌类游戏',300, 0)
INSERT GameTypeItem (TypeID, TypeName, SortID, Nullity) VALUES ( 4, '棋类游戏',400, 0)
INSERT GameTypeItem (TypeID, TypeName, SortID, Nullity) VALUES ( 5, '麻将游戏',500, 0)
INSERT GameTypeItem (TypeID, TypeName, SortID, Nullity) VALUES ( 6, '休闲游戏',600, 0)


----------------------------------------------------------------------------------------------------

-- 财富游戏
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 1, 1, '梭哈', 'ShowHand.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 3, 1, '炸金花', 'ZaJinHua.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 5, 1, '牛牛', 'Ox.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 2, 1, '港式五张', 'HKFiveCard.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 7, 1, '德州扑克', 'DZShowHand.EXE', 67078, 100, 0, 'QPTreasureDB')
-- 牌类游戏

INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 102, 3, '疯狂斗地主', 'LandCrazy.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 145, 3, '疯狂干瞪眼', 'Gandengyan.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 121, 3, '清墩', 'QingDun.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 502, 3, '斗地主', 'Land.exe', 67078, 100, 0, 'QPTreasureDB')
-- 棋类游戏
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 506, 4, '中国象棋', 'ChinaChess.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 203, 4, '五子棋', 'GoBang.exe', 67078, 100, 0, 'QPTreasureDB')

-- 麻将游戏
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 301, 5, '广东麻将', 'SparrowGD.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 528, 5, '大众麻将', 'SparrowDZVD.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 324, 5, '成都麻将', 'SparrowCD.exe', 67078, 100, 0, 'QPTreasureDB')

----视频类
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 500, 2, '视频升级', 'UpGradeVD.exe', 1, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 100, 2, '视频斗地主', 'LandVD.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 135, 2, '四川斗地主', 'GuanPaiVD.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 522, 2, '疯狂斗地主', 'LandCrazyVD.exe', 67078, 100, 0, 'QPTreasureDB')
INSERT GameKindItem (KindID, TypeID, KindName, ProcessName, MaxVersion, SortID, Nullity, DataBaseName) VALUES ( 537, 2, '视频同花', 'BurrowVD.exe', 67078, 100, 0, 'QPTreasureDB')

----------------------------------------------------------------------------------------------------

GO