<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim id
  id=replace(trim(request.QueryString("id")),"'","")
  if id="" or isnumeric(id)=false then
     response.Write("<script language='javascript'>alert('非法操作！');window.close();</script>")
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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="223" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="223" height="37" align="right" valign="bottom" class="k_16">多彩赛事报名人员</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><br>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#DDDDDD">
            <tr align="center" bgcolor="#FFFFFF">
              <td width="165" height="24">姓名</td>
              <td width="104">年龄</td>
              <td width="151">职业</td>
              <td width="175">报名时间</td>
            </tr>
            <%CxGame.DbConn("News")%>
            <%
				  dim sql
				  set rs=server.CreateObject("adodb.recordset")
				  sql="select * from MatchUserInfo where C_id='"& id &"' order by userid desc"
				  rs.open sql,conn,1,1
				  if not rs.eof then
				  'dim page
				  page=request("page")
				  if page="" or isnumeric(page)=false then page=1
				  page=clng(page)
				  rs.pagesize=25
				  dim pages
				  pages=rs.pagecount
				  if page > pages then page=pages
				  rs.absolutepage=page
				%>
            <%for i = 1 to rs.pagesize%>
            <tr align="center" bgcolor="#FFFFFF">
              <td height="24"><%=rs("Username")%></td>
              <td><%=rs("Age")%></td>
              <td><%=rs("Profession")%></td>
              <td><%=formatdatetime(rs("CollectDate"),2)%></td>
            </tr>
            <%
				  rs.movenext
				  if rs.eof or rs.bof then exit for
				  next
				  end if
				%>
          </table>
          <br>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td align="right"><a href="Match.asp?id=<%=id%>">我要报名&gt;&gt;&gt;</a>&nbsp;&nbsp; </td>
            </tr>
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
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>
</body>
</html>