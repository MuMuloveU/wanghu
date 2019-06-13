<!--#include file = "Startup.asp"-->
<%

' ======================
' 功能：修改新闻保存页
' 描述：对modify.asp文件提交过来的表单数据进行保存操作，有保存新闻标题，新闻内容，新闻标题图片；
'       同时保存所有此篇新闻所有相关的上传或远程获取的文件信息，有源文件名，保存文件名，保存路径文件名。
' ======================

Call Header("修改新闻保存")
Call Content()
Call Footer()


' 本页内容区
Sub Content()
	Dim i

	' 传入参数：新闻ID
	Dim sNewsID
	sNewsID = Trim(Request("id"))

	' 新闻ID有效性验证，防止有些人恶意的破坏此演示程序
	If IsNumeric(sNewsID) = False Then
		GoError "请通过页面上的链接进行操作，不要试图破坏此演示系统。"
	End If

	' 取提交过来的数据
	' 注意取新闻内容的方法，因为对大表单的自动处理，一定要使用循环，否则大于100K的内容将取不到，单个表单项的限制为102399字节（100K左右）
	Dim sTitle, sContent, sPicture
	sTitle = Request.Form("d_title")
	sPicture = Request.Form("d_picture")


	' 开始：eWebEditor编辑区取值-----------------
	sContent = ""
	For i = 1 To Request.Form("d_content").Count
		sContent = sContent & Request.Form("d_content")(i)
	Next
	' 结束：eWebEditor编辑区取值-----------------	


	' 以下为所有通过编辑器上传的所有文件相关信息，包括编辑区手动上传的和自动远程上传的
	' GetSafeStr函数为过滤一些特殊字符，防止有些人恶意的破坏此演示程序
	' 上传或远程获取前的原文件名，多个以"|"分隔
	Dim sOriginalFileName
	' 上传后保存到本地服务器的文件名（不带路径），多个以"|"分隔
	Dim sSaveFileName
	' 上传后保存到本地服务器的路径文件名，多个以"|"分隔
	Dim sSavePathFileName
	sOriginalFileName = GetSafeStr(Request.Form("d_originalfilename"))
	sSaveFileName = GetSafeStr(Request.Form("d_savefilename"))
	sSavePathFileName = GetSafeStr(Request.Form("d_savepathfilename"))


	' 修改指定新闻内容
	sSql = "SELECT * FROM NewsData WHERE D_ID=" & sNewsID
	oRs.Open sSql, oConn, 1, 3
	If Not oRs.Eof Then
		oRs("D_Title") = sTitle
		oRs("D_Content") = sContent
		oRs("D_Picture") = sPicture
		oRs("D_OriginalFileName") = sOriginalFileName
		oRs("D_SaveFileName") = sSaveFileName
		oRs("D_SavePathFileName") = sSavePathFileName
		oRs.Update
	Else
		GoError "无效的新闻ID，请点页面上的链接进行操作！"
	End If
	oRs.Close

	' 输出成功保存信息
	Response.Write "新闻（ID：" & sNewsID & "）修改保存成功！"

End Sub

%>

