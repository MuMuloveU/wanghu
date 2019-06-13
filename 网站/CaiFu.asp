<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="153" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="153" height="37" align="right" valign="bottom" class="k_16">财富排行</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><br>
          <table width="600" border="0" cellpadding="0" cellspacing="1" bgcolor="#C0E9FF">
            <tr>
              <td width="138" height="24" align="center" bgcolor="#FFFFFF">排名</td>
              <td width="262" align="center" bgcolor="#FFFFFF">游戏昵称</td>
              <td width="200" align="center" bgcolor="#FFFFFF">财富值</td>
            </tr>
<%CxGame.DbConn("QPTreasureDB")%>
<%
  Set Rs=Conn.Execute("Select top 51 Score,userid From GameScoreInfo order by Score desc")
  if not rs.eof then
  for i = 1 to 51
  dim rs1
      set rs1=server.CreateObject("adodb.recordset")
	  CxGame.DbConn("QPGameUserDB")
	  set rs1=conn.Execute("select Accounts from AccountsInfo where userid="& rs(1) &"")
	  if not rs1.eof then
%>
            <tr>
              <td height="24" align="center" bgcolor="#FFFFFF"><span class="hui"><%=i-1%></span></td>
              <td align="center" bgcolor="#FFFFFF"><span class="hui"><%=rs1(0)%></span></td>
              <td align="center" bgcolor="#FFFFFF"><span class="hui"><%=rs(0)%></span></td>
            </tr>
<%
  end if
  rs.movenext
  if rs.eof or rs.bof then exit for
  next
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
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>
</body>
</html>
