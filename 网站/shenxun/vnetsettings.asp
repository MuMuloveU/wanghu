<%

'以下代码为变量声明
dim agentID,agentPWD,receive_url,custom,agentreq,serverurl,agentversion

agentID="10785"           ' 网站加盟编号 5位， 请改为自己的加盟编号。

agentPWD="510129198402020034"   '984616516893646869 网站加盟密码 18位 ，请改为自己的加盟密码。
custom=getDateStr      '自己定义字段,只能数字和字母.长度为18字符以内. 自己的订单号码 ，可以放在req.asp 里面，但是前后要一致。（有几个地方用到此订单，请自由处理）

'客户浏览器接收的返回页面地址 请换成自己网站的返回页面 如:http://你的域名/你的文件夹callbackclient.asp
receive_url="http://www.dc173.com/shenxun/callbackclient.asp"  

''错误时返回地址 如:http://你的域名/你的文件夹/error.asp
errorcallback="http://www.dc173.com/shenxun/error.asp"  


'用户注册地址 即填写用户名（你方站的用户ID）和卡号的页面.如:http://你的域名/你的文件夹/client.asp" 
agentclient="http://www.dc173.com/shenxun/ipsinput.asp"

 '商户自己的请求地址  如:http://你的域名/你的文件夹/req.asp
agentreq="http://www.dc173.com/shenxun/req.asp" 


'合作方服务器接收的 或 补单地址(补单和接收是同一个页面) 如:http://你的域名/你的文件夹/callbackplug.asp
callbackplug="http://www.dc173.com/shenxun/callbackplug.asp" 

'盈华讯方服务方接收地址 请不要随便修改.
serverurl="http://s.vnetone.com/netserver.aspx"  

'盈华讯方接口版本 现在版本是2001
agentversion="2001"

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
%>