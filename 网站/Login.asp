<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
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
            <td width="153" height="37" align="right" valign="bottom" class="k_16">多彩登陆</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><table width="670" border="0" align="center" cellpadding="5" cellspacing="0">
          <tr>
            <td valign="top" bgcolor="#FFFFFF" ><form name="form1"  method="post" action="" >
                <table border="0" align="center"  cellpadding="5" cellspacing="0" class="boxlogin" style="margin:10px auto 0 auto;">
                  <tr>
                    <td height="35" colspan="2" align="left" background="img/index_title_bg.gif" id="err"><strong><font color="#000000">输入用户名或密码进行登入</font></strong>
                        <%CxGame.CheckLogin()%>                    </td>
                  </tr>
                  <tr bgcolor="#FFFFFF">
                    <td width="194"><div align="center" id="userid">用 户:</div></td>
                    <td width="675" align="left"><input name="UserName" type="text" class="user" id="UserName">
                    </td>
                  </tr>
                  <tr bgcolor="#FFFFFF">
                    <td width="194"><div align="center">密 码:</div></td>
                    <td align="left"><input name="PassWord" type="password" class="user" id="PassWord"></td>
                  </tr>
                  <tr bgcolor="#FFFFFF">
                    <td colspan="2" bgcolor="#D8E9FA"><div align="center" class="box4"> 这里是指登入密码,而非银行密码,请注意! </div></td>
                  </tr>
                  <tr bgcolor="#FFFFFF">
                    <td width="194"><div align="center">验证码:</div></td>
                    <td align="left"><input name="getcode" type="text" class="user" id="GetCode" style="ime-mode:disabled" onKeyDown="if(event.keyCode==13)event.keyCode=9">
                        <%CxGame.Vcode()%>
                        <input name="login" type="hidden" id="login" value="true">
                    </td>
                  </tr>
                  <tr bgcolor="#FFFFFF">
                    <td><div align="center"> </div></td>
                    <td align="left"><input type="submit" name="Submit" value="用户登入"></td>
                  </tr>
                </table>
              <p align="center"><a href="Reg.asp">点这里注册帐号</a></p>
            </form></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td><img src="images/l_43.jpg" width="699" height="23"></td>
      </tr>
    </table></td>
    <td width="88" align="center" valign="top"><table width="89" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td><img src="images/l_34.jpg" width="90" height="39"></td>
      </tr>
      <tr>
        <td><img src="images/l_37.jpg" width="90" height="66"></td>
      </tr>
      <tr>
        <td><img src="images/l_38.jpg" width="90" height="64"></td>
      </tr>
      <tr>
        <td><img src="images/l_39.jpg" width="90" height="65"></td>
      </tr>
      <tr>
        <td><img src="images/l_40.jpg" width="90" height="65"></td>
      </tr>
      <tr>
        <td><img src="images/l_41.jpg" width="90" height="65"></td>
      </tr>
      <tr>
        <td><img src="images/l_42.jpg" width="90" height="29"></td>
      </tr>
    </table></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>
</body>
</html>