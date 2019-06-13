<%
  dim username1
  username1=replace(trim(request.Form("username1")),"'","")
%>
<!--#include file="vnetsettings.asp"-->

<%
  userIP=request.ServerVariables("REMOTE_HOST")
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>多彩视频游戏|充值|全国网上银行充值</title>
<style type="text/css">
<!--
body {
	background-image: url(../images/body_bg.jpg);
	background-repeat: repeat-x;
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #7bdd14;
}
-->
</style>
<link href="images/css.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
a {
	font-size: 14px;
	color: #FFFFFF;
}
a:visited {
	color: #FFFFFF;
	text-decoration: none;
}
a:hover {
	color: b9b9b9;
	text-decoration: underline;
}
a:link {
	text-decoration: none;
}
a:active {
	text-decoration: none;
}
body,td,th {
	font-size: 12px;
}
-->
</style></head>

<body>
<br>
<br>
<br>
<br>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="252" height="182" align="center" valign="top"><table width="252" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td>&nbsp;</td>
      </tr>
    </table>
      <table width="150" border="0" cellpadding="0" cellspacing="1" bgcolor="#CCCCCC">
        <tr>
          <td height="65" align="center" bgcolor="#FFFFFF"><img src="tel.jpg" width="59" height="56"></td>
        </tr>
        <tr>
          <td height="24" align="center" bgcolor="#FFFFFF">固话、小灵通</td>
        </tr>
      </table>
      <table width="252" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="200" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td height="24">用户名：<%=username1%></td>
        </tr>
      </table></td>
    <td width="518" align="center"><table  border="0" align="center" >
      <form name=pay action='<%=agentreq%>' method=post>
        <!--加盟网站自己的请求地址-->
        <tr>
          <td>用户名:</td>
          <td><input name= "userID" type=text  id="userID" value="<%=username1%>"  size="15" maxlength="30" /></td>
        </tr>
        <tr>
          <td>卡号：</td>
          <td><input name= "cardid"   type=text id="cardid"  size="15" maxlength="15" />
              <input type=hidden name= "userIP" id="userIP"  value='<%=userIP%>' >          </td>
        </tr>
        <tr>
          <td>验证码:</td>
          <td><input name="vcode" type="text" id="vcode"  size="5" maxlength="4">
              <img src="getcode.asp" alt="验证码为4位数字,看不清楚?请点击刷新验证码" style="cursor : pointer;"  onClick="this.src='getcode.asp'"> </td>
        </tr>
        <tr align="center" >
          <td colspan="2" ><input name="vnetpay" type=submit  id="vnetpay" value="充值">          </td>
        </tr>
      </form>
    </table>
    <br>
    <br></td>
  </tr>
  <tr>
    <td height="182" colspan="2" align="center" valign="top">&nbsp;</td>
  </tr>
</table>

</body>
</html>
