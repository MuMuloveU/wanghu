<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<!--#include file="../inc/conn.asp"-->
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
<td class="forumRowHighlight" align="center" background="../images/admin_bg_1.gif"><b><font color="red">多彩家族管理</font></b></td>
</tr>
<tr> 
<td class="forumRowHighlight" align="center" ><table width="950" border="0" cellpadding="0" cellspacing="1" bgcolor="#BACAEF">
  <tr>
    <td width="88" height="24" align="center" bgcolor="#FFFFFF">家族名</td>
    <td width="84" align="center" bgcolor="#FFFFFF">会员名</td>
    <td width="105" align="center" bgcolor="#FFFFFF">会员QQ群</td>
    <td width="91" align="center" bgcolor="#FFFFFF">会员QQ</td>
    <td width="142" align="center" bgcolor="#FFFFFF">家族论坛</td>
    <td width="114" align="center" bgcolor="#FFFFFF">联系电话</td>
    <td width="94" align="center" bgcolor="#FFFFFF">电子邮箱</td>
    <td width="117" align="center" bgcolor="#FFFFFF">联系地址</td>
    <td width="75" align="center" bgcolor="#FFFFFF">编辑</td>
  </tr>
  
  
  <%
    set rs=server.CreateObject("adodb.recordset")
    sql="select * from Tuandui order by T_type asc"
	rs.open sql,conn,1,1
	if not rs.eof then
	do while not rs.eof
  %>
  <tr>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs("T_name")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("U_name")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("T_qq")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("QQ")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("T_url")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("T_tel")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("E_mail")%></td>
    <td align="center" bgcolor="#FFFFFF"><%=rs("Address")%></td>
    <td align="center" bgcolor="#FFFFFF">
	<%
	  if rs("T_type")=1 then
	  response.Write "<a href=""T_del.asp?T_id="& rs("T_id") &""">删除</a>"
	  else
	  response.Write "<a href=""T_open.asp?T_id="& rs("T_id") &""">通过</a>"
	  response.Write "&nbsp;&nbsp;"
	  response.Write "<a href=""T_del.asp?T_id="& rs("T_id") &""">删除</a>"
	  end if
	%></td>
  </tr>
  <%
    rs.movenext
	loop
	end if
	rs.close
  %>
  
  
</table></td>
  </tr>
</table>
</BODY>
</HTML>v