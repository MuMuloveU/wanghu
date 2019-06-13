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
  set rs=conn.execute("select top 1 C_title,C_content,C_time from C_content where C_id="&id)
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
<style type="text/css">
<!--
.STYLE1 {	color: #FFFFFF;
	font-weight: bold;
}
-->
</style>
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
            <td width="153" height="37" align="right" valign="bottom" class="k_16">赛事动态</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><table width="600" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td height="24" align="right"><a href="/index.asp" class="hui_link">首页</a> <span class="hui">&gt;</span> <span class="hui">正文&nbsp;&nbsp; </span></td>
          </tr>
        </table>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td height="24" align="center" class="shangxia"><%=newstitle%></td>
            </tr>
          </table>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td height="24" align="center" class="hui_hang"><%=formatdatetime(newsdate,2)%></td>
            </tr>
          </table>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td align="left" class="hui_hang" style="table-layout: fixed;WORD-BREAK: break-all; WORD-WRAP: break-word"><%=newsinfo%></td>
            </tr>
            <tr>
              <td height="24" align="right" class="hui_hang" style="table-layout: fixed;WORD-BREAK: break-all; WORD-WRAP: break-word"><a href="People.asp?id=<%=id%>">查看报名人员</a>&nbsp;&nbsp;&nbsp; </td>
            </tr>
            <tr>
              <td height="24" align="right" class="hui_hang" style="table-layout: fixed;WORD-BREAK: break-all; WORD-WRAP: break-word"><a href="Match.asp?id=<%=id%>">我要报名&gt;&gt;&gt;</a> &nbsp;&nbsp; &nbsp;</td>
            </tr>
          </table>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td height="30" align="center" bgcolor="#1D84D4"><span class="STYLE1">比赛奖品</span></td>
            </tr>
          </table>
          <table width="600" height="24" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
            <%
  set rs=conn.execute("select t_name from C_type")
  if not rs.eof then
  do while not rs.eof
%>
            <tr>
              <td width="13">&nbsp;</td>
              <td width="35" align="right" bgcolor="99ddff"><img src="user/images/xing.jpg" width="18" height="18" /></td>
              <td width="593" height="24" align="left" bgcolor="99ddff" class="lan">&nbsp;<%=rs(0)%></td>
              <td width="116" bgcolor="99ddff">&nbsp;</td>
              <td width="13">&nbsp;</td>
            </tr>
            <tr>
              <td>&nbsp;</td>
              <td colspan="3" align="right"><table width="600" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
                  <%
	  dim rs1,jj
	  set rs1=conn.execute("select C_name,C_money,C_pic,C_vip,C_cun,C_id from Jiangpin where C_type='"& rs(0) &"' order by C_time desc")
	  if not rs1.eof then
	  jj=0
	  do while not rs1.eof
	  jj = jj + 1
	  if jj mod 3 = 1 then
	%>
                  <tr>
                    <td width="33%" height="150"><br />
                        <table width="120" border="0" align="center" cellpadding="0" cellspacing="0">
                          <tr>
                            <td colspan="3"><img src="user/images/ding.jpg" width="140" height="9" /></td>
                          </tr>
                          <tr>
                            <td width="6" rowspan="2" class="left">&nbsp;</td>
                            <td width="127" height="92" align="center"><a target="_blank" href="user/ExChanges.asp?c_id=<%=rs1("c_id")%>"><img src="../<%=rs1(2)%>" width="119" height="89" border="0" /></a></td>
                            <td width="7" rowspan="2" class="ss_ss">&nbsp;</td>
                          </tr>
                          <tr>
                            <td height="24" align="center"><%=rs1(0)%></td>
                          </tr>
                          <tr>
                            <td colspan="3"><img src="user/images/di.jpg" width="140" height="9" /></td>
                          </tr>
                          <tr>
                            <td height="30" colspan="3" align="center">库存：<%=rs1(4)%>件</td>
                          </tr>
                      </table></td>
                    <%else%>
                    <td width="33%"><br />
                        <table width="120" border="0" align="center" cellpadding="0" cellspacing="0">
                          <tr>
                            <td colspan="3"><img src="user/images/ding.jpg" width="140" height="9" /></td>
                          </tr>
                          <tr>
                            <td width="6" rowspan="2" class="left">&nbsp;</td>
                            <td width="127" height="92" align="center"><a target="_blank" href="user/ExChanges.asp?c_id=<%=rs1("c_id")%>"><img src="../<%=rs1(2)%>" width="119" height="89" border="0" /></a></td>
                            <td width="7" rowspan="2" class="right">&nbsp;</td>
                          </tr>
                          <tr>
                            <td height="24" align="center"><%=rs1(0)%></td>
                          </tr>
                          <tr>
                            <td colspan="3"><img src="user/images/di.jpg" width="140" height="9" /></td>
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
              </table></td>
              <td>&nbsp;</td>
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
"