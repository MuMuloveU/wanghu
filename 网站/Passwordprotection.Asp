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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="188" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="188" height="37" align="right" valign="bottom" class="k_16">多彩密码保护</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><form name="form1" method="post" action="">
          <table width="670" border="0" align="center" cellpadding="5" cellspacing="0" class="boxlogin">
            <tr>
              <td height="35" colspan="2" align="left" background="img/index_title_bg.gif" id="err"><font color="#000000"><strong>申请密码保护</strong></font> </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td width="100" height="35" align="left"><div align="center">登入密码:</div></td>
              <td align="left"><input name="PassWord" type="password" class="input" id="PassWord">
                  <span class="box2">输入您的登入密码! </span></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">银行密码:</div></td>
              <td align="left"><div align="left">
                  <input name="BankPassWord" type="password" class="input" id="BankPassWord">
                  <span class="box2">输入您的银行密码! </span></div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td align="left" valign="top"><div align="center">请设定安全码:</div></td>
              <td align="left" valign="top"><div align="left">
                  <input name="code" type="password" class="input" id="PassWord3" maxlength="40">
                </div>
                  <div align="left" class="box2" id="codeid"> 安全码一旦设定,将不可更改,安全码的作用是找回用户密码的终极依据!建议用字母+数字混合组成!6位以上<br>
                      <br>
                  注意:不能与登入密码和银行密码相同! </div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">确定安全码:</div></td>
              <td align="left"><input name="code2" type="password" class="input" id="code2" maxlength="40"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">验证码:</div></td>
              <td align="left"><input name="getcode" type="text" class="input" id="GetCode" style="ime-mode:disabled" onkeydown="if(event.keyCode==13)event.keyCode=9">
                  <%CxGame.Vcode()%>
                  <input name="loginsev" type="hidden" id="loginsev" value="true">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center"> </div></td>
              <td align="left"><input type="submit" name="Submit" value="确定申请密码保护"></td>
            </tr>
          </table>
          <br>
          <br>
          <br>
          <br>
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
    <td><img src="bookpic/DC173_02.jpg" width="1002" height="237"></td>
  </tr>
</table>
</body>
</html>