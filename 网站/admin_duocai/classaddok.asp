<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<% dim clsssname,rs,classename,re
classname=trim(request.form("classname"))
classpic=trim(request.form("classpic"))
code=request.form("code")
if classname="" then
response.write "<script language=JavaScript>" & chr(13) & "alert('分类名不能为空');" & "history.back()" & "</script>"
Response.End
end if
codeq=len(code)+3
set re=server.createobject("adodb.recordset")
sqe="select top 1 * from class where len(classcode)="&codeq&" and classcode like '"&code&"%' order by classcode desc"
re.open sqe,conn,1,1
if re.eof and re.bof then
classcode=code&"100"
else
classcode=re("classcode")
x=len(classcode)
y=mid(classcode,x-2)
y=y+1
w=mid(classcode,1,x-3)
classcode=w&y
end if
set rs=server.createobject("adodb.recordset")
sql="select * from class"
rs.open sql,conn,1,3
rs.addnew
rs("classcode")=classcode
rs("classname")=classname
rs("classpic")=classpic
rs.update
rs.close
re.close
set rs=nothing
set re=nothing
Response.Write "<script language=JavaScript1.1>alert('增加分类成功');" 
Response.Write " document.location='classedit.asp';</script>" 
Response.End
%>