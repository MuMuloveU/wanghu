<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<%
  dim U_name,T_name,T_qq,T_url,QQ,E_mail,T_tel,Address,T_zongzhi,T_content
  U_name=replace(trim(request.Form("username")),"","'")
  T_name=replace(trim(request.Form("tname")),"","'")
  T_qq=replace(trim(request.Form("tqq")),"","'")
  T_url=replace(trim(request.Form("luntan")),"","'")
  QQ=replace(trim(request.Form("QQ")),"","'")
  E_mail=replace(trim(request.Form("email")),"","'")
  T_tel=replace(trim(request.Form("tel")),"","'")
  Address=replace(trim(request.Form("diz")),"","'")
  T_zongzhi=request.Form("T_zongzhi")
  T_content=request.Form("T_content")
  
  if U_name="" or T_name="" or T_qq="" or T_url="" or QQ="" or E_mail="" or T_tel="" or Address="" or T_zongzhi="" or T_content="" then
  response.Write("<script language='javascript'>alert('请填写完整');history.go(-1);</script>")
  response.End()
  end if
  if isnumeric(t_qq)=false then
  response.Write("<script language='javascript'>alert('群QQ请填写数字');history.go(-1);</script>")
  response.End()
  end if
  if isnumeric(qq)=false then
  response.Write("<script language='javascript'>alert('QQ号请填写数字');history.go(-1);</script>")
  response.End()
  end if
%>

<%
  dim sql
  set rs=server.CreateObject("adodb.recordset")
  
  CxGame.DbConn("QPGameUserDB")
  sql="select * from AccountsInfo where Accounts='"& U_name &"'"
  rs.open sql,conn,1,1
  if rs.eof then
  response.Write("<script language='javascript'>alert('您还没有注册');history.go(-1);</script>")
  response.End()
  end if
  rs.close
CxGame.DbConn("News")
  sql="select * from Tuandui where U_name='"& U_name &"'"
  'response.Write sql
'  response.End()
  rs.open sql,conn,1,1
  if not rs.eof then
  response.Write("<script language='javascript'>alert('您已经创建了家族或正在审核中');history.go(-1);</script>")
  response.End()
  end if
  rs.close
  sql="select * from Tuandui where T_name='"& T_name &"'"
  rs.open sql,conn,1,1
  if not rs.eof then
  response.Write("<script language='javascript'>alert('已经创建了此家族请换一个名称');history.go(-1);</script>")
  response.End()
  end if
  sql="insert into Tuandui (U_name,T_name,T_qq,T_url,QQ,E_mail,T_tel,Address,T_zongzhi,T_content) values ('"& U_name &"','"& T_name &"',"& T_qq &",'"& T_url &"',"& QQ &",'"& E_mail &"','"& T_tel &"','"& Address &"','"& T_zongzhi &"','"& T_content &"')"
  conn.execute(sql)
  conn.close()
  set conn=nothing
%>
<SCRIPT LANGUAGE="javascript"> 
<!-- 
window.open ('http://wpa.qq.com/msgrd?V=1&amp;Uin=271955666&amp;Site=qq') 
--> 
</SCRIPT>
<%
    response.Write("<script language='javascript'>alert('创建成功请等待审核,请与商务客服联系');location.href='index.asp';</script>")
%>