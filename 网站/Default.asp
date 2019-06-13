<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%CxGame.DbConn("News")%>
<%
  dim G_pic(5),G_http(5),kk
  set rs=conn.execute("select top 5 G_pic,G_http from Guanggao where G_type='网站' order by G_time desc")
  if not rs.eof then
     for kk=1 to 5
	     G_pic(kk)=rs(0)
		 G_http(kk)=rs(1)
	 rs.movenext
	' response.Write G_pic(kk)
	 if rs.eof or rs.bof then exit for
	 next
  end if  
  rs.close  
%>
<script language="javascript" src="images/Table.js"></script>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>多彩网络游戏</title>
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
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="156" align="right"><img src="d_images/1.jpg" width="138" height="41"></td>
    <td width="104"><a href="index.asp" target="_blank"><img src="d_images/2.jpg" width="104" height="41" border="0"></a></td>
    <td width="96"><a href="HuoDong.asp" target="_blank"><img src="d_images/3.jpg" width="96" height="41" border="0"></a></td>
    <td width="98"><a href="Down.asp" target="_blank"><img src="d_images/4.jpg" width="98" height="41" border="0"></a></td>
    <td width="135"><a href="Tui.asp" target="_blank"><img src="d_images/5.jpg" width="135" height="41" border="0"></a></td>
    <td width="98"><a href="PayBuy.asp" target="_blank"><img src="d_images/6.jpg" width="98" height="41" border="0"></a></td>
    <td width="102"><a href="GameUser.asp" target="_blank"><img src="d_images/7.jpg" width="102" height="41" border="0"></a></td>
    <td width="97"><a href="Tuandui.asp" target="_blank"><img src="d_images/8.jpg" width="97" height="41" border="0"></a></td>
    <td width="116" align="left"><a href="bbs/" target="_blank"><img src="d_images/9.jpg" width="94" height="41" border="0"></a></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td height="4"></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td height="8" bgcolor="0089D9"></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td height="4"></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="16" align="center" valign="top">&nbsp;</td>
    <td width="12" align="center" valign="top"><table width="766"  border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF" class="bk">
      <tr>
        <td colspan="2" align="center" valign="top"><table width="334"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td width="52"><img src="images/left.jpg" width="52" height="35"></td>
              <td width="261" align="center" background="images/middle.jpg"><table width="240" border="0" cellspacing="0" cellpadding="0">
                  <tr>
                    <td width="60" height="35" align="center" valign="bottom" background="images/n_bj.jpg" class="icoWhite14px" id="tableB1" style="CURSOR: hand" onMouseOver="channeTableB(1)">最新</td>
                    <td width="60" align="center" valign="bottom" class="icoWhite13px" id="tableB2" style="CURSOR: hand" onMouseOver="channeTableB(2)">新闻</td>
                    <td width="60" align="center" valign="bottom" class="icoWhite13px" id="tableB3" style="CURSOR: hand" onMouseOver="channeTableB(3)">公告</td>
                    <td width="60" align="center" valign="bottom" class="icoWhite13px" id="tableB4" style="CURSOR: hand" onMouseOver="channeTableB(4)">维护</td>
                  </tr>
              </table></td>
              <td width="18"><img src="images/right.jpg" width="17" height="35"></td>
            </tr>
          </table>          
          <table width="334" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td width="16" height="257" background="images/right_334.jpg">&nbsp;</td>
              <td width="318" align="center" valign="top"><table width="318" border="0" cellpadding="0" cellspacing="0">
                  <tr>
                    <td height="57" align="center" valign="middle"><table width="318" border="0" cellpadding="0" cellspacing="0" class="dixu">
                        <%CxGame.DbConn("News")%>
                        <%
					set rs=conn.execute("SELECT TOP (3) newstitle, id, classcode, newsdate FROM news WHERE (pl = 1) and classcode in(select classcode from class where classname<>'团队信息') ORDER BY newsdate DESC")
					'set rs=server.CreateObject("adodb.recordset")
