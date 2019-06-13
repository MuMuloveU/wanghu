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
<%
  C_name=replace(trim(request.Form("C_name")),"'","")
  C_pic=replace(trim(request.Form("bookpic")),"'","")
  C_type=replace(trim(request.Form("C_type")),"'","")
  C_content=replace(trim(request.Form("bookcontent")),"'","")
  C_money=replace(trim(request.Form("C_money")),"'","")
  C_vip=replace(trim(request.Form("C_vip")),"'","")
  C_cun=replace(trim(request.Form("C_cun")),"'","")
  
  if C_name="" then 
	  response.Write("<script language='javascript'>alert('请填写完整');window.close();</script>")
	  response.End()
  end if
%>
<!--#include file="../inc/conn.asp"-->
<%
  set rs=server.CreateObject("adodb.recordset")
  sql="insert into Jiangpin (C_name,C_pic,C_content,C_type,C_money,C_vip,C_time,C_cun) values ('"& C_name &"','"& C_pic &"','"& C_content &"','"& C_type &"','"& C_money &"','"& C_vip &"','"& date() &"',"& C_cun &")"
  conn.execute(sql)
  response.Write("<script language='javascript'>window.parent.opener.location=window.parent.opener.location;alert('添加成功！');window.close();</script>")
%>