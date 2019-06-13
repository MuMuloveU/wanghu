@echo off

TITLE 棋牌数据库【Ver6.6_Spreader】 建立脚本启动中... [期间请勿关闭]

md D:\数据库

Rem 建主数据库三个
set rootPath=数据库脚本\
osql -E -i "%rootPath%数据库删除.sql"
osql -E -i "%rootPath%1_1_用户库脚本.sql"
osql -E -i "%rootPath%1_2_平台库脚本.sql"
osql -E -i "%rootPath%1_3_财富库脚本.sql"
osql -E -i "%rootPath%1_4_积分库脚本.sql"
osql -E -i "%rootPath%2_1_用户表脚本.sql"
osql -E -i "%rootPath%2_2_平台表脚本.sql"
osql -E -i "%rootPath%2_3_财富表脚本.sql"
osql -E -i "%rootPath%2_4_积分表脚本.sql"


Rem 建连接服务和填充数据
set rootPath=数据脚本\
osql -E -i "%rootPath%连接信息.sql"
osql -E -i "%rootPath%游戏列表.sql"
osql -E -i "%rootPath%财富链接.sql"
osql -E -i "%rootPath%平台链接.sql"
osql -E -i "%rootPath%用户链接.sql"
osql -E -i "%rootPath%鲜花道具.sql"


Rem 存储过程
set rootPath=存储过程\用户库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%帐号登录.sql"
osql -E  -i "%rootPath%注册帐号.sql"
osql -E  -i "%rootPath%用户权限.sql"
osql -E  -i "%rootPath%禁用帐号.sql"
osql -E  -i "%rootPath%自定义头像.sql"

set rootPath=存储过程\平台库\
osql -E  -i "%rootPath%加载类型.sql"
osql -E  -i "%rootPath%加载种类.sql"
osql -E  -i "%rootPath%连接信息.sql"
osql -E  -i "%rootPath%加载节点.sql"

set rootPath=存储过程\财富库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%加载机器人.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%游戏写分.sql"
osql -E  -i "%rootPath%用户权限.sql"
osql -E  -i "%rootPath%购买道具.sql"
osql -E  -i "%rootPath%魅力兑换.sql"
osql -E  -i "%rootPath%鲜花赠送.sql"
osql -E  -i "%rootPath%银行记录.sql"

set rootPath=存储过程\积分库\
osql -E  -i "%rootPath%标识登录.sql"
osql -E  -i "%rootPath%加载机器人.sql"
osql -E  -i "%rootPath%离开房间.sql"
osql -E  -i "%rootPath%游戏写分.sql"
osql -E  -i "%rootPath%用户权限.sql"
osql -E  -i "%rootPath%购买道具.sql"
osql -E  -i "%rootPath%魅力兑换.sql"
osql -E  -i "%rootPath%鲜花赠送.sql"
osql -E  -i "%rootPath%银行记录.sql"

pause

CLS
@echo off
@echo 正在生成游戏ID ……
set rootPath=数据脚本\
osql -E  -i "%rootPath%标识生成.sql"

COLOR 0A
CLS
@echo off
cls
echo ------------------------------
echo.
echo	主要数据库建立完成，请根据自己平台的积分游戏执行 
echo.
echo.
echo	版权所有： 深圳市网狐科技有限公司
echo ------------------------------

pause


