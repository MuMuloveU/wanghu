<%
    if request.Cookies("admin99aw")="" then
	response.Write "<script language='javascript'>alert('网络超时或您还没有登陆！');window.location.href='admin_login.asp';</script>"
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
  dim S_id
  S_id=replace(trim(request("S_id")),"'","")
  if S_id="" then
	  response.Write("<script language='javascript'>alert('请选择你要删除的信息');history.go(-1);</script>")
	  response.End()
  end if
%>
<%
  sql="delete from Jiangpin where C_id in ("& S_id &")"
  conn.execute(sql)
  response.Redirect("Product.asp")
%>