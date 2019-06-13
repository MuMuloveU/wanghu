<%
 if request.Cookies("admin99aw")="" or request.Cookies("flag")="" then
response.write "<script language='javascript'>"
response.write"parent.location.href='\admin_duocai/admin_login.asp';</SCRIPT>" 
response.end
end if
%>
<link href="../inc/STYLE.CSS" rel="stylesheet" type="text/css"> 
<%
Dim sAction, sPosition
sAction = UCase(Trim(Request.QueryString("action")))
Sub Footer()
	Response.Write "<table border=0 cellpadding=0 cellspacing=0 width=100% align=center height=23>"
	Response.Write "<table border=0 cellpadding=0 cellspacing=3 align=center>"
	Response.Write "</body></html>"
End Sub
sPosition = sPosition & "上传文件管理"
Call Content()
Call Footer()
Sub Content()
	If IsObjInstalled("Scripting.FileSystemObject") = False Then
		Response.Write "此功能要求服务器支持文件系统对象（FSO），而你当前的服务器不支持！"
		Exit Sub
	End If
	Select Case sAction
	Case "DELALL"
		Call DoDelAll()
	Case "DEL"
		Call DoDel()
	End Select
	Call ShowList()
End Sub
Sub ShowList()
	Response.Write "<p align=center><b>以下即bookpic目录下的所有文件列表：</b></p>"
	Response.Write "<table border=0 cellpadding=0 cellspacing=0 class=list1>" & _
		"<form action='?action=del' method=post name=myform>" & _
		"<tr align=center>" & _
			"<th width=140>缩略图</th>" & _
			"<th width=100>大小</th>" & _
			"<th width=130>最后访问</th>" & _
			"<th width=130>上传日期</th>" & _
			"<th width=30>删除</th>" & _
		"</tr>"
	Dim sCurrPage, nCurrPage, nFileNum, nPageNum, nPageSize
	sCurrPage = Trim(Request("page"))
	nPageSize = 20
	If sCurrpage = "" Or Not IsNumeric(sCurrPage) Then
		nCurrPage = 1
	Else
		nCurrPage = CLng(sCurrPage)
	End If
	Dim oFSO, oUploadFolder, obookpics, obookpic, sFileName
	Set oFSO = Server.CreateObject("Scripting.FileSystemObject")
	Set oUploadFolder = oFSO.GetFolder(Server.MapPath("bookpic\"))
	Set obookpics = oUploadFolder.Files
	nFileNum = obookpics.Count
	nPageNum = Int(nFileNum / nPageSize)
	If nFileNum Mod nPageSize > 0 Then
		nPageNum = nPageNum+1
	End If
	If nCurrPage > nPageNum Then
		nCurrPage = 1
	end If
	Dim i
	i = 0
	For Each obookpic In obookpics
		i = i + 1
		If i > (nCurrPage - 1) * nPageSize And i <= nCurrPage * nPageSize Then
			sFileName = obookpic.Name
			Response.Write "<tr align=center>" & _
				"<td align=center><a href=""bookpic/" & sFileName & """ target=_blank><img src=bookpic/" & sFileName & " height=40 border=0></a></td>" & _
				"<td>" & obookpic.size & " B </td>" & _
				"<td>" & obookpic.datelastaccessed & "</td>" & _
				"<td>" & obookpic.datecreated & "</td>" & _
				"<td><input type=checkbox name=delfilename value=""" & sFileName & """></td></tr><tr><td height=10></td></tr>"
		Elseif i > nCurrPage * nPageSize Then
			Exit For
		End If
	Next
	Set oUploadFolder = Nothing
	Set obookpics = Nothing
	If nFileNum <= 0 Then
		Response.Write "<tr><td colspan=6>bookpic目录下现在还没有文件！</td></tr>"
	End If
	Response.Write "</table>"
	If nFileNum > 0 Then
		' 分页
		Response.Write "<table border=0 cellpadding=3 cellspacing=0 width='100%'><tr><td>"
		If nCurrPage > 1 Then
			Response.Write "<a href='?page=1'>首页</a>&nbsp;&nbsp;<a href='?page="& nCurrPage - 1 & "'>上一页</a>&nbsp;&nbsp;"
		Else
			Response.Write "首页&nbsp;&nbsp;上一页&nbsp;&nbsp;"
		End If
		If nCurrPage < i / nPageSize Then
			Response.Write "<a href='?page=" & nCurrPage + 1 & "'>下一页</a>&nbsp;&nbsp;<a href='?page=" & nPageNum & "'>尾页</a>"
		Else
			Response.Write "下一页&nbsp;&nbsp;尾页"
		End If
		Response.Write "&nbsp;&nbsp;&nbsp;&nbsp;共<b>" & nFileNum & "</b>个&nbsp;&nbsp;页次:<b><span class=highlight2>" & nCurrPage & "</span>/" & nPageNum & "</b>&nbsp;&nbsp;<b>" & nPageSize & "</b>个文件/页"
		Response.Write "</td></tr></table>"
	End If
	Response.Write "<p align=center><input type=submit name=b value='删除' class=in> <input type=button name=b1 value='清空' class=in onclick=""javascript:if (confirm('你确定要清空所有文件吗？')) {location.href='admin_bookpic.asp?action=delall';}""></p></form>"
End Sub
Sub DoDel()
	Dim sFileName, oFSO, sMapFileName
	Set oFSO = Server.CreateObject("Scripting.FileSystemObject")
	For Each sFileName In Request.Form("delfilename")
		sMapFileName = Server.MapPath("bookpic/" & sFileName)
		If oFSO.FileExists(sMapFileName) Then
			oFSO.DeleteFile(sMapFileName)
		End If
	Next
	Set oFSO = Nothing
End Sub
Sub DoDelAll()
	On Error Resume Next
	Dim sFileName, oFSO, sMapFileName, oFolder, oFiles, oFile
	Set oFSO = Server.CreateObject("Scripting.FileSystemObject")
	Set oFolder = oFSO.GetFolder(Server.MapPath("bookpic\"))
	Set oFiles = oFolder.Files
	For Each oFile In oFiles
		sFileName = oFile.Name
		sMapFileName = Server.MapPath("bookpic/" & sFileName)
		If oFSO.FileExists(sMapFileName) Then
			oFSO.DeleteFile(sMapFileName)
		End If
	Next
	Set oFile = Nothing
	Set oFolder = Nothing
	Set oFSO = Nothing
End Sub
Function IsObjInstalled(strClassString)
	On Error Resume Next
	IsObjInstalled = False
	Err = 0
	Dim xTestObj
	Set xTestObj = Server.CreateObject(strClassString)
	If 0 = Err Then IsObjInstalled = True
	Set xTestObj = Nothing
	Err = 0
End Function
Function FileName2Pic(sFileName)
	Dim sExt, sPicName
	sExt = UCase(Mid(sFileName, InstrRev(sFileName, ".")+1))
	Select Case sExt
	Case "TXT"
		sPicName = "txt.gif"
	Case "CHM", "HLP"
		sPicName = "hlp.gif"
	Case "DOC"
		sPicName = "doc.gif"
	Case "PDF"
		sPicName = "pdf.gif"
	Case "MDB"
		sPicName = "mdb.gif"
	Case "GIF", "JPG", "PNG", "BMP"
		sPicName = "pic.gif"
	Case "ASP", "JSP", "JS", "PHP", "PHP3", "ASPX"
		sPicName = "code.gif"
	Case "HTM", "HTML", "SHTML"
		sPicName = "htm.gif"
	Case "ZIP", "RAR"
		sPicName = "zip.gif"
	Case "EXE"
		sPicName = "exe.gif"
	Case "AVI", "MPG", "MPEG", "ASF"
		sPicName = "mp.gif"
	Case "RA", "RM"
		sPicName = "rm.gif"
	Case "MID", "WAV", "MP3", "MIDI"
		sPicName = "audio.gif"
	Case "XLS"
		sPicName = "xls.gif"
	Case "PPT", "PPS"
		sPicName = "ppt.gif"
	Case Else
		sPicName = "unknow.gif"
	End Select
	FileName2Pic = "<img border=0 src='sysimage/file/" & sPicName & "'>"
End Function
%><style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style>