<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim T_id,U_url,id
  id=replace(trim(request.QueryString("t_id")),"","'")
  T_id=replace(trim(request.QueryString("id")),"","'")
  if T_id="" or isnumeric(T_id)=false or id="" or isnumeric(id)=false then
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
  sql="update jiaz set T_type=1 where J_id="&T_id
  conn.execute(sql)
  
  sql="update Tuandui set t_num=t_num+1 where T_id="&id
  conn.execute(sql)
  'response.Write sql
  'response.End()
  response.Write("<script language='javascript'>alert('Í¨¹ıÉóºË');location.href='"& u_url &"';</script>")

  
  
  conn.close
  set conn=nothing
%>