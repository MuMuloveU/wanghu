<%
	Dim flashack_Post,flashack_Get,flashack_In,flashack_Inf,flashack_Xh,flashack_db,flashack_dbstr 
	flashack_In = "'※;※and※exec※insert※select※delete※update※count※*※%※chr※mid※master※truncate※char※declare" 
	flashack_Inf = split(flashack_In,"※") 
	
	
	If Request.QueryString<>"" Then 
	For Each flashack_Get In Request.QueryString 
	For flashack_Xh=0 To Ubound(flashack_Inf) 
	If Instr(LCase(Request.QueryString(flashack_Get)),flashack_Inf(flashack_Xh))<>0 Then 
	Response.Write "<Script Language=JavaScript>alert('flashack防注入系统提示你↓\n\n请不要在参数中包含非法字符尝试注入,QQ:200160！\n\nHTTP://www.dc173.com');</Script>" 
	Response.Write "非法操作！flashack已经给你做了如下记录↓<br>" 
	Response.Write "操作IP："&Request.ServerVariables("REMOTE_ADDR")&"<br>" 
	Response.Write "操作时间："&Now&"<br>" 
	Response.Write "操作页面："&Request.ServerVariables("URL")&"<br>" 
	Response.Write "提交方式：GET<br>" 
	Response.Write "提交参数："&flashack_Get&"<br>" 
	Response.Write "提交数据："&Request.QueryString(flashack_Get)
	Response.End 
	End If 
	Next 
	Next 
	End If 
%>




















<%
SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=1111;PWD=11111;DATABASE=QPGameUserDB;"
Set Conn= Server.CreateObject("ADODB.Connection")
Conn.Open SqlConnectionString
%>  