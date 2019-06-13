<!--#include file = "Startup.asp"-->
<!--#include file = "../../Include/DeCode.asp"-->

<%

' ======================
' 功能：显示新闻
' 描述：显示编辑的内容页，此页注意一下DeCode接口函数的调用。
' ======================

Call Header("显示新闻内容")
Call Content()
Call Footer()


' 本页内容区
Sub Content()

	' 传入参数：新闻ID
	Dim sNewsID
	sNewsID = Trim(Request("id"))

	' 新闻ID有效性验证，防止有些人恶意的破坏此演示程序
	If IsNumeric(sNewsID) = False Then
		GoError "请通过页面上的链接进行操作，不要试图破坏此演示系统。"
	End If

	' 从数据库中取初始值
	Dim sTitle, sContent, sPicture, sOriginalFileName, sSaveFileName, sSavePathFileName
	sSql = "SELECT * FROM NewsData WHERE D_ID=" & sNewsID
	oRs.Open sSql, oConn, 0, 1
	If Not oRs.Eof Then
		sTitle = oRs("D_Title")
		sContent = oRs("D_Content")
		sPicture = oRs("D_Picture")
		sOriginalFileName = oRs("D_OriginalFileName")
		sSaveFileName = oRs("D_SaveFileName")
		sSavePathFileName = oRs("D_SavePathFileName")
	Else
		GoError "无效的新闻ID，请点页面上的链接进行操作！"
	End If
	oRs.Close

	' 禁用某些标签，如出于安全考虑的Script标签，等
	' 要使用此功能需要先包含"Include/DeCode.asp"文件。
	' 此例只过滤SCRIPT标签，即意味着内容中的客户端脚本不会生效，您可根据实际的需要加入其它标签。
	' 当前支持过滤的标签，可以查看DeCode.asp文件中的说明。
	sContent = eWebEditor_DeCode(sContent, "SCRIPT")


	' 输出新闻
	Response.Write "<table border=0 cellpadding=5 width='90%' align=center>" & _
		"<tr><td align=center><b>" & sTitle & "</b></td></tr>" & _
		"<tr><td>" & sContent & "</td></tr>" & _
		"</table>"

	' 输出相关文件信息
	Response.Write "<p><b>此新闻的相关上传文件信息如下：</b></p>"

	' 把带"|"的字符串转为数组，用于列出显示
	Dim aOriginalFileName, aSaveFileName, aSavePathFileName
	aOriginalFileName = Split(sOriginalFileName, "|")
	aSaveFileName = Split(sSaveFileName, "|")
	aSavePathFileName = Split(sSavePathFileName, "|")

	Response.Write "<table border=1 cellpadding=3 cellspacing=0>" & _
		"<tr>" & _
			"<td>序号</td>" & _
			"<td>原文件名(接口：d_originalfilename)</td>" & _
			"<td>保存文件名(接口：d_savefilename)</td>" & _
			"<td>保存路径文件名(接口：d_savepathfilename)</td>" & _
		"</tr>"
	Dim i
	For i = 0 To UBound(aOriginalFileName)
		Response.Write "<tr>" & _
				"<td>" & CStr(i + 1) & "</td>" & _
				"<td>" & aOriginalFileName(i) & "</td>" & _
				"<td>" & aSaveFileName(i) & "</td>" & _
				"<td>" & aSavePathFileName(i) & "</td>" & _
			"</tr>"
	Next
	Response.Write "</table>"

End Sub

%>
  fa