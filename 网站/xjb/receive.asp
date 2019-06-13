<!--#include file="vnetchar.asp"-->
<%
Response.Buffer = True  '别删除
%>
<HTML>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />

</head>
<body>
<table border="0" align="center" cellpadding="0" cellspacing="0">
<tr>
<td width="243">
<%
'定义sp号码,sp18位密钥
spid="10785"  'sp号码 换成商户自己的SP号码5位
sppwd="510129198402020034" 'sp校验密钥18位 换成商户自己的 
'接受服务器url get参数
rtmd5=Request("v1")    'V币服务器MD5 
trka=Request("v2")     'V币号码15位
rtmi=Request("v3")     'V币密码6位 （可能为空 老V币没有密码）
rtmz=Request("v4")     '面值 1-999 整数面值
rtlx=Request("v5")     '卡的类型1，2，3 。  1:正式卡 2：测试卡 3 ：促销卡
rtoid=Request("v6")    '网盈一号通服务器端订单 10位  
rtcoid=Request("v7")   '商户自己订单 
rtuserid=Request("v8") '商户的用户ID 
'rtuserid=trim(server.URLEncode(rtuserid))
rtcustom=Request("v9") '商户自己定义数据 
rtflag=Request("v10")  '返回状态. 1为正常发送回来 2为补单发送回来

rtmzrtmz=clng(rtmz)*5000





'//'流程 第1次访问此页面 是盈华讯方服务器调用此页面1次 ，你先判断订单（订单包含金额，面值，'状态等消息）
'//'如果订单是没有使用状态并金额一致就MD5校验，校验通过 你发送“Response.AddHeader "Data-Received","ok_vpay8"  ”
'//'给盈华讯方服务器，服务器接收到这个头 后判断（ 你已经给用户加点了)，然后就把本页面转向给你
'//，此时转发的数据和第一次服务器直接调用的一致 此次你只需要根据订单，给用户提示冲值成功就可以了。
'//如果你没有发送头或网络中断，我就会给用户提示补单的消息（他可以自己点击，也可以我方服务器自动
'//补发，补发数据的MD5和第一发送的不一样，里面的“$_GET['v10']”'返回状态. 1为正常发送回来 2为补单发送回来）
'// 做好加点后一定要把订单 状态改变。！并注意安全处理。  
' //关于订单安全做法 如有不明请电话：0755-82126136 徐先生 

'//此页面不要使用SEESION(盈)  请用数据库管理你的订单



get_key=trka&rtmi&rtoid&spid&sppwd&rtcoid&rtflag&rtmz


'卡+密+网盈一号通服务器端订单+ 5位spid+ 18位SP密码+商户订单+rtflag返回类型1或2 +返回面值
'LCase函数是将字符转换为小写; Ucase函数是将字符转换为大写
'全国声讯支付联盟全国声讯电话支付接口对MD5值只认大写字符串，所以小写的MD5值得转换为大写
md5password=Ucase(trim(md5(get_key)))  '先MD5 32 然后转大写
'response.write("md5password:"+md5password)


'步骤1  :包含A和B
'A步骤,盈华讯方服务器第一次探测商户服务器状态.并给该订单充值 
'B步骤,检查该订单如果订单使用了 就把该订单的充值记录给用户打印.


if(rtflag="1") then
if (md5password=rtmd5) then
'第1步	//////////////////////////////////////

    SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=dcgame_123;DATABASE=QPGameUserDB;"
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString
   
    set rs=conn.execute("select * from GameCardNoInfo where CardNo='"&rtcoid&"'")
    if not rs.eof then
    'response.Write rs("Nullity")
if (rs("Nullity")=true) then 'A步骤    
'此处根据已用的订单显示用户充值信息.
'这里显示用户充值的信息。如充值多少元，加上什么服务等。
Response.write("充值成功！"+"<br>")
Response.write("网盈一号通订单号码为："+rtoid+"<br>")
Response.write("网站订单号码为："+rtcoid+"<br>")
Response.write("您本次充值："+rtmz+"元V币"+"<br>")
Response.AddHeader "Data-Received","ok_vpay8"   '此句不能删除也不能修改，作为探测成功之用
end if 

'第2步 //////////////////////////////////////
'校验商户订单号码状态 
if (rs("Nullity")=false) then    'b步骤      
'给用户加上相应的服务。
'加服务之处begin 请操作自己的数据库
    
	set rs=conn.execute("update GameCardNoInfo set Nullity='"& true &"',UseDate='"& date() &"' where CardNo='"& rtcoid &"'")
	
    SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=dcgame_123;DATABASE=QPTreasureDB;"
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString 
	set rss=conn.execute("update GameScoreInfo set score = score + "&rtmzrtmz&" where userid="& rtuserid &"")	
    
Response.write("~~数据库加服务 充值成功！"+"<br>")
Response.write("~~数据库加服务 网盈一号通订单号码为："+rtoid+"<br>")
Response.write("~~数据库加服务 网站订单号码为："+rtcoid+"<br>")
Response.write("~~数据库加服务 您本次充值："+rtmz+"元V币"+"<br>")
'加服务结束end   并修改订单状态为使用了。

'修改订单为使用状态.
Response.AddHeader "Data-Received","ok_vpay8"   '此句不能删除也不能修改，作为探测成功之用
end if 
	


end if 
end if 
end if

'第3步 //////////////////////////////////////

'步骤2  :包含A补单
'A补单：如果由于网络服务器中断或加点失败,会在5分钟内开始给用户补发数据.持续200次 24个小时
if(rtflag="2") then
if (md5password=rtmd5) then


    SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=dcgame_123;DATABASE=QPGameUserDB;"
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString
	'sql="select * from GameCardNoInfo where CardNo='"&rtcoid&"'"
	'response.Write sql
   
    set rs=conn.execute("select * from GameCardNoInfo where CardNo='"&rtcoid&"'")
    if not rs.eof then
'response.Write rs("Nullity")
if (rs("Nullity")=true) then    '为了安全要继续校验订单号码是否已经使用  
Response.write("充值成功！"+"<br>")
Response.write("一号通订单号码为："+rtoid+"<br>")
Response.write("网站订单号码为："+rtcoid+"<br>")
Response.write("您本次充值："+rtmz+"元V币"+"<br>")
Response.AddHeader "Data-Received","ok_vpay8"	' 发送此句。我就不会给该订单补发数据了。
end  if


if (rs("Nullity")=false) then    '为了安全要继续校验订单号码是否已经使用
'给用户加上相应的服务。
'加服务之处begin 请操作自己的数据库


    set rs=conn.execute("update GameCardNoInfo set Nullity='"& true &"',UseDate='"& date() &"' where CardNo='"& rtcoid &"'") 
	
    SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=dcgame_123;DATABASE=QPTreasureDB;"
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString 
	set rss=conn.execute("update GameScoreInfo set score = score + "&rtmzrtmz&" where userid="& rtuserid &"")

Response.write("补单继续给用户加点  充值成功！"+"<br>")
Response.write("补单一号通订单号码为："+rtoid+"<br>")
Response.write("补单网站订单号码为："+rtcoid+"<br>")
Response.write("补单您本次充值："+rtmz+"元V币"+"<br>")
'加服务结束end   并修改订单状态为使用了。
Response.AddHeader "Data-Received","ok_vpay8"	'此句不能删除也不能修改，作为探测成功之用。
end if


end if 
end if 

end if
			
%>
『<a href="vpay8.asp" target="_self">继续充值！</a>』&nbsp;&nbsp; 『<a href="../index.asp" target="_self">返回首页</a>』</td>
</tr>
</table>

</body>
</HTML>