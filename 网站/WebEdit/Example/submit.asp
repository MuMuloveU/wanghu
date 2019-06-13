
<%
' 如果编辑的内容很多，上传速度太慢，请设置以下的时间，单位秒
'Server.ScriptTimeout = 600

Dim sContent1, i

For i = 1 To Request.Form("content1").Count 
	sContent1 = sContent1 & Request.Form("content1")(i) 
Next 

Response.Write "编辑内容如下：<br><br>" & sContent1
Response.Write "<br><br><p><input type=button value=' 退回 ' onclick='history.back()'></p>"

%>