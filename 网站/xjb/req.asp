<!--#include file="vnetchar.asp"-->
<%
spid="10785"  'sp号码 换成商户自己的SP号码5位
sppwd="510129198402020034" 'sp校验密钥18位 换成商户自己的   
spreq="http://www.dc173.com/xjb/vpay8.asp" '换成商户请求地址
sprec="http://www.dc173.com/xjb/receive.asp" '换成商户接收地址
spversion="vpay1001"   '此接口的版本号码 此版本是"vpay1001"
money= request.Form("money")  '接参数面值元 1-999 整数面值 不要写小数点。
spname=Server.UrlEncode("多彩棋牌游戏")   '接参数网站中文名 用UrlEncode编码
userid=Server.UrlEncode(request.Form("userid"))        '接参数用户ID 用UrlEncode编码
userid_d=request.Form("userid")
spcustom= Server.UrlEncode("多彩棋牌游戏")     '客户自定义30字符内 用UrlEncode编码 
urlcode ="GB2312" '编码  gbk  gb2312   utf-8  unicode   big5(注意不能一个繁体和简体字交叉写) 
userip=request.ServerVariables("REMOTE_HOST")  '客户支付ip地址 
servadd="http://s2.vnetone.com/Default.aspx" '服务器提交地址

'插入一个时间随机号码
Randomize
Codeabc= mid(cStr(1e+6+Int(999999 * Rnd)),2)
dim dddd
dddd=cStr(now())
dddd=replace(dddd,"-","")
dddd=replace(dddd,":","")
dddd=replace(dddd," ","")
dddd=dddd&codeabc

orderId=codeabc   '客户订单 请妥善管理客户的订单  30位和30位以下 字母或数字 不能汉字订单 长度：30字符以内（只能是数字、字母或数字字母的组合。不能用汉字订单）
post_key=orderId&spreq&sprec&spid&sppwd&spversion&money 
'网站订单号码+ 请求地址+ 接收地址 + 5位spid+ 18位SP密码+支付的版本号+支付金额
'LCase函数是将字符转换为小写; Ucase函数是将字符转换为大写
'全国声讯支付联盟全国声讯电话支付接口对MD5值只认大写字符串，所以小写的MD5值得转换为大写
md5password=Ucase(trim(md5(post_key)))  '先MD532 然后转大写

'此处操作数据库 （把商户订单+用户名+面值等插入数据库）
'response.Write orderId


if userid_d="" then
response.Write("<script language='javascript'>alert('请输入你的用户名!');history.go(-1);</script>")
response.End()
end if	
	SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=dcgame_123;DATABASE=QPGameUserDB;"
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString

    set rs=conn.execute("select top 1 * from AccountsInfo where accounts='"& userid_d &"'")
	if rs.eof then
	response.Write("<script language='javascript'>alert('对不起没有此用户,请重新确认!');history.go(-1);</script>")
	response.End()
	else
	useriid=rs("userid")
	end if




'CardNo,CardPass,CardTypeID,BatchNo,Nullity,CreateDate,UseDate,UserID,Accounts
    set rs=conn.execute("select * from GameCardNoInfo where CardNo='"& orderId &"'")
	if rs.eof then
    set rs=conn.execute("insert into GameCardNoInfo (CardNo,CardPass,CardTypeID,BatchNo,Nullity,CreateDate,UserID,Accounts) values ('"& orderId &"','"& md5password &"',0,0,'"& false &"','"& date() &"','"& useriid &"','"& userid_d &"')")
    end if 
'sql="insert into GameCardNoInfo (CardNo,CardPass,CardTypeID,BatchNo,Nullity,CreateDate,UserID,Accounts) values ('"& orderId &"','"& md5password &"',0,0,'"& false &"','"& date() &"','"& useriid &"','"& userid_d &"')"
'response.Write sql
%>

<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<style type="text/css">
<!--
.STYLE1 {
	color: red;
	font-size: 12px;
}
-->
</style>
</head>	
		
		
	
	<body onLoad="document.forms[0].submit();">
	<span class="STYLE1">正在连接中.....................请不要刷新和关闭浏览器。	</span>
	<form name=form1 action='<%=servadd %>' method=post ><!--盈华讯方服务器页面POST提交-->
	 
	  <p>
	    <input name='spid'       type=hidden    value='<%=spid%>' >   
	    <!--spID 5位-->
         <input name='spname'    type=hidden  value='<%=spname%>' >   
	    <!--sp网站 客户网站名-->
         <input name='spoid'      type=hidden   value='<%=orderId%>' >
	    <!--sp订单   唯一订单码，长度不超过30字符-->
         <input name='spreq'     type=hidden   value='<%=spreq%>' >   
	    <!--网站请求地址-->
         <input name='sprec'     type=hidden  value='<%=sprec%>' > 
	    <!--网站接收地址 请直接以ASP/php/jsp结尾 不要带参数 我方会给此地址加上？参数=XXX 等格式   客户站接收GET URL数据 -->
         <input name='userid'    type=hidden  value='<%=useriid%>' > 
	    <!--客户ID-->
         <input name='userip'    type=hidden   value='<%=userip%>' > 
	    <!--客户浏览器IP-->
         <input name='spmd5'     type=hidden  value='<%=md5password%>' > 
	    <!--MD5-->
        <input name='spcustom'  type=hidden  value='<%=spcustom%>' >   
	    <!--用户自己定义30个字符以内 -->
         <input name='spversion'  type=hidden  value='<%=spversion%>' >  <!--支付版本号码 -->
   
         <input name='money'  type=hidden  value='<%=money%>' >        <!--用户根据网站自己定义的整数面值如1,2,3,4.....-->   
	 <input name='urlcode'  type=hidden  value='<%=urlcode%>' > 
   
    </form>	
		

</body>
			

</html>