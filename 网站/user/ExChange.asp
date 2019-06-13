<!--#include file="../Inc/Config.asp" -->
<%CxGame.DbConn("News")%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>礼品兑换--多彩网游竞技！祝你玩的开心！</title>
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
	background-image: url();
	background-repeat: repeat-x;
	background-color: #1d84d4;
}
-->
</style>
<link href="images/css.css" rel="stylesheet" type="text/css">
</head>

<body>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="31" height="22" class="blue_b">&nbsp;&nbsp;&nbsp;</td>
    <td width="536" height="22" align="left" class="blue_b">&nbsp;</td>
    <td width="203" height="22"><a href="../index.asp" class="blue_link">返回首页</a></td>
  </tr>
</table>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="13">&nbsp;</td>
    <td width="116"><img src="images/remen.jpg" width="116" height="37"></td>
    <td width="512" background="images/middle_remen.jpg">&nbsp;</td>
    <td width="116"><img src="images/remen_wei.jpg" width="116" height="37"></td>
    <td width="13">&nbsp;</td>
  </tr>
</table>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>


<table width="770" height="24" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
<%
  set rs=conn.execute("select t_name from C_type")
  if not rs.eof then
  do while not rs.eof
%>
  <tr>
    <td width="13">&nbsp;</td>
    <td width="35" align="right" bgcolor="99ddff"><img src="images/xing.jpg" width="18" height="18"></td>
    <td width="593" height="24" bgcolor="99ddff" class="blue_b">&nbsp;<%=rs(0)%></td>
    <td width="116" bgcolor="99ddff">&nbsp;</td>
    <td width="13">&nbsp;</td>
  </tr>
  <tr>
    <td>&nbsp;</td>
    <td colspan="3" align="right">
	<table width="744" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
	<%
	  dim rs1,jj
	  set rs1=conn.execute("select C_name,C_money,C_pic,C_vip,C_cun,C_id from Jiangpin where C_type='"& rs(0) &"' order by C_time desc")
	  if not rs1.eof then
	  jj=0
	  do while not rs1.eof
	  jj = jj + 1
	  if jj mod 4 = 1 then
	%>
      <tr>
        <td width="25%" height="150"><br>
            <table width="120" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td colspan="3"><img src="images/ding.jpg" width="140" height="9"></td>
              </tr>
              <tr>
                <td width="6" rowspan="2" class="left">&nbsp;</td>
                <td width="127" height="92" align="center"><a target="_blank" href="ExChanges.asp?c_id=<%=rs1("c_id")%>"><img src="../<%=rs1(2)%>" width="119" height="89" border="0" /></a></td>
                <td width="7" rowspan="2" class="right">&nbsp;</td>
              </tr>
              <tr>
                <td height="24" align="center"><%=rs1(0)%></td>
              </tr>
              <tr>
                <td colspan="3"><img src="images/di.jpg" width="140" height="9"></td>
              </tr>
              <tr>
                <td height="30" colspan="3" align="center">库存：<%=rs1(4)%>件</td>
              </tr>
           </table></td>
		  <%else%>
        <td width="25%"><br>
          <table width="120" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td colspan="3"><img src="images/ding.jpg" width="140" height="9"></td>
            </tr>
            <tr>
              <td width="6" rowspan="2" class="left">&nbsp;</td>
              <td width="127" height="92" align="center"><a target="_blank" href="ExChanges.asp?c_id=<%=rs1("c_id")%>"><img src="../<%=rs1(2)%>" width="119" height="89" border="0" /></a></td>
              <td width="7" rowspan="2" class="right">&nbsp;</td>
            </tr>
            <tr>
              <td height="24" align="center"><%=rs1(0)%></td>
            </tr>
            <tr>
              <td colspan="3"><img src="images/di.jpg" width="140" height="9"></td>
            </tr>
            <tr>
              <td height="30" colspan="3" align="center">库存：<%=rs1(4)%>件</td>
            </tr>
          </table></td>
		  <%
		    end if
			rs1.movenext
			loop
			end if
			rs1.close
		  %>
      </tr>
    </table>
	</td>
    <td>&nbsp;</td>
  </tr>
  <%
    rs.movenext
	loop
	end if
	rs.close
  %>
</table>



<table width="770" height="24" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="13">&nbsp;</td>
    <td width="35"><img src="images/shuoming.jpg" width="116" height="37"></td>
    <td width="593" background="images/middle_shuo.jpg"><img src="images/middle_shuo.jpg" width="1" height="37"></td>
    <td width="116"><img src="images/shuoming_wei.jpg" width="116" height="37"></td>
    <td width="13">&nbsp;</td>
  </tr>
</table>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="13" height="177">&nbsp;</td>
    <td width="744">&nbsp;</td>
    <td width="13">&nbsp;</td>
  </tr>
</table>
</body>
</html>
b