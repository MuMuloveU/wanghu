<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim T_id,U_url
  T_id=replace(trim(request.QueryString("id")),"","'")
  if T_id="" or isnumeric(T_id)=false then
  response.Write("<script language='javascript'>alert('²Ù×÷´íÎó');history.go(-1);</script>")
  response.End()
  end if
  U_url=request.ServerVariables("HTTP_REFERER")
'  response.Write u_url
'  response.End()
%>
<%CxGame.DbConn("News")%>
<%
  dim sql
  sql="delete from jiaz where J_id="&T_id
  conn.execute(sql)
  response.Write("<script language='javascript'>alert('É¾³ý³É¹¦');location.href='"& u_url &"';</script>")
  response.End()
  conn.close
  set conn=nothing
%>