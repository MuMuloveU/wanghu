<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if
%>
<!--#include file="../inc/conn.asp"-->
<%
   dim C_title,C_content,C_date,C_over,C_id
   C_id=request.QueryString("C_id")
   C_title=request.form("C_title")
   C_content=request.form("newsinfo")
   C_date=request.form("C_date")
   C_over=request.form("C_over")
   if C_id="" or isnumeric(C_id)=false then
   response.Write("<script language='javascript'>alert('操作错误请与管理员联系');history.go(-1);</script>")
   response.End()
   end if
   if C_content="" or C_title="" or C_date="" or C_over="" then
   response.write("<script language='javascript'>alert('请填写完整！');history.go(-1);</script>")
   response.end()
   end if
   
   sql="update C_content set C_title='"& C_title &"',C_content='"& C_content &"',C_date="& C_date &",C_over="& C_over &" where C_id="&C_id
   conn.execute(sql)
   
   response.write("<script language='javascript'>alert('更新成功！');location.href='Edit_content.asp';</script>")
   response.end
   
   conn.close
   set conn=nothing
%>