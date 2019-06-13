<%
Sub goPage(pagecount,page,pz)
maxpage=pagecount/pz
maxpage=clng(maxpage)
if maxpage<pagecount/pz then
	maxpage=maxpage+1
end if
	Dim query, a, x, temp
	action = Request.ServerVariables("SCRIPT_NAME")
	query = Split(Request.ServerVariables("QUERY_STRING"), "&")
	For Each x In query
	    a = Split(x, "=")
	    If StrComp(a(0), "page", vbTextCompare) <> 0 Then
	        temp = temp & a(0) & "=" & a(1) & "&"
	    End If
	Next
Response.Write("<table>" & vbCrLf )		
Response.Write("<form method=get onsubmit=""document.location = '" & action & "?" & temp & "Page='+ this.page.value;return false;""><TR>" & vbCrLf )
Response.Write("<TD align=right>" & vbCrLf )
Response.Write("记录数:"&pagecount&  vbCrLf)
Response.Write("共" & maxpage & "页 "&pz&"条/页 " & vbCrLf)
	if page<5 then
		Response.Write (" <font face=webdings>9</font> " & vbCrLf)		
	else		
		Response.Write(" <A HREF=" & action & "?" & temp & "Page=1 target=""_self"" title=首页><font face=webdings>9</font></A> " & vbCrLf)
	end if
pagep=page-4
pagen=page+5
if page<5 or page="" then
	pagep=1
	pagen=page+(10-page)
end if
if page<5 then
	Response.Write(" <font face=webdings>7</font> [" & vbCrLf)
else
	Response.Write(" <A HREF=" & action & "?" & temp & "Page=" & (page-10) & " target=""_self"" title=上十页><font face=webdings>7</font></A> [" & vbCrLf)
end if
for j=pagep to pagen
	if page=j then
		pa="<font color=#FF0000><strong>"&j&"</strong></font>"
	else
		pa=j
	end if
	Response.Write("<A HREF=" & action & "?" & temp & "Page=" & (j) & " target=""_self"">"&pa&"</A>" & vbCrLf)
	if j=maxpage then 
	j=j+1
	exit for
	end if
next
u=10
if maxpage-page<10 then
	u=maxpage-page
end if
if j-1=maxpage then
	Response.Write("] <font face=webdings>8</font> " & vbCrLf)
else
	Response.Write("] <A HREF=" & action & "?" & temp & "Page=" & (page+u) & " target=""_self"" title=下十页><font face=webdings>8</font></A> " & vbCrLf)
end if
if page>=maxpage then
	Response.Write (" <font face=webdings>:</font>" & vbCrLf)
else
	Response.Write(" <A HREF=" & action & "?" & temp & "Page=" & maxpage & " target=""_self"" title=尾页><font face=webdings>:</font></A> " & vbCrLf)			
end if
response.write("<select name=page>")
for i=1 to maxpage
%>
<option value="<%=i%>" <%if i=page then%>selected<%end if%>><%=i%></option>
<%
next
response.write("</select> <input type=submit name=Submit value=GO>")
	Response.Write("</TD>" & vbCrLf )				
	Response.Write("</TR></form>" & vbCrLf )		
	Response.Write("</table>" & vbCrLf )		
End Sub
%>
