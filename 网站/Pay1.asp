<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim ppageuser,userid
  ppageuser=replace(trim(request("username1")),"'","")
   
	  if ppageuser="" then
		  response.Write("<script language='javascript'>alert('请填写好你的用户名！');history.go(-1);</script>")
		  response.End()
	  end if
	  CxGame.DbConn("QPGameUserDB")
	  set rs=conn.execute("select userid from AccountsInfo where Accounts='"& ppageuser &"'")
	  if not rs.eof then
		  userid=rs(0)
	  else
		  response.Write("<script language='javascript'>alert('没有此用户名！');history.go(-1);</script>")
		  response.End()
	  end if
	  rs.close
  

%>
<%CxGame.Pay()%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
 
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #56cce6;
}
-->
</style>
<link href="images/css.css" rel="stylesheet" type="text/css">
</head>

<body>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><!--#include file="top.asp"--></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="6" valign="top"><img src="images/L_33.jpg" width="6" height="384"></td>
    <td width="207" align="center" valign="top" bgcolor="#56CCE6"><table width="207" border="0" cellpadding="0" cellspacing="0" background="images/left_bj.jpg">
        <tr>
          <td align="center"><!--#include file="left.asp"--></td>
        </tr>
      </table></td>
    <td width="701" valign="top"><table width="699" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="153" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="153" height="37" align="right" valign="bottom" class="k_16">多彩充值</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><form name="form1" method="post" action="">
          <br>
          <br>
          <table width="450" border="0" align="center" cellpadding="5" cellspacing="0" class="box">
            <tr>
              <td height="28" colspan="2" bgcolor="#A2FF72" id="err"><font color="#6666FF"><strong>请输入您的卡号和密码进行充值</strong></font></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td width="100" align="left"><div align="center">这里输入卡号:</div></td>
              <td align="left"><input name="CardCode" type="text" class="input2" id="CardCode">              </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td align="left"><div align="center">这里输入密码:</div></td>
              <td><div align="left">
                  <input name="CardPass" type="text" class="input2" id="CardPass">
              </div></td>
            </tr>
            <tr align="left" bgcolor="#FFFFFF">
              <td><div align="center">验证码:</div></td>
              <td><input name="getcode" type="text" class="input" id="getcode" style="ime-mode:disabled" onkeydown="if(event.keyCode==13)event.keyCode=9">
                  <%CxGame.Vcode()%>
                  <input name="login" type="hidden" id="login2" value="true">
                  <input name="userid" type="hidden" id="userid" value="<%=userid%>" /></td>
            </tr>
            <tr align="left" bgcolor="#FFFFFF">
              <td align="left"><div align="center">用户名: </div></td>
              <td height="30"><input name="username1" type="text" class="input2" id="username1" value="<%=ppageuser%>" /></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="30">&nbsp;</td>
              <td height="30" align="left"><input name="imageField" type="image" src="image/pay.jpg" width="150" height="34" border="0" /></td>
            </tr>
          </table>
        </form></td>
      </tr>
      <tr>
        <td><img src="images/l_43.jpg" width="699" height="23"></td>
      </tr>
    </table></td>
    <td width="88" align="center" valign="top"><!--#include file="Right.asp"--></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>
</body>
</html>