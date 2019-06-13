<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
response.write "<script language='javascript'>"
response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
response.end
end if
%>