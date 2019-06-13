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
    <td width="6" height="1264" valign="top"><img src="images/L_33.jpg" width="6" height="384"></td>
    <td width="207" align="center" valign="top" bgcolor="#56CCE6"><table width="207" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td><!--#include file="left.asp"--></td>
      </tr>
    </table></td>
    <td width="701" valign="top"><table width="699" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td height="39" valign="middle" background="images/l_35.jpg"> <table width="153" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="153" height="37" align="right" valign="bottom" class="k_16">多彩社团</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1202" align="center" valign="top" background="images/l_36.jpg"><table width="690" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>&nbsp;</td>
          </tr>
        </table>          <table width="657" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td height="36" align="left" valign="bottom" background="images/t_1.jpg"> &nbsp; <img src="images/t_xiaoxi.jpg" width="137" height="28"></td>
          </tr>
          <tr>
            <td height="183" valign="top" class="tuandui"><table width="640" height="1" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#ACA899">
              <tr>
                <td></td>
              </tr>
            </table>
              <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
                <tr>
                  <td width="212" height="182" align="center"><img src="bookpic/社团——more_03.jpg" width="163" height="142"></td>
                  <td width="438" valign="top">
				  
				  <%CxGame.DbConn("News")%>
				  <table width="430" border="0" cellspacing="0" cellpadding="0">
				  <%
				    set rs=conn.execute("select top 6 * from News where classcode in(select classcode from class where classname='团队信息') order by newsdate desc")
					if not rs.eof then
					do while not rs.eof
				  %>
                    <tr>
                      <td width="59" height="30" align="center"><img src="images/TD_bj.jpg" width="43" height="17"></td>
                      <td width="263" align="left"><a href="Show.asp?id=<%=rs("id")%>" target="_blank" class="hui_link"><%=rs("newstitle")%></a></td>
                      <td width="108" valign="middle" class="hui"><%=formatdatetime(rs("newsdate"),2)%></td>
                    </tr>
				  <%
				    rs.movenext
					loop
					end if
					rs.close
				  %>
                  </table>
				  
				  
				  </td>
                </tr>
              </table></td>
          </tr>
          <tr>
            <td height="6"><img src="images/t_2.jpg" width="657" height="6"></td>
          </tr>
        </table>
        <table width="690" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>&nbsp;</td>
          </tr>
        </table>
        <table width="657" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td height="36" align="left" valign="bottom" background="images/t_1.jpg"> &nbsp; <img src="images/t_paihang.jpg" width="125" height="28"></td>
          </tr>
          <tr>
            <td height="348" valign="top" class="tuandui"><table width="640" height="1" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#ACA899">
                <tr>
                  <td></td>
                </tr>
            </table>
              <table width="644" border="0" align="center" cellpadding="0" cellspacing="2">
                <tr>
                  <td width="241" height="28" align="center" background="images/She_bj.jpg" class="bai_12">社团名称</td>
                  <td width="189" align="center" background="images/She_bj.jpg" class="bai_12">社团人数</td>
                  <td width="206" align="center" background="images/She_bj.jpg" class="bai_12">成立时间</td>
                </tr>
				
				<%
				  set rs=conn.execute("select * from Tuandui where T_type=1 order by t_num desc")
				  if not rs.eof then
				  do while not rs.eof 
				%>
                <tr>
                  <td height="30" align="center" bgcolor="#A4D5EE"><a href="Show_T.asp?id=<%=rs("T_id")%>" target="_blank" class="hui_link"><%=rs("T_name")%></a></td>
                  <td align="center" bgcolor="#A4D5EE" class="blue"><%=rs("T_num")%></td>
                  <td align="center" bgcolor="#A4D5EE" class="blue"><%=formatdatetime(rs("T_date"),2)%></td>
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
            <td height="6"><img src="images/t_2.jpg" width="657" height="6"></td>
          </tr>
        </table>
        <table width="690" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>&nbsp;</td>
          </tr>
        </table>
        <table width="657" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td height="36" align="left" valign="bottom" background="images/t_1.jpg"> &nbsp; <img src="images/t_new.jpg" width="130" height="26"></td>
          </tr>
          <tr>
            <td height="373" valign="top" class="tuandui"><table width="640" height="1" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#ACA899">
                <tr>
                  <td></td>
                </tr>
            </table>
              <table width="644" border="0" align="center" cellpadding="0" cellspacing="2">
                <tr>
                  <td width="241" height="28" align="center" background="images/She_bj.jpg" class="bai_12">社团名称</td>
                  <td width="189" align="center" background="images/She_bj.jpg" class="bai_12">社团人数</td>
                  <td width="206" align="center" background="images/She_bj.jpg" class="bai_12">成立时间</td>
                </tr>
                <%
				  set rs=conn.execute("select * from Tuandui where T_type=1 order by t_date desc")
				  if not rs.eof then
				  do while not rs.eof 
				%>
                <tr>
                  <td height="30" align="center" bgcolor="#A4D5EE"><a href="Show_T.asp?id=<%=rs("T_id")%>" target="_blank" class="hui_link"><%=rs("T_name")%></a></td>
                  <td align="center" bgcolor="#A4D5EE" class="blue"><%=rs("T_num")%></td>
                  <td align="center" bgcolor="#A4D5EE" class="blue"><%=formatdatetime(rs("T_date"),2)%></td>
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
            <td height="6"><img src="images/t_2.jpg" width="657" height="6"></td>
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