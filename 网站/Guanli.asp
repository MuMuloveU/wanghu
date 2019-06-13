<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim T_id
  T_id=replace(trim(request.QueryString("id")),"","'")
  if T_id="" or isnumeric(T_id)=false then
  response.Write("<script language='javascript'>alert('操作错误');history.go(-1);</script>")
  response.End()
  end if
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
 
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #56cce6;
}
-->
</style>
<link href="images/css.css" rel="stylesheet" type="text/css">
</head>

<body>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><!--#include file="top.asp"--></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="6" valign="top"><img src="images/L_33.jpg" width="6" height="384"></td>
    <td width="207" align="center" valign="top" bgcolor="#56CCE6"><table width="207" border="0" cellpadding="0" cellspacing="0" background="images/left_bj.jpg">
        <tr>
          <td align="center"><!--#include file="left.asp"--></td>
        </tr>
      </table></td>
    <td width="701" valign="top"><table width="699" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="186" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="186" height="37" align="right" valign="bottom" class="k_16">多彩团队管理</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><br>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#1D84D4">
            <tr>
              <td width="101" height="24" align="center" bgcolor="#FFFFFF">用户编号</td>
              <td width="142" align="center" bgcolor="#FFFFFF">用户名</td>
              <td width="177" align="center" bgcolor="#FFFFFF">用户ID</td>
              <td width="175" align="center" bgcolor="#FFFFFF">家族管理</td>
            </tr>
            <%				  
				  CxGame.DbConn("News")
				  dim sql
				  set rs=server.CreateObject("adodb.recordset")
				  sql="select * from Tuandui where U_name='"& Session("UserName") &"'"
				  rs.open sql,conn,1,1
				  if rs.eof then
				  response.Write("<script language='javascript'>alert('你不是族长，不能使用此功能');history.go(-1);</script>")
				  response.End()
				  end if
				  rs.close
				  sql="select * from jiaz where T_id="&T_id
				  rs.open sql,conn,1,1
				  if not rs.eof then
				  i=0
				  do while not rs.eof
				  i=i+1
				%>
            <tr>
              <td height="24" align="center" bgcolor="#FFFFFF"><%=i%></td>
              <td align="center" bgcolor="#FFFFFF"><%
				    CxGame.DbConn("QPGameUserDB")
				    dim sqll,rss
					set rss=server.CreateObject("adodb.recordset")
					sqll="select * from AccountsInfo where userid="&rs("U_id")
					rss.open sqll,conn,1,1
					if not rss.eof then					
				  %>
                  <%=rss("Accounts")%>
                  <%end if%>
              </td>
              <td align="center" bgcolor="#FFFFFF"><%=rs("U_id")%></td>
              <td align="center" bgcolor="#FFFFFF"><% 
				      if rs("T_type")=1 then
					  response.Write "<a href=""T_del.asp?id="& rs("J_id") &""">删除</a>"
					  else
					  response.Write "<a href=""T_open.asp?id="& rs("J_id") &"&t_id="& T_id &""">通过</a>"
					  response.Write "&nbsp;&nbsp;"
					  response.Write "<a href=""T_del.asp?id="& rs("J_id") &""">删除</a>"
					  end if
				  %>
              </td>
            </tr>
            <%
				   rs.movenext
				   loop
				   end if 
				   rs.close
				 %>
          </table></td>
      </tr>
      <tr>
        <td><img src="images/l_43.jpg" width="699" height="23"></td>
      </tr>
    </table></td>
    <td width="88" align="center" valign="top"><!--#include file="Right.asp"--></td>
  </tr>
</table>
<!--#include file="copy.asp"-->
</body>
</html>
l