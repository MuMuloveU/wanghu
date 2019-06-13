<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<% dim clsssname,rs,re,yy,xx,list,classcode,classcoden,codeq
id=request("id")
classpic=trim(request.form("classpic"))
classname=trim(request.form("classname"))
code=request.form("code")
class_=request.form("class_")
'数据验证
if code=class_ then
  response.write "<script language=JavaScript>" & chr(13) & "alert('分类不可以向本分类转移或者分类管理者不能改变管理范围内最上级的分类');" & "history.back()" & "</script>"
  Response.End
end if

if classname="" then
  response.write "<script language=JavaScript>" & chr(13) & "alert('分类名不能为空');" & "history.back()" & "</script>"
  Response.End
end if

if code<>left(class_,len(class_)-3) then
'自动检测本分类的上级分类的子分类最后一个CLASSCODE,若没有,连上:&100,若有,取得最后三位加1然后把CODE去掉最后三位连上最新加的最后三位
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
'取得本分类原来的CLASSCODE
set rs=server.createobject("adodb.recordset")
sql="select * from class where id="&id
rs.open sql,conn,1,3
classcoden=rs("classcode")
xx=len(classcoden)
zz=len(code)
if xx<zz and left(code,xx)=classcoden then
  response.write "<script language=JavaScript>" & chr(13) & "alert('分类不能向本分类的任何下级子分类转移');" & "history.back()" & "</script>"
  Response.End
end if
'更新子分类CLASSCODE
set rs2=server.createobject("adodb.recordset")
list="select classcode from class where classcode like '"&classcoden&"%'"
rs2.open list,conn,1,3
do while not rs2.eof
  yy=mid(rs2("classcode"),xx+1)
  rs2("classcode")=classcode&yy
  rs2.update
  rs2.movenext
loop
rs2.close
'更新新闻CLASSCODE
set rs2=server.createobject("adodb.recordset")
list="select classcode from news where classcode like '"&classcoden&"%'"
rs2.open list,conn,1,3
if not rs2.eof then
do while not rs2.eof
  yy=mid(rs2("classcode"),xx+1)
  rs2("classcode")=classcode&yy
  rs2.update
  rs2.movenext
loop
rs2.close
end if
'更新管理员classcode
set rs2=server.createobject("adodb.recordset")
list="select classcode from admin where classcode like '"&classcoden&"%'"
rs2.open list,conn,3,3
if not rs2.eof then
do while not rs2.eof
  yy=mid(rs2("classcode"),xx+1)
  rs2("classcode")=classcode&yy
  rs2.update
  rs2.movenext
loop
rs2.close
end if
'更新本分类CLASSCODE
classcode=cstr(classcode)
rs("classcode")=classcode
rs("classpic")=classpic
rs("classname")=classname
rs.update
rs.close
re.close
set rs=nothing
set re=nothing
Response.Write "<script language=JavaScript1.1>alert('修改分类成功 ');" 
Response.Write " document.location='classedit.asp';</script>" 
Response.End
else
set rs=server.CreateObject("adodb.recordset")
sql="select * from class where id="&id
rs.open sql,conn,1,3
rs("classname")=classname
rs("classpic")=classpic
rs.update
rs.close
set rs=nothing
Response.Write "<script language=JavaScript1.1>alert('修改分类成功 ');" 
Response.Write " document.location='classedit.asp';</script>" 
Response.End
end if
%>