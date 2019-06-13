<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
%>
<%dim GetCode%>
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
        <td class="14_px">&nbsp;&nbsp;修改保密资料</td>
      </tr>
    </table>
      <hr width="580" size="1">
	  
	  
	  
      <table width="530" border="0" cellspacing="0" cellpadding="0">
	  <form action="EditProtect_CheckCert.asp" method="post" name="myform" onSubmit="return place()">
        <tr>
          <td width="91" height="40" align="right" class="hui_12">游戏用户名：</td>
          <td width="439" height="30" class="hui_12"><span class="red_12px">
            <input name="username" type="text" class="box" id="username">
          </span></td>
        </tr>
        <tr>
          <td height="40" align="right" class="hui_12">验证码：</td>
          <td height="30" class="hui_12"><INPUT name=verifycode value="<%If GetCode=9999 Then Response.Write "9999"%>" 
                  size=10 maxLength=5 onKeyPress	= "return regInput(this,	/^[0-9]*$/,		String.fromCharCode(event.keyCode))" 
		onpaste		= "return regInput(this,	/^[0-9]*$/,		window.clipboardData.getData('Text'))"
		ondrop		= "return regInput(this,	/^[0-9]*$/,		event.dataTransfer.getData('Text'))">
            请在附加码框输入 <img src="../admin_yingjia/GetCode.asp"> </td>
        </tr>
        <tr>
          <td height="40" colspan="2" align="left" class="red_12px">&nbsp;&nbsp; 请输入您的用户名。</td>
          </tr>
        <tr>
          <td height="40" class="hui_12">&nbsp;</td>
          <td height="30" class="hui_12"><input type="submit" name="button1" value="确   认"></td>
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
	if(myform.username.value==""){
	alert("请输入你的用户名!");
	myform.username.focus();
	return (false);
	}
	if(myform.verifycode.value==""){
	alert("请输入验证码！");
	myform.verifycode.focus();
	return (false);
	}
	}
</script> 