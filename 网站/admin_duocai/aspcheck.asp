<%@ Language="VBScript" %>
<% Option Explicit %>
<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
  response.write "<script language='javascript'>"
  response.write"parent.location.href='admin_login.asp';</SCRIPT>" 
  response.end
 end if

'不使用输出缓冲区，直接将运行结果显示在客户端
Response.Buffer = False

'声明待检测数组
Dim ObjTotest(26,4)

ObjTotest(0,0) = "MSWC.AdRotator"
ObjTotest(1,0) = "MSWC.BrowserType"
ObjTotest(2,0) = "MSWC.NextLink"
ObjTotest(3,0) = "MSWC.Tools"
ObjTotest(4,0) = "MSWC.Status"
ObjTotest(5,0) = "MSWC.Counters"
ObjTotest(6,0) = "IISSample.ContentRotator"
ObjTotest(7,0) = "IISSample.PageCounter"
ObjTotest(8,0) = "MSWC.PermissionChecker"
ObjTotest(9,0) = "Scripting.FileSystemObject"
	ObjTotest(9,1) = "(FSO 文本文件读写)"
ObjTotest(10,0) = "adodb.connection"
	ObjTotest(10,1) = "(ADO 数据对象)"
	
ObjTotest(11,0) = "SoftArtisans.FileUp"
	ObjTotest(11,1) = "(SA-FileUp 文件上传)"
ObjTotest(12,0) = "SoftArtisans.FileManager"
	ObjTotest(12,1) = "(SoftArtisans 文件管理)"
ObjTotest(13,0) = "LyfUpload.UploadFile"
	ObjTotest(13,1) = "(文件上传组件)"
ObjTotest(14,0) = "Persits.Upload.1"
	ObjTotest(14,1) = "(ASPUpload 文件上传)"
ObjTotest(15,0) = "w3.upload"
	ObjTotest(15,1) = "(Dimac 文件上传)"

ObjTotest(16,0) = "JMail.SmtpMail"
	ObjTotest(16,1) = "(Dimac JMail 邮件收发) <a href='http://www.5757.net'>中文手册下载</a>"
ObjTotest(17,0) = "CDONTS.NewMail"
	ObjTotest(17,1) = "(虚拟 SMTP 发信)"
ObjTotest(18,0) = "Persits.MailSender"
	ObjTotest(18,1) = "(ASPemail 发信)"
ObjTotest(19,0) = "SMTPsvg.Mailer"
	ObjTotest(19,1) = "(ASPmail 发信)"
ObjTotest(20,0) = "DkQmail.Qmail"
	ObjTotest(20,1) = "(dkQmail 发信)"
ObjTotest(21,0) = "Geocel.Mailer"
	ObjTotest(21,1) = "(Geocel 发信)"
ObjTotest(22,0) = "IISmail.Iismail.1"
	ObjTotest(22,1) = "(IISmail 发信)"
ObjTotest(23,0) = "SmtpMail.SmtpMail.1"
	ObjTotest(23,1) = "(SmtpMail 发信)"
	
ObjTotest(24,0) = "SoftArtisans.ImageGen"
	ObjTotest(24,1) = "(SA 的图像读写组件)"
ObjTotest(25,0) = "W3Image.Image"
	ObjTotest(25,1) = "(Dimac 的图像读写组件)"

public IsObj,VerObj

'检查预查组件支持情况及版本

dim i
for i=0 to 25
	on error resume next
	IsObj=false
	VerObj=""
	dim TestObj
	set TestObj=server.CreateObject(ObjTotest(i,0))
	If -2147221005 <> Err then		'感谢网友的宝贵建议
		IsObj = True
		VerObj = TestObj.version
		if VerObj="" or isnull(VerObj) then VerObj=TestObj.about
	end if
	ObjTotest(i,2)=IsObj
	ObjTotest(i,3)=VerObj
next

'检查组件是否被支持及组件版本的子程序
sub ObjTest(strObj)
	on error resume next
	IsObj=false
	VerObj=""
	dim TestObj
	set TestObj=server.CreateObject (strObj)
	If -2147221005 <> Err then		'感谢网友5757的宝贵建议
		IsObj = True
		VerObj = TestObj.version
		if VerObj="" or isnull(VerObj) then VerObj=TestObj.about
	end if	
