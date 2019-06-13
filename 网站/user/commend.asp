<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
CxGame.IsCheckLogin()
%>

<link href="images/css.css" rel="stylesheet" type="text/css">
<table width="770" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td><!--#include file="Top.asp" --></td>
  </tr>
</table>
<table width="889" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="204" height="312" align="center" valign="top"><table width="180" height="4" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td></td>
      </tr>
    </table> <!--#include file="left.asp"--> </td>
    <td align="center" valign="top"><table width="580" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td>&nbsp;</td>
      </tr>
    </table>
    <table width="585" height="24" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td class="hui_12">&nbsp;&nbsp;个人营销系统,获得金币的最佳途径(<a href="../tui.asp?xxx=<%=session("UserName")%>">详情请点击</a>)</td>
      </tr>
    </table>
      <table width="580" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td height="122" align="center" valign="top"><table width="570" border="0" cellpadding="0" cellspacing="1" bgcolor="#56AA0A">
            <tr>
              <td width="47" height="24" align="center" bgcolor="#FFFFFF" class="hang">序号</td>
              <td width="93" align="center" bgcolor="#FFFFFF" class="hang">被推荐用户</td>
              <td width="141" align="center" bgcolor="#FFFFFF" class="hang">用户注册时间</td>
              <td width="141" align="center" bgcolor="#FFFFFF" class="hang">充值贡献金币</td>
              <td width="142" align="center" bgcolor="#FFFFFF" class="hang">在线时间完成度</td>
            </tr>
			  <%CxGame.DbConn("QPGameUserDB")%>

			 <%		
			 dim userrid,commid	,rs1,price0,pricet,rs2,PlayTimeCount,sql,PlayTimeCount1,rs3,rs4,kk,pages,summoney

			   set rs=server.CreateObject("adodb.recordset")
			   sql="select userid,Accounts,RegisterDate from AccountsInfo where SpreaderID='"& session("userid") &"'"
			   rs.open sql,conn,1,1
			        if not rs.eof then
					page=request("page")
					if page ="" or isnumeric(page)=false then page = 1
					page=clng(page)
					rs.pagesize=20
					if page < 1 then page = 1
					pages=rs.pagecount
					if page > pages then page=pages
					rs.absolutepage=page
					for kk=1 to rs.pagesize
			   
			   CxGame.DbConn("CxGameTreasuredb")

			   set rs1=conn.execute("select price,postdate,UserID from PostLog where userid ='"& rs(0) &"'")
			   if not rs1.eof then
			      price0=rs1(0)*100
			   else
			      price0=0
			   end if
			   pricet=clng(price0)/10
			   
			   CxGame.DbConn("QPTreasureDB")
			   set rs2=conn.execute("select * from GameScoreInfo where userid='"& rs(0) &"'")
			   if not rs2.eof then
			      PlayTimeCount=left(rs2("PlayTimeCount")/3600*10,2)&"%"
				  PlayTimeCount1=left(rs2("PlayTimeCount")/3600*10,2)
			   end if
			   if PlayTimeCount="" then PlayTimeCount="0%"
			   
			   if clng(PlayTimeCount1) >=100 then 
			      PlayTimeCount="100%"
				  
				  set rs3=conn.execute("select * from GameScoreInfo where ok<>1 and userid='"& rs(0) &"'")
				  if rs.eof then
					  set rs4=conn.execute("update GameScoreInfo set Score=Score+10000 where userid='"& session("userid") &"'")
					  set rs4=conn.execute("update GameScoreInfo set Score=Score+10000 where userid='"& rs(0) &"'")
					  set rs4=conn.execute("update GameScoreInfo set ok=1 where userid='"& rs(0) &"'")
				  end if
				  pricet=pricet+10000
			   end if
			   summoney=0
			   summoney=clng(pricet)+clng(summoney)
			   if summoney="" then summoney=0
			 %>
            <tr>
              <td width="47" height="24" align="center" bgcolor="#FFFFFF" class="hangxi"><%=rs(0)%></td>
              <td width="93" align="center" bgcolor="#FFFFFF" class="hangxi"><%=rs(1)%></td>
              <td width="141" align="center" bgcolor="#FFFFFF" class="hangxi"><%=formatdatetime(rs(2),2)%></td>
              <td width="141" align="center" bgcolor="#FFFFFF" class="hangxi"><%=pricet%></td>
              <td width="142" align="center" bgcolor="#FFFFFF" class="hangxi"><%=PlayTimeCount%></td>
            </tr>
		
			  <%
			   rs.movenext
			   if rs.eof or rs.bof then exit for
			   next
			   end if			   
			 %> 
			  
          </table>
            <table>
              <tr>
                <td class="hang"><%
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
		%>                </td>
              </tr>
            </table>
            <table width="500" height="24" border="0" cellpadding="0" cellspacing="0">
              <tr class="hang">
                <td width="145" align="right" class="hang">推荐用户数量：</td>
                <td width="57" class="red_12px"><%=rs.recordcount%></td>
                <td width="75" class="hang">历史收入：</td>
                <td width="223" align="left" class="red_12px"><%=summoney%></td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
      <hr width="580" size="1">
      <br>
      <table width="565" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td height="13" class="hang">注：
            <br>
           &nbsp;&nbsp; 1、充值贡献金币采用即时结算方式，只要被推荐人充值，那么您的帐号立刻就会获得10%的金<br>
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 币奖励。<br>
          &nbsp;&nbsp;
2、您的推荐用户使用的地址：http://www.dc173.com/reg.asp?introducer=<%=Session("UserName")%> </td>
        </tr>
        <tr>
          <td height="13" align="center" class="hang"> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <img src="images/copy.gif" style="cursor:hand;" onClick="copy('http://www.dc173.com/reg.asp?introducer=<%=Session("UserName")%>')" alt="复制" /></td>
        </tr>
      </table>
    <br></td>
  </tr>
</table>

<table width="913" height="85" border="0" align="center" cellpadding="0" cellspacing="0" class="bj">
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><!--#include file="../copy.asp"--></td>
  </tr>
</table>
<script>
    function copy(cc)
    {
        var text=cc;
        if (clipboardData.setData("text",cc))
        {
            alert("已复制到剪切板!");
        }else
        {
            alert("复制失败");
        }
    }
</script>