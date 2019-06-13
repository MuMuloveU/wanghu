<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
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
    </table>
      <table width="204" height="4" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td bgcolor="#FFFFFF"></td>
      </tr>
    </table><!--#include file="left.asp"--></td>
    <td align="center" valign="top"><table width="585" height="24" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td height="30" class="14_px">&nbsp;&nbsp;转帐服务提示:只显示最近的50条转帐记录!</td>
      </tr>
    </table>
      <hr width="580" size="1">
      <table width="583" border="0" cellpadding="2" cellspacing="1" bgcolor="#CCCCCC">
        <tr>
          <td width="18%" bgcolor="#FFFFFF" class="12_pxcu"><div align="center">数额</div></td>
          <td width="22%" bgcolor="#FFFFFF" class="12_pxcu"><div align="center">支付人</div></td>
          <td width="18%" bgcolor="#FFFFFF" class="12_pxcu"><div align="center">收款人</div></td>
          <td width="18%" bgcolor="#FFFFFF" class="12_pxcu"><div align="center">手续费</div></td>
          <td width="24%" bgcolor="#FFFFFF" class="12_pxcu"><div align="center">时间</div></td>
        </tr>
        <%
				CxGame.DbConn(RLWebDBPrefix&"TreasureDb")
				Set Rs=Conn.Execute("Select Top 50 UserName,GameName,Money,BMoney,DateTime From GameScoreTransfersLog Where UserID="&Session("UserID")&" Or GameID="&Session("UserID")&" Order By ID Desc")
				Do While Not Rs.Eof
				%>
        <tr>
          <td bgcolor="#FFFFFF" class="hui_12"><div align="center"><%=Rs(2)%></div></td>
          <td bgcolor="#FFFFFF" class="hui_12"><div align="center"><%=Rs(0)%></div></td>
          <td bgcolor="#FFFFFF" class="hui_12"><div align="center"><%=Rs(1)%></div></td>
          <td bgcolor="#FFFFFF" class="hui_12"><div align="center"><%=Rs(3)%></div></td>
          <td bgcolor="#FFFFFF" class="hui_12"><div align="center"><%=Rs(4)%></div></td>
        </tr>
        <%
				Rs.MoveNext
				Loop
				CxGame.RsClose:CxGame.DbClose
				%>
      </table></td>
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