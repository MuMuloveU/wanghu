<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%CxGame.DbConn("News")%>
<%
	  dim tupian1,tupian2,tupian3,tupian4,tupian5,tupian6
	  set rs=conn.execute("select * from wm_gg")
	  if not rs.eof then
	      tupian1=rs("tupian1")
		  tupian2=rs("tupian2")
		  tupian3=rs("tupian3")
		  tupian4=rs("tupian4")
		  tupian5=rs("tupian5")
		  tupian6=rs("tupian6")
	  end if
	  rs.close
	%><style type="text/css">
<!--
body {
	background-color: #88E0F8;
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
}
-->
</style>
<table width="360" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td><img src="images/x_top.jpg" width="360" height="10"></td>
  </tr>
  <tr>
    <td height="97" align="center" bgcolor="#D7F2F9"><div id="demo" style="OVERFLOW: hidden; WIDTH: 340; COLOR: #ffffff">
      <table cellspacing="0" cellpadding="0" align="left" border="0" cellspace="0">
        <tbody>
          <tr>
            <td width="140" valign="top" id="demo1"><table width="160" height="55"  border="0" cellpadding="0" cellspacing="0">
              <tr>
                
				
                <td width="160" height="90" align="center"><div align="center">
                  <table width="149" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td colspan="3"><img src="images/kuang_top.jpg" width="136" height="8"></td>
                    </tr>
                    <tr>
                      <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                      <td width="116" align="center"><img src="<%=tupian1%>" height="93" width="116"></td>
                      <td width="23"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                    </tr>
                    <tr>
                      <td height="5" colspan="3"><img src="images/kuang_down.jpg" width="136" height="5"></td>
                    </tr>
                  </table>
                </div></td>
				<td width="160" height="90" align="center"><div align="center">
                  <table width="149" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td colspan="3"><img src="images/kuang_top.jpg" width="136" height="8"></td>
                    </tr>
                    <tr>
                      <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                      <td width="116" align="center"><img src="<%=tupian2%>" height="93" width="116"></td>
                      <td width="23"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                    </tr>
                    <tr>
                      <td height="5" colspan="3"><img src="images/kuang_down.jpg" width="136" height="5"></td>
                    </tr>
                  </table>
                </div></td>
				<td width="160" height="90" align="center"><div align="center">
                  <table width="149" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td colspan="3"><img src="images/kuang_top.jpg" width="136" height="8"></td>
                    </tr>
                    <tr>
                      <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                      <td width="116" align="center"><img src="<%=tupian3%>" height="93" width="116"></td>
                      <td width="23"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                    </tr>
                    <tr>
                      <td height="5" colspan="3"><img src="images/kuang_down.jpg" width="136" height="5"></td>
                    </tr>
                  </table>
                </div></td>
				<td width="160" height="90" align="center"><div align="center">
                  <table width="149" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td colspan="3"><img src="images/kuang_top.jpg" width="136" height="8"></td>
                    </tr>
                    <tr>
                      <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                      <td width="116" align="center"><img src="<%=tupian4%>" height="93" width="116"></td>
                      <td width="23"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                    </tr>
                    <tr>
                      <td height="5" colspan="3"><img src="images/kuang_down.jpg" width="136" height="5"></td>
                    </tr>
                  </table>
                </div></td>
				<td width="160" height="90" align="center"><div align="center">
                  <table width="149" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                      <td colspan="3"><img src="images/kuang_top.jpg" width="136" height="8"></td>
                    </tr>
                    <tr>
                      <td width="10"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                      <td width="116" align="center"><img src="<%=tupian5%>" height="93" width="116"></td>
                      <td width="23"><img src="images/kuang_left.jpg" width="10" height="93"></td>
                    </tr>
                    <tr>
                      <td height="5" colspan="3"><img src="images/kuang_down.jpg" width="136" height="5"></td>
                    </tr>
                  </table>
                </div></td>

              </tr>
            </table></td>
            <td width="16" valign="top" id="demo2"></td>
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
    <td><img src="images/x_down.jpg" width="360" height="10"></td>
  </tr>
</table>
