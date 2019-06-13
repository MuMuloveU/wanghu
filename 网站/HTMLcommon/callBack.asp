<!-- #include file="yeepayCommon.asp" -->
<%
'**************************************************
'* @Description 易宝支付产品通用支付接口范例  
'* @V3.0
'* @Author rui.xin
'**************************************************

	'	只有支付成功时易宝支付才会通知商户.
	''支付成功回调有两次，都会通知到在线支付请求参数中的p8_Url 上：浏览器重定向;服务器点对点通讯.
	
	Dim r0_Cmd
	Dim r1_Code
	Dim r2_TrxId
	Dim r3_Amt
	Dim r4_Cur
	Dim r5_Pid
	Dim r6_Order
	Dim r7_Uid
	Dim r8_MP
	Dim r9_BType
	Dim hmac
	
	Dim bRet
	Dim returnMsg
	
	'解析返回参数
	Call getCallBackValue(r0_Cmd,r1_Code,r2_TrxId,r3_Amt,r4_Cur,r5_Pid,r6_Order,r7_Uid,r8_MP,r9_BType,hmac)
	'判断返回签名是否正确（True/False）
	bRet = CheckHmac(r0_Cmd,r1_Code,r2_TrxId,r3_Amt,r4_Cur,r5_Pid,r6_Order,r7_Uid,r8_MP,r9_BType,hmac)
	'以上代码和变量不需要修改.
	
	
	'校验码正确
	returnMsg	= ""
	If bRet = True Then
	  If(r1_Code="1") Then
	  	
		'需要比较返回的金额与商家数据库中订单的金额是否相等，只有相等的情况下才认为是交易成功.
		'并且需要对返回的处理进行事务控制，进行记录的排它性处理，防止对同一条交易重复发货的情况发生.	  	      	  
		
			If(r9_BType="1") Then
				'	在线支付页面返回	
				returnMsg	= returnMsg	&	r3_Amt & "元 支付成功！"				
			ElseIf(r9_BType="2") Then				
	  		'	如果需要应答机制则必须回写以"success"开头的stream,大小写不敏感.
	  		''易宝支付收到该stream，便认为商户已收到；否则将继续发送通知，直到商户收到为止。
		 		Response.write("success")
				'Call createLog("HTMLcommon")
				returnMsg	= returnMsg	& "在线支付服务器返回"				
			ElseIf(r9_BType="3") Then
				returnMsg	= returnMsg	&	"电话支付通知页面返回"
			End IF  
	  End IF
	Else
		returnMsg	= returnMsg	&	"交易信息被篡改"
	End If


%>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>易宝支付 - 网上开店 | 电子商务首选服务商，支付用易宝，方便又可靠！</title>
<meta name="description" content="易宝支付作为第三方金融服务提供商，与工商银行、招商银行、建设银行、农业银行、民生银行等多家国 内银行及VISA、MasterCard外卡组织紧密合作，为个人客户提供在线支付、电话支付、虚拟账户理财服务， 为企业商户提供银行网关支付、代收代付、委托结算以及B2B转款等多项增值业务。">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<link href="css/yeepaytest.css" type="text/css" rel="stylesheet" />
</head>
	<body>
		<table width="50%" border="0" align="center" cellpadding="0" cellspacing="0" style="border:solid 1px #107929">
		  <tr>
		    <td><table width="100%" border="0" align="center" cellpadding="5" cellspacing="1">
		  </tr>
		  <tr>
		    <td height="30" align="left"><a href="http://www.yeepay.com/"><img src="http://www.yeepay.com/new-indeximages/yeepay-indexlogo.gif" alt="YeePay易宝支付 创新的多元化支付平台 安全 可靠 便捷 自助接入" width="141" height="47" border="0" /></a></td>
		  </tr>
		  <tr>
		  	<td height="30" colspan="2" bgcolor="#6BBE18"><span style="color: #FFFFFF">感谢您使用易宝支付平台</span></td>
		  </tr>
		  <tr>
		  	<td colspan="2" bgcolor="#CEE7BD">易宝支付通用接口演示：</td>
		  </tr>
		  <tr>
		  	<td align="left" width="30%">&nbsp;&nbsp;商户订单号</td>
		  	<td align="left">&nbsp;&nbsp;<%= r6_Order %></td>
      </tr>
		  <tr>
		  	<td align="left">&nbsp;&nbsp;支付结果</td>
		  	<td align="left">&nbsp;&nbsp;<%= returnMsg %></td>
      </tr>
      <tr>
      	<td height="5" bgcolor="#6BBE18" colspan="2"></td>
      </tr>
      </table></td>
        </tr>
      </table>
<%
  SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=159357;DATABASE=QPGameUserDB;"
  Set Conn= Server.CreateObject("ADODB.Connection")
  Conn.Open SqlConnectionString
  dim gender,C_idno,c_address,c_email,c_pic,sql,UserID,SpreaderID
  set rs=conn.execute("select top 1 UserID,SpreaderID from AccountsInfo where Accounts='"& r8_MP &"'")
  if not rs.eof then
	 UserID=rs(0)
	 SpreaderID=rs(1)
  end if
  rs.close
 ' response.Write userid
%>
<%

	SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=159357;DATABASE=QPTreasureDB;"
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString

%>
	  <%
	  youxibi=r3_Amt*10000
	  payAmount1=r3_Amt*1000
	 ' response.Write payAmount1
	  
	  'response.Write youxibi
					  set rs=conn.execute("update GameScoreInfo set score=score+ "& youxibi &" where userid='"& UserID &"'")
					  
					  if SpreaderID =""then
					  else
						 set rs=conn.execute("update set GameScoreInfo score=score+"& payAmount1 &" where userid="&SpreaderID)
					  end if	
					  response.Write("<script language='javascript'>alert('充值成功！');location.href='../index.asp';</script>")				  
	  %>
	</body>
</html>

