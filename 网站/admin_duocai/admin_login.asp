<HTML><HEAD><TITLE>Untitled Document</TITLE>
<META http-equiv=Content-Type content="text/html; chaRset=gb2312"><LINK 
href="images/Style.css" type=text/css rel=stylesheet>
<META content="MSHTML 6.00.2800.1522" name=GENERATOR></HEAD>
<BODY leftMargin=0 topMargin=0 rightMargin=0>
<DIV align=center><BR>
<P>&nbsp;</P>
<P>&nbsp;</P>
<form name="admininfo" method="post" action="chkadmin.asp" onSubmit="return check();">
<TABLE 
style="BORDER-RIGHT: 3px outset; BORDER-TOP: 3px outset; BORDER-LEFT: 3px outset; WIDTH: 0px; BORDER-BOTTOM: 3px outset" 
cellSpacing=0 cellPadding=1 align=center border=0>
  <TBODY>
  <TR>
    <TD>
      <TABLE cellSpacing=0 cellPadding=0 width=605 align=center border=0>
        <TBODY>
        <TR>
          <TD><IMG height=17 src="images/admin_login_r1_c1.jpg" 
          width=605></TD></TR></TBODY></TABLE>
      <TABLE cellSpacing=0 cellPadding=0 width=605 align=center border=0>
        <TBODY>
        <TR>
          <TD width=24><IMG height=212 src="images/admin_login_r2_c1.jpg" 
            width=23></TD>
          <TD vAlign=top align=middle width=494 bgColor=#f7faff>
            <TABLE height=100 cellSpacing=0 cellPadding=0 width="100%" 
              border=0><TBODY>
              <TR>
                <TD background=images/admin_login_r2_c2.jpg><IMG height=101 
                   src="images/admin_login_r2_c2.jpg" 
                  width=494 border=0></TD>
              </TR></TBODY></TABLE>
            <TABLE cellSpacing=5 cellPadding=0 width="100%" align=center 
            border=0>
              <TBODY>
              <TR>
                          <TD align=middle width="22%" rowSpan=4>&nbsp;</TD>
                <TD width="78%">用户名：<INPUT 
                  onmouseover="this.style.background='#FFFFFF';" 
                  onfocus=this.select(); 
                  onmouseout="this.style.background='#F8F8F8'" size=28 
                  name=admin></TD></TR>
              <TR>
                <TD>密　码：<INPUT onMouseOver="this.style.background='#FFFFFF';" 
                  onfocus=this.select(); 
                  onmouseout="this.style.background='#F8F8F8'" type=password 
                  size=28 name=password></TD></TR>
              <TR>
                          <TD>附加码：<INPUT name=verifycode 
                  onfocus="this.select(); " onMouseOver="this.style.background='#FFFFFF';" 
                  onmouseout="this.style.background='#F8F8F8'" value="<%If GetCode=9999 Then Response.Write "9999"%>" 
                  size=10 maxLength=5>请在附加码框输入 <img src=GetCode.asp> </TD></TR>
              <TR>
                <TD><INPUT class=Button onclick=window.location.reload() type=button value=刷新本页 name=Submit4> 
                  　<INPUT class=Button onClick="javascript:location.href='../'" type=button value=返回首页 name=Submit1>　 
<INPUT class=Button type=submit value=管理登陆 name=Submit></TD></TR></TBODY></TABLE></TD>
          <TD width=88><IMG height=212 src="images/admin_login_r2_c3.jpg" 
            width=88></TD></TR></TBODY></TABLE>
      <TABLE cellSpacing=0 cellPadding=0 width=605 align=center border=0>
        <TBODY>
        <TR>
          <TD><IMG height=21 src="images/admin_login_r4_c1.jpg" 
          width=605></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></FORM>

<P align=center></P></DIV></BODY></HTML>
<script language="javascript">
function check(){
  if(admininfo.admin.value==""){
  alert("请填写好用户名！");
  admininfo.admin.focus();
  return (false);
  }
  if(admininfo.password.value==""){
  alert("请填写好密码！");
  admininfo.password.focus();
  return (false);
  }
  if(admininfo.verifycode.value==""){
  alert("请填写好验证码！")
  admininfo.verifycode.focus();
  return (false);
  }
}
</script>
