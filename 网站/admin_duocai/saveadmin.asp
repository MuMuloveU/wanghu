<!--#include file="../inc/conn.asp"-->
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
<!--#include file="md5.asp"-->
<%dim action,admin,adminid
action=request.QueryString("action")
id=request.QueryString("id")
admin=trim(request("admin"))
select case action
'//修改数据
case "edit"
set rs=server.CreateObject("adodb.recordset")
rs.Open "select * from admin where id="&id,conn,1,3
rs("admin")=admin
if trim(request("password"))<>"" then
rs("password")=md5(trim(request("password")))
end if
rs("classname")=request("flag")
rs.Update
rs.Close
set rs=nothing
response.Write "<script language=javascript>alert('修改成功！');history.go(-1);</script>"
response.End
'//添加新数据
case "add"
set rs=server.CreateObject("adodb.recordset")
rs.open "select * from admin where admin='"& trim(request("admin2")) &"'",conn,1,3
if not rs.eof then
response.Write("<script language='javascript'>alert('已有此用户,请更换!');history.go(-1);</script>")
response.End()
end if
rs.addnew
rs("admin")=trim(request("admin2"))
rs("password")=md5(trim(request("password2")))
rs("classname")=request("flag2")
rs.update
rs.close
set rs=nothing
response.Write "<script language=javascript>alert('添加成功！');location.href='manageadmin.asp';</script>"
response.End
'//删除数据
case "del"
conn.execute ("delete from admin where id="&request.QueryString("id"))
response.Redirect "manageadmin.asp"
end select
%>