<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim id
  id=replace(trim(request.QueryString("id")),"","'")
  if id="" or isnumeric(id)=false then
  response.Write("<script language='javascript'>alert('非法操作');window.close();</script>")
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
    <td width="6" height="1264" valign="top"><img src="images/L_33.jpg" width="6" height="384"></td>
    <td width="207" align="center" valign="top" bgcolor="#56CCE6"><table width="207" border="0" cellpadding="0" cellspacing="0" background="images/left_bj.jpg">
      <tr>
        <td align="center"><!--#include file="left.asp"--></td>
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
        </table>          
          <table width="650" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td width="11"><img src="images/S_1.jpg" width="11" height="34"></td>
              <td width="552" align="left" background="images/S_2.jpg">&nbsp;&nbsp; <a href="TuanDui.asp" class="hei_link">社团首页</a> |&nbsp; <a href="HY.asp?id=<%=id%>" class="hei_link">会员情况</a> |&nbsp; <a href="Create.asp" class="hei_link">创建社团 </a>| <%if Session("UserID")<>"" then%>
				  <a href="guanli.asp?id=<%=id%>" class="hei_link">管理</a>
				  <%else%>
				  管理
				  <%end if%></td>
              <td width="87" align="left"><img src="images/S_3.jpg" width="12" height="34"></td>
            </tr>
          </table>
          <table width="690" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>&nbsp;</td>
          </tr>
        </table>
		
		<%CxGame.DbConn("News")%>
		<%
		  set rs=conn.execute("select * from Tuandui where T_id="& id &"")
		  if not rs.eof then
		%>            
        <table width="657" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td height="36" align="left" valign="middle" background="images/t_1.jpg">  <table width="630" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td width="162" height="22" align="left" background="images/S_4.jpg" class="hui">&nbsp; ·社团实力</td>
                <td width="482" background="images/S_5.jpg"><img src="images/S_5.jpg" width="1" height="22"></td>
                <td width="5" align="left" valign="top"><img src="images/S_6.jpg" width="5" height="16"></td>
              </tr>
            </table></td>
          </tr>
          <tr>
            <td height="373" valign="top" class="tuandui"><table width="630" border="0" align="center" cellpadding="0" cellspacing="0">
                <tr>
                  <td height="242" align="center"><table width="630" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td width="116" height="30" align="right" class="hui">团队名称：&nbsp; </td>
                      <td width="310" height="30" align="left" class="blue"><%=rs("t_name")%></td>
                      <td width="204" height="30"><a href="SQ.asp?id=<%=rs("T_id")%>" target="_blank"><img src="images/s_7.jpg" width="100" height="22" border="0"></a></td>
                    </tr>
                    <tr>
                      <td height="30" align="right" class="hui">团队ID：&nbsp; </td>
                      <td height="30" align="left" class="blue"><%=rs("t_id")%></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                    <tr>
                      <td height="30" align="right" class="hui">管理员：&nbsp; </td>
                      <td height="30" align="left" class="blue"><%=rs("U_name")%></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                    <tr>
                      <td height="30" align="right" class="hui">联系方式：&nbsp; </td>
                      <td height="30" align="left" class="blue">联系QQ<%=rs("QQ")%>&nbsp; QQ群<%=rs("T_qq")%></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                    <tr>
                      <td height="30" align="right" class="hui">团队主页：&nbsp; </td>
                      <td height="30" align="left" class="blue"><%=rs("T_url")%></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                    <tr>
                      <td align="right" class="hui">团队人数：&nbsp; </td>
                      <td height="30" align="left"><font color="red"><%=rs("T_num")%></font></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                    <tr>
                      <td align="right" class="hui">成立时间：&nbsp; </td>
                      <td height="30" align="left"><font color="red"><%=formatdatetime(rs("T_date"),2)%></font></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                    <tr>
                      <td height="30" align="right" class="hui">团队BBS：&nbsp; </td>
                      <td height="30" align="left"><a href="<%=rs("T_luntan")%>" target="_blank"><img src="images/s_8.jpg" width="101" height="22" border="0"></a></td>
                      <td height="30">&nbsp;</td>
                    </tr>
                  </table></td>
                </tr>
                <tr>
                  <td><table width="630" border="0" align="center" cellpadding="0" cellspacing="0">
                    <tr>
                      <td width="162" height="22" align="left" background="images/S_4.jpg" class="hui">&nbsp; ·社团宗旨</td>
                      <td width="482" background="images/S_5.jpg"><img src="images/S_5.jpg" width="1" height="22"></td>
                      <td width="5" align="left" valign="top"><img src="images/S_6.jpg" width="5" height="16"></td>
                    </tr>
                  </table></td>
                </tr>
                <tr>
                  <td height="68" valign="top"><table width="630" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td>&nbsp;</td>
                    </tr>
                  </table>
                    <table width="630" border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td class="hui_hang"><%=rs("T_zongzhi")%></td>
                      </tr>
                    </table></td>
                </tr>
                <tr>
                  <td><table width="630" border="0" align="center" cellpadding="0" cellspacing="0">
                    <tr>
                      <td width="162" height="22" align="left" background="images/S_4.jpg" class="hui">&nbsp; ·社团介绍</td>
                      <td width="482" background="images/S_5.jpg"><img src="images/S_5.jpg" width="1" height="22"></td>
                      <td width="5" align="left" valign="top"><img src="images/S_6.jpg" width="5" height="16"></td>
                    </tr>
                  </table></td>
                </tr>
                <tr>
                  <td height="260" valign="top"><table width="630" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td>&nbsp;</td>
                    </tr>
                  </table>
                    <table width="630" border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td class="hui_hang"><%=rs("T_content")%></td>
                      </tr>
                    </table></td>
                </tr>
              </table></td>
          </tr>
          <tr>
            <td height="6"><img src="images/t_2.jpg" width="657" height="6"></td>
          </tr>
        </table>
		<%end if%>
		</td>
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