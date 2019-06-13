 <%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
response.write "<script language='javascript'>"
response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
response.end
end if
%>
<!--#include file="../inc/conn.asp"-->
<HTML><head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title></title>
<LINK href="images/Admin_left.CSS" rel=stylesheet type=text/css>

<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style>
<link href="images/style11.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
.STYLE1 {color: red}
-->
</style>
</HEAD>

<body>
<br>
<br>
<table width=180 border='0' align=center cellpadding=0 cellspacing=0>
  <tr>
    <td height=44 valign=top><img src='images/title.gif'></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=180 align=center>
  <tr>
    <td height=26 class=menu_title onMouseOver="this.className='menu_title2';" onMouseOut="this.className='menu_title';" background='images/title_bg_quit.gif' id='menuTitle0'>&nbsp;&nbsp;<A  href="aspcheck.asp" target=main><b><span class='glow'>管理首页</span></b></a><span class='glow'> | </span><a href='exit.asp' target='_top'><b><span class='glow'>退出</span></b></a> </td>
  </tr>
  <tr>
    <td height=97 valign="middle" background='images/title_bg_admin.gif' id='submenu0' style='display:'>
	<table align="center" cellpadding="5" cellspacing="0" > 
  <tr>
        <td align="center">欢迎使用后台管理系统 
		</td>
  </tr>
      <tr>
        <td style="word-break:break-all ">您的用户名：<span style="color:red; "><%=request.Cookies("admin99aw")%></span> </td>
      </tr>
</table></td>
  </tr>
</table>


<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/Admin_left_12.gif' id=menuTitle402  style='cursor:hand;'><span>管理员管理</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu801'><div class=sec_menu style='width:165'>
      <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
        <TBODY>
          <TR>
            <TD height=5 colspan="2"></TD>
          </TR>
          <TR>
            <TD width="16" height=20 valign="middle"><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD width="132" valign="middle"><a href="Manageadmin.asp" target="main">管理员管理</a></TD>
          </TR>
        <TBODY>
        </TBODY>
      </TABLE>
    </div>
        <div  style='width:167'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
        </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/Admin_left_7.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>平台动态</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
        <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
              <TBODY>
              <TR>
                <TD height=5 colspan="2"></TD></TR>
              <TR>
                <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
                <TD width="133"><a href="classedit.asp" target="main">网站分类管理</a></TD>
              </TR>
              <TR>
                <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
                <TD><a href="newsadd.asp" target="main">新增文章信息</a></TD>
              </TR>
              <TR>
                <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
                <TD><a href="newsedit.asp" target="main">文章信息管理</a></TD>
              </TR>
              <TR>
                <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
                <TD><a href="../htmledit/Admin_UploadFile.asp" target="main">管理上传图片</a></TD>
              </TR>
              <TR>
                <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
                <TD><a href="../Admin_UploadFile.asp" target="main">管理照片</a></TD>
              </TR>
              <TBODY></TBODY></TABLE>
      </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
      </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/admin_left_1.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>比赛信息</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
      <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
        <TBODY>
          <TR>
            <TD height=5 colspan="2"></TD>
          </TR>
          <TR>
            <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD width="133"><a href="Content.asp" target="main">信息发布</a></TD>
          </TR>
          <TR>
            <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD><a href="edit_content.asp" target="main">信息管理</a></TD>
          </TR>
          <TR>
            <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD><a href="jinbi.asp" target="main">比赛金币</a></TD>
          </TR>
        <TBODY>
        </TBODY>
      </TABLE>
    </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
        </div></td>
  </tr>
</table>

<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/admin_left_1.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>游戏兑奖</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
        <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
          <TBODY>
            <TR>
              <TD height=5 colspan="2"></TD>
            </TR>
            <TR>
              <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
              <TD width="133"><a href="Type.asp" target="main">奖品类别</a></TD>
            </TR>
            <TR>
              <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
              <TD><a href="Product.asp" target="main">奖品发布</a></TD>
            </TR>
            <TR>
              <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
              <TD><a href="People.asp" target="main">兑换会员</a></TD>
            </TR>
          <TBODY>
          </TBODY>
        </TABLE>
      </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
      </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/admin_left_1.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>网站首页广告</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
      <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
        <TBODY>
          <TR>
            <TD height=5 colspan="2"></TD>
          </TR>
          <TR>
            <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD width="133"><a href="Guanggao.asp" target="main">广告管理</a></TD>
          </TR>
        <TBODY>
        </TBODY>
      </TABLE>
    </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
        </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/admin_left_1.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>多彩家族管理</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
      <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
        <TBODY>
          <TR>
            <TD height=5 colspan="2"></TD>
          </TR>
          <TR>
            <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD width="133"><a href="dating.asp" target="main">家族管理</a></TD>
          </TR>
        <TBODY>
        </TBODY>
      </TABLE>
    </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
        </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/admin_left_1.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>大厅图片</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
      <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
        <TBODY>
          <TR>
            <TD height=5 colspan="2"></TD>
          </TR>
          <TR>
            <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD width="133"><a href="tupian.asp" target="main">图片管理</a></TD>
          </TR>
        <TBODY>
        </TBODY>
      </TABLE>
    </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
        </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
  <tr>
    <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/admin_left_1.gif' id=menuTitle502  style='cursor:hand;'><span class=glow>申诉用户</span></td>
  </tr>
  <tr>
    <td align='right' id='submenu502'><div class=sec_menu style='width:165'>
      <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
        <TBODY>
          <TR>
            <TD height=5 colspan="2"></TD>
          </TR>
          <TR>
            <TD width="15" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
            <TD width="133"><a href="user.asp" target="main">用户管理</a></TD>
          </TR>
        <TBODY>
        </TBODY>
      </TABLE>
    </div>
        <div  style='width:158'>
          <table cellpadding=0 cellspacing=0 align=center width=130>
            <tr>
              <td height=5></td>
            </tr>
          </table>
        </div></td>
  </tr>
</table>
<table cellpadding=0 cellspacing=0 width=167 align=center>
   <tr>
     <td height=28 class=menu_title onmouseover=this.className='menu_title2'; onmouseout=this.className='menu_title'; background='images/Admin_left_04.gif' id=menuTitle301  style='cursor:hand;'><span class=glow>系统信息</span></td>
   </tr>
   <tr>
     <td align='right' id='submenu301'><div class=sec_menu style='width:165'>
         <TABLE cellSpacing=0 cellPadding=0 width=150 align=center>
           <TBODY>
             <TR>
               <TD height=5 colspan="2"></TD>
             </TR>
             <TR>
               <TD width="18" height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
               <TD width="130"><span class="STYLE1">版权：多彩网络科技</span></TD>
             </TR>
             <TR>
               <TD height=20><IMG height=20 alt="" 
                  src="images/bullet.gif" width=15 border=0></TD>
               <TD height=20><font color="red">设计：<span class="STYLE1">多彩网络</span>技术部</font></TD>
             </TR>
           <TBODY>
           </TBODY>
         </TABLE>
     </div>
         <div  style='width:167'>
           <table cellpadding=0 cellspacing=0 align=center width=130>
             <tr>
               <td height=5></td>
             </tr>
           </table>
         </div></td>
   </tr>
</table>
</body></HTML>
 