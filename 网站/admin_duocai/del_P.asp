<%if request.Cookies("admin99aw")="" then
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
  dim id
  id=replace(trim(request.QueryString("id")),"'","")
  if id="" or isnumeric(id)=false then
	  response.Write("<script language='javascript'>alert('非法操作');history.go(-1);</script>")
	  response.End()
  end if
  
  set rs=conn.execute("delete from Duihuan where id="&id)
  response.Redirect("People.asp")
%>