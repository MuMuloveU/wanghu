<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<%
id=request("id")
if id="" then
response.write("请选择您要修改的文章")
response.end
end if
newstitle=trim(request.Form("newstitle"))
newscu=trim(request.Form("newscu"))
classcode=trim(request.Form("classcode"))
newszz=trim(request.Form("newszz"))
newsdate=request.Form("newsdate")
newsinfo=request.form("newsinfo")
pl=request.form("pl")
keyword=trim(request.form("keyword"))
gd=request.form("gd")
down=request.form("pic")
top2=request.form("top")
if not isdate(newsdate) then
  response.write "<script language=JavaScript>" & chr(13) & "alert('请输入正确的时间格式');" & "history.back()" & "</script>"
  Response.End
end if
newsdate=cdate(newsdate)
'set rs=server.createobject("adodb.recordset")
'sql="select * from news where id="&id
'rs.open sql,conn,1,3
'rs("newstitle")=newstitle
'rs("newscu")=newscu
'rs("classcode")=classcode
'rs("newszz")=newszz
'rs("newsdate")=newsdate
'rs("newsinfo")=newsinfo
'rs("pl")=pl
''rs("gd")=gd
'rs("keyword")=keyword
'rs("down")=down
'rs.update
'rs.close

sql="update news set newstitle='"& newstitle &"',newscu='"& newscu &"',classcode='"& classcode &"',newszz='"& newszz &"',newsdate='"& newsdate &"',newsinfo='"& newsinfo &"',pl='"& pl &"',keyword='"& keyword &"',down='"& down &"' where id="&id
conn.execute(sql)

set rs=nothing
Response.Write "<script language=JavaScript1.1>alert('修改文章成功 ');" 
Response.Write " document.location='newsedit.asp';</script>" 
Response.End
%>
