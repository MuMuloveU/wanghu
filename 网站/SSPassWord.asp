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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="193" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="193" height="37" align="right" valign="bottom" class="k_16">多彩密码找回</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><form name="form1" method="post" action="">
          <table width="670" border="0" align="center" cellpadding="4" cellspacing="0" class="boxlogin">
            <tr>
              <td height="35" colspan="2" align="left" background="img/index_title_bg.gif" id="err"><strong><a href="SSPassWord.asp"><font color="#FF0000">密码申诉</font></a></strong> | <a href="SSSelect.asp"><font color="#FF0000"><strong>申诉结果查询</strong></font></a></td>
            </tr>
            <tr>
              <td height="30" colspan="2" align="left" background="img/index_title_bg.gif" id="err"><strong>密码申诉(密码申诉并不能保证一定能找回您的密码,如果你申请了密码保护请点这里<a href="FindPassWord.asp">重设密码</a>)</strong></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="30" colspan="2" align="left" bgcolor="#F3F1EC"><div align="left" class="box3" id="usernameerr">您在游戏中心注册时所填写的资料</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td width="208" height="35" align="left">游戏中心用户名:</td>
              <td width="412" height="30" align="left"><input name="UserName" type="text" class="input" id="UserName2" maxlength="20">
                * 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">注册时用的邮箱:</div></td>
              <td height="30" align="left"><input name="Nmail" type="text" class="input" id="Nmail">
                * 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">注册时用的身份证:</div></td>
              <td height="30" align="left"><input name="Ncode" type="text" class="input" id="Ncode">
                * 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">注册时用的地址:</div></td>
              <td height="30" align="left"><input name="Nadd" type="text" class="input" id="Nadd">
                * 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">密码提示问题:</div></td>
              <td height="30" align="left"><input name="PassW" type="text" class="input" id="PassW">
                * 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">密码提示答案:</div></td>
              <td height="30" align="left"><input name="PassD" type="text" class="input" id="Ncode32">
                * 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">您的联系方式:</div></td>
              <td height="30" align="left"><input name="Tel" type="text" class="input" id="Tel">
                可以是邮箱,手机,电话* 必填</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td align="left"><div align="center">
                  <p>描述:<br>
                    (可填写你希望更改的密码,您的一些转帐记录,注册时间等资料) </p>
              </div></td>
              <td align="left"><textarea name="Txt" cols="40" rows="5" id="PassD"></textarea>
                限700字</td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" colspan="2" align="left" bgcolor="#F3F1EC"><div align="left" class="box3" id="codeerr">验证码:输入您所看到的右边的数字!</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center">验证码:</div></td>
              <td height="35" align="left"><input name="getcode" type="text" class="input" id="getcode" style="ime-mode:disabled" onkeydown="if(event.keyCode==13)event.keyCode=9">
                  <%CxGame.Vcode2()%>
                  <input name="sspassword" type="hidden" id="login2" value="true">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="35" align="left"><div align="center"> </div></td>
              <td height="35" align="left"><input type="submit" name="Submit" value="确定申诉"></td>
            </tr>
          </table>
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