<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<!--#include file="../inc/conn.asp"-->
<%
   dim C_title,C_content,C_date,C_over
   C_title=request.form("C_title")
   C_content=request.form("newsinfo")
   C_date=request.form("C_date")
   C_over=request.form("C_over")
   
   if C_content="" or C_title="" or C_date="" or C_over="" then
   response.write("<script language='javascript'>alert('请填写完整！');history.go(-1);</script>")
   response.end
   end if
   
   sql="insert into C_content (C_title,C_content,C_date,C_over) values ('"& C_title &"','"& C_content &"',"& C_date &","& C_over &")"
   conn.execute(sql)
   
   response.write("<script language='javascript'>alert('填加成功！');location.href='Content.asp';</script>")
   response.end
   
   conn.close
   set conn=nothing
%>