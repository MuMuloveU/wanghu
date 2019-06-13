<!--#include file="md5.asp"-->
<%
dim payerName,productName,orderAmount,bgUrl
payerName=server.URLEncode(replace(trim(request.Form("username1")),"'",""))
productName=server.URLEncode(replace(trim(request.Form("car")),"'",""))
productName1=replace(trim(request.Form("car")),"'","")
orderAmount=clng(replace(trim(request.Form("money")),"'",""))
orderAmount1=orderAmount*0.01
bgUrl="http://www.dc173.com/szxcar/receive.asp"
'*
'* @Description: 快钱神州行支付网关接口范例
'* @Copyright (c) 上海快钱信息服务有限公司
'* @version 2.0
'*


'神州行网关账户号
''请与快钱联系索取
merchantAcctId="1000781683604"

'设置人民币网关密钥
''区分大小写
key="XNXHGS3S4938F6X8"


'字符集.固定选择值。可为空。
''只能选择1、2、3.
''1代表UTF-8; 2代表GBK; 3代表gb2312
''默认值为1
inputCharset="3"

'接受支付结果的页面地址.与[bgUrl]不能同时为空。必须是绝对地址。
''如果[bgUrl]为空，快钱将支付结果Post到[pageUrl]对应的地址。
''如果[bgUrl]不为空，并且[bgUrl]页面指定的<redirecturl>地址不为空，则转向到<redirecturl>对应的地址
pageUrl=bgUrl
'服务器接受支付结果的后台地址.与[pageUrl]不能同时为空。必须是绝对地址。
''快钱通过服务器连接的方式将交易结果发送到[bgUrl]对应的页面地址，在商户处理完成后输出的<result>如果为1，页面会转向到<redirecturl>对应的地址。
''如果快钱未接收到<redirecturl>对应的地址，快钱将把支付结果post到[pageUrl]对应的页面。
bgUrl=""


'网关版本.固定值
''快钱会根据版本号来调用对应的接口处理程序。
''本代码版本号固定为v2.0
version="v2.0"

'语言种类.固定选择值。
''只能选择1、2、3
''1代表中文；2代表英文
''默认值为1
language="1"

'签名类型.固定值
''1代表MD5签名
''当前版本固定为1
signType="1"	

   
'支付人姓名
''可为中文或英文字符
payerName=payerName

'支付人联系方式类型.固定选择值
''只能选择1、2
''1代表Email；2代表手机号
payerContactType="1"	

'支付人联系方式
''只能选择Email或手机号
payerContact=""

'商户订单号
''由字母、数字、或[-][_]组成
orderId=getDateStr()

'订单金额
''以分为单位，必须是整型数字
''比方2，代表0.02元
orderAmount=orderAmount

'支付方式.固定选择值
''只能选择00
''00：代表支持神州行卡和快钱帐户支付
payType="44"

'神州行卡序号
''仅在商户定制了神州行卡密直连功能时填写
cardNumber=""

'神州行卡密码
''仅在商户定制了神州行卡密直连功能时填写
cardPwd=""

'全额支付标志
''只能选择数字 0 或 1
''0代表卡面额小于订单金额时返回支付结果为失败；1代表卡面额小于订单金额是返回支付结果为成功，同时订单金额和实际支付金额都为神州行卡的面额.如果商户定制神州行卡密直连时，本参数固定值为1
fullAmountFlag="0"

'订单提交时间
''14位数字。年[4位]月[2位]日[2位]时[2位]分[2位]秒[2位]
''如；20080101010101
orderTime=getDateStr()

'商品名称
''可为中文或英文字符
productName=productName

'商品数量
''可为空，非空时必须为数字
productNum="1"

'商品代码
''可为字符或者数字
productId=""

'商品描述
productDesc=""
	
'扩展字段1
''在支付结束后原样返回给商户
ext1=payerName

'扩展字段2
''在支付结束后原样返回给商户
ext2=""


   
'生成加密签名串
''请务必按照如下顺序和规则组成加密串！
	signMsgVal=appendParam(signMsgVal,"inputCharset",inputCharset)
	signMsgVal=appendParam(signMsgVal,"bgUrl",bgUrl)
	signMsgVal=appendParam(signMsgVal,"pageUrl",pageUrl)
	signMsgVal=appendParam(signMsgVal,"version",version)
	signMsgVal=appendParam(signMsgVal,"language",language)
	signMsgVal=appendParam(signMsgVal,"signType",signType)
	signMsgVal=appendParam(signMsgVal,"merchantAcctId",merchantAcctId)
	signMsgVal=appendParam(signMsgVal,"payerName",payerName)
	signMsgVal=appendParam(signMsgVal,"payerContactType",payerContactType)
	signMsgVal=appendParam(signMsgVal,"payerContact",payerContact)
	signMsgVal=appendParam(signMsgVal,"orderId",orderId)
	signMsgVal=appendParam(signMsgVal,"orderAmount",orderAmount)
    signMsgVal=appendParam(signMsgVal,"payType",payType)
	signMsgVal=appendParam(signMsgVal,"cardNumber",cardNumber)
	signMsgVal=appendParam(signMsgVal,"cardPwd",cardPwd)
	signMsgVal=appendParam(signMsgVal,"fullAmountFlag",fullAmountFlag)
	signMsgVal=appendParam(signMsgVal,"orderTime",orderTime)
	signMsgVal=appendParam(signMsgVal,"productName",productName)
	signMsgVal=appendParam(signMsgVal,"productNum",productNum)
	signMsgVal=appendParam(signMsgVal,"productId",productId)
	signMsgVal=appendParam(signMsgVal,"productDesc",productDesc)
	signMsgVal=appendParam(signMsgVal,"ext1",ext1)
	signMsgVal=appendParam(signMsgVal,"ext2",ext2)
	signMsgVal=appendParam(signMsgVal,"key",key)
