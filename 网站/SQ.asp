<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%CxGame.IsCheckLogin()%>
<%
   dim sql,UU_name,id
   id=request.QueryString("id")
   if id="" or isnumeric(id)=false then
   response.Write("<script language='javascript'>alert('操作错误，请与管理员联系');window.close();</script>")
   response.End()
   end if   
   'response.Write Session("UserID")
'   response.End()
%>
<%CxGame.DbConn("News")%>
<%
  set rs=server.CreateObject("adodb.recordset")
  sql="select * from Jiaz where U_id="& Session("UserID") &""
  rs.open sql,conn,3,3
  if not rs.eof then
  response.Write("<script language='javascript'>alert('你已经有家族了或正在审核中');window.close();</script>")
  response.End()
  end if
  rs.close
  sql="insert into Jiaz (T_id,U_id) values ("& id &","& Session("UserID") &")"
  conn.execute(sql)
  response.Write("<script language='javascript'>alert('申请成功，请等待审核！');window.close();</script>")
  response.End()
%>
