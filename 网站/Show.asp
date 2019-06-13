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
<%CxGame.DbConn("News")%>
<%
  dim newstitle,newsinfo,newsdate
  set rs=conn.execute("select top 1 newstitle,newsinfo,newsdate from news where id="&id)
  if not rs.eof then
     newstitle=rs(0)
	 newsinfo=rs(1)
	 newsdate=rs(2)
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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="153" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="153" height="37" align="right" valign="bottom" class="k_16">多彩动态</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td height="24" align="right"><a href="/index.asp" class="hui_link">首页</a> <span class="hui">&gt;</span> <a href="newslist.asp?classcode=100&amp;classname=新闻公告" class="hui_link">新闻动态</a> <span class="hui">&gt; 正文&nbsp;&nbsp; </span></td>
          </tr>
        </table>
          <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td height="24" align="center" class="shangxia"><%=newstitle%></td>
            </tr>
          </table>
          <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td height="24" align="center" class="hui_hang"><%=formatdatetime(newsdate,2)%></td>
            </tr>
          </table>
          <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td align="left" class="hui_hang" style="table-layout: fixed;WORD-BREAK: break-all; WORD-WRAP: break-word"><%=newsinfo%></td>
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
       