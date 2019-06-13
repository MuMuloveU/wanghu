<!-- #include file="merchantProperties.asp" -->
<%
'**************************************************
'* @Description 易宝支付产品通用支付接口范例 
'* @V3.0
'* @Author rui.xin
'**************************************************

	Dim reqURL_onLine
	Dim reqURL_QryOrd
	Dim reqURL_RefOrd
	Dim p0_Cmd
	Dim p9_SAF
	Dim sbOld

	'Asp非虚拟主机需注册组件Hmac-Md5-API.dll
	Set mctSDK = Server.CreateObject("HmacMd5API.HmacMd5Com")

	
Function HTMLcommom(p1_MerId,p2_Order,p3_Amt,p4_Cur,p5_Pid,p6_Pcat,p7_Pdesc,p8_Url,pa_MP,pd_FrpId,pr_NeedResponse)
	
	'	在线支付请求地址,无需更改
	reqURL_onLine = "https://www.yeepay.com/app-merchant-proxy/node"    '	正式地址
	'reqURL_onLine = "http://tech.yeepay.com:8080/robot/debug.action"    '	测试使用
	'测试时必须使用范例中提供的测试商户编号和密钥（测试商户编号及密钥见范例程序。）
	'1.将支付/查询/退款请求发送到该地址；
	'2.该应用会将请求参数逐一分析，如请求有问题，则会报告详细信息；
	'3.请求参数如正确无误，该应用将生成一个模拟的支付成功数据，您可以使用该数据测试支付结果的接收程序。

	' 业务类型
	''在线支付请求，固定值 ”Buy” .	
	p0_Cmd = "Buy"
	
	'	送货地址
	''为“1”: 需要用户将送货地址留在易宝支付系统;为“0”: 不需要，默认为 ”0”.
	p9_SAF = "0"				'需要填写送货信息 0：不需要  1:需要
	
	sbOld  = ""

	'	进行签名处理，一定按照文档中标明的签名顺序进行
	sbOld = sbOld + p0_Cmd
	sbOld = sbOld + p1_MerId
	sbOld = sbOld + p2_Order
	sbOld = sbOld + CStr(p3_Amt)		
	sbOld = sbOld + p4_Cur					
	sbOld = sbOld + p5_Pid
	sbOld = sbOld + p6_Pcat
	sbOld = sbOld + p7_Pdesc
	sbOld = sbOld + p8_Url
	sbOld = sbOld + p9_SAF
	sbOld = sbOld + pa_MP
	sbOld = sbOld + pd_FrpId
	sbOld = sbOld + pr_NeedResponse
	
	HTMLcommom = mctSDK.HmacMd5(sbOld,merchantKey) 
	
End Function


Function getCallbackHmacString(r0_Cmd,r1_Code,r2_TrxId,r3_Amt,r4_Cur,r5_Pid,r6_Order,r7_Uid,r8_MP,r9_BType)

	'取得签名数据前的字符串，一定按照文档中标明的签名顺序进行
	sbOld = sbOld + CStr(p1_MerId)
	sbOld = sbOld + r0_Cmd
	sbOld = sbOld + r1_Code
	sbOld = sbOld + r2_TrxId
	sbOld = sbOld + r3_Amt
	sbOld = sbOld + r4_Cur
	sbOld = sbOld + r5_Pid
	sbOld = sbOld + r6_Order
	sbOld = sbOld + r7_Uid
	sbOld = sbOld + r8_MP
	sbOld = sbOld + r9_BType

	getCallbackHmacString = mctSDK.HmacMd5(sbOld,merchantKey)
	
End Function


Function CheckHmac(r0_Cmd,r1_Code,r2_TrxId,r3_Amt,r4_Cur,r5_Pid,r6_Order,r7_Uid,r8_MP,r9_BType,hmac)
	if(hmac=getCallbackHmacString(r0_Cmd,r1_Code,r2_TrxId,r3_Amt,r4_Cur,r5_Pid,r6_Order,r7_Uid,r8_MP,r9_BType)) Then
		CheckHmac = True
	ELSE
		CheckHmac = False
	END IF
End Function


