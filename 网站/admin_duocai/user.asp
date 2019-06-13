<!--#include file="../Inc/Config.asp" -->
<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<%CxGame.DbConn("QPGameUserDB")%>

<%
  dim action
  action=replace(trim(request.QueryString("action")),"'","")
  if action="del" then
     dim id
	 id=replace(trim(request.QueryString("id")),"'","")
	 if id="" or isnumeric(id)=false then
	    response.Write("<script language='javascript'>alert('非法操作！');history.go(-1);</script>")
		response.End()
	 end if
	 set rs=conn.execute("delete from PassWordList where id="&id)
	 response.Redirect("user.asp")
  end if
  
  if action="up" then
     id=replace(trim(request.QueryString("id")),"'","")
	 if id="" or isnumeric(id)=false then
	    response.Write("<script language='javascript'>alert('非法操作！');history.go(-1);</script>")
		response.End()
	 end if
     set rs=conn.execute("update PassWordList set iscut=1 where id="&id)
	 response.Redirect("user.asp")
  end if
%>
<HTML><HEAD><TITLE>----</TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link href="images/style.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
.STYLE4 {
	color: red;
	font-weight: bold;
}
-->
</style></HEAD>
<BODY>
<table class="tableBorder" width="90%" border="0" align="center" cellpadding="3" cellspacing="1" bgcolor="#FFFFFF">
<tr> 
<td class="forumRowHighlight" align="center" background="../images/admin_bg_1.gif"><span class="STYLE4">申诉用户</span></td>
</tr>
<tr> 
<td class="forumRowHighlight" align="center" ><table width="939" border="0" cellpadding="0" cellspacing="1" bgcolor="#BACAEF">
  <tr>
    <td width="91" height="24" align="center" bgcolor="#FFFFFF">用户名：</td>
    <td width="127" height="24" align="center" bgcolor="#FFFFFF">电子邮箱</td>
    <td width="139" height="24" align="center" bgcolor="#FFFFFF">密码提示问题</td>
    <td width="125" height="24" align="center" bgcolor="#FFFFFF">提示问题答案</td>
    <td width="132" height="24" align="center" bgcolor="#FFFFFF">身份证</td>
    <td width="135" height="24" align="center" bgcolor="#FFFFFF">联系方式</td>
    <td width="78" height="24" align="center" bgcolor="#FFFFFF">描述</td>
    <td width="103" height="24" align="center" bgcolor="#FFFFFF">删除</td>
  </tr>
  
  <%
    dim sql
    set rs=server.CreateObject("adodb.recordset")
	sql="select userid,email,passw,passd,code,telmail,txt,id,iscut from PassWordList order by ssdate desc"
	rs.open sql,conn,1,1
	if not rs.eof then
	dim pages
	page=clng(request("page"))
	rs.pagesize=18
	if page < 1 then page = 1
	pages=rs.pagecount
	if page > pages then page=pages
	rs.absolutepage=page
	for i=1 to rs.pagesize
  %>
  <tr>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(0)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(1)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(2)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(3)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(4)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(5)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF"><%=rs(6)%></td>
    <td height="24" align="center" bgcolor="#FFFFFF">[<a href="?id=<%=rs(7)%>&action=del" onClick="return confirm('你确定要删除吗？');">删除</a>] [<%if clng(rs(8))=1 then%>已处理<%else%><a href="?id=<%=rs(7)%>&action=up">处理</a><%end if%>] </td>
  </tr>
  <%
    rs.movenext
	if rs.eof or rs.bof then exit for
	next
	end if
  %>
  
</table>
  <br>
  <table>
    <tr>
      <td><%
		  if page < 2 then
		  response.Write "&nbsp;首页&nbsp;上一页"
		  else
		  response.Write "&nbsp;<a class=hui_link href='?page=1'>首页</a>"
		  response.Write "&nbsp;<a class=hui_link href='?page="& (page - 1) &"'>上一页</a>"
		  end if
		  if pages - page < 1 then
		  response.Write "&nbsp;下一页&nbsp;尾页"
		  else
		  response.Write "&nbsp;<a class=hui_link href='?page="& (page + 1) &"'>下一页</a>"
		  response.Write "&nbsp;<a class=hui_link href='?page="& pages &"'>尾页</a>"
		  end if
		  response.write "&nbsp;页次：<strong><font color=red>"&Page&"</font>/"& pages &"</strong>页 "
          response.write "&nbsp;共<b><font color='#FF0000'>"& rs.recordcount &"</font></b>条记录 <b>"& rs.pagesize &"</b>条记录/页"
		%>
      </td>
    </tr>
  </table></td>
  </tr>
</table>
</BODY>
</HTML>  
t