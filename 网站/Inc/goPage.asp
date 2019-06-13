<%
Sub goPage(pagecount,page)
	Dim query, a, x, temp
	action = Request.ServerVariables("SCRIPT_NAME")

	query = Split(Request.ServerVariables("QUERY_STRING"), "&")
	For Each x In query
	    a = Split(x, "=")
	    If StrComp(a(0), "page", vbTextCompare) <> 0 Then
	        temp = temp & a(0) & "=" & a(1) & "&"
	    End If
	Next

	Response.Write("<table " & Table_style & ">" & vbCrLf )		
	Response.Write("<form method=get onsubmit=""document.location = '" & action & "?" & temp & "Page='+ this.page.value;return false;""><TR>" & vbCrLf )
	Response.Write("<TD align=right>" & vbCrLf )
	Response.Write(font_style & vbCrLf )	
		
	if page<=1 then
		Response.Write ("[首页] " & vbCrLf)		
		Response.Write ("[上页] " & vbCrLf)
	else		
		Response.Write("[<A HREF=" & action & "?" & temp & "Page=1>首页</A>] " & vbCrLf)
		Response.Write("[<A HREF=" & action & "?" & temp & "Page=" & (Page-1) & ">上页</A>] " & vbCrLf)
	end if

	if page>=pagecount then
		Response.Write ("[下页] " & vbCrLf)
		Response.Write ("[尾页]" & vbCrLf)			
	else
		Response.Write("[<A HREF=" & action & "?" & temp & "Page=" & (Page+1) & ">下页</A>] " & vbCrLf)
		Response.Write("[<A HREF=" & action & "?" & temp & "Page=" & pagecount & ">尾页</A>]" & vbCrLf)			
	end if
		
	Response.Write(" 第" & "<INPUT TYEP=TEXT NAME=page SIZE=2 Maxlength=5 VALUE=" & page & " style=""font-size: 9pt"">" & "页"  & vbCrLf & "<INPUT type=submit style=""font-size: 7pt"" value=GO>")
	Response.Write(" 共 " & pageCount & " 页" &  vbCrLf)			'pageCount是调用页面中赋值总页数
	Response.Write("</TD>" & vbCrLf )				
	Response.Write("</TR></form>" & vbCrLf )		
	Response.Write("</table>" & vbCrLf )		
End Sub
%>