'取得在线支付返回数据中的参数
Sub getCallBackValue(ByRef r0_Cmd,ByRef r1_Code,ByRef r2_TrxId,ByRef r3_Amt,ByRef r4_Cur,ByRef r5_Pid,ByRef r6_Order,ByRef r7_Uid,ByRef r8_MP,ByRef r9_BType,ByRef hmac)
	r0_Cmd		=	Request.QueryString("r0_Cmd")
	r1_Code		=	Request.QueryString("r1_Code")
	r2_TrxId	=	Request.QueryString("r2_TrxId")
	r3_Amt		=	Request.QueryString("r3_Amt")
	r4_Cur		=	Request.QueryString("r4_Cur")
	r5_Pid		=	Request.QueryString("r5_Pid")
	r6_Order	=	Request.QueryString("r6_Order")
	r7_Uid		=	Request.QueryString("r7_Uid")
	r8_MP			=	Request.QueryString("r8_MP")
	r9_BType	=	Request.QueryString("r9_BType") 	
	hmac			=	Request.QueryString("hmac")
End Sub


'取得订单查询的结果
Function queryOrdDetail(p2_Order)

	'订单查询请求地址,无需更改
	reqURL_QryOrd	= "https://www.yeepay.com/app-merchant-proxy/command"    '	正式地址			
	'reqURL_QryOrd = "http://tech.yeepay.com:8080/robot/debug.action"    '	测试使用
	'测试时必须使用范例中提供的测试商户编号和密钥（测试商户编号及密钥见范例程序。）
	'1.将支付/查询/退款请求发送到该地址；
	'2.该应用会将请求参数逐一分析，如请求有问题，则会报告详细信息；
	'3.请求参数如正确无误，该应用将生成一个模拟的支付成功数据，您可以使用该数据测试支付结果的接收程序。
	
	'订单查询请求，固定值"QueryOrdDetail"					
	p0_Cmd					= "QueryOrdDetail"																									

	'进行加密串处理，一定按照下列顺序进行
	sbOld =	""
	sbOld = sbOld + p0_Cmd					'加入业务类型
	sbOld = sbOld + p1_MerId				'加入商户编号
	sbOld = sbOld + p2_Order				'加入商户订单号
	
	hmac	= ""
	hmac	= mctSDK.HmacMd5(sbOld,merchantKey)
	
	'进行加密串处理，一定按照下列顺序进行
	sFormString = ""
	sFormString = sFormString + "p0_Cmd="		 + p0_Cmd			'加入业务类型
	sFormString = sFormString + "&p1_MerId=" + p1_MerId		'加入商户编号
	sFormString = sFormString + "&p2_Order=" + p2_Order		'加入商户订单号
	sFormString = sFormString + "&hmac="		 + hmac				'加入签名数据
	
  set objHttp = Server.CreateObject("MSXML2.ServerXMLHTTP")
	objHttp.open "POST", reqURL_QryOrd , false
	objHttp.setRequestHeader "Content-type", "application/x-www-form-urlencoded"
	objHttp.Send sFormString
   
  if (objHttp.status <> 200 ) then
	'HTTP 错误处理
		response.Write("Status="&objHttp.status)
		response.end
	else
		
		strCallBack = objHttp.responseText
		aryCallBack = Split(strCallBack,vbLf)
		
		For i = LBound(aryCallBack) To UBound(aryCallBack) -1
			aryReturn = Split(aryCallBack(i),"=")
			If UBound(aryReturn) < 1 then
				returnMsg = aryCallBack(i)
			Else	
				sKey = aryReturn(0)
				sValue = aryReturn(1)
			
				If sKey = "r0_Cmd" Then								'取得业务类型
					r0_Cmd = sValue
				ElseIf sKey = "r1_Code" Then					'取得查询结果
					r1_Code = sValue  	      	
				ElseIf sKey = "r2_TrxId" Then					'取得易宝支付交易流水号
					r2_TrxId = sValue  	      	
				ElseIf sKey = "r3_Amt" Then						'取得支付金额
					r3_Amt = sValue
				ElseIf sKey = "r4_Cur" Then						'取得交易币种
					r4_Cur = sValue
				ElseIf sKey = "r5_Pid" Then						'取得商品名称
					r5_Pid = sValue
				ElseIf sKey = "r6_Order" Then					'取得商户订单号
					r6_Order = sValue
				ElseIf sKey = "r8_MP" Then						'取得商户扩展信息
					r8_MP = sValue
				ElseIf sKey = "rb_PayStatus" Then			'取得支付状态
					rb_PayStatus = sValue
				ElseIf sKey = "rc_RefundCount" Then		'取得已退款次数
					rc_RefundCount = sValue
				ElseIf sKey = "rd_RefundAmt" Then			'取得已退款金额
					rd_RefundAmt = sValue
				ElseIf sKey = "hmac" Then							'取得签名数据
					hmac = sValue
				Else
					returnMsg = returnMsg + aryCallBack(i)   
				End If	
			End If	
		Next
		
		If trim(returnMsg)<> "" then
			queryOrdDetail = returnMsg
			exit Function
		End If
	
		'进行校验码检查 取得加密前的字符串
		sbOld	=	""
		sbOld = sbOld + r0_Cmd									'加入消息类型
		sbOld = sbOld + r1_Code    							'加入返回状态    
		sbOld = sbOld + r2_TrxId								'加入平台交易流水号
		sbOld = sbOld + r3_Amt									'加入交易金额
		sbOld = sbOld + r4_Cur									'加入交易币种
		sbOld = sbOld + r5_Pid									'加入商品ID
		sbOld = sbOld + r6_Order								'加入取得订单号
		sbOld = sbOld + r8_MP										'加入商家属性
		sbOld = sbOld + rb_PayStatus						'加入银行交易订单号
		sbOld = sbOld + rc_RefundCount					'加入交易状态
		sbOld = sbOld + rd_RefundAmt						'加入交易状态
	
		sNewString = ""
		sNewString = mctSDK.HmacMd5(sbOld,merchantKey)

		'校验码正确
		If sNewString = hmac Then   	
			If r1_Code = "1" Then
				'returnMsg	= returnMsg	& "订单查询成功" 
				'returnMsg	= returnMsg	& "<BR><BR>订单号 "& r6_Order &" " 
				If rb_PayStatus = "SUCCESS" Then
					returnMsg	= returnMsg	&	"该订单已支付" 
				ElseIf rb_PayStatus = "CANCELED" Then
					returnMsg	= returnMsg	& "该订单已取消" 
				ElseIf rb_PayStatus = "AUTHORIZED" Then
					returnMsg	= returnMsg	& "该订单未支付" 
				ElseIf rb_PayStatus = "INIT" Then
					returnMsg	= returnMsg	& "该订单未支付" 
		  	End If
		  ElseIf r1_Code = "50" Then
				returnMsg	= returnMsg	& "该订单不存在" 
			End If
		Else
			returnMsg	= returnMsg	& "交易信息被篡改!<BR>"
			returnMsg	= returnMsg	& "<BR>localhost: " & sNewString
			returnMsg	= returnMsg	& "<BR>YeePay: " & hmac
		End If
	End If
	
	queryOrdDetail = returnMsg

