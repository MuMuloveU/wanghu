<%@ Language=VBScript CODEPAGE=936%>
<% Option Explicit %>


<%

' ============================================
' 常用全局变量
' ============================================
' 数据库对象
Dim oConn, oRs, sSql


' ============================================
' 初始数据处理
' ============================================
' 执行每天只需处理一次的事件
'Call BrandNewDay()

' 初始化数据库连接
'Call DBConnBegin()


' ********************************************
' 以下为初始函数
' ********************************************
' ============================================
' 执行每天只需处理一次的事件
' ============================================
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



' ********************************************
' 以下为数据库相关函数
' ********************************************
' ============================================
' 初始化数据库连接对象
' 使用原则：最迟调用，最早释放
' ============================================
Sub DBConnBegin()
	' 如果数据库对象已打开，不要再打开
	If IsObject(oConn) = True Then Exit Sub

	' 你可以不需要打开数据库连接对象而直接打开记录集对象，但如果你需要打开多个记录集对象的话，效率是很低的。
	' 如果你不创建一个数据库连接对象，ADO会在每个记录集打开时自动创建一个新的数据库连接对象，就算你用的是相同的SQL语句。
	Set oConn = Server.CreateObject("ADODB.Connection")

	On Error Resume Next
	' Access数据库
	oConn.Open "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=" & Server.MapPath("db/ewebeditor.mdb")
	' SQL Server 2000数据库
	'oConn.Open "Provider=SQLOLEDB.1;Server=localhost;UID=ewebeditor;PWD=123456;Database=ewebeditor"
	
	If Err.Number > 0 Then
		' 显示错误信息,并且发送邮件通知管理员
		'Call DBConnError(Err)
		
		' 完全地退出正在运行的脚本
		Response.End
	End If

	' 创建一个记录集
	Set oRs = Server.CreateObject( "ADODB.Recordset" )
End Sub

' ============================================
' 释放数据库连接对象
' ============================================
Sub DBConnEnd()
	On Error Resume Next
	oRs.Close
	Set oRs = Nothing
	oConn.Close
	Set oConn = Nothing
End Sub



' ********************************************
' 以下为常用函数
' ********************************************
' ============================================
' 错误返回处理
' ============================================
Sub Go_Error(str)
	Call DBConnEnd()
	Response.Write "<script language=javascript>alert('" & str & "\n\n系统将自动返回前一页面...');history.back();</script>"
	Response.End
End Sub

' ============================================
' 格式化时间(显示)
' 参数：n_Flag
'	1:"yyyy-mm-dd hh:mm:ss"
'	2:"yyyy-mm-dd"
'	3:"hh:mm:ss"
'	4:"yyyy年mm月dd日"
'	5:"yyyymmdd"
' ============================================
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
		' yyyy-mm-dd hh:mm:ss
		Format_Time = y & "-" & m & "-" & d & " " & h & ":" & mi & ":" & s
	Case 2
		' yyyy-mm-dd
		Format_Time = y & "-" & m & "-" & d
	Case 3
		' hh:mm:ss
		Format_Time = h & ":" & mi & ":" & s
	Case 4
		' yyyy年mm月dd日
		Format_Time = y & "年" & m & "月" & d & "日"
	Case 5
		' yyyymmdd
		Format_Time = y & m & d
	End Select
End Function

' ============================================
' 把字符串进行HTML解码,替换server.htmlencode
' 去除Html格式，用于显示输出
' ============================================
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

' ============================================
' 去除Html格式，用于从数据库中取出值填入输入框时
' 注意：value="?"这边一定要用双引号
' ============================================
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

' ============================================
' 检测上页是否从本站提交
' 返回:True,False
' ============================================
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

' ============================================
' 得到安全字符串,在查询中使用
' ============================================
Function Get_SafeStr(str)
	Get_SafeStr = Replace(Replace(Replace(Trim(str), "'", ""), Chr(34), ""), ";", "")
End Function

' ============================================
' 取实际字符长度
' ============================================
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

' ============================================
' 判断是否安全字符串,在注册登录等特殊字段中使用
' ============================================
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