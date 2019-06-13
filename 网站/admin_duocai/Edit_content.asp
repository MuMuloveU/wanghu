<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<!--#include file="../inc/Page.asp" -->

<%
  dim act
  act=request("act")
  if act="del" then
     dim C_id
	 C_id=request("id")
	 if C_id="" then
	 response.write("<script language='javascript'>alert('请选择你要删除的信息！');history.back();</script>")
	 response.end 
	 end if
	 
	 sql="delete from C_content where C_id in ("& C_id &")"
	 conn.execute(sql)
	 response.write("<script language='javascript'>alert('删除成功');location.href='edit_content.asp';</script>")
	 response.end
	 
  end if
%>
<link href="csss.css" rel="stylesheet" type="text/css">
<link href="../inc/STYLE.CSS" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
.style1 {color: red}
-->
</style>
<body bgcolor="#5578b8" leftmargin="0" topmargin="0">
<script language="javascript" type="text/javascript">
<!--
function lo()
{
document.location="content.asp";
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
  <table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
    <tr>
    <td bgcolor="#999999"><table width="100%" border="0" cellspacing="1" cellpadding="0">
        <tr>
            <td height="29" bgcolor="#cfdded">
<div align="center" class="style1">管理信息</div></td>
        </tr>
      </table></td>
  </tr>
</table>
<form name="form1" method="post" action="edit_content.asp?act=del">
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
          sql="select * from C_content order by C_id desc"       
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
                <input name="id" type="checkbox" id="id" value="<%=rs("C_id")%>">
              </div></td>
            <td height="24" bgcolor="#f0f5f9"> <div align="center"><%=rs("C_title")%></div></td>
            <td height="24"> <div align="center"><%=rs("C_time")%></div></td>
            <td height="24"> <div align="center"><a href="content_mm.asp?id=<%=rs("C_id")%>">编缉</a></div></td>
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

