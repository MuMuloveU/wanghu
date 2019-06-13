<!--#include file="../Inc/Config.asp" -->
<%if request.Cookies("admin99aw")="" then
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
  dim id,userid,C_money,C_vip,MemberOrder,C_id
  C_money=clng(replace(trim(request.QueryString("C_money")),"'",""))
  C_vip=clng(replace(trim(request.QueryString("C_vip")),"'",""))
  userid=replace(trim(request.QueryString("userid")),"'","")
  id=replace(trim(request.QueryString("id")),"'","")
  C_id=replace(trim(request.QueryString("C_id")),"'","")
  if id="" or isnumeric(id)=false then
	  response.Write("<script language='javascript'>alert('非法操作');history.go(-1);</script>")
	  response.End()
  end if
  if userid="" or isnumeric(userid)=false then
	  response.Write("<script language='javascript'>alert('非法操作');history.go(-1);</script>")
	  response.End()
  end if
  if C_id="" or isnumeric(C_id)=false then
	  response.Write("<script language='javascript'>alert('非法操作');history.go(-1);</script>")
	  response.End()
  end if
  
  CxGame.DbConn("News")
  set rs=conn.execute("select * from Jiangpin where C_id="&C_id)
  if rs.eof then
     response.Write("<script language='javascript'>alert('无此奖品！');history.go(-1);</script>")
	 response.End()
  else
     if clng(rs("C_cun")) < 1 then
	 response.Write("<script language='javascript'>alert('此奖品已经兑换完！');history.go(-1);</script>")
	 response.End()
	 end if
  end if
  set rs=conn.execute("update Duihuan set Y_huan=1 where id="&id)
  set rs=conn.execute("update Jiangpin set C_cun=C_cun - 1 where C_id="&C_id)
  
  CxGame.DbConn("QPGameUserDB")
  set rs=conn.execute("select * from AccountsInfo where UserID="& userid &"")
  if not rs.eof then
	 MemberOrder=clng(rs("MemberOrder"))		  
  end if
   
  CxGame.DbConn(RLWebDBPrefix&"TreasureDb")
  
  if MemberOrder< 1 then
     set rs=conn.execute("update GameScoreInfo set bnak=bnak-"& C_money &" where UserID="& userid &"")
  else
     set rs=conn.execute("update GameScoreInfo set bnak=bnak-"& C_vip &" where UserID="& userid &"")
  end if
  
  response.Redirect("People.asp")
%>
