<%@ Language=VBScript CODEPAGE=936%>
<% Option Explicit %>
<%
'☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆
'★                                                                  ★
'☆                eWebEditor - eWebSoft在线编辑器                   ☆
'★                                                                  ★
'☆  版权所有: eWebSoft.com                                          ☆
'★                                                                  ★
'☆  程序制作: eWeb开发团队                                          ☆
'★            email:webmaster@webasp.net                            ★
'☆            QQ:589808                                             ☆
'★                                                                  ★
'☆  相关网址: [产品介绍]http://www.eWebSoft.com/Product/eWebEditor/ ☆
'★            [支持论坛]http://bbs.eWebSoft.com/                    ★
'☆                                                                  ☆
'★  主页地址: http://www.eWebSoft.com/   eWebSoft团队及产品         ★
'☆            http://www.webasp.net/     WEB技术及应用资源网站      ☆
'★            http://bbs.webasp.net/     WEB技术交流论坛            ★
'★                                                                  ★
'☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆
%>

<%

' ============================================
' 常用全局变量
' ============================================
' 数据库对象
Dim oConn, oRs, sSql


' ============================================
' 初始数据处理
' ============================================
' 初始化数据库连接
Call DBConnBegin()


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
	oConn.Open "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=" & Server.MapPath("db.mdb")
	
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
Sub GoError(str)
	Call DBConnEnd()
	Response.Write "<script language=javascript>alert('" & str & "\n\n系统将自动返回前一页面...');history.back();</script>"
	Response.End
End Sub

' ============================================
' 得到安全字符串,在查询中或有必要强行替换的表单中使用
' ============================================
Function GetSafeStr(str)
	GetSafeStr = Replace(Replace(Replace(Trim(str), "'", ""), Chr(34), ""), ";", "")
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

' ===============================================
' 初始化下拉框
'	s_FieldName	: 返回的下拉框名	
'	a_Name		: 定值名数组
'	a_Value		: 定值值数组
'	v_InitValue	: 初始值
'	s_Sql		: 从数据库中取值时,select name,value from table
'	s_AllName	: 空值的名称,如:"全部","所有","默认"
' ===============================================
Function InitSelect(s_FieldName, a_Name, a_Value, v_InitValue, s_Sql, s_AllName)
	Dim i
	InitSelect = "<select name='" & s_FieldName & "' size=1>"
	If s_AllName <> "" Then
		InitSelect = InitSelect & "<option value=''>" & s_AllName & "</option>"
	End If
	If s_Sql <> "" Then
		oRs.Open s_Sql, oConn, 0, 1
		Do While Not oRs.Eof
			InitSelect = InitSelect & "<option value=""" & inHTML(oRs(1)) & """"
			If oRs(1) = v_InitValue Then
				InitSelect = InitSelect & " selected"
			End If
			InitSelect = InitSelect & ">" & outHTML(oRs(0)) & "</option>"
			oRs.MoveNext
		Loop
		oRs.Close
	Else
		For i = 0 To UBound(a_Name)
			InitSelect = InitSelect & "<option value=""" & inHTML(a_Value(i)) & """"
			If a_Value(i) = v_InitValue Then
				InitSelect = InitSelect & " selected"
			End If
			InitSelect = InitSelect & ">" & outHTML(a_Name(i)) & "</option>"
		Next
	End If
	InitSelect = InitSelect & "</select>"
End Function

' ============================================
' 每页头部内容
' ============================================
Sub Header(str)
	Response.Write "<HTML><HEAD><TITLE>eWebEditor在线编辑器 - 例子：新闻系统</TITLE><style>body,p,td,input {font-size:9pt}</style></HEAD><BODY>"
	Response.Write "<p align=center><a href='list.asp'>新闻列表</a> | <a href='add.asp'>增加新闻</a></p>"
	Response.Write "<p align=center><b>" & str & "</b></p>"
End Sub

' ============================================
' 每页底部内容
' ============================================
Sub Footer()
	Call DBConnEnd()
	Response.Write "</BODY></HTML>"
End Sub

%>