'					sql="select top 3 newstitle,id,classcode,newsdate from news where pl="&true
'					rs.open sql,conn
					if not rs.eof then
					dim id(3),t(3),d(3),n(3)
					for kk=1 to 3
					    id(kk)=rs(1)
						t(kk)=rs(0)
						d(kk)=rs(3)
						n(kk)=rs(2)
					rs.movenext
					if rs.eof or rs.bof then exit for
					next
					end if
					rs.close
				  %>
                        <tr>
                          <td height="24" colspan="2" align="center"><a href="Show.asp?id=<%=id(1)%>" target="_blank" class="red_link"><%=t(1)%></a></td>
                        </tr>
                        <tr>
                          <td width="159" height="24" align="right"><a href="Show.asp?id=<%=id(2)%>" target="_blank" class="ju_link">[<%=t(2)%>]</a>&nbsp;</td>
                          <td width="159" height="24" align="left">&nbsp;<a href="Show.asp?id=<%=id(3)%>" target="_blank" class="ju_link">[<%=t(3)%>]</a></td>
                        </tr>
                    </table></td>
                  </tr>
                </table>
                  <table width="318" border="0" cellspacing="0" cellpadding="0" id="b1">
                    <tr>
                      <td><table width="318" border="0" cellspacing="0" cellpadding="0">
                          <%
					  set rs=conn.execute("select top 8 newstitle,id,classcode,newsdate from news WHERE (pl = 0) and classcode in(select classcode from class where classname<>'团队信息') order by newsdate desc")
					  if not rs.eof then
					  do while not rs.eof
					%>
                          <%
					  dim classname,rs1
					  set rs1=conn.execute("select classname from class where classcode='"& rs(2) &"'")
					  if not rs1.eof then
						 classname=rs1(0)
					  end if
					%>
                          <tr>
                            <td width="57" align="center" class="News"><font color="#FFFFFF"><%=classname%></font></td>
                            <td width="183"><a href="Show.asp?id=<%=rs(1)%>" target="_blank" class="hui_link"><%=left(rs(0),10)%></a></td>
                            <td width="78" height="24" class="blue">[<%=formatdatetime(rs(3),2)%>]</td>
                          </tr>
                          <%
					  rs.movenext
					  loop
					  end if
					  rs.close
					%>
                      </table></td>
                    </tr>
                  </table>
                <table width="318" border="0" cellspacing="0" cellpadding="0" id="b2" style="display:none">
                    <tr>
                      <td><table width="318" border="0" cellspacing="0" cellpadding="0">
                          <%
					  set rs=conn.execute("select top 8 newstitle,id,classcode,newsdate from news where classcode in(select classcode from class where classname='新闻') order by newsdate desc")
					  if not rs.eof then
					  do while not rs.eof
					%>
                          <tr>
                            <td width="57" align="center" class="News"><font color="#FFFFFF">新闻</font></td>
                            <td width="183"><a href="Show.asp?id=<%=rs(1)%>" target="_blank" class="hui_link"><%=left(rs(0),10)%></a></td>
                            <td width="78" height="24" class="blue">[<%=formatdatetime(rs(3),2)%>]</td>
                          </tr>
                          <%
					    rs.movenext
						loop
						end if
						rs.close
					  %>
                      </table></td>
                    </tr>
                  </table>
                <table width="318" border="0" cellspacing="0" cellpadding="0" id="b3" style="display:none">
                    <tr>
                      <td><table width="318" border="0" cellspacing="0" cellpadding="0">
                          <%
					  set rs=conn.execute("select top 8 newstitle,id,classcode,newsdate from news where classcode in(select classcode from class where classname='公告') order by newsdate desc")
					  if not rs.eof then
					  do while not rs.eof
					%>
                          <tr>
                            <td width="57" align="center" class="News"><font color="#FFFFFF">公告</font></td>
                            <td width="183"><a href="Show.asp?id=<%=rs(1)%>" target="_blank" class="hui_link"><%=left(rs(0),10)%></a></td>
                            <td width="78" height="24" class="blue">[<%=formatdatetime(rs(3),2)%>]</td>
                          </tr>
                          <%
					    rs.movenext
						loop
						end if
						rs.close
					  %>
                      </table></td>
                    </tr>
                  </table>
                <table width="318" border="0" cellspacing="0" cellpadding="0" id="b4" style="display:none">
                    <tr>
                      <td><table width="318" border="0" cellspacing="0" cellpadding="0">
                          <%
					  set rs=conn.execute("select top 8 newstitle,id,classcode,newsdate from news where classcode in(select classcode from class where classname='维护') order by newsdate desc")
					  if not rs.eof then
					  do while not rs.eof
					%>
                          <tr>
                            <td width="57" align="center" class="News"><font color="#FFFFFF">维护</font></td>
                            <td width="183"><a href="Show.asp?id=<%=rs(1)%>" target="_blank" class="hui_link"><%=left(rs(0),10)%></a></td>
                            <td width="78" height="24" class="blue">[<%=formatdatetime(rs(3),2)%>]</td>
                          </tr>
                          <%
					    rs.movenext
						loop
						end if
						rs.close
					  %>
                      </table></td>
                    </tr>
                </table></td>
            </tr>
          </table></td>
        <td width="292" height="225" align="center" valign="top"><table width="277"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td colspan="3"><img src="images/ding.jpg" width="277" height="12"></td>
            </tr>
            <tr>
              <td width="8"><img src="images/ding_l.jpg" width="8" height="273"></td>
              <td width="261" align="center" valign="top"><a target="_self" href="javascript:goUrl()"> <span class="f14b">
                  <script type="text/javascript">
