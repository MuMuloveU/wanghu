<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<!--#include file="../inc/conn.asp"-->
<%
  dim sql
  set rs=server.CreateObject("adodb.recordset")
  sql="select top 1 jinbi from jinbi"
  rs.open sql,conn,1,1
  if not rs.eof then
  dim jinbi
  jinbi=rs(0)
  end if
%>
<%
  dim action
  action=request.QueryString("action")
  if action="up" then
     dim jb
	 jb=replace(trim(request.Form("jinbi")),"","'")
	 if jb="" or isnumeric(jb)=false then
	 response.Write("<script language='javascript'>alert('请输入正确的金币值');history.go(-1);</script>")
	 response.End()
	 end if
	 sql="update jinbi set jinbi='"& jb &"'"
	 conn.execute(sql)
	 response.Write("<script language='javascript'>alert('更新成功');location.href='jinbi.asp';</script>")
	 response.End()	 
  end if
%>
<HTML><HEAD><TITLE></TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link href="images/style.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style></HEAD>
<BODY>
<table class="tableBorder" width="90%" border="0" align="center" cellpadding="3" cellspacing="1" bgcolor="#FFFFFF">
<tr> 
<td class="forumRowHighlight" align="center" background="../images/admin_bg_1.gif"><b><font color="red">报名金币调整</font></b></td>
</tr>
<tr> 
<td class="forumRowHighlight" align="center" >


<table width="600" border="0" cellspacing="0" cellpadding="0">
<form action="?action=up" method="post" name="theform">
  <tr>
    <td width="118" align="center">金币：</td>
    <td width="224" align="center"><input name="jinbi" type="text" class="text_t" value="<%=jinbi%>" maxlength="9"></td>
    <td width="258" align="left"><input name="button" type="submit" class="Button" value="确认修改"></td>
  </tr>
</form>
</table>


</td>
  </tr>
</table>
</BODY>
</HTML>