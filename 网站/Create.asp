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
<style type="text/css">
<!--
.STYLE1 {color: red}
-->
</style>
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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="185" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="185" height="37" align="right" valign="bottom" class="k_16">多彩社团创建</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><table width="690" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td>&nbsp;</td>
              </tr>
            </table>
            <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td width="11"><img src="images/S_1.jpg" width="11" height="34"></td>
                <td width="552" align="left" background="images/S_2.jpg">&nbsp;&nbsp; <a href="TuanDui.asp" class="hei_link">社团首页</a> |&nbsp;<a href="Create.asp" class="hei_link">创建社团</a></td>
                <td width="87" align="left"><img src="images/S_3.jpg" width="12" height="34"></td>
              </tr>
            </table>
            <table width="690" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td>&nbsp;</td>
              </tr>
            </table>
            <br>
              <table width="640" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#1D84D4">
          <form action="ADD_Create.asp" method="post" name="theform" onsubmit="return check()">
            <tr>
              <td width="84" height="30" align="center" bgcolor="#FFFFFF">用户名：</td>
              <td colspan="3" align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="username" type="text" class="txt" /></td>
            </tr>
            <tr>
              <td height="30" align="center" bgcolor="#FFFFFF">游戏社团名：</td>
              <td colspan="3" align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="tname" type="text" class="txt" id="tname" maxlength="8" />
                  <span class="STYLE1">取名如：【皇冠】→小新；『凤舞』→白龙；全家福→龙太子等。</span></td>
            </tr>
            <tr>
              <td height="30" align="center" bgcolor="#FFFFFF">群QQ：</td>
              <td width="214" align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="tqq" type="text" class="txt" id="tqq" /></td>
              <td width="84" align="center" bgcolor="#FFFFFF">游戏社团论坛：</td>
              <td width="253" align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="luntan" type="text" class="txt" id="luntan" /></td>
            </tr>
            <tr>
              <td height="30" align="center" bgcolor="#FFFFFF">用户QQ：</td>
              <td align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="qq" type="text" class="txt" id="qq" /></td>
              <td align="center" bgcolor="#FFFFFF">E_Mail:</td>
              <td align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="email" type="text" class="txt" id="email" /></td>
            </tr>
            <tr>
              <td height="30" align="center" bgcolor="#FFFFFF">联系电话：</td>
              <td align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="tel" type="text" class="txt" id="tel" /></td>
              <td align="center" bgcolor="#FFFFFF">联系地址：</td>
              <td align="left" bgcolor="#FFFFFF">&nbsp;
                  <input name="diz" type="text" class="txt" id="diz" /></td>
            </tr>
            <tr>
              <td height="78" align="center" bgcolor="#FFFFFF">团队宗旨：</td>
              <td height="78" colspan="3" align="left" bgcolor="#FFFFFF"> &nbsp;
                <textarea cols="70" rows="5" name="T_zongzhi"></textarea></td>
              </tr>
            <tr>
              <td height="76" align="center" bgcolor="#FFFFFF">团队介绍：</td>
              <td height="76" colspan="3" align="left" bgcolor="#FFFFFF"> &nbsp;
                <textarea cols="70" rows="5" name="T_content"></textarea></td>
              </tr>
            <tr>
              <td height="30" colspan="4" align="center" bgcolor="#FFFFFF"><input type="submit" name="button1" value="确认创建" />
&nbsp;&nbsp;&nbsp;&nbsp;
<input type="reset" name="reset" value="重新填写" /></td>
            </tr>
          </form>
        </table></td>
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
<script language="javascript">
function check(){
  if(theform.username.value==""){
  alert("请填写好用户名");
  theform.username.focus();
  return (false);
  }
  if(theform.tname.value==""){
  alert("请填写好家族名");
  theform.tname.focus();
  return (false);
  }
  if(theform.tqq.value==""){
  alert("请填写好家族QQ群");
  theform.tqq.focus();
  return (false);
  }
  if(theform.luntan.value==""){
  alert("请填写好家族论坛");
  theform.luntan.focus();
  return (false);
  }
  if(theform.qq.value==""){
  alert("请填写好您的QQ");
  theform.qq.focus();
  return (false);
  }
  if(theform.email.value==""){
  alert("请填写好您的电子邮箱");
  theform.email.focus();
  return (false);
  }
  if(theform.tel.value==""){
  alert("请填写好您的电话");
  theform.tel.focus();
  return (false);
  }
  if(theform.diz.value==""){
  alert("请填写好您的联系地址");
  theform.diz.focus();
  return (false);
  }
  if(theform.T_zongzhi.value==""){
  alert("请填写好团队宗旨");
  theform.T_zongzhi.focus();
  return (false);
  }
  if(theform.T_content.value==""){
  alert("请填写好团队介绍");
  theform.T_content.focus();
  return (false);
  }
}
</script>
</body>
</html>
 