End sub
%>
<HTML><HEAD><TITLE>ASP探针</TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link href="images/style.css" rel="stylesheet" type="text/css">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style></HEAD>
<BODY>
<table class="tableBorder" width="90%" border="0" align="center" cellpadding="3" cellspacing="1" bgcolor="#FFFFFF">
<tr> 
<td class="forumRowHighlight" align="center" background="../images/admin_bg_1.gif"><b><font color="#ffffff">服务器的有关参数</font></b></td>
</tr>
<tr> 
<td class="forumRowHighlight" align="center" >
		<table width=500 border=1 cellpadding=0 cellspacing=0 style="border-collapse: collapse" bordercolor="#6B8FC8">
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器名</td>
            <td class="forumRowHighlight" >&nbsp;<%=Request.ServerVariables("SERVER_NAME")%></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器IP</td>
            <td class="forumRowHighlight" >&nbsp;<%=Request.ServerVariables("LOCAL_ADDR")%></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器端口</td>
            <td class="forumRowHighlight" >&nbsp;<%=Request.ServerVariables("SERVER_PORT")%></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器时间</td>
            <td class="forumRowHighlight" >&nbsp;<%=now%></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;IIS版本</td>
            <td class="forumRowHighlight" >&nbsp;<%=Request.ServerVariables("SERVER_SOFTWARE")%></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;脚本超时时间</td>
            <td class="forumRowHighlight" >&nbsp;<%=Server.ScriptTimeout%> 秒</td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;本文件路径</td>
            <td class="forumRowHighlight" >&nbsp;<%=server.mappath(Request.ServerVariables("SCRIPT_NAME"))%></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器CPU数量</td>
            <td class="forumRowHighlight" >&nbsp;<%=Request.ServerVariables("NUMBER_OF_PROCESSORS")%> 个</td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器解译引擎</td>
            <td class="forumRowHighlight" >&nbsp;<%=ScriptEngine & "/"& ScriptEngineMajorVersion &"."&ScriptEngineMinorVersion&"."& ScriptEngineBuildVersion %></td>
          </tr>
          <tr  height=18>
            <td class="forumRowHighlight" align=left>&nbsp;服务器操作系统</td>
            <td class="forumRowHighlight" >&nbsp;<%=Request.ServerVariables("OS")%></td>
          </tr>
        </table>
		<%
	Dim strClass
	strClass = Trim(Request.Form("classname"))
	If "" <> strClass then
	Response.Write "<br>您指定的组件的检查结果："
	ObjTest(strClass)
	  If Not IsObj then 
		Response.Write "<br><font color=red>很遗憾，该服务器不支持 " & strclass & " 组件！</font>"
	  Else
		Response.Write "<br><font class=fonts>恭喜！该服务器支持 " & strclass & " 组件。该组件版本是：" & VerObj & "</font>"
	  End If
	  Response.Write "<br>"
	end if
	%>
		◇ IIS自带的ASP组件 ◇
        <table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#6B8FC8" width="500">
          <tr height=18 class=backs align=center> 
            <td class="forumRowHighlight" width=320>组 件 名 称</td>
            <td class="forumRowHighlight" width=130>支持及版本</td>
          </tr>
          <%For i=0 to 10%>
          <tr height="18" class=backq> 
            <td class="forumRowHighlight" align=left>&nbsp;<%=ObjTotest(i,0) & "<font color=#888888>&nbsp;" & ObjTotest(i,1)%></td>
            <td class="forumRowHighlight" align=left>&nbsp; 
              <%
		If Not ObjTotest(i,2) Then 
			Response.Write "<font color=red><b>×</b></font>"
		Else
			Response.Write "<font class=fonts><b>√</b></font> <a title='" & ObjTotest(i,3) & "'>" & left(ObjTotest(i,3),11) & "</a>"
		End If%>
            </td>
          </tr>
          <%next%>
        </table>
        ◇ 常见的文件上传和管理组件 ◇  
        <table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#6B8FC8" width="500">
          <tr height=18 class=backs align=center> 
            <td class="forumRowHighlight" width=320>组 件 名 称</td>
            <td class="forumRowHighlight" width=130>支持及版本</td>
          </tr>
          <%For i=11 to 15%>
          <tr height="18" class=backq> 
            <td class="forumRowHighlight" align=left>&nbsp;<%=ObjTotest(i,0) & "<font color=#888888>&nbsp;" & ObjTotest(i,1)%></td>
            <td class="forumRowHighlight" align=left>&nbsp; 
              <%
		If Not ObjTotest(i,2) Then 
			Response.Write "<font color=red><b>×</b></font>"
		Else
			Response.Write "<font class=fonts><b>√</b></font> <a title='" & ObjTotest(i,3) & "'>" & left(ObjTotest(i,3),11) & "</a>"
		End If%>
            </td>
          </tr>
          <%next%>
        </table>
        ◇ 常见的收发邮件组件 ◇ 
        <table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#6B8FC8" width="500">
          <tr height=18 class=backs align=center> 
            <td class="forumRowHighlight" width=320>组 件 名 称</td>
            <td class="forumRowHighlight" width=130>支持及版本</td>
          </tr>
          <%For i=16 to 23%>
          <tr height="18" class=backq> 
            <td class="forumRowHighlight" align=left>&nbsp;<%=ObjTotest(i,0) & "<font color=#888888>&nbsp;" & ObjTotest(i,1)%></td>
            <td class="forumRowHighlight" align=left>&nbsp; 
              <%
		If Not ObjTotest(i,2) Then 
			Response.Write "<font color=red><b>×</b></font>"
		Else
			Response.Write "<font class=fonts><b>√</b></font> <a title='" & ObjTotest(i,3) & "'>" & left(ObjTotest(i,3),11) & "</a>"
		End If%>
            </td>
          </tr>
          <%next%>
        </table>
		◇ 图像处理组件 ◇
        <table border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#6B8FC8" width="500">
          <tr height=18 class=backs align=center> 
            <td class="forumRowHighlight" width=320>组 件 名 称</td>
            <td class="forumRowHighlight" width=130>支持及版本</td>
          </tr>
          <%For i=24 to 25%>
          <tr height="18" class=backq> 
            <td class="forumRowHighlight" height="22" align=left>&nbsp;<%=ObjTotest(i,0) & "<font color=#888888>&nbsp;" & ObjTotest(i,1)%></td>
            <td class="forumRowHighlight" align=left>&nbsp; 
              <%
		If Not ObjTotest(i,2) Then 
			Response.Write "<font color=red><b>×</b></font>"
		Else
			Response.Write "<font class=fonts><b>√</b></font> <a title='" & ObjTotest(i,3) & "'>" & left(ObjTotest(i,3),11) & "</a>"
		End If%>
            </td>
          </tr>
          <%next%>
        </table>
      ◇ ASP脚本解释和运算速度测试 ◇<br>
      <table class=backq border="1" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#6B8FC8" width="499">
          <tr height=18 class=backs align=center> 
            <td class="forumRowHighlight" width=351>服&nbsp;&nbsp;&nbsp;务&nbsp;&nbsp;&nbsp;器</td>
            <td class="forumRowHighlight" width=142>完成时间</td>
          </tr>
          <form action="<%=Request.ServerVariables("SCRIPT_NAME")%>" method=post>
            <%
	
	dim t1,t2,lsabc,thetime
	t1=timer
	for i=1 to 500000
		lsabc= 1 + 1
	next
	t2=timer

	thetime=cstr(int(( (t2-t1)*10000 )+0.5)/10)
%>
            <tr height=18> 
              <td class="forumRowHighlight" align=left width="351">&nbsp;<font color=red>您正在使用的这台服务器</font>&nbsp;</td>
              <td class="forumRowHighlight" width="142">&nbsp;<font color=red><%=thetime%> 毫秒</font></td>
            </tr>
          </form>
        </table>
      </div>    </td>
  </tr>
</table>
</BODY>
</HTML>