End Function


'取得订单退款申请请求处理结果.
Function refundOrd(p2_Order,pb_TrxId,p3_Amt,p4_Cur,p5_Desc)

	'订单退款请求地址,无需更改
	reqURL_RefOrd	= "https://www.yeepay.com/app-merchant-proxy/command"    '	正式地址			
	'reqURL_RefOrd = "http://tech.yeepay.com:8080/robot/debug.action"    '	测试使用
	'测试时必须使用范例中提供的测试商户编号和密钥（测试商户编号及密钥见范例程序。）
	'1.将支付/查询/退款请求发送到该地址；
	'2.该应用会将请求参数逐一分析，如请求有问题，则会报告详细信息；
	'3.请求参数如正确无误，该应用将生成一个模拟的支付成功数据，您可以使用该数据测试支付结果的接收程序。
	
	'订单退款请求，固定值"RefundOrd"					
	p0_Cmd				= "RefundOrd"																									

	'进行加密串处理，一定按照下列顺序进行
	sbOld =	""
	sbOld = sbOld + p0_Cmd					'加入业务类型
	sbOld = sbOld + p1_MerId				'加入商户编号
	sbOld = sbOld + pb_TrxId				'加入易宝支付交易流水号
	sbOld = sbOld + p3_Amt					'加入退款金额
	sbOld = sbOld + p4_Cur					'加入交易币种
	sbOld = sbOld + p5_Desc					'加入退款说明
	
	hmac	= ""
	hmac	= mctSDK.HmacMd5(sbOld,merchantKey)
	
	'进行加密串处理，一定按照下列顺序进行
	sFormString = ""
	sFormString = sFormString + "p0_Cmd="		 + p0_Cmd			'加入业务类型
	sFormString = sFormString + "&p1_MerId=" + p1_MerId		'加入商户编号
	sFormString = sFormString + "&pb_TrxId=" + pb_TrxId		'加入易宝支付交易流水号
	sFormString = sFormString + "&p3_Amt="	 + p3_Amt			'加入退款金额
	sFormString = sFormString + "&p4_Cur="   + p4_Cur			'加入交易币种
	sFormString = sFormString + "&p5_Desc="  + p5_Desc		'加入退款说明
	sFormString = sFormString + "&hmac="		 + hmac				'加入签名数据
	
  set objHttp = Server.CreateObject("MSXML2.ServerXMLHTTP")
	objHttp.open "POST", reqURL_RefOrd , false
	objHttp.setRequestHeader "Content-type", "application/x-www-form-urlencoded"
	objHttp.Send sFormString
   
  if (objHttp.status <> 200 ) then
	'HTTP 错误处理
		response.Write("Status="&objHttp.status)
		response.end
	else
		
		strCallBack = objHttp.responseText
		aryCallBack = Split(strCallBack,vbLf)
		returnMsg = ""
		
		For i = LBound(aryCallBack) To UBound(aryCallBack) -1
			aryReturn = Split(aryCallBack(i),"=")
			If UBound(aryReturn) < 1 then
				returnMsg = aryCallBack(i)
			Else	
				sKey = aryReturn(0)
				sValue = aryReturn(1)
		
				If sKey = "r0_Cmd" Then								'取得业务类型
					r0_Cmd = sValue
				ElseIf sKey = "r1_Code" Then					'取得退款申请结果
					r1_Code = sValue  	      	
				ElseIf sKey = "r2_TrxId" Then					'取得易宝支付交易流水号
					r2_TrxId = sValue  	      	
				ElseIf sKey = "r3_Amt" Then						'取得退款金额
					r3_Amt = sValue
				ElseIf sKey = "r4_Cur" Then						'取得交易币种
					r4_Cur = sValue
				ElseIf sKey = "hmac" Then							'取得签名数据
					hmac = sValue
				Else
					returnMsg = returnMsg + aryCallBack(i)   
				End If
			End If
		Next

		If trim(returnMsg)<> "" then
			refundOrd = returnMsg
			exit Function
		End If
		

		'进行校验码检查 取得加密前的字符串
		sbOld	=	""
		sbOld = sbOld + r0_Cmd									'加入消息类型
		sbOld = sbOld + r1_Code    							'加入返回状态    
		sbOld = sbOld + r2_TrxId								'加入易宝支付交易流水号
		sbOld = sbOld + r3_Amt									'加入退款金额
		sbOld = sbOld + r4_Cur									'加入交易币种
	
		sNewString = ""
		sNewString = mctSDK.HmacMd5(sbOld,merchantKey)

		'校验码正确
		returnMsg	= ""
		If sNewString = hmac Then   	
			If CStr(r1_Code) = "1" Then
				returnMsg	= returnMsg	&	"订单退款请求提交成功!" 
		  Else
				returnMsg	= returnMsg	& "订单退款请求提交失败!" 
			End If
		Else
			returnMsg	= returnMsg	& "交易信息被篡改!<BR>"
			returnMsg	= returnMsg	& "<BR>localhost: " & sNewString
			returnMsg	= returnMsg	& "<BR>YeePay: " & hmac
		End If
	End If
	
	refundOrd = returnMsg

End Function

'callback在线支付服务器返回，服务器点对点通讯
'写入 onLine.log 这里用来调试接口
Sub createLog(ByRef returnMsg)
    filename = "./" & returnMsg & ".log"
    content = now()		&	","							&	request.ServerVariables("REMOTE_ADDR")
    content = content &	","							&	returnMsg
    content = content &	",商户订单号:"	& r6_Order
    content = content &	",支付金额:"		& r3_Amt
    content = content &	",签名数据:"		& hmac
    
    Set FSO = Server.CreateObject("Scripting.FileSystemObject")   
    Set TS = FSO.OpenTextFile(Server.MapPath(filename),8,true) 
    TS.write content   
    TS.Writeline ""
    TS.Writeline ""
    Set TS = Nothing   
    Set FSO = Nothing   
End Sub
%>