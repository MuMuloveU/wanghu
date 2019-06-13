<%@ Language=VBScript CODEPAGE=936%>
<!--#include file="../../admin_duocai/chak.asp" -->
<%
Dim oConn, oRs, sSql
Sub BrandNewDay()
	Dim sDate, y, m, d, w
	Dim sDateChinese
	sDate = Date()
	If Application("date_today") = sDate Then Exit Sub

	y = CStr(Year(sDate))
	m = CStr(Month(sDate))
	If Len(m) = 1 Then m = "0" & m
	d = CStr(Day(sDate))
	If Len(d) = 1 Then d = "0" & d
	w = WeekdayName(Weekday(sDate))
	sDateChinese = y & "年" & m & "月" & d & "日&nbsp;" & w

	Application.Lock
	Application("date_today") = sDate
	Application("date_chinese") = sDateChinese		'今天的中文样式
	Application.Unlock
End Sub
Sub DBConnBegin()
	If IsObject(oConn) = True Then Exit Sub
	Set oConn = Server.CreateObject("ADODB.Connection")
	On Error Resume Next
	oConn.Open "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=" & Server.MapPath("che~!@@##@@/##`!@#@serA5#g#@%&(.asa")
	If Err.Number > 0 Then
		Response.End
	End If
	Set oRs = Server.CreateObject( "ADODB.Recordset" )
End Sub
Sub DBConnEnd()
	On Error Resume Next
	oRs.Close
	Set oRs = Nothing
	oConn.Close
	Set oConn = Nothing
End Sub
Sub Go_Error(str)
	Call DBConnEnd()
	Response.Write "<script language=javascript>alert('" & str & "\n\n系统将自动返回前一页面...');history.back();</script>"
	Response.End
End Sub
Function Format_Time(s_Time, n_Flag)
	Dim y, m, d, h, mi, s
	Format_Time = ""
	If IsDate(s_Time) = False Then Exit Function
	y = cstr(year(s_Time))
	m = cstr(month(s_Time))
	If len(m) = 1 Then m = "0" & m
	d = cstr(day(s_Time))
	If len(d) = 1 Then d = "0" & d
	h = cstr(hour(s_Time))
	If len(h) = 1 Then h = "0" & h
	mi = cstr(minute(s_Time))
	If len(mi) = 1 Then mi = "0" & mi
	s = cstr(second(s_Time))
	If len(s) = 1 Then s = "0" & s
	Select Case n_Flag
	Case 1
		Format_Time = y & "-" & m & "-" & d & " " & h & ":" & mi & ":" & s
	Case 2
		Format_Time = y & "-" & m & "-" & d
	Case 3
		Format_Time = h & ":" & mi & ":" & s
	Case 4
		Format_Time = y & "年" & m & "月" & d & "日"
	Case 5
		Format_Time = y & m & d
	End Select
End Function
Function outHTML(str)
	Dim sTemp
	sTemp = str
	outHTML = ""
	If IsNull(sTemp) = True Then
		Exit Function
	End If
	sTemp = Replace(sTemp, "&", "&amp;")
	sTemp = Replace(sTemp, "<", "&lt;")
	sTemp = Replace(sTemp, ">", "&gt;")
	sTemp = Replace(sTemp, Chr(34), "&quot;")
	sTemp = Replace(sTemp, Chr(10), "<br>")
	outHTML = sTemp
End Function
Function inHTML(str)
	Dim sTemp
	sTemp = str
	inHTML = ""
	If IsNull(sTemp) = True Then
		Exit Function
	End If
	sTemp = Replace(sTemp, "&", "&amp;")
	sTemp = Replace(sTemp, "<", "&lt;")
	sTemp = Replace(sTemp, ">", "&gt;")
	sTemp = Replace(sTemp, Chr(34), "&quot;")
	inHTML = sTemp
End Function
Function IsSelfRefer()
	Dim sHttp_Referer, sServer_Name
	sHttp_Referer = CStr(Request.ServerVariables("HTTP_REFERER"))
	sServer_Name = CStr(Request.ServerVariables("SERVER_NAME"))
	If Mid(sHttp_Referer, 8, Len(sServer_Name)) = sServer_Name Then
		IsSelfRefer = True
	Else
		IsSelfRefer = False
	End If
End Function
Function Get_SafeStr(str)
	Get_SafeStr = Replace(Replace(Replace(Trim(str), "'", ""), Chr(34), ""), ";", "")
End Function
Function Get_TrueLen(str)
	Dim l, t, c, i
	l = Len(str)
	t = l
	For i = 1 To l
		c = Asc(Mid(str, i, 1))
		If c < 0 Then c = c + 65536
		If c > 255 Then t = t + 1
	Next
	Get_TrueLen = t
End Function
Function IsSafeStr(str)
	Dim s_BadStr, n, i
	s_BadStr = "' 　&<>?%,;:()`~!@#$^*{}[]|+-=" & Chr(34) & Chr(9) & Chr(32)
	n = Len(s_BadStr)
	IsSafeStr = True
	For i = 1 To n
		If Instr(str, Mid(s_BadStr, i, 1)) > 0 Then
			IsSafeStr = False
			Exit Function
		End If
	Next
End Function
%>