<%
    if request.Cookies("admin99aw")="" then
	response.Write "<script language='javascript'>alert('网络超时或您还没有登陆！');window.top.location.href='admin_login.asp';</script>"
	response.End
	else
	if request.Cookies("flag")<>"超级管理员" then
	response.Write "<p align=center><font color=red>您没有此项目管理权限！</font></p>"
	response.End
	end if
	end if
%>
<!--#include file="../inc/conn.asp"-->
<%
  set rs=server.CreateObject("adodb.recordset")
  dim action,T_name
  action=replace(trim(request.QueryString("action")),"'","")
  T_name=replace(trim(request.form("T_name")),"'","")
  id=replace(trim(request.QueryString("id")),"'","")
  select case action
  
  case "add"   
      sql="insert into C_type (T_name) values ('"& T_name &"')"
      conn.execute(sql)
	  response.Write("<script language='javascript'>alert('添加成功');location.href='Type.asp';</script>")
  case "up"
      sql="update C_type set T_name='"& T_name &"' where id="&id
	  conn.execute(sql)
	  response.Write("<script language='javascript'>alert('更新成功');location.href='Type.asp';</script>")
  case "del"
      sql="delete from C_type where id="&id
	  conn.execute(sql)
	  response.Write("<script language='javascript'>alert('删除成功');location.href='Type.asp';</script>")
  end select
  
%>
<HTML><HEAD><TITLE></TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link href="images/style.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
.STYLE4 {
	color: red;
	font-weight: bold;
}
.STYLE5 {color: red}
-->
</style></HEAD>
<BODY>
<table class="tableBorder" width="90%" border="0" align="center" cellpadding="3" cellspacing="1" bgcolor="#FFFFFF">
<tr> 
<td align="center" background="../images/admin_bg_1.gif" class="forumRowHighlight STYLE4">奖品兑换类别</td>
</tr>
<tr> 
<td class="forumRowHighlight" align="center" ><br>
  <table width="530" border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td height="24" colspan="3" align="center"><span class="STYLE5">类别添加</span></td>
    </tr>
	<form action="?action=add" name="myform" method="post" onSubmit="return place()">
  <tr>
    <td width="120" height="24" align="center">类别名称：</td>
    <td width="218" height="24"><input type="text" name="T_name" class="text_t"></td>
    <td width="192" height="24"><input type="submit" name="button1" class="Button" value="确定添加"></td>
  </tr>
    </form>
</table>
  <br>
  <br>
  <table width="530" border="0" cellpadding="0" cellspacing="1" bgcolor="#BACAEF">
    <tr>
      <td height="24" colspan="4" align="center" bgcolor="#FFFFFF"><span class="STYLE5">产品类别</span></td>
      </tr>
	  
	 <%
	   sql="select T_name,id from C_type"
	   rs.open sql,conn,1,1
	   if not rs.eof then
	 %> 
	 <%do while not rs.eof%>
	 <form action="?action=up&id=<%=rs(1)%>" name="theform" method="post">
    <tr>
      <td width="120" height="24" align="center" bgcolor="#FFFFFF">类别名称：</td>
      <td width="220" height="24" bgcolor="#FFFFFF"><input name="T_name" type="text" class="text_t" id="T_name" value="<%=rs(0)%>"></td>
      <td width="94" height="24" align="center" bgcolor="#FFFFFF"><input type="submit" name="button12" class="Button" value="更    改"></td>
      <td width="96" height="24" align="center" bgcolor="#FFFFFF"><input onClick="return del(<%=rs(1)%>)" type="button" name="button13" class="Button" value="删    除"></td>
    </tr>
	</form>
	 <%
	   rs.movenext
	   loop
	   end if
	   rs.close
	 %>
  </table>
  <br></td>
  </tr>
</table>
<script language="javascript">
function place(){
if(myform.T_name.value==""){
alert("请填写好类别名称！");
myform.T_name.focus();
return (false);
}
}
function del(id){
if(confirm("你真的要删除？"))
location.replace("type.asp?id="+id+"&action="+"del")
else
alert("请小心操作！");
}
</script>
</BODY>
</HTML>