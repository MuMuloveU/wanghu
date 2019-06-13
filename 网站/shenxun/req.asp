<!--#include file="../inc/config.asp"-->
<%
  dim post_key,md5password,cardid,userID,userIP,errorcallback,agentclient,callbackplug,varchar,code,codearr,J
%>
<!--#include file="vnetsettings.asp"-->
<!--#include file="vnetchar.asp"-->
<%
post_key=custom&receive_url&agentID&agentPWD '此顺序不要改，数据都不要为空。
'LCase函数是将字符转换为小写; Ucase函数是将字符转换为大写
'全国声讯支付联盟全国声讯电话支付接口对MD5值只认大写字符串，所以小写的MD5值得转换为大写
md5password=Ucase(trim(md6(post_key)))  '先MD5 32位 然后在转大写  

cardid=request.Form("cardid")  '15位卡号
userID=request.Form("userID")  '用户名
userIP=request.Form("userIP")  '用户IP地址


'response.Write post_key
'response.End()
'此处验证用户名是否存在  请加入SQL 语句 sql="select count(*) as dds from users where userid='"&userid&"'" 

if (trim(request.Form("vcode"))=Session("GetCode")) and cardid<>"" and userID<>""    then

	CxGame.DbConn("QPGameUserDB")
	set rs=conn.execute("select * from AccountsInfo where Accounts='"& userID &"'")
	
	if not rs.eof then
		CxGame.DbConn("CxGameTreasureDb")
		set rs=conn.execute("select * from yinghua where agentself='"& custom &"'")
		if rs.eof then
		   set rs=conn.execute("insert into yinghua (username,agentself) values ('"& userid &"','"& custom &"')")
		end if
	else
		response.Write("<script language='javascript'>alert('没有此用户！');location.href='../paybuy.asp';</script>")
		response.End()
	end if
%>
<HTML>
	<HEAD>
		<title>盈华讯方充值接口2001版本</title>
		
	<meta http-equiv="Content-Type" content="text/html; charset=gb2312"><style type="text/css">
<!--
body,td,th {
	color: red;
	font-size: 12px;
}
-->
</style></HEAD>

	 <!--为防止客户自己关掉JAVASTRIPT 脚本 请做做手动提交 。以下是自动提交。-->
	<body onLoad="document.forms[0].submit();">
	
	
	
正在充值中.....................请不要刷新和关闭浏览器。

		<form name=form1 action='<%=serverurl%>' method=post ><!--盈华讯方服务器页面-->
	<input type=hidden name=agentID  value='<%=agentID%>' >   <!--分配给代理商的代理商代码ID -->
<input name= userID type=hidden  value='<%=userID%>' ><!--充值帐号-->

<input name=cardid type=hidden  value='<%=cardid%>' > <!--充值15位数字卡号-->
  <!--充值卡号 -->
	<input type=hidden name= version  value='<%=agentversion%>'> <!--支付程序版本号：2001 -->
	<input type=hidden name= agentself  value='<%=custom%>' >   <!--用户自己定义18个字符以内 -->
	
		<input type=hidden name= agentcallback  value='<%=receive_url%>' > <!--用户自己返回页面URL 指客户浏览器URL 以"http://"开始 -->
		
			<input type=hidden name=errorcallback  value='<%=errorcallback%>' > <!--用户充值错误返回地址 以"http://"开始 -->
			
				<input type=hidden name= md5password value='<%=md5password%>' >   
				
	<input type=hidden name= userIP value='<%=userIP%>' >
	
		<input type=hidden name=callbackplug value='<%=callbackplug%>' > <!--后台地址和补单地址 -->
	
   <input name="vnetpay" type=hidden id="Submit" value="充值">
		</form>
	
	
	</body>
</HTML>
<%

else

response.Write("错误的填写或验证码错误.请返回后重填.")
response.write("<a href="&agentclient& ">" & "返回充值页面" & "</a>")

%>

<%end if %>