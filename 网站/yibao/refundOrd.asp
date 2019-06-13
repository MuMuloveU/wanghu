<!-- #include file="yeepayCommon.asp" -->
<%
'**************************************************
'* @Description 易宝支付订单退款请求接口范例 
'* @V3.0
'* @Author rui.xin
'**************************************************

	Dim pb_TrxId
	Dim p3_Amt
	Dim p4_Cur
	Dim p5_Desc
	Dim returnMsg

	' 易宝支付交易流水号																					
	pb_TrxId	= request("pb_TrxId")
	
	' 退款金额
	''单位:元，精确到分.
	p3_Amt		= request("p3_Amt")
	
	'	交易币种,固定值"CNY".
	p4_Cur		=	"CNY"			

	' 退款说明
	''详细描述退款原因的信息.
	p5_Desc		= request("p5_Desc")
	
	'取得订单退款请求处理结果.
	returnMsg = refundOrd(p2_Order,pb_TrxId,p3_Amt,p4_Cur,p5_Desc)
	
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
		  	<td colspan="2" bgcolor="#CEE7BD">易宝支付订单退款请求接口演示：</td>
		  </tr>
		  <tr>
		  	<td align="left" width="30%">&nbsp;&nbsp;易宝支付交易流水号</td>
		  	<td align="left">&nbsp;&nbsp;<input type="text" name="pb_TrxId" id="pb_TrxId" value="<%= pb_TrxId %>" /></td>
      </tr>
		  <tr>
		  	<td align="left" width="30%">&nbsp;&nbsp;退款金额(元)</td>
		  	<td align="left">&nbsp;&nbsp;<input type="text" name="p3_Amt" id="p3_Amt" value="<%= p3_Amt %>" /></td>
      </tr>
		  <tr>
		  	<td align="left" height="20">&nbsp;&nbsp;订单退款请求处理结果</td>
		  	<td align="left">&nbsp;&nbsp;<%= returnMsg %></td>
      </tr>
     <tr>
      	<td height="5" bgcolor="#6BBE18" colspan="2"></td>
      </tr>
      </table></td>
        </tr>
      </table>
	</body>
</html>