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
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="197" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="197" height="37" align="right" valign="bottom" class="k_16">多彩玩家靓照</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><%CxGame.DbConn("QPGameUserDB")%>
          <table width="600" border="0" cellspacing="0" cellpadding="0">
            <%
			    dim mm,kk,pages,sql
			    set rs=server.CreateObject("adodb.recordset")
				sql="select c_pic,UserID from AccountsInfo where c_pic is not null order by UserID desc"
				rs.open sql,conn,1,1
				if not rs.eof then				
				mm=0
				page=request("page")
				if page ="" or isnumeric(page)=false then page = 1
				page=clng(page)
				rs.pagesize=28
				if page < 1 then page = 1
				pages=rs.pagecount
				if page > pages then page=pages
				rs.absolutepage=page
			  %>
            <%
			    for kk=1 to rs.pagesize
				mm = mm + 1
				if mm mod 4 = 1 then
			  %>
            <tr>
              <td width="150" height="111" align="center"><table width="120" height="100" border="0" cellpadding="0" cellspacing="1" bgcolor="#FFFFFF">
                  <tr>
                    <td align="center" bgcolor="#7BDD14"><a href="<%=rs(0)%>" target="_blank"><img src="<%=rs(0)%>" width="111" height="91" border="0" /></a></td>
                  </tr>
              </table></td>
              <%else%>
              <td width="150" align="center"><table width="120" height="100" border="0" cellpadding="0" cellspacing="1" bgcolor="#FFFFFF">
                  <tr>
                    <td align="center" bgcolor="#7BDD14"><a href="<%=rs(0)%>" target="_blank"><img src="<%=rs(0)%>" width="111" height="91" border="0" /></a></td>
                  </tr>
              </table></td>
              <%
				    end if
					rs.movenext
					if rs.eof or rs.bof then exit for
					next
					end if
					'rs.close
				  %>
            </tr>
          </table>
          <br />
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
		  response.write "&nbsp;页次：<strong><font color=red>"& Page &"</font>/"& pages &"</strong>页 "
          response.write "&nbsp;共<b><font color='#FF0000'>"& rs.recordcount &"</font></b>条记录 <b>"& rs.pagesize &"</b>条记录/页"
		%>
              </td>
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
