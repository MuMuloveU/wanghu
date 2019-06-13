<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
  CxGame.IsCheckLogin()
%>
<%CxGame.DbConn("QPGameUserDB")%>
<%
  dim zhengjian,C_PROTECTQUES
  zhengjian=replace(trim(request.Form("zhengjian")),"'","")
  set rs=conn.execute("select * from AccountsInfo where C_idno='"& zhengjian &"' and UserID="&Session("UserID")&"")
  if rs.eof then
	  response.Write("<script language='javascript'>alert('证件号不正确！');history.go(-1);</script>")
	  response.End()
  end if
  C_PROTECTQUES=rs("C_PROTECTQUES")
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
          <td>&nbsp;&nbsp;&nbsp;<STRONG class="12_pxcu">&nbsp;请输入您的保密答案</STRONG></td>
        </tr>
      </table>
      <hr width="580" size="1">
	  
	  
	  
      <table width="530" border="0" cellspacing="0" cellpadding="0">
	  <form action="EditProtect_NewPwd.asp" method="post" name="myform" onSubmit="return place()">
        <tr>
          <td width="100" height="40" align="right" class="hui_12">您的保密问题是：</td>
          <td width="430" height="30" class="hui_12"><span class="red_12px">
            <input name="C_PROTECTQUES" type="text" class="box" id="C_PROTECTQUES" value="<%=C_PROTECTQUES%>">
          </span></td>
        </tr>
        <tr>
          <td height="40" align="right" class="hui_12">保密答案：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="C_PROTECTANSW" type="text" class="box" id="C_PROTECTANSW">
          </span></td>
        </tr>
        <tr>
          <td height="40" colspan="2" align="left" class="red_12px">&nbsp;请输入您的保密码答案。</td>
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
	alert("请输入提示问题答案！");
	myform.C_PROTECTANSW.focus();
	return (false);
	}
	}
</script>