imgUrl1="<%=G_pic(1)%>";
imgtext1=""
imgLink1=escape("<%=G_http(1)%>");
imgUrl2="<%=G_pic(2)%>";
imgtext2=""
imgLink2=escape("<%=G_http(2)%>");
imgUrl3="<%=G_pic(3)%>";
imgtext3=""
imgLink3=escape("<%=G_http(3)%>");
imgUrl4="<%=G_pic(4)%>";
imgtext4=""
imgLink4=escape("<%=G_http(4)%>");
imgUrl5="<%=G_pic(5)%>";
imgtext5=""
imgLink5=escape("<%=G_http(5)%>");

 var focus_width=261
 var focus_height=270
 var text_height=1
 var swf_height = focus_height+text_height
 
 var pics=imgUrl1+"|"+imgUrl2+"|"+imgUrl3+"|"+imgUrl4+"|"+imgUrl5
 var links=imgLink1+"|"+imgLink2+"|"+imgLink3+"|"+imgLink4+"|"+imgLink5
 var texts=imgtext1+"|"+imgtext2+"|"+imgtext3+"|"+imgtext4+"|"+imgtext5
 
 document.write('<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0" width="'+ focus_width +'" height="'+ swf_height +'">');
 document.write('<param name="allowScriptAccess" value="sameDomain"><param name="movie" value="focus.swf"><param name="quality" value="high"><param name="bgcolor" value="#F0F0F0">');
 document.write('<param name="menu" value="false"><param name=wmode value="opaque">');
 document.write('<param name="FlashVars" value="pics='+pics+'&links='+links+'&texts='+texts+'&borderwidth='+focus_width+'&borderheight='+focus_height+'&textheight='+text_height+'">');
 document.write('<embed src="pixviewer.swf" wmode="opaque" FlashVars="pics='+pics+'&links='+links+'&texts='+texts+'&borderwidth='+focus_width+'&borderheight='+focus_height+'&textheight='+text_height+'" menu="false" bgcolor="#F0F0F0" quality="high" width="'+ focus_width +'" height="'+ focus_height +'" allowScriptAccess="sameDomain" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" />');  document.write('</object>');
          </script></td>
              <td width="8"><img src="images/ding_r.jpg" width="8" height="273"></td>
            </tr>
            <tr>
              <td colspan="3"><img src="images/ding_d.jpg" width="277" height="7"></td>
            </tr>
        </table></td>
        <td width="139" align="left" valign="top"><table width="124"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/bian_t.jpg" width="124" height="7"></td>
          </tr>
          <tr>
            <td><a href="guizhi/scLand.asp" target="_blank"><img src="images/dizhu.jpg" width="124" height="45" border="0"></a></td>
          </tr>
          <tr>
            <td><a href="guizhi/Sparrowcd.asp" target="_blank"><img src="images/majiang.jpg" width="124" height="47" border="0"></a></td>
          </tr>
          <tr>
            <td><a href="guizhi/ding2huang.asp" target="_blank"><img src="images/paijiu.jpg" width="124" height="47" border="0"></a></td>
          </tr>
          <tr>
            <td><a href="guizhi/HKFiveCard.asp" target="_blank"><img src="images/wuzhang.jpg" width="124" height="47" border="0"></a></td>
          </tr>
          <tr>
            <td><a href="guizhi/gandengyan.asp" target="_blank"><img src="images/dengyan.jpg" width="124" height="47" border="0"></a></td>
          </tr>
          <tr>
            <td><a href="guizhi/ChinaChess.asp" target="_blank"><img src="images/xiangqi.jpg" width="124" height="46" border="0"></a></td>
          </tr>
          <tr>
            <td><img src="images/bian_d.jpg" width="124" height="6"></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td width="16" align="center" valign="top" background="images/right_334.jpg">&nbsp;</td>
        <td width="319" align="center" valign="top">&nbsp;</td>
        <td height="13" align="center" valign="middle">&nbsp;</td>
        <td height="13" align="center" valign="middle">&nbsp;</td>
      </tr>

      <tr>
        <td height="12" align="left" valign="top" background="images/right_334.jpg" bgcolor="#56CCE6">&nbsp;</td>
        <td height="12" colspan="3" align="center" valign="top" bgcolor="#FFFFFF" class="bk"><table width="731" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td width="731"><img src="d_images/10.jpg" width="731" height="14"></td>
          </tr>
          <tr>
            <td height="111" align="center" bgcolor="D6F2F6"><div id="demo" style="OVERFLOW: hidden; WIDTH: 730; COLOR: #ffffff">
                              <table cellspacing="0" cellpadding="0" align="left" border="0" cellspace="0">
                                <tbody>
                                  <tr>
                                    <td id="demo1" valign="top"><table width="140" height="55"  border="0" cellpadding="0" cellspacing="0">
                                        <tr>
										
										
										<%
										  CxGame.DbConn("QPGameUserDB")
										  set rs=conn.execute("select * from AccountsInfo where C_pic is not null order by userid desc")
										  if not rs.eof then
										  do while not rs.eof
										%>
										
                                          <td height="90" align="center"><div align="center">
                                            <table width="136" border="0" cellspacing="0" cellpadding="0">
                                              <tr>
                                                <td colspan="3"><img src="images/kuang_top.jpg" width="136" height="8"></td>
                                              </tr>
                                              <tr>
                                                <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                                                <td width="116" align="center"><img src="<%=rs("c_pic")%>" height="93" width="116"></td>
                                                <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                                              </tr>
                                              <tr>
                                                <td height="5" colspan="3"><img src="images/kuang_down.jpg" width="136" height="5"></td>
                                              </tr>
                                            </table>
                                          </div></td>
										  
										<%
										  rs.movenext
										  loop
										  end if
										  rs.close
										%> 
										  
										  
                                        </tr>
                                    </table></td>
                                    <td id="demo2" valign="top"></td>
                                  </tr>
                                </tbody>
                              </table>
                            </div>
                            <script>
