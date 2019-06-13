<%
    if request.Cookies("admin99aw")="" then
	response.Write "<script language='javascript'>alert('网络超时或您还没有登陆！');window.location.href='admin_login.asp';</script>"
	response.End
	else
	if request.Cookies("flag")<>"超级管理员" then
	response.Write "<p align=center><font color=red>您没有此项目管理权限！</font></p>"
	response.End
	end if
	end if
%>
<!--#include file="../inc/conn.asp"-->
<%
  dim C_id
  C_id=replace(trim(request("C_id")),"'","")
  if C_id="" then
	  response.Write("<script language='javascript'>alert('操作错误');window.close();</script>")
	  response.End()
  end if
%>
<%
  set rs=server.CreateObject("adodb.recordset")
  sql="select * from Jiangpin where C_id="&C_id
  rs.open sql,conn,1,1
  if not rs.eof then
     C_name=rs("C_name")
	 C_type=rs("C_type")
	 C_pic=rs("C_pic")
	 C_content=rs("C_content")
	 C_money=rs("C_money")
	 C_vip=rs("C_vip")
	 C_cun=rs("C_cun")
  end if
%>
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
}
-->
</style>
<link href="images/css.css" rel="stylesheet" type="text/css" />
<table width="748" height="100%" border="0" align="center" cellpadding="2" cellspacing="1" bgcolor="#0B55D2">
<form action="edit_Ss.asp" method="post" name="form" onSubmit="return place();">
  <tr>
    <td height="26" colspan="4" bgcolor="#3366FF"><font color="#FFFFFF">::更新奖品信息</font></td>
  </tr>
  <tr bgcolor="#eeeeee">
    <td width="11%" height="24" align="right">名&nbsp; 称：</td>
    <td width="41%" height="24"><input name="C_name" type="text" class=form id="C_name" value="<%=C_name%>" size="20" maxlength="100">
      (必填)      <input type="hidden" name="C_id" value="<%=C_id%>" /></td>
    <td width="11%" align="right">兑换价格：</td>
    <td width="37%" height="24"><input name="C_money" type="text" class="form" id="C_money"  onkeypress	= "return regInput(this,	/^[0-9]*$/,		String.fromCharCode(event.keyCode))" value="<%=C_money%>" size="10" maxlength="100"
		onpaste		= "return regInput(this,	/^[0-9]*$/,		window.clipboardData.getData('Text'))"
		ondrop		= "return regInput(this,	/^[0-9]*$/,		event.dataTransfer.getData('Text'))"/></td>
  </tr>
  <tr bgcolor="#eeeeee">
    <td height="24" align="right">类&nbsp; 别：</td>
    <td height="24"><select name="C_type" class="form">
	<option value="">---请选择---</option>
	<%
	  set rs=conn.execute("select T_name from C_type")
	  if not rs.eof then
	  do while not rs.eof
	%>
	<option <%if C_type=rs(0) then%> selected <%end if%> value="<%=rs(0)%>">-<%=rs(0)%>-</option>
	<%
	  rs.movenext
	  loop
	  end if
	  rs.close
	%>
	</select></td>
    <td height="24" align="right">会员价格：</td>
    <td><input name="C_vip" type="text" class="form" id="C_vip"  onkeypress	= "return regInput(this,	/^[0-9]*$/,		String.fromCharCode(event.keyCode))" value="<%=C_vip%>" size="10" maxlength="100"
		onpaste		= "return regInput(this,	/^[0-9]*$/,		window.clipboardData.getData('Text'))"
		ondrop		= "return regInput(this,	/^[0-9]*$/,		event.dataTransfer.getData('Text'))"/></td>
  </tr>
  <tr align="center" bgcolor="#D4D0C8">
    <td height="24" align="right" valign="middle" bgcolor="#FFFFFF">图&nbsp; 片：</td>
    <td height="24" align="left" valign="middle" bgcolor="#FFFFFF"><span class="forumRowHighlight">
      <input name="bookpic" type="text" class="title" id="bookpic" value="<%=C_pic%>" size="30" readonly>
      <input name="Submit2" type="button" class="Button" onClick="window.open('../situjiaduotu.asp?formname=form&editname=bookpic&uppath=bookpic&filelx=jpg','','status=no,scrollbars=no,top=100,left=200,width=420,height=165')" value="上传图片">
    </span></td>
    <td height="24" align="right" valign="middle" bgcolor="#FFFFFF">库 存 量：</td>
    <td height="24" align="left" valign="middle" bgcolor="#FFFFFF"><input name="C_cun" type="text" class="form" id="C_cun"  onkeypress	= "return regInput(this,	/^[0-9]*$/,		String.fromCharCode(event.keyCode))" value="<%=C_cun%>" size="10" maxlength="100"
		onpaste		= "return regInput(this,	/^[0-9]*$/,		window.clipboardData.getData('Text'))"
		ondrop		= "return regInput(this,	/^[0-9]*$/,		event.dataTransfer.getData('Text'))"/></td>
  </tr>
  <tr align="center" bgcolor="#eeeeee">
    <td height="40" colspan="4"><textarea name="bookcontent" style="display:none"><%=server.HTMLEncode(C_content)%></textarea>
      <iframe ID="bookcontent" src="../webedit/ewebeditor.asp?id=bookcontent&style=standard" frameborder="0" scrolling="no" width="748" HEIGHT="350"></iframe></td>
    </tr>
  <tr align="center" bgcolor="#eeeeee">
    <td height="24" colspan="4"><table width="100%" border="0" cellspacing="0" cellpadding="5">
      <tr>
        <td align="center"><input name="submit" type=submit class=Button value="　保　存　" >&nbsp;
            <input name="YL" type=button class=Button value="　关　闭　" onClick="window.close()"></td>
      </tr>
    </table></td>
    </tr>
  </form>
</table>
<script language="javascript">
function place(){
  if(form.C_name.value==""){
  alert("名称不能为空哦");
  form.C_name.focus();
  return (false);
  }
  if(form.C_type.value==""){
  alert("请选择类别");
  form.C_type.focus();
  return (false);
  }
  if(form.C_money.value==""){
  alert("请输入会员价格");
  form.C_money.focus();
  return (false);
  }
  if(form.bookpic.value==""){
  alert("请上传图片");
  form.bookpic.focus();
  return (false);
  }
  if(form.C_vip.value==""){
  alert("请输入VIP价格");
  form.C_vip.focus();
  return (false);
  }
  if(form.C_cun.value==""){
  alert("请输入库存量！");
  form.C_cun.focus();
  return (false);
  }
}
function regInput(obj, reg, inputStr)
	{
		var docSel	= document.selection.createRange()
		if (docSel.parentElement().tagName != "INPUT")	return false
		oSel = docSel.duplicate()
		oSel.text = ""
		var srcRange	= obj.createTextRange()
		oSel.setEndPoint("StartToStart", srcRange)
		var str = oSel.text + inputStr + srcRange.text.substr(oSel.text.length)
		return reg.test(str)
	}
</script>