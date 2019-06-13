<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
  CxGame.IsCheckLogin()
%>
<%CxGame.DbConn("QPGameUserDB")%>
<%
  dim C_PROTECTANSW,C_PROTECTQUES
  C_PROTECTANSW=replace(trim(request.form("C_PROTECTANSW")),"'","")
  C_PROTECTQUES=replace(trim(request.form("C_PROTECTQUES")),"'","")
  
  set rs=conn.execute("update AccountsInfo set C_PROTECTANSW='"& C_PROTECTANSW &"',C_PROTECTQUES='"& C_PROTECTQUES &"' where UserID="&Session("UserID")&"")
  response.Write("<script language='javascript'>alert('更改成功!');location.href='default.asp';</script>")
%>