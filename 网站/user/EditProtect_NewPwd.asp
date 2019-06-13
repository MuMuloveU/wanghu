<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
%>
<%CxGame.DbConn("QPGameUserDB")%>
<%
  dim C_PROTECTANSW,C_PROTECTQUES
  C_PROTECTANSW=replace(trim(request.form("C_PROTECTANSW")),"'","")
  C_PROTECTQUES=replace(trim(request.form("C_PROTECTQUES")),"'","")
  
  set rs=conn.execute("select * from AccountsInfo where C_PROTECTANSW='"& C_PROTECTANSW &"' and C_PROTECTQUES='"& C_PROTECTQUES &"' and UserID="&Session("UserID")&"")
  if rs.eof then
	  response.Write("<script language='javascript'>alert('提示问题答案不正确！');history.go(-1);</script>")
	  response.End()
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
        <td class="14_px">&nbsp;&nbsp;修改保密资料</td>
      </tr>
    </table>
      <table width="580" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="585" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;&nbsp;&nbsp;<STRONG class="12_pxcu">&nbsp;输入保密码答案</STRONG></td>
        </tr>
      </table>
      <hr width="580" size="1">
	  
	  
	  
      <table width="530" border="0" cellspacing="0" cellpadding="0">
	  <form action="answer.asp?action=up" method="post" name="myform" onSubmit="return place()">
        <tr>
          <td width="91" height="40" align="right" class="hui_12">问题：</td>
          <td width="439" height="30" class="hui_12"><span class="red_12px">
            <input name="C_PROTECTQUES" type="text" class="box" id="C_PROTECTQUES">
          </span></td>
        </tr>
        <tr>
          <td height="40" align="right" class="hui_12">答案：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="C_PROTECTANSW" type="text" class="box" id="C_PROTECTANSW">
          </span></td>
        </tr>
        <tr>
          <td height="40" align="right" class="hui_12">确认答案：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="C_PROTECTANSW1" type="text" class="box" id="C_PROTECTANSW1">
          </span></td>
        </tr>
        <tr>
          <td height="40" colspan="2" class="red_12px">&nbsp;&nbsp;&nbsp;&nbsp; 请牢记您输入的保密码问题和答案。 </td>
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
	function place(){
	if(myform.C_PROTECTQUES.value==""){
	alert("请输入密码提示问题!");
	myform.C_PROTECTQUES.focus();
	return (false);
	}
	if(myform.C_PROTECTANSW.value==""){
	alert("请输入密码提示答案！");
	myform.C_PROTECTANSW.focus();
	return (false);
	}
	if(myform.C_PROTECTANSW1.value==""){
	alert("请确认密码提示答案！");
	myform.C_PROTECTANSW1.focus();
	return (false);
	}
	if(myform.C_PROTECTANSW.value!=myform.C_PROTECTANSW1.value){
	alert("两次输入的答案不一致！");
	myform.C_PROTECTANSW1.focus();
	return (false);
	}
	}
</script>