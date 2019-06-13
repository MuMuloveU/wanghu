<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
IF Request.form("regok")<>"true" Then
	Response.Redirect("Reg.asp")
	Response.End
End IF
IF Request.Cookies("cxgame")("reg")="reok" Then
	Response.Write "<div align=""center""><font color=""#FF0000"" size=""+2"">对不起,本游戏中心一个IP一天只能注册一个用户!</font></div>"
	Response.End
End IF
dim introducer
introducer=replace(trim(request.Form("introducer")),"'","")
%>
<script language="javascript" src="passwordstrength.js"></script>
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
            <td width="153" height="37" align="right" valign="bottom" class="k_16">多彩注册</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><form name="form2" method="post" action="reg2.asp" onsubmit="return place()">
          <table width="650" border="0" align="center" cellpadding="4" cellspacing="0" class="box3">
            <tr>
              <td height="35" colspan="2" id="err" background="img/index_title_bg.gif"><font color="#000000"><strong>注册用户</strong></font></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td colspan="2" bgcolor="#E8F6FF"><div align="left" class="box3" id="usernameerr">登陆多彩游戏中心的用户名！汉字、字母、数字、下划线等都可!</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td width="104"><div align="center">用户名:</div></td>
              <td width="398"><input name="UserName" type="text" class="input" id="UserName" value="<%=CxGame.GetInfo(0,"form","UserName")%>" maxlength="8">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td colspan="2"><div align="left" class="box3" id="passworderr">密码最好由字母加数字混合组成,6位数以上,防止被盗!</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">密码强度:</div></td>
              <td><script language="javascript">
		var psa = new PasswordStrength();
		psa.setSize("220","20");
		psa.setMinLength(1);
	    </script></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">登入密码:</div></td>
              <td><input name="PassWord" type="password" class="input" id="PassWord" value="<%=CxGame.GetInfo(0,"form","PassWord")%>" maxlength="14" onKeyUp="psa.update(this.value);">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">重复密码:</div></td>
              <td><input name="PassWord2" type="password" class="input" id="PassWord2" value="<%=CxGame.GetInfo(0,"form","PassWord2")%>" maxlength="14"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td colspan="2" valign="top" bgcolor="#E8F6FF"><div align="left" class="box3" id="BankPassWorderr">游戏银行密码,是在您转帐,取银子时需要用到的,所以请不要将银行密码设成和用户名、登入密码相同，最好是您字母加数字混合</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td valign="top"><div align="center">密码强度:</div></td>
              <td height="30" valign="top"><script language="javascript">
		var ps = new PasswordStrength();
		ps.setSize("220","14");
		ps.setMinLength(1);
	    </script></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td valign="top"><div align="center">游戏银行密码:</div></td>
              <td height="30" valign="top"><div align="left">
                  <input name="BankPassWord" type="password" class="input" id="PassWord3" value="<%=CxGame.GetInfo(0,"form","BankPassWord")%>" maxlength="20" onKeyUp="ps.update(this.value);">
              </div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">确定银行密码:</div></td>
              <td height="30"><input name="BankPassWord2" type="password" class="input" id="BankPassWord2" value="<%=CxGame.GetInfo(0,"form","BankPassWord2")%>" maxlength="20"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td colspan="2" bgcolor="#E8F6FF"><div align="center" class="box3" id="getpass">以下为必填,建议您正确填写,并牢记,可能会对您以后找回密码有所帮助</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">您的性别：</div></td>
              <td height="30"><table width="100%" border="0" cellspacing="0" cellpadding="0">
                  <tr>
                    <td width="42%"><select name="sex" id="select">
                        <option value="1" selected>男</option>
                        <option value="0">女</option>
                      </select>
                      选择头像：
                      <select name="ff" id="ff" onChange="fff()">
                        <%
					Dim Y
					for Y=1 To 60 
					%>
                        <option value="<%=y%>"><%=y%></option>
                        <%
					Next
					%>
                      </select>
                    </td>
                    <td width="58%"><div id="f"></div></td>
                  </tr>
              </table></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">邮箱:</div></td>
              <td height="30"><input name="Nmail" type="text" class="input" id="Nmail" value="<%=CxGame.GetInfo(0,"form","Nmail")%>"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">身份证:</div></td>
              <td height="30"><input name="Ncode" type="text" class="input" id="Ncode" value="<%=CxGame.GetInfo(0,"form","Ncode")%>"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">地址:</div></td>
              <td height="30"><input name="Nadd" type="text" class="input" id="Nadd" value="<%=CxGame.GetInfo(0,"form","Nadd")%>"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">密码提示问题:</div></td>
              <td height="30"><input name="PassW" type="text" class="input" id="PassW" value="<%=CxGame.GetInfo(0,"form","PassW")%>"></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">密码提示答案:</div></td>
              <td height="30"><input name="PassD" type="text" class="input" id="Ncode32" value="<%=CxGame.GetInfo(0,"form","PassD")%>">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td colspan="2" bgcolor="#E8F6FF"><div align="left" class="box3" id="codeerr">验证码:输入您所看到的右边的数字!</div></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center">推广员:</div></td>
              <td height="30"><input name="tuiguang" type="text" class="input" id="PassD" value="<%=introducer%>" /></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td><div align="center"> 验证码:</div></td>
              <td height="30"><input name="getcode" type="text" class="input" id="GetCode" style="ime-mode:disabled" onkeydown="if(event.keyCode==13)event.keyCode=9" />
                  <%CxGame.Vcode2():CxGame.UserReg()%>
                  <input name="reg" type="hidden" id="reg" value="true" />
                  <input name="regok" type="hidden" id="regok" value="true" /></td>
            </tr>
            <tr bgcolor="#FFFFFF">
              <td height="24">&nbsp;</td>
              <td height="30"><input type="submit" name="Submit" value="注册用户" /></td>
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
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>
<script>
function fff(){
f.innerHTML="<img src=ff/1_"+form2.ff.value+".jpg border=0>";
}
function place(){
    if(form2.UserName.value==""){
	alert("请填写好用户名！");
	form2.UserName.focus();
	return (false);
	}
	if(form2.UserName.value==form2.tuiguang.value){
	alert("自己不能是自己的推广员！");
	form2.UserName.focus();
	return (false);
}
}
</script>
</body>
</html>
