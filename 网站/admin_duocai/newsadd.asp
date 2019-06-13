<!--#include file="chak.asp"-->
<!--#include file="../inc/conn.asp"-->
<Script Language=JavaScript>
function doChange(objText, objDrop){
	if (!objDrop) return;
	var str = objText.value;
	var arr = str.split("|");
	objDrop.length=0;
	for (var i=0; i<arr.length; i++){
		objDrop.options[i] = new Option(arr[i], arr[i]);
	}
		objDrop.options[i] = new Option("请选择图片", "");

}
</Script>

<script language="JavaScript">
<!--
function checkForm()
{
	  	if (document.add.classcode.value=="") {
		alert("请输入文章分类.");
		document.add.classcode.focus();
		return false;
	}
	if (document.add.newstitle.value=="") {
		alert("请输入文章标题.");
		document.add.newstitle.focus();
		return false;
	}	
	return true
}
//-->
</script>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>添加文章</title>
<link href="../inc/STYLE.CSS" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
.head {
	display: none;
}
body,td,th {
	font-size: 12px;
}
body {
	background-color: #bacaef;
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
}
-->
</style>
</head>
<body>
<center>
  <form action="newsaddok.asp" method="post" name=add id="add" onSubmit="return checkForm()">
    <table width="800" height="544" border="1" cellpadding="0" cellspacing="0" bordercolorlight="#575757" bordercolordark="#FFFFFF">
      <tr bgcolor="#cfdded"> 
        <td height="40" colspan="4"> 
          <p align="center"><strong><font color="#000000">添加文章</font></strong></td>
      </tr>
      <tr> 
        <td width="145" height="40" bgcolor="#FFFFFF" class="txt"><div align="center">文章分类</div></td>
        <td colspan="3" bgcolor="#FFFFFF">　 
          <select name="classcode" id="classcode">
            <option value="" selected>┏选择文章分类　　　　　　　　　　　　</option>
            <%
				  classcode=session("user")
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
          </select> <font color="#FF0000">*</font></td>
      </tr>
      <tr> 
        <td height="40" bgcolor="#FFFFFF" class="txt"> <div align="center">文章标题</div></td>
        <td colspan="3" bgcolor="#FFFFFF"> <div align="left"> 　 
            <input name="newstitle" type="text" id="pic43" size="40">
            <font color="#FF0000">* 
            <input name="keyword" type="hidden" id="keyword">
            推荐？ 
            <select name="pl" id="pl">
              <option value="0" selected>否</option>
              <option value="1">是</option>
            </select>
            </font></div></td>
      </tr>
      <tr class="head"> 
        <td height="40" bgcolor="#FFFFFF"> 
          <div align="center">文章出处</div></td>
        <td bgcolor="#FFFFFF"> 
          <div align="left"> 　 
            <input name="newscu" type="text" id="pic43" size="20">
          </div></td>
        <td bgcolor="#FFFFFF"> 
          <div align="center">是否允许评论</div></td>
        <td bgcolor="#FFFFFF"> 
          <div align="center"> </div></td>
      </tr>
      <tr class="head"> 
        <td height="40" bgcolor="#FFFFFF"> 
          <div align="center">文章作者</div></td>
        <td width="242" bgcolor="#FFFFFF"> 
          <div align="left"> 　 
            <input name="newszz" type="text" id="newszz" size="20">
          </div></td>
        <td width="138" bgcolor="#FFFFFF"> 
          <div align="center"> 
            <p class="txt">发布时间</p>
          </div></td>
        <td width="166" bgcolor="#FFFFFF"> 
          <div align="center"> 
            <input name="newsdate" type="text" id="newsdate" value="<%=date%>" size="20">
          </div></td>
      </tr>
      <tr bgcolor="#FFFFFF" class="head"> 
        <td height="37"> 
          <div align="center" class="txt">是否加到首页弹出窗口：</div></td>
        <td colspan="3">　 
          <input name="top" type="checkbox" id="id" value="1"> </td>
      </tr>
      <tr bgcolor="#FFFFFF"> 
        <td height="35"><div align="center"> 
            <p class="txt">相关图片</p>
          </div></td>
        <td height="35" colspan="3" class="txt"> <input type=hidden id=myText2 style="width:200px" onChange="doChange(this,myDrop2)">
          　 
          <select name="select" size=1 id=myDrop2 style="width:350px">
            <option value="" selected>选择图片</option>
          </select>
          (可选,若加了图片则在首页显示图片)</td>
      </tr>
      <tr bgcolor="#cfdded"> 
        <td height="35" colspan="4" align="left"> 
        <div align="left">&nbsp;&nbsp;&nbsp; 文章内容：<font color="#FF0000">*</font></div></td>
      </tr>
      <tr> 
        <td height="350" colspan="4" valign="middle"><p align="center"> 
            <IFRAME ID="eWebEditor1" src="../htmledit/ewebeditor.asp?id=newsinfo&style=s_light&savefilename=myText2" frameborder="0" scrolling="no" width="800" height="300"></IFRAME>
            <input name="newsinfo" type="hidden" id="newsinfo">
        </td>
      </tr>
      <tr bgcolor="#cfdded"> 
        <td height="35" colspan="4"> 
          <p align="center"> 
            <input type="submit" value="确 定" name="B1"  class=button>
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
            <input type="reset" value="重 写" name="B2"  class=button>
        </td>
      </tr>
    </table>    
  </form>    
</center>    
</body>    
