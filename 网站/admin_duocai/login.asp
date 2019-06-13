<%
  if request.Cookies("admin99aw")="" then
response.Write "<script language='javascript'>alert('网络超时或您还没有登陆！');window.location.href='admin_login.asp';</script>"
response.End
end if
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>--</title>
</head>
<frameset rows="*" cols="200,*" framespacing="0" frameborder="0" border="false" id="frame" scrolling="yes">
  <frame name="left" scrolling="yes" marginwidth="0" marginheight="0" src="Left.asp">
  <frameset rows="60,*" cols="*" framespacing="0" border="false" rows="35,*" frameborder="0" scrolling="yes">
    <frame name="top" scrolling="no" src="Top.asp">
    <frame name="main" scrolling="auto" src="aspcheck.asp">
  </frameset>
</frameset>
<noframes>
  <body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0">
  <p>你的浏览器版本过低！！！本系统要求IE5及以上版本才能使用本系统。</p>
  </body>
</noframes>
</html>  gu