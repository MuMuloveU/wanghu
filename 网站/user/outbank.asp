<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
%>
<%CxGame.OutBank%>
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
      <table width="170" border="0" cellpadding="0" cellspacing="1">
      <tr>
        <td></td>
      </tr>
    </table><!--#include file="left.asp"--></td>
    <td align="center" valign="top"><table width="585" height="24" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td class="14_px">&nbsp;&nbsp;金币保险箱服务</td>
      </tr>
    </table>
      <table width="580" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="585" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;&nbsp;&nbsp; <span class="12_pxcu">金币保险箱存款</span></td>
        </tr>
      </table><hr width="580" size="1">
      <br>
	  
	  
	  
      <table width="530" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#6FCE12">
	  <form action="" method="post" name="myform" onSubmit="return check();">
        <tr>
          <td width="137" height="40" align="right" bgcolor="#E3F8E0" class="12_pxcu">当前兑换用户：</td>
          <td width="363" height="40" bgcolor="#E3F8E0">&nbsp;<span class="hui_12">ID:</span><span class="red_12px"><%=Session("UserID")%></span>&nbsp;&nbsp;<span class="hui_12">用户名：</span><span class="red_12px"><%=Session("UserName")%></span></td>
        </tr>
        <tr>
          <td height="40" align="right" bgcolor="#E3F8E0" class="12_pxcu">当前金币：</td>
          <td height="40" bgcolor="#E3F8E0" class="hui_12">&nbsp;金币:<%=Session("money")%> 保险箱存款:<%=Session("Deposits")%> <a href="javascript:window.location.reload();">刷新</a></td>
        </tr>
        <tr>
          <td height="40" align="right" bgcolor="#E3F8E0" class="12_pxcu">取出金币：</td>
          <td height="40" bgcolor="#E3F8E0">&nbsp;<input name="money2" type="text" class="box" id="money2" onKeyPress	= "return regInput(this,	/^[0-9]*$/,		String.fromCharCode(event.keyCode))" value="0" size="10"
		onpaste		= "return regInput(this,	/^[0-9]*$/,		window.clipboardData.getData('Text'))"
		ondrop		= "return regInput(this,	/^[0-9]*$/,		event.dataTransfer.getData('Text'))">
            <span class="hui_12">金币</span></td>
        </tr>
        <tr>
          <td height="40" align="right" bgcolor="#E3F8E0" class="12_pxcu">游戏银行密码：</td>
          <td height="40" bgcolor="#E3F8E0">&nbsp;<input name="password" type="password" class="box" id="password">
            <input name="login" type="hidden" id="login4" value="true"></td>
        </tr>
        <tr>
          <td height="50" colspan="2" bgcolor="#E3F8E0" class="hang">
            
            &nbsp;&nbsp; 取出保险箱中的金币需要游戏银行密码才能取出,请先确认您是否记得游戏银行密码。<br>
            &nbsp;&nbsp; 取出的金币将不能得到保护，请注意保护好账号安全，以免给您造成损失。          </td>
        </tr>
        <tr>
          <td height="40" colspan="2" align="center" bgcolor="#E3F8E0"><input type="submit" name="button" value="提   交"></td>
        </tr>
		</form>
      </table>
	  
	  
    </td>
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
	function check(){
	if((myform.money2.value=="")||(myform.money2.value==0)){
	alert("请输入你要取出的数目!");
	myform.money2.focus();
	return (false);
	}
	if(myform.password.value==""){
	alert("请输入游戏银行密码！");
	myform.password.focus();
	return (false);
	}
	}
</script>