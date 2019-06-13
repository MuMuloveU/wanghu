<!--#include file = "Startup.asp"-->

<%

' ======================
' 功能：删除新闻
' 描述：新闻删除后，页面转向新闻列表页。
'       删除新闻的同时，删除此新闻相关的上传文件。
' ======================

Call Header("删除新闻")
Call Content()
Call Footer()


' 本页内容区
Sub Content()

	' 取参数：新闻ID
	Dim sNewsID
	sNewsID = Trim(Request("id"))

	' 新闻ID有效性验证，防止有些人恶意的破坏此演示程序
	If IsNumeric(sNewsID) = False Then
		GoError "请通过页面上的链接进行操作，不要试图破坏此演示系统。"
	End If

	' 从新闻数据表中取出相关的上传文件
	' 上传后保存到本地服务器的路径文件名，多个以"|"分隔
	' 删除文件，要取带路径的文件名才可以，并且只要这个就可以了，原来存的原文件名或不带路径的保存文件名可用于其它地方使用
	Dim sSavePathFileName
	
	sSql = "SELECT D_SavePathFileName FROM NewsData WHERE D_ID=" & sNewsID
	oRs.Open sSql, oConn, 0, 1
	If Not oRs.Eof Then
		sSavePathFileName = oRs("D_SavePathFileName")
	Else
		GoError "无效的新闻ID，请点页面上的链接进行操作！"
	End If
	oRs.Close

	' 把带"|"的字符串转为数组
	Dim aSavePathFileName
	aSavePathFileName = Split(sSavePathFileName, "|")

	' 删除新闻相关的文件，从文件夹中
	Dim i
	For i = 0 To UBound(aSavePathFileName)
		' 按路径文件名删除文件
		Call DoDelFile(aSavePathFileName(i))
	Next

	' 删除新闻
	sSql = "DELETE FROM NewsData WHERE D_ID=" & sNewsID
	oConn.Execute sSql

	' 3秒转向新闻列表页
	response.write "<p align=center>新闻删除成功，3秒后自动返回新闻列表页！<script>window.setTimeout(""location.href='list.asp'"",3000);</script></p>"

End Sub

' 删除指定的文件
Sub DoDelFile(sPathFile)
	On Error Resume Next
	Dim oFSO
	Set oFSO = Server.CreateObject("Scripting.FileSystemObject")
	oFSO.DeleteFile(Server.MapPath(sPathFile))
	Set oFSO = Nothing
End Sub

%>