var speed3=25//速度数值越大速度越慢
demo2.innerHTML=demo1.innerHTML
function Marquee(){
if(demo2.offsetWidth-demo.scrollLeft<=0)
demo.scrollLeft-=demo1.offsetWidth
else{
demo.scrollLeft++
}
}
var MyMar=setInterval(Marquee,speed3)
demo.onmouseover=function() {clearInterval(MyMar)}
demo.onmouseout=function() {MyMar=setInterval(Marquee,speed3)}
          </script>
            </td>
          </tr>
          <tr>
            <td><img src="d_images/11.jpg" width="731" height="13"></td>
          </tr>
        </table></td>
        </tr>
      <tr>
        <td height="12" colspan="4" align="left" valign="top" bgcolor="#56CCE6"><img src="images/bk_d.jpg" width="765" height="19"></td>
      </tr>
    </table>
      <table width="761" height="8" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
          <td></td>
        </tr>
      </table>
      <table width="761" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
          <td><img src="images/huo_d.jpg" width="380" height="31"></td>
          <td><img src="images/saishi.jpg" width="381" height="31"></td>
        </tr>
      </table>
      <table width="761" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#8BE0F7">
        <tr>
          <td width="380" height="102" align="right"><table width="366" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td><img src="images/h_d.jpg" width="366" height="18"></td>
              </tr>
              <tr>
                <td height="78" align="center" class="h_bj"><table width="330" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td width="102" align="center"><table width="92" border="0" cellpadding="0" cellspacing="2" bgcolor="#A8B3B7">
                          <tr>
                            <td align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_03``11.jpg" width="92" height="92"></td>
                          </tr>
                      </table></td>
                      <td width="228" align="center" valign="top"><table width="220" border="0" cellspacing="0" cellpadding="0">
					  <%CxGame.DbConn("News")%>
                          <%
					  set rs=conn.execute("select top 4 newstitle,id,classcode,newsdate from news where classcode in(select classcode from class where classname='活动') order by newsdate desc")
					  if not rs.eof then
					  do while not rs.eof
					%>
                          <tr>
                            <td width="31" height="24" align="center"><img src="images/xing.jpg" width="11" height="12"></td>
                            <td width="189" align="left"><a href="Show.asp?id=<%=rs(1)%>" target="_blank" class="ju_link"><%=left(rs(0),15)%></a></td>
                          </tr>
                          <%
					  rs.movenext
					  loop
					  end if
					  rs.close
					%>
                      </table></td>
                    </tr>
                </table></td>
              </tr>
              <tr>
                <td><img src="images/h_t.jpg" width="366" height="18"></td>
              </tr>
          </table></td>
          <td width="380"><table width="366" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td><img src="images/h_d.jpg" width="366" height="18"></td>
              </tr>
              <tr>
                <td height="94" align="center" class="h_bj"><table width="330" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td width="102" align="center"><table width="92" border="0" cellpadding="0" cellspacing="2" bgcolor="#A8B3B7">
                          <tr>
                            <td align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_03``11.jpg" width="92" height="92"></td>
                          </tr>
                      </table></td>
                      <td width="228" align="center" valign="top"><table width="220" border="0" cellspacing="0" cellpadding="0">
                          <%
					  set rs=conn.execute("select top 4 * from C_content order by C_id desc")
					  if not rs.eof then
					  do while not rs.eof
					%>
                          <tr>
                            <td width="31" height="24" align="center"><img src="images/xing.jpg" width="11" height="12"></td>
                            <td width="189" align="left"><a href="Contentlist.asp" class="ju_link" target="_blank"><%=left(rs("C_title"),15)%></a></td>
                          </tr>
                          <%
					  rs.movenext
					  loop
					  end if
					  rs.close
					%>
                      </table></td>
                    </tr>
                </table></td>
              </tr>
              <tr>
                <td><img src="images/h_t.jpg" width="366" height="18"></td>
              </tr>
          </table></td>
        </tr>
      </table>
      <table width="761" height="8" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#8BE0F7">
        <tr>
          <td></td>
        </tr>
      </table>
      <table width="761" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#8BE0F7">
        <tr>
          <td><table width="735" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td><img src="images/g_top.jpg" width="735" height="14"></td>
              </tr>
              <tr>
                <td height="90" bgcolor="E4E9FC"><table width="720" border="0" align="center" cellpadding="0" cellspacing="0">
                    <tr align="center">
                      <td width="180" height="61"><table width="172" border="0" cellspacing="0" cellpadding="0">
                          <tr>
                            <td><img src="images/l_top.jpg" width="172" height="4"></td>
                          </tr>
                          <tr>
                            <td height="55" align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_.jpg" width="165" height="70"></td>
                          </tr>
                          <tr>
                            <td height="4"><img src="images/l_d.jpg" width="172" height="3"></td>
                          </tr>
                      </table></td>
                      <td width="180"><table width="172" border="0" cellspacing="0" cellpadding="0">
                          <tr>
                            <td><img src="images/l_top.jpg" width="172" height="4"></td>
                          </tr>
                          <tr>
                            <td height="55" align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_.jpg" width="165" height="70"></td>
                          </tr>
                          <tr>
                            <td height="4"><img src="images/l_d.jpg" width="172" height="3"></td>
                          </tr>
                      </table></td>
                      <td width="180"><table width="172" border="0" cellspacing="0" cellpadding="0">
                          <tr>
                            <td><img src="images/l_top.jpg" width="172" height="4"></td>
                          </tr>
                          <tr>
                            <td height="55" align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_.jpg" width="165" height="70"></td>
                          </tr>
                          <tr>
                            <td height="4"><img src="images/l_d.jpg" width="172" height="3"></td>
                          </tr>
                      </table></td>
                      <td width="180"><table width="172" border="0" cellspacing="0" cellpadding="0">
                          <tr>
                            <td><img src="images/l_top.jpg" width="172" height="4"></td>
                          </tr>
                          <tr>
                            <td height="55" align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_.jpg" width="165" height="70"></td>
                          </tr>
                          <tr>
                            <td height="4"><img src="images/l_d.jpg" width="172" height="3"></td>
                          </tr>
                      </table></td>
                    </tr>
                </table></td>
              </tr>
              <tr>
                <td><img src="images/g_down.jpg" width="735" height="14"></td>
              </tr>
          </table></td>
        </tr>
      </table>
      <table width="761" height="8" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#8BE0F7">
        <tr>
          <td></td>
        </tr>
      </table>
      <table width="761" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#8BE0F7">
        <tr align="center">
          <td height="188"><table width="367" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td colspan="2"><img src="images/wanjia.jpg" width="367" height="45"></td>
              </tr>
              <tr>
                <td width="345" align="right" bgcolor="#FFFFFF" class="left_b"><table width="330" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td width="102" align="center"><table width="92" border="0" cellpadding="0" cellspacing="2" bgcolor="#A8B3B7">
                          <tr>
                            <td align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_03``11.jpg" width="92" height="92"></td>
                          </tr>
                      </table></td>
                      <td width="228" align="center" valign="top"><!--#include file="inc/lun_conn.asp"-->
                          <table width="220" border="0" cellspacing="0" cellpadding="0">
                            <%
			  dim sql
			  sql="select top 4 title,Topicid,Boardid,dateandtime from Dv_Topic order by LastSmsTime desc"
			  rs.open sql,conn,1,1
			  if not rs.eof then
			  do while not rs.eof
			  %>
                            <tr>
                              <td width="31" height="24" align="center"><img src="images/xing.jpg" width="11" height="12"></td>
                              <td width="189" align="left"><a class="hui_link" href="bbs/dispbbs.asp?boardid=<%=rs(2)%>&Id=<%=rs(1)%>" target="_blank"><%=left(rs(0),15)%></a></td>
                            </tr>
                            <%
					  rs.movenext
					  loop
					  end if
					  rs.close
					%>
                        </table></td>
                    </tr>
                </table></td>
                <td width="22"><img src="images/w_bian.jpg" width="22" height="113"></td>
              </tr>
              <tr>
                <td height="27" colspan="2"><img src="images/wan_d.jpg" width="367" height="27"></td>
              </tr>
          </table></td>
          <td><table width="367" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td colspan="2"><img src="images/youxi_st.jpg" width="367" height="45"></td>
              </tr>
              <tr>
                <td width="345" align="right" bgcolor="#FFFFFF" class="left_b"><table width="330" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td width="102" align="center"><table width="92" border="0" cellpadding="0" cellspacing="2" bgcolor="#A8B3B7">
                          <tr>
                            <td align="center" bgcolor="#FFFFFF"><img src="bookpic/DC173_03``11.jpg" width="92" height="92"></td>
                          </tr>
                      </table></td>
                      <td width="228" align="center" valign="top"><table width="220" border="0" cellspacing="0" cellpadding="0">
                          <%CxGame.DbConn("News")%>
                          <%
				  sql="select * from Tuandui where T_type=1 order by T_id desc"
				  rs.open sql,conn,1,1
				  if not rs.eof then
				  do while not rs.eof
				%>
                          <tr>
                            <td width="31" height="24" align="center"><img src="images/xing.jpg" width="11" height="12"></td>
                            <td width="189" align="left"><a href="Libiao.asp?id=<%=rs("T_id")%>" class="hui_link"><%=left(rs("T_name"),15)%></a></td>
                          </tr>
                          <%
				  rs.movenext
				  loop
				  end if
				  rs.close
				%>
                      </table></td>
                    </tr>
                </table></td>
                <td width="22"><img src="images/w_bian.jpg" width="22" height="113"></td>
              </tr>
              <tr>
                <td colspan="2"><img src="images/wan_d.jpg" width="367" height="27"></td>
              </tr>
          </table></td>
        </tr>
      </table>
      <table width="761" height="131" border="0" align="center" cellpadding="0" cellspacing="0">
        <tr>
          <td height="131" bgcolor="#88E0F4"><table width="754" height="129" border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td width="18" rowspan="3" bgcolor="#A8E6F5"><img src="d_images/12.jpg" width="18" height="129"></td>
              <td width="718" height="5" background="d_images/14.jpg"><img src="d_images/14.jpg" width="699" height="5"></td>
              <td width="18" rowspan="3" bgcolor="#8EE0F8"><img src="d_images/13.jpg" width="18" height="129"></td>
            </tr>
            <tr>
              <td height="119" bgcolor="#FFFFFF"><table width="718" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="47%"><table width="45%" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td width="48%" align="center"><table width="100" border="0" cellspacing="0" cellpadding="0">
                          <tr>
                            <td><img src="images/g_t.jpg" width="154" height="4"></td>
                          </tr>
                          <tr>
                            <td height="63" align="center" class="s_xian"><img src="bookpic/DC173_03.jpg" width="143" height="87"></td>
                          </tr>
                          <tr>
                            <td><img src="images/g_d.jpg" width="154" height="4"></td>
                          </tr>
                      </table></td>
                      <td width="52%" align="center"><table width="100" border="0" cellspacing="0" cellpadding="0">
                          <tr>
                            <td><img src="images/g_t.jpg" width="154" height="4"></td>
                          </tr>
                          <tr>
                            <td height="63" align="center" class="s_xian"><img src="bookpic/DC173_03.jpg" width="143" height="87"></td>
                          </tr>
                          <tr>
                            <td><img src="images/g_d.jpg" width="154" height="4"></td>
                          </tr>
                      </table></td>
                    </tr>
                  </table></td>
                  <td width="53%"><table width="380"  border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td><table width="100" border="0" align="left" cellpadding="0" cellspacing="0">
                          <tr>
                            <td height="4"></td>
                          </tr>
                          <tr>
                            <td align="left" class="k_16">&nbsp;<a href="New.html" target="_blank" class="k_16"> 新手指南</a></td>
                          </tr>
                          <tr>
                            <td height="2" bgcolor="56C7FF"></td>
                          </tr>
                      </table></td>
                    </tr>
                  </table>
                    <table width="380" height="1" border="0" cellpadding="0" cellspacing="0" bgcolor="56C7FF">
                      <tr>
                        <td></td>
                      </tr>
                    </table>
                    <table width="380" height="6" border="0" cellpadding="0" cellspacing="0">
                      <tr>
                        <td></td>
                      </tr>
                    </table>
                    <table width="400" border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td height="20" align="left">·<a href="Question.asp" target="_blank" class="lan_link">有关多彩游戏系统信息。</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ·<a href="Question.asp" target="_blank" class="lan_link">请问我想充值怎么购买点卡？</a></td>
                      </tr>
                      <tr>
                        <td height="20" align="left">·<a href="Question.asp" target="_blank" class="lan_link">如何才能防止作弊呢？</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ·<a href="Question.asp" target="_blank" class="lan_link">请问我在家想玩游戏应该怎么做？</a></td>
                      </tr>
                      <tr>
                        <td height="20" align="left">·<a href="Question.asp" target="_blank" class="lan_link">有关充值的问题。</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ·<a href="Question.asp" target="_blank" class="lan_link">请问我的密码忘了应该怎么办？</a> </td>
                      </tr>
                      <tr>
                        <td height="20" align="left">·<a href="Question.asp" target="_blank" class="lan_link">密码输入不正确应该怎么办？</a>&nbsp;&nbsp; ·<a href="Question.asp" target="_blank" class="lan_link">可以在游戏中加好友么？</a></td>
                      </tr>
                    </table></td>
                </tr>
              </table></td>
            </tr>
            <tr>
              <td height="5" background="d_images/15.jpg"><img src="d_images/15.jpg" width="699" height="5"></td>
            </tr>
          </table></td>
        </tr>
      </table>
      <table width="761" height="8" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#8BE0F7">
        <tr>
          <td><img src="images/di_bian.jpg" width="761" height="9"></td>
        </tr>
      </table></td>
    <td align="center" valign="top"><table width="209" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td align="center" background="images/left_bj.jpg "><table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
          <tr>
            <td></td>
          </tr>
        </table>
          <table width="188" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td height="1" background="images/heng.jpg"><img src="images/heng.jpg" width="6" height="1" /></td>
            </tr>
          </table>
          <table width="188" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/gouka.jpg" width="190" height="39" /></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td colspan="3"><img src="images/11.jpg" width="190" height="7" /></td>
            </tr>
            <tr>
              <td width="4"><img src="images/13.jpg" width="4" height="111" /></td>
              <td width="182" background="images/13_bj.jpg"><table width="180" border="0" cellspacing="0" cellpadding="0">
                  <tr class="bj_niu">
                    <td width="90" height="30" align="left" background="images/k_1.gif">&nbsp;&nbsp;&nbsp;<a href="Paybuy.asp" class="bai_link">网上银行</a> </td>
                    <td width="90" height="30" align="left" background="images/k_1.gif">&nbsp;&nbsp; <a href="Paybuy.asp" class="bai_link">多彩购买</a> </td>
                  </tr>
                  <tr class="bj_niu">
                    <td width="90" height="30" align="left" background="images/k_2.gif">&nbsp;&nbsp; <a href="Paybuy.asp" class="bai_link">电话充值</a></td>
                    <td width="90" height="30" align="left" background="images/k_2.gif">&nbsp;&nbsp; <a href="Paybuy.asp" class="bai_link">银行汇款</a></td>
                  </tr>
                  <tr>
                    <td width="90" height="30" align="left" class="bj_niu">&nbsp;&nbsp; <a href="Paybuy.asp" class="bai_link">网吧购买</a></td>
                    <td width="90" height="30" align="left" class="bj_shen">&nbsp; <a class="bai_link" href="Paybuy.asp">神州行购买</a> </td>
                  </tr>
              </table></td>
              <td width="4"><img src="images/13.jpg" width="4" height="111" /></td>
            </tr>
            <tr>
              <td height="6" colspan="3"><img src="images/12.jpg" width="190" height="6" /></td>
            </tr>
          </table>
          <table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="188" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/kehu.jpg" width="190" height="34" /></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td colspan="3"><img src="images/11.jpg" width="190" height="7" /></td>
            </tr>
            <tr>
              <td width="4"><img src="images/14.jpg" width="4" height="120" /></td>
              <td width="182" background="images/14_bj.jpg"><table width="170" border="0" align="center" cellpadding="0" cellspacing="0">
                <tr>
                  <td width="23" height="24" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                  <td width="66" height="24" align="center" class="blue">答疑客服：</td>
                  <td width="81" height="24" align="center"><a target="blank" href="http://wpa.qq.com/msgrd?V=1&amp;Uin=200160&amp;Site=qq" msg&menu="yes"><img src="images/qq.jpg" alt="点击这里给我发消息" width="74" height="23" border="0" /></a></td>
                </tr>
                <tr>
                  <td height="24" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                  <td height="24" align="center" class="blue">充值客服：</td>
                  <td height="24" align="center"><a target="blank" href="http://wpa.qq.com/msgrd?V=1&amp;Uin=200160&amp;Site=qq" msg&menu="yes"><img src="images/qq.jpg" alt="点击这里给我发消息" width="74" height="23" border="0" /></a></td>
                </tr>
                <tr>
                  <td height="24" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                  <td height="24" align="center" class="blue">商务客服：</td>
                  <td height="24" align="center"><a target="blank" href="http://wpa.qq.com/msgrd?V=1&amp;Uin=200160&amp;Site=qq" msg&menu="yes"><img src="images/qq.jpg" alt="点击这里给我发消息" width="74" height="23" border="0" /></a></td>
                </tr>
                <tr>
                  <td height="24" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                  <td height="24" align="center" class="blue">客服电话：</td>
                  <td height="24" align="center" class="blue">***********</td>
                </tr>
                <tr>
                  <td height="24" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                  <td height="24" align="center" class="blue">答疑时间：</td>
                  <td height="24" align="center" class="blue">9:00--17:50</td>
                </tr>
              </table></td>
              <td width="4"><img src="images/14.jpg" width="4" height="120" /></td>
            </tr>
            <tr>
              <td height="6" colspan="3"><img src="images/12.jpg" width="190" height="6" /></td>
            </tr>
          </table>
          <table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="180" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td colspan="3"><img src="images/paihang.jpg" width="189" height="40" /></td>
            </tr>
            <tr>
              <td width="5"><img src="images/15.jpg" width="5" height="49" /></td>
              <td width="170" align="center" valign="middle" background="images/15_b.jpg"><table width="170" border="0" cellspacing="0" cellpadding="0">
                  <tr>
                    <td width="24" height="24" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                    <td width="60" height="22" align="center"><a href="CaiFu.asp" class="hui_link">财富排行</a></td>
                    <td width="26" height="22" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                    <td width="60" height="22" align="center"><a href="help.asp" class="hui_link">魅力排行</a></td>
                  </tr>
                  <tr>
                    <td height="22" align="center"><img src="images/k_5.gif" width="7" height="7" /></td>
                    <td height="22" align="center"><a href="PaiHang.asp" class="hui_link">游戏排行</a></td>
                    <td height="22" align="center">&nbsp;</td>
                    <td height="22" align="center">&nbsp;</td>
                  </tr>
              </table></td>
              <td width="5"><img src="images/15.jpg" width="5" height="49" /></td>
            </tr>
            <tr>
              <td colspan="3"><img src="images/16.jpg" width="189" height="7" /></td>
            </tr>
          </table>
          <table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="188" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td height="1" background="images/heng.jpg"><img src="images/heng.jpg" width="6" height="1" /></td>
            </tr>
          </table>
          <table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><a href="help.asp"><img src="images/meili.jpg" width="188" height="42" border="0" /></a></td>
            </tr>
            <tr>
              <td><a href="CaiFu.asp"><img src="images/caifu.jpg" width="188" height="40" border="0" /></a></td>
            </tr>
            <tr>
              <td><a href="PaiHang.asp"><img src="images/youxi.jpg" width="188" height="43" border="0" /></a></td>
            </tr>
          </table>
          <table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="188" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td height="1" background="images/heng.jpg"><img src="images/heng.jpg" width="6" height="1" /></td>
            </tr>
          </table>
          <table width="188" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><a href="PayBuy.asp"><img src="images/vip.jpg" width="188" height="67" border="0" /></a></td>
            </tr>
          </table>
          <table width="190" height="2" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><a href="user/default.asp"><img src="images/yonghu.jpg" width="188" height="69" border="0" /></a></td>
            </tr>
          </table>
          <table width="190" height="2" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><a href="Tui.asp"><img src="images/tuiguang.jpg" width="188" height="71" border="0" /></a></td>
            </tr>
          </table>
          <table width="190" height="2" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="190" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/shangwu.jpg" width="188" height="73" /></td>
            </tr>
          </table>
          <table width="190" height="8" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td></td>
            </tr>
          </table>
          <table width="188" border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td height="1" background="images/heng.jpg"><img src="images/heng.jpg" width="6" height="1" /></td>
            </tr>
          </table>
          <table width="190" border="0" cellpadding="0" cellspacing="0" bgcolor="#56CCE6">
            <tr>
              <td height="11" bgcolor="#56CCE6"><img src="images/di.jpg" width="209" height="11" /></td>
            </tr>
          </table></td>
      </tr>
    </table>
      <table width="100%" height="4" border="0" cellpadding="0" cellspacing="0">
        <tr>
          <td></td>
        </tr>
      </table>
      <table width="210" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td width="214"><img src="d_images/16.jpg" width="208" height="40"></td>
        </tr>
        <tr>
          <td height="130" align="center" background="images/zhong_b.jpg"><table width="180" border="0" cellspacing="0" cellpadding="0">
              <tr>
                <td height="24" class="hui">抵制不良游戏，拒绝盗版游戏。</td>
              </tr>
              <tr>
                <td height="24" class="hui">注意自我保护，谨防上当受骗。</td>
              </tr>
              <tr>
                <td height="24" class="hui">适当游戏益脑，沉迷游戏伤身。</td>
              </tr>
              <tr>
                <td height="24" class="hui">合理安排时间，享受健康生活。</td>
              </tr>
          </table></td>
        </tr>
        <tr>
          <td><img src="d_images/17.jpg" width="208" height="7"></td>
        </tr>
      </table></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>

  <param name="movie" value="images/music.swf" />
  <param name="quality" value="high" />
  <embed src="images/music.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="1" height="1"></embed>
</object>
</body>
</html>
    