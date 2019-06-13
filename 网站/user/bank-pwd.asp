<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
%>
<%CxGame.DbConn("QPGameUserDB")%>
<%
  dim action
  action=replace(trim(request.QueryString("action")),"'","")
  if action="up" then
     dim password,password1
	 password=replace(trim(request.form("password")),"'","")
	 password1=replace(trim(request.form("password1")),"'","")
	 Set md5= New MD5obj
	 password=md5.calcMD5(password)
	 password1=md5.calcMD5(password1)
	 
	 set rs=conn.execute("select * from AccountsInfo where Accounts='"& session("UserName") &"' and InsurePass='"& password &"'")
	 if not rs.eof then	 
	 set rs=conn.execute("update AccountsInfo set InsurePass='"& password1 &"' where UserID="& Session("UserID") &"")
	 response.Write("<script language='javascript'>alert('银行密码更改成功！');location.href='default.asp';</script>")
	 else
	 response.Write("<script language='javascript'>alert('旧密码错误！');history.go(-1);</script>")
	 response.End()
	 end if
  end if
%>
<link href="images/css.css" rel="stylesheet" type="text/css">
<table width="770" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td><!--#include file="Top.asp" --></td>
  </tr>
</table>
<table width="889" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="204" height="312" align="center" valign="top"><table width="180" height="4" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td></td>
      </tr>
    </table><!--#include file="left.asp"--></td>
    <td align="center" valign="top"><table width="585" height="24" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td class="14_px">&nbsp;&nbsp;个人管理中心</td>
      </tr>
    </table>
      <table width="580" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="585" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;&nbsp;&nbsp;<STRONG class="12_pxcu">&nbsp;修改银行密码</STRONG></td>
        </tr>
      </table>
      <hr width="580" size="1">
	  
	  
	  
      <table width="530" border="0" cellspacing="0" cellpadding="0">
	  <form action="?action=up" method="post" name="myform" onSubmit="return place();">
        <tr>
          <td width="91" height="40" align="right" class="hui_12">旧密码：</td>
          <td width="439" height="30" class="hui_12"><span class="red_12px">
            <input name="password" type="password" class="box" id="username">
          *</span></td>
        </tr>
        <tr>
          <td height="40" align="right" class="hui_12">新密码：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="password1" type="password" class="box" id="password1">
          *</span></td>
        </tr>
        <tr>
          <td height="40" align="right" class="hui_12">确认新密码：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="password2" type="password" class="box" id="password2">
          *</span></td>
        </tr>
        <tr>
          <td height="40" class="hui_12">&nbsp;</td>
          <td height="30" class="hui_12"><input type="submit" name="button1" value="确认修改">
            &nbsp; <input type="reset" name="button12" value="重    值"></td>
        </tr>
		</form>
      </table>
	  
	  
	  
      <br></td>
  </tr>
</table>

<table width="913" height="85" border="0" align="center" cellpadding="0" cellspacing="0" class="bj">
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><!--#include file="../copy.asp"--></td>
  </tr>
</table>
<script>
	function regInput(obj, reg, inputStr)
	{
		var docSel	= document.selection.createRange()
		if (docSel.parentElement().tagName != "INPUT")	return false
		oSel = docSel.duplicate()
		oSel.text = ""
		var srcRange	= obj.createTextRange()
		oSel.setEndPoint("StartToStart", srcRange)
		var str = oSel.text + inputStr + srcRange.text.substr(oSel.text.length)
		return reg.test(str)
	}
	function place(){
	if(myform.password.value==""){
	alert("请输入你的旧密码!");
	myform.password.focus();
	return (false);
	}
	if(myform.password1.value==""){
	alert("请输入新密码！");
	myform.password1.focus();
	return (false);
	}
	if(myform.password2.value==""){
	alert("请确认新密码！");
	myform.password2.focus();
	return (false);
	}
	if(myform.password1.value!=myform.password2.value){
	alert("两次输入的密码不一致！");
	myform.password2.focus();
	return (false);
	}
	}
</script>