<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
%>
<%CxGame.DbConn("QPGameUserDB")%>
<%
  dim gender,C_idno,c_address,c_email,c_pic
  set rs=conn.execute("select top 1 gender,C_idno,c_address,c_email,c_pic from AccountsInfo where UserID="&Session("UserID")&" ")
  if not rs.eof then
     gender=int(rs(0))
	 C_idno=rs(1)
	 c_address=rs(2)
	 c_email=rs(3)
	 c_pic=rs(4)
  end if
  'response.Write gender
  rs.close
%>
<%
  dim action
  action=replace(trim(request.QueryString("action")),"'","")
  if action="up" then
     'dim  C_idno,C_email,C_address,sex
	 gender=replace(trim(request.form("sex")),"'","")
	 C_idno=replace(trim(request.form("C_idno")),"'","")
	 c_address=replace(trim(request.form("c_address")),"'","")
	 c_email=replace(trim(request.form("c_email")),"'","")
  set rs=conn.execute("update AccountsInfo set gender="& gender &",C_idno='"& C_idno &"',c_address='"& c_address &"',c_email='"& c_email &"' where UserID="&Session("UserID")&"")
  response.Write("<script language='javascript'>alert('更改成功');location.href='my.asp';</script>")
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
    </table><!--#include file="left.asp"--> </td>
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
          <td>&nbsp;&nbsp;&nbsp; <span class="red_12px">账号：<%=Session("UserName")%> </span></td>
        </tr>
      </table>
      <table width="530" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td>&nbsp;</td>
        </tr>
      </table>
      <table width="585" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td>&nbsp;&nbsp;&nbsp;<STRONG class="12_pxcu">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 修改个人信息</STRONG></td>
        </tr>
      </table>
      <hr width="580" size="1">
	  
	  
	  
      <table width="530" border="0" cellspacing="0" cellpadding="0">
	  <form action="?action=up" method="post" name="myform" onSubmit="return place();">
        <tr>
          <td height="30" align="right" class="hui_12">电子邮件：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="c_email" type="text" class="box" id="c_email" value="<%=c_email%>" <%if c_email<>"" then%>readonly<%end if%>>
          </span></td>
        </tr>
        <tr>
          <td height="30" align="right" class="hui_12">&nbsp;</td>
          <td height="30" class="hui_12">邮箱不能随意修改，如有特殊原因需要更改，请单击此处与客服联系。</td>
        </tr>
        <tr>
          <td height="30" align="right" class="hui_12">性&nbsp;&nbsp;&nbsp; 别：</td>
          <td height="30" class="hui_12"><input name="sex" type="radio" value="1" <%if gender=1 then%> checked <%end if%>>男&nbsp;<input <%if gender=0 then%> checked <%end if%> type="radio" name="sex" value="0">女</td>
        </tr>
        <tr>
          <td height="30" align="right" class="hui_12">所 在 地：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="c_address" type="text" class="box" id="c_address" value="<%=c_address%>">
          </span></td>
        </tr>
        <tr>
          <td height="30" align="right" class="hui_12">身 份 证：</td>
          <td height="30" class="hui_12"><span class="red_12px">
            <input name="C_idno" type="text" class="box" id="C_idno" value="<%=C_idno%>" maxlength="18">
          </span></td>
        </tr>
        <tr>
          <td height="30" class="hui_12">&nbsp;</td>
          <td height="30" class="hui_12"><input type="submit" name="button1" value="保存修改"></td>
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
