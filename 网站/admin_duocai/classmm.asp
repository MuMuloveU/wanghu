<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<%
id=request("id")
set rs=server.createobject("adodb.recordset")
sql = "select * from class where id="&id&""
rs.open sql,conn,1,1
code=rs("classcode")
code=left(code,len(code)-3)
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title></title>
<link href="css.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style></head>

<body leftmargin="0" topmargin="100">
<form name="form1" method="post" action="classmmok.asp?id=<%=id%>">
  <table width="650" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#666666">
    <tr> 
      <td colspan="2" bgcolor="#666666"><table width="100%" border="0" cellspacing="1" cellpadding="0">
          <tr bgcolor="#efefef"> 
            <td height="30" colspan="2" class="txt"><div align="center">修改文章分类(<font color="#FF0000">*注:名类名不允许有&amp;%#'等符号</font>)</div></td>
          </tr>
          <tr> 
            <td width="29%" bgcolor="#FFFFFF" class="txt"><div align="center">选择上级分类:</div></td>
            <td width="71%" height="26" bgcolor="#FFFFFF"><div align="left">&nbsp;<select name="code" id="code">
                  <%
				classcode=session("user")
				if len(classcode)=1 then
				%>
                  <option value="" selected>┏最上级分类　　　　　　　　　　　　</option>
                  <%end if%>
                  <%
				  set rt=server.CreateObject("adodb.recordset")
				  sqt="select * from class where classcode like '"&classcode&"%' order by classcode"
				  rt.open sqt,conn,1,1
				  do while not rt.eof
				  spr=""
				  for i=6 to len(rt("classcode")) step 3
				  spr=spr&"┃"
				  next
				  %>
                  <option <%if rt("classcode")=code then%> selected<%end if%> value="<%=rt("classcode")%>"><%=spr%>┠<b><%=rt("classname")%></b></option>
                  <%
				  rt.movenext
				  loop
				  %>
                </select>
                <input name="class_" type="hidden" id="class_" value="<%=rs("classcode")%>">
              </div></td>
          </tr>
          <tr> 
            <td bgcolor="#FFFFFF" class="txt"><div align="center">新的分类名称:</div></td>
            <td height="26" bgcolor="#FFFFFF"> <div align="left">&nbsp;<input name="classname" type="text" id="clssname13" value="<%=rs("classname")%>" size="35">
              </div></td>
          </tr>
          <tr> 
            <td bgcolor="#FFFFFF" class="txt"><div align="center">分类相关图片:</div></td>
            <td height="27" bgcolor="#FFFFFF"> <div align="left">&nbsp;<input name="classpic" type="text" id="clssname13" value="<%=rs("classpic")%>" size="60">
              </div></td>
          </tr>
          <tr bgcolor="#FFFFFF"> 
            <td height="26">&nbsp;</td>
            <td height="26"><div align="center"> 
                <p class="txt">相关图片用 | 隔开如:../img/pic.jpg|../img/home.jpg|../img/huo.jpg</p>
              </div></td>
          </tr>
          <tr bgcolor="#FFFFFF"> 
            <td colspan="2"><div align="center"><br>
                <input type="submit" name="Submit" value="确定">
                　 
                <input type="reset" name="Submit2" value="重置">
                <br>
                <br>
              </div></td>
          </tr>
        </table></td>
    </tr>
  </table>
</form>
</body>
</html>
