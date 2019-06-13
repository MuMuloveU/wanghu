<!--#include file="../inc/config.asp"-->
<%dim varchar,code,codearr,J%>
<!--#include file="md6.asp" -->
<%
  dim merchantAcctId,key,version,language,signType,payType,bankId,orderId,orderTime,orderAmount,dealId,bankDealId,dealTime,payAmount,fee,ext1,ext2,payResult,errCode,signMsg,youxibi,payAmount1,merchantSignMsgVal,merchantSignMsg,v_result
%>
<%
'*
'* @Description: 快钱人民币支付网关接口范例
'* @Copyright (c) 上海快钱信息服务有限公司
'* @version 2.0
'*

'获取人民币网关账户号
merchantAcctId=trim(request("merchantAcctId"))

'设置人民币网关密钥
''区分大小写
key="ZEYAX39WBIMRQ578"

'获取网关版本.固定值
''快钱会根据版本号来调用对应的接口处理程序。
''本代码版本号固定为v2.0
version=trim(request("version"))

'获取语言种类.固定选择值。
''只能选择1、2、3
''1代表中文；2代表英文
''默认值为1
language=trim(request("language"))

'签名类型.固定值
''1代表MD5签名
''当前版本固定为1
signType=trim(request("signType"))

'获取支付方式
''值为：10、11、12、13、14
''00：组合支付（网关支付页面显示快钱支持的各种支付方式，推荐使用）10：银行卡支付（网关支付页面只显示银行卡支付）.11：电话银行支付（网关支付页面只显示电话支付）.12：快钱账户支付（网关支付页面只显示快钱账户支付）.13：线下支付（网关支付页面只显示线下支付方式）.14：B2B支付（网关支付页面只显示B2B支付，但需要向快钱申请开通才能使用）
payType=trim(request("payType"))

'获取银行代码
''参见银行代码列表
bankId=trim(request("bankId"))

'获取商户订单号
orderId=trim(request("orderId"))

'获取订单提交时间
''获取商户提交订单时的时间.14位数字。年[4位]月[2位]日[2位]时[2位]分[2位]秒[2位]
''如：20080101010101
orderTime=trim(request("orderTime"))

'获取原始订单金额
''订单提交到快钱时的金额，单位为分。
''比方2 ，代表0.02元
orderAmount=trim(request("orderAmount"))

'获取快钱交易号
''获取该交易在快钱的交易号
dealId=trim(request("dealId"))

'获取银行交易号
''如果使用银行卡支付时，在银行的交易号。如不是通过银行支付，则为空
bankDealId=trim(request("bankDealId"))

'获取在快钱交易时间
''14位数字。年[4位]月[2位]日[2位]时[2位]分[2位]秒[2位]
''如；20080101010101
dealTime=trim(request("dealTime"))

'获取实际支付金额
''单位为分
''比方 2 ，代表0.02元
payAmount=trim(request("payAmount"))

'获取交易手续费
''单位为分
''比方 2 ，代表0.02元
fee=trim(request("fee"))

'获取扩展字段1
ext1=trim(request("ext1"))

'获取扩展字段2
ext2=trim(request("ext2"))

'获取处理结果
''10代表 成功11代表 失败
payResult=trim(request("payResult"))

'获取错误代码
''详细见文档错误代码列表
errCode=trim(request("errCode"))

'获取加密签名串
signMsg=trim(request("signMsg"))



'生成加密串。必须保持如下顺序。
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"merchantAcctId",merchantAcctId)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"version",version)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"language",language)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"signType",signType)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"payType",payType)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"bankId",bankId)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"orderId",orderId)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"orderTime",orderTime)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"orderAmount",orderAmount)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"dealId",dealId)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"bankDealId",bankDealId)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"dealTime",dealTime)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"payAmount",payAmount)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"fee",fee)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"ext1",ext1)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"ext2",ext2)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"payResult",payResult)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"errCode",errCode)
	merchantSignMsgVal=appendParam(merchantSignMsgVal,"key",key)
    merchantSignMsg= md6(merchantSignMsgVal)


'初始化结果及地址


'商家进行数据处理，并跳转会商家显示支付结果的页面
''首先进行签名字符串验证


If UCase(signMsg)=UCase(merchantSignMsg) then

	''接着进行支付结果判断
	Select Case payResult
	
		  case "10"
			
			'*  
			' 商户网站逻辑处理，比方更新订单支付状态为成功
			' 特别注意：只有UCase(signMsg)=UCase(merchantSignMsgVal)，且payResult=10，才表示支付成功！同时将订单金额与提交订单前的订单金额进行对比校验。
			'*
			
			'报告给快钱处理结果，并提供将要重定向的地址。
            v_result="成功"


            payAmount=clng(payAmount)
			youxibi=payAmount*100
			payAmount1=youxibi/10
			
			  CxGame.DbConn("QPGameUserDB")
			  dim gender,C_idno,c_address,c_email,c_pic,sql,UserID,SpreaderID
			  set rs=conn.execute("select top 1 UserID,SpreaderID from AccountsInfo where Accounts='"& ext1 &"'")
			  if not rs.eof then
				 UserID=rs(0)
				 SpreaderID=clng(rs(1))
			  end if
			  rs.close
			  
			  if ext2="红色会员卡" then
			  CxGame.DbConn("QPGameUserDB")
				  set rs=conn.execute("update AccountsInfo set MemberOrder=1 where UserID="&UserID)
				  response.Write("<script language='javascript'>alert('充值成功！');location.href='../index.asp';</script>")
			  else
				  CxGame.DbConn("CxGameTreasureDb")
				  set rs=conn.execute("select * from PostLog where PostCode='"& orderId &"'")
				  if rs.eof then
					  set rs=conn.execute("insert into PostLog (UserID,PostCode,Price,PostDate) values ("& UserID &","& orderId &","& payAmount &","& date() &")")
					  CxGame.DbConn("QPTreasureDB")
					  set rs=conn.execute("update GameScoreInfo set score=score+"& youxibi &" where userid="&UserID)
					  
					  if SpreaderID > 0 then
						 set rs=conn.execute("update set GameScoreInfo score=score+"& payAmount1 &" where userid="&SpreaderID)
					  end if					  
				  end if
			  end if
			  response.Write("<script language='javascript'>alert('充值成功！');location.href='../index.asp';</script>")
		 Case else

			v_result="失败"

	End select

else

	v_result="签名错误"

End if

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


'以下报告给快钱处理结果，并提供将要重定向的地址

%>
<!doctype html public "-//w3c//dtd html 4.0 transitional//en" >
<html>
	<head>
		<title>快钱支付结果</title>
		<meta http-equiv="content-type" content="text/html; charset=gb2312" >
	</head>
	
<BODY>
	
	<div align="center">
		<table width="259" border="0" cellpadding="1" cellspacing="1" bgcolor="#CCCCCC" >
			<tr bgcolor="#FFFFFF">
				<td width="80">支付方式:</td>
				<td >快钱[99bill]</td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td >订单编号:</td>
				<td ><%=orderId %></td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td>订单金额:</td>
				<td><%=(orderAmount)&"分" %></td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td>支付结果:</td>
				<td><%=v_result %></td>
			</tr>
			<tr>
				<td></td>
				<td></td>
			</tr>
	  </table>
	</div>

</BODY>
</HTML>