signMsg= Ucase(md5(signMsgVal))
	
	'功能函数。将变量值不为空的参数组成字符串
	Function appendParam(returnStr,paramId,paramValue)

		If returnStr <> "" Then
			If paramValue <> "" then
				returnStr=returnStr&"&"&paramId&"="&paramValue
			End if
		Else 
			If paramValue <> "" then
				returnStr=paramId&"="&paramValue
			End if
		End if
		
		appendParam=ReturnStr

	End Function
	'功能函数。将变量值不为空的参数组成字符串。结束

	'功能函数。获取14位的日期
	Function getDateStr() 

	dim dateStr1,dateStr2,strTemp 
	dateStr1=split(cstr(formatdatetime(now(),2)),"-") 
	dateStr2=split(cstr(right(formatdatetime(now(),3),8)),":") 

	for each StrTemp in dateStr1 
	if len(StrTemp)<2 then 
	getDateStr=getDateStr & "0" & strTemp 
	else 
	getDateStr=getDateStr & strTemp 
	end if 
	next 

	for each StrTemp in dateStr2 
	if len(StrTemp)<2 then 
	getDateStr=getDateStr & "0" & strTemp 
	else 
	getDateStr=getDateStr & strTemp 
	end if
	next
	End function 
	'功能函数。获取14位的日期。结束
	
%>

<!doctype html public "-//w3c//dtd html 4.0 transitional//en" >
<html>
	<head>
		<title>使用快钱支付</title>
		<meta http-equiv="Content-Type" content="text/html; charset=gb2312" >
	<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style></head>
	
<body oncontextmenu="return false" ondragstart="return false" onselectstart ="return false" onselect="document.selection.empty()" oncopy="document.selection.empty()" onbeforecopy="return false" onmouseup="document.selection.empty()">
	<div align="center">
		<table width="259" border="0" cellpadding="1" cellspacing="1" bgcolor="#CCCCCC" >
			<tr bgcolor="#FFFFFF">
				<td width="80" height="24">支付方式:</td>
				<td height="24" >快钱[99bill]</td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24" >订单编号:</td>
				<td height="24" ><%=orderId %></td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24">订单金额:</td>
				<td height="24"><%=orderAmount1%>元</td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24">支付人:</td>
				<td height="24"><%=payerName %></td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24">商品名称:</td>
				<td height="24"><%=productName1 %></td>
			</tr>
			<tr>
				<td></td>
				<td></td>
			</tr>
	  </table>
	</div>

	<div align="center" style="font-size=12px;font-weight: bold;color=red;">
		<form name="kqPay" method="post" action="https://www.99bill.com/szxgateway/recvMerchantInfoAction.htm">
            <input type="hidden" name="inputCharset" value="<%=inputCharset %>">
			<input type="hidden" name="bgUrl" value="<%=bgUrl %>">
			<input type="hidden" name="pageUrl" value="<%=pageUrl %>">
			<input type="hidden" name="version" value="<%=version %>">
			<input type="hidden" name="language" value="<%=language %>">
			<input type="hidden" name="signType" value="<%=signType %>">			
			<input type="hidden" name="merchantAcctId" value="<%=merchantAcctId %>">
			<input type="hidden" name="payerName" value="<%=payerName %>">
			<input type="hidden" name="payerContactType" value="<%=payerContactType %>">
			<input type="hidden" name="payerContact" value="<%=payerContact %>">
			<input type="hidden" name="orderId" value="<%=orderId %>">
			<input type="hidden" name="orderAmount" value="<%=orderAmount %>">
            <input type="hidden" name="payType" value="<%=payType %>">
			<input type="hidden" name="cardNumber" value="<%=cardNumber %>">
			<input type="hidden" name="cardPwd" value="<%=cardPwd %>">
			<input type="hidden" name="fullAmountFlag" value="<%=fullAmountFlag %>">
			<input type="hidden" name="orderTime" value="<%=orderTime %>">
			<input type="hidden" name="productName" value="<%=productName %>">
			<input type="hidden" name="productNum" value="<%=productNum %>">
			<input type="hidden" name="productId" value="<%=productId %>">
			<input type="hidden" name="productDesc" value="<%=productDesc %>">
			<input type="hidden" name="ext1" value="<%=ext1 %>">
			<input type="hidden" name="ext2" value="<%=ext2 %>">	
			<input type="hidden" name="signMsg" value="<%=signMsg %>">	
			<input type="submit" name="submit" value="提交到快钱">
			
		</form>		
	</div>
	
</BODY>
</HTML>
  50