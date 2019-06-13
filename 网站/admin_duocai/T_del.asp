<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<%
  dim T_id
  T_id=replace(trim(request.QueryString("T_id")),"","'")
  if T_id="" or isnumeric(T_id)=false then
    response.Write("<script language='javascript'>alert('操作错误请与管理员联系');history.go(-1);</script>")
	response.End()
  end if
%>
<!--#include file="../inc/conn.asp"-->
<%
  sql="delete from Tuandui where T_id="&T_id
  conn.execute(sql)
  response.Write("<script language='javascript'>alert('删除成功');location.href='dating.asp';</script>")
  response.End()
  conn.close
  set conn=nothing
%>