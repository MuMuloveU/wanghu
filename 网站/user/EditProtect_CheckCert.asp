<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
  CxGame.IsCheckLogin()
%>
<%CxGame.DbConn("QPGameUserDB")%>
<%
  dim username,verifycode
  username=replace(trim(request.form("username")),"'","")
  verifycode=replace(trim(request.form("verifycode")),"'","")
  
  if verifycode<>request.Cookies("GetCode") then
	  response.Write("<script language='javascript'>alert('验证码不正确！');history.go(-1);</script>")
	  response.End()
  end if
  set rs=conn.execute("select * from AccountsInfo where Accounts='"& username &"' and UserID="&Session("UserID")&"")
  'sql="select * from AccountsInfo where LogonPass='"& username &"' and UserID="&Session("UserID")&""
  'response.Write sql
  if rs.eof then
  	  response.Write("<script language='javascript'>alert('用户名不正确！');history.go(-1);</script>")
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
          <td>&nbsp;&nbsp;&nbsp;<STRONG class="12_pxcu">&nbsp;请输入证件号码</STRONG></td>
        </tr>
      </table>
      <hr width="580" size="1">
	  
	  
	  
      <table width="530" border="0" cellspacing="0" cellpadding="0">
	  <form action="EditProtect_CheckQuestion.asp" method="post" name="myform" onSubmit="return place()">
        <tr>
          <td width="91" height="40" align="right" class="hui_12">证件号码：</td>
          <td width="439" height="30" class="hui_12"><span class="red_12px">
            <input name="zhengjian" type="text" class="box" id="zhengjian">
          </span></td>
        </tr>
        <tr>
          <td height="40" colspan="2" align="left" class="red_12px">&nbsp;&nbsp;
            <ul>
              <LI>&nbsp;&nbsp; 请输入您的证件号码</LI>
            </ul>
            </td>
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
<script language="javascript">
	function place(){
	if(myform.zhengjian.value==""){
	alert("请输入你的证件号码!");
	myform.zhengjian.focus();
	return (false);
	}
	}
</script>