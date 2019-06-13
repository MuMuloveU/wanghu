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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="191" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="191" height="37" align="right" valign="bottom" class="k_16">多彩团队会员</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><table width="690" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>&nbsp;</td>
          </tr>
        </table>
          <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td width="11"><img src="images/S_1.jpg" width="11" height="34"></td>
              <td width="552" align="left" background="images/S_2.jpg">&nbsp;&nbsp; <a href="TuanDui.asp" class="hei_link">社团首页</a> |&nbsp; <a href="HY.asp" class="hei_link">会员情况</a> |&nbsp; <a href="Create.asp" class="hei_link">创建社团</a></td>
              <td width="87" align="left"><img src="images/S_3.jpg" width="12" height="34"></td>
            </tr>
          </table>
          <table width="690" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td>&nbsp;</td>
            </tr>
          </table>
          <table width="664" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#1D84D4">
            <tr>
              <td width="158" height="24" align="center" bgcolor="#FFFFFF">用户编号</td>
              <td width="258" align="center" bgcolor="#FFFFFF">家族成员</td>
              <td width="244" align="center" bgcolor="#FFFFFF">用户ID</td>
            </tr>
            <%				  
				  CxGame.DbConn("News")
				  dim sql
				  set rs=server.CreateObject("adodb.recordset")
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
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><img src="bookpic/DC173_02.jpg" width="1002" height="237"></td>
  </tr>
</table>
</body>
</html>