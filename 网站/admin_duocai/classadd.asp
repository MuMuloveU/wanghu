<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
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
<form name="form1" method="post" action="classaddok.asp">
  <table width="650" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#666666">
    <tr> 
      <td colspan="2" bgcolor="#666666"><table width="100%" border="0" cellspacing="1" cellpadding="0">
          <tr bgcolor="#efefef"> 
            <td height="30" colspan="2" class="txt"><div align="center"><br>
                新增新闻分类(<font color="#FF0000">*注:名类名不允许有&amp;%#'等符号</font>)<br>
                <br>
                <font color="#FF0000">除管理员外,操作员只可管理本分类下的子分类</font><br>
                <br>
              </div></td>
          </tr>
          <tr> 
            <td width="22%" bgcolor="#FFFFFF" class="txt"><div align="center">选择上级分类:</div></td>
            <td width="78%" height="26" bgcolor="#FFFFFF"> <div align="left">&nbsp; 
                <select name="code" id="code">
                  <option value="" selected>┏最上级分类　　　　　　　　　　　　</option>
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
                  <option value="<%=rt("classcode")%>"><%=spr%>┠<%=rt("classname")%></option>
                  <%
				  rt.movenext
				  loop
				  %>
                  <%
				if len(classcode)=1 then
				%>
                  <option value="">┗最上级分类　　　　　　　　　　　　</option>
                  <%end if%>
                </select>
              </div></td>
          </tr>
          <tr> 
            <td bgcolor="#FFFFFF" class="txt"><div align="center">新的分类名称:</div></td>
            <td height="26" bgcolor="#FFFFFF"> <div align="left">&nbsp; 
                <input name="classname" type="text" id="clssname13" size="35">
              </div></td>
          </tr>
          <tr> 
            <td bgcolor="#FFFFFF" class="txt"><div align="center">分类相关图片:</div></td>
            <td height="27" bgcolor="#FFFFFF"> <div align="left">&nbsp; 
                <input name="classpic" type="text" id="clssname13" size="60">
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
