<!--#include file = "Startup.asp"-->

<%

' ======================
' 功能：新闻列表
' 描述：本页代码没有使用到eWebEditor相关的内容，只提供新闻列表，及显示、修改、删除新闻页的链接。
' ======================

Call Header("新闻列表")
Call Content()
Call Footer()


' 本页内容区
Sub Content()

	' 从新闻库中取出所有新闻，按ID倒排序
	sSql = "SELECT * FROM NewsData ORDER BY D_ID DESC"
	oRs.Open sSql, oConn, 0, 1
	If Not oRs.Eof Then
		Do While Not oRs.Eof
			Response.Write "<li>"
			' 是否有新闻标题图片，如有则输出显示
			If oRs("D_Picture") <> "" Then
				Response.Write "<img border=0 src='" & oRs("D_Picture") & "'>"
			End If
			' 显示标题
			' 函数outHTML：去除Html格式，用于显示输出,具体请见startup.asp文件中的说明
			Response.Write "<a href='show.asp?id=" & oRs("d_id") & "'>" & outHTML(oRs("d_title")) & "</a>&nbsp;&nbsp;"
			' 显示修改链接
			Response.Write "[<a href='modify.asp?id=" & oRs("d_id") & "'>修改</a>]"
			' 显示删除链接
			Response.Write "[<a href='delete.asp?id=" & oRs("d_id") & "'>删除</a>]"
			oRs.MoveNext
		Loop
	Else
		Response.Write "现在数据库中还没有新闻！"
	End If
	oRs.Close

End Sub

%>
   e)