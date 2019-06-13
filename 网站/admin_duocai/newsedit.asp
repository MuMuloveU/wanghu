<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<!--#include file="../inc/Page.asp" -->
<link href="csss.css" rel="stylesheet" type="text/css">
<link href="../inc/STYLE.CSS" rel="stylesheet" type="text/css">
<body bgcolor="#5578b8" leftmargin="0" topmargin="0">
<script language="javascript" type="text/javascript">
<!--
function lo()
{
document.location="newsadd.asp";
}

function CheckOthers(form)
{
     for (var i=0;i<form.elements.length;i++)
     {
           var e = form.elements[i];
                 if (e.checked==false)
                 {
                       e.checked = true;
                 }
                 else
                 {
                       e.checked = false;
                 }
     }
}
function CheckAll(form)
{
     for (var i=0;i<form.elements.length;i++)
     {
           var e = form.elements[i];
                 e.checked = true
     }
}
function ConfirmDel()
{
   if(confirm("确定要删除您选中的文章及本文章所有的评论吗？一旦删除将不能恢复！"))
     return true;
   else
     return false;
	 
}
//-->
</script>
<%
classcode=session("user")
act=request("act")
if act="t" then
url=Request.ServerVariables("HTTP_REFERER")
classcode=request("class")
newid=request("id")
ts=request("ts")
set rs=server.CreateObject("adodb.recordset")
sql="select top 1 ts from news where classcode like '"&classcode&"%' and ts> #"&ts&"# order by ts"
rs.open sql,conn,1,3
if rs.eof and rs.bof then
Response.Write "<script language=JavaScript1.1>alert('对不起,已经是该条新闻所属分类的最前一条不能提升 ');" 
Response.Write " document.location='"&url&"';</script>" 
Response.End
end if
ts2=rs("ts")
rs("ts")=ts
rs.update
set rs=server.CreateObject("adodb.recordset")
sql="update news set ts='"&ts2&"' where id="&newid
rs.open sql,conn,1,3
Response.Write "<script language=JavaScript1.1>document.location='"&url&"';</script>" 
response.end
rs.close
end if
if act="del" then
id=request.form("id")
if id="" then
Response.Write "<script language=JavaScript1.1>alert('对不起,您没有选择要删除的新闻 ');" 
Response.Write " document.location='newsedit.asp';</script>" 
Response.End
end if
set del=server.CreateObject("adodb.recordset")
delsql="delete from news where id in ("&id&")"
del.open delsql,conn,1,3


Response.Write "<script language=JavaScript1.1>alert(' 删除文章成功 ');" 
Response.Write " document.location='newsedit.asp';</script>" 
Response.End
end if
%><form name="form2" method="get" action="">
  <table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
    <td bgcolor="#999999"><table width="100%" border="0" cellspacing="1" cellpadding="0">
        <tr>
            <td height="29" bgcolor="#cfdded">
<div align="center">管理文章</div></td>
        </tr>
        <tr>
            <td height="29" bgcolor="#f0f5f9"> 
              <div align="center"> 
                <select name="class" id="class" OnChange="submit(this)">
				  <option value="" selected>┏全部文章　　　　　　　　　　　　    </option>
                  <%
				  set rt=server.CreateObject("adodb.recordset")
				  sqt="select * from class order by classcode"
				  rt.open sqt,conn,1,1
				  do while not rt.eof
				  spr=""
				  for i=6 to len(rt("classcode")) step 3
				  spr=spr&"┃"
				  next
				  %>
                  <option value="<%=rt("classcode")%>"><%=spr%>┣<%=rt("classname")%></option>
                  <%
				  rt.movenext
				  loop
				  %>
                </select>
              </div>
            </td>
        </tr>
      </table></td>
  </tr>
</table></form>
<form name="form1" method="post" action="newsedit.asp?act=del">
  <table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
      <td>
<table width="100%" border="0" cellspacing="1" cellpadding="0">
          <tr bgcolor="#cfdded" class="txt"> 
            <td width="36" height="24"> <div align="center">选中</div></td>
            <td width="312" height="24" bgcolor="#cfdded"> <div align="center">文章标题</div></td>
            <td width="169" height="24"> <div align="center">发布时间</div></td>
            <%if request("class")<>"" then%>
            <%end if%>
            <td width="64" height="24"> <div align="center">修改</div></td>
          </tr>
          <%
		  newsclass=request("class")
		  if newsclass<>"" then
		     sql="select * from news where classcode like '"&newsclass&"%'"
		  else
		     sql="select * from news where classcode like '"&classcode&"%'"
		  end if
		  sql=sql&" order by id desc"
		  set rs=server.createobject("adodb.recordset")
rs.open sql,conn,1,3					
rs.PageSize =20
result_n=rs.RecordCount
if result_n<=0 then
	word="对不起,没有任何文章!"
else
	maxpage=rs.PageCount 
	page=request("page")	
	if Not IsNumeric(page) or page="" then
		page=1
	else
		page=cint(page)
	end if
	
	if page<1 then
		page=1
	elseif  page>maxpage then
		page=maxpage
	end if	
	rs.AbsolutePage=Page
end if

		  if rs.bof and rs.eof then
		  %>
          <tr bgcolor="#FFFFFF" class="txt"> 
            <td height="24" colspan="4"><div align="center">对不起,没有找到任何文章</div></td>
          </tr>
          <%
		response.end
		end if 
		for i=1 to rs.PageSize
		%>
          <tr bgcolor="#f0f5f9" class="txt"> 
            <td height="24"> <div align="center"> 
                <input name="id" type="checkbox" id="id" value="<%=rs("id")%>">
              </div></td>
            <td height="24" bgcolor="#f0f5f9"> <div align="center"><%=rs("newstitle")%></div></td>
            <td height="24"> <div align="center"><%=rs("newsdate")%></div></td>
            <td height="24"> <div align="center"><a href="newsmm.asp?id=<%=rs("id")%>">编缉</a></div></td>
          </tr>
          <%
		  rs.movenext
		  if rs.eof then exit for
		  next
		  %>
          <tr bgcolor="#FFFFFF" class="txt"> 
            <td height="32" colspan="4"> <div align="center"></div>
              <div align="center"></div>
              <div align="center"></div>
              <div align="center"> 
                <INPUT name=ddd type=button class="in" onclick=CheckAll(this.form) value=全选>
                &nbsp;&nbsp;&nbsp; 
                <INPUT name=cccc type=button class="in" onclick=CheckOthers(this.form) value=反选>
                &nbsp;&nbsp;&nbsp; 
                <input name="Submit" type="reset" class="in" value="重选">
                &nbsp;&nbsp;&nbsp; 
                <INPUT name=cccc2 type=submit class="in" value=删除 onClick="return ConfirmDel();">
                &nbsp;&nbsp;&nbsp; 
                <INPUT name=cccc22 type=button class="in" value=新增 onClick="lo();">
              </div></td>
          </tr>
          <tr bgcolor="#FFFFFF"> 
            <td height="24" colspan="4" class="txt"> <div align="center"></div>
              <div align="center"> 
                <%call goPage(result_n,page,20)%>
              </div></td>
          </tr>
        </table></td>
  </tr>
</table>
</form>

