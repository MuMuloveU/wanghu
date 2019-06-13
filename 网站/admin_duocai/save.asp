<!--#include file="conn.asp"-->
<!--#include file="md5.asp"-->
<%
  dim gonggao,xuesu,yanjiu,benke,xuesheng,jiuye,shehui,adminid
  adminid=replace(trim(request.QueryString("adminid")),"'","")
  gonggao=replace(trim(request.Form("gonggao")),"'","")
  xuesu=replace(trim(request.Form("xuesu")),"'","")
  yanjiu=replace(trim(request.Form("yanjiu")),"'","")
  benke=replace(trim(request.Form("benke")),"'","")
  xuesheng=replace(trim(request.Form("xuesheng")),"'","")
  jiuye=replace(trim(request.Form("jiuye")),"'","")
  shehui=replace(trim(request.Form("shehui")),"'","")
  zhuanti=replace(trim(request.Form("zhuanti")),"'","")
  gongshi=replace(trim(request.Form("gongshi")),"'","")
  
  if gonggao="" then gonggao=0
  if xuesu="" then xuesu=0
  if yanjiu="" then yanjiu=0
  if benke="" then benke=0
  if xuesheng="" then xuesheng=0
  if jiuye="" then jiuye=0
  if shehui="" then shehui=0
  if zhuanti="" then zhuanti=0
  if gongshi="" then gongshi=0
%>
<%
  if adminid="" or isnumeric(adminid)=false then
     response.Write("<script language='javascript'>alert('操作错误');window.close();</script>")
	 response.End()
  end if
%>
<%
  sql="update admin set gonggao="& gonggao &",xuesu="& xuesu &",yanjiu="& yanjiu &",benke="& benke &",xuesheng="& xuesheng &",jiuye="& jiuye &",shehui="& shehui &",gongshi="& gongshi &",zhuanti="& zhuanti &" where adminid="& adminid
  conn.execute(sql)
%>
<%
response.Write("<script language='javascript'>window.parent.opener.location=window.parent.opener.location;alert('修改成功！');window.close();</script>")
%> n>