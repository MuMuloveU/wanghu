<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<HTML><HEAD><TITLE></TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link href="images/style.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
.head {	display: none;
}
.style4 {color: red}
-->
</style></HEAD>
<BODY>
<table class="tableBorder" width="90%" border="0" align="center" cellpadding="3" cellspacing="1" bgcolor="#FFFFFF">
<form action="Add_c.asp" method="post" name=add id="add" onSubmit="return checkForm()">
<tr> 
<td class="forumRowHighlight" align="center" background="../images/admin_bg_1.gif"><b><font color="red">比赛信息发布</font></b></td>
</tr>
<tr> 
<td class="forumRowHighlight" align="center" ><table width="800" border="0" cellpadding="0" cellspacing="1" bgcolor="#575757">
  <tr bgcolor="#FFFFFF">
    <td width="148" height="30" align="center">信息标题：</td>
    <td colspan="3" align="left"> &nbsp;
      <input name="C_title" type="text" class="text_t" id="pic43" size="60"></td>
    </tr>
  <tr bgcolor="#FFFFFF">
    <td height="30" align="center">报名时间：</td>
    <td width="252" align="left">
	   &nbsp;
	   <select name="C_date">
	  <option value="1">---1天---</option>
	  <option value="2">---2天---</option>
	  <option value="3">---3天---</option>
	  <option value="7">--一周---</option>
	  <option value="15">--半个月--</option>
	  <option value="30">--一个月--</option>
	</select>	</td>
    <td width="148" align="center">比赛时间：</td>
    <td width="252" align="left"> &nbsp;
      <select name="C_over">
      <option value="1">---1天---</option>
      <option value="2">---2天---</option>
      <option value="3">---3天---</option>
      <option value="7">--一周---</option>
      <option value="15">--半个月--</option>
      <option value="30">--一个月--</option>
    </select>
      <span class="style4">从报名结束算</span></td>
  </tr>
  <tr align="left" bgcolor="#BACAEF">
    <td height="30" colspan="4">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 信息内容 <font color="#FF0000">*</font></td>
    </tr>
  <tr bgcolor="#FFFFFF">
    <td height="200" colspan="4"><IFRAME ID="eWebEditor1" src="../htmledit/ewebeditor.asp?id=newsinfo&style=s_light&savefilename=myText2" frameborder="0" scrolling="no" width="800" height="300"></IFRAME>
          <input name="newsinfo" type="hidden" id="newsinfo"></td>
    </tr>
  <tr align="center" bgcolor="#FFFFFF">
    <td height="30" colspan="4"><input type="submit" value="确 定" name="B1"  class=button>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="reset" value="重 写" name="B2"  class=button></td>
  </tr>
</table></td>
  </tr>
  </form>
</table>
</BODY>
</HTML>
<script language="JavaScript">
<!--
function checkForm()
{
	if (document.add.C_title.value=="") {
		alert("请输入文章标题.");
		document.add.C_title.focus();
		return false;
	}	
	return true
}
//-->
</script>
