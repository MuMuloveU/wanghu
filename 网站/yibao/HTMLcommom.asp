<%
  dim money,car,content,pd_FrpId
  car=replace(trim(request.Form("car")),"'","")
  money=replace(trim(request.Form("money")),"'","")
  content=replace(trim(request.Form("content")),"'","")
  pd_FrpId=replace(trim(request.Form("pd_FrpId")),"'","")
  if money="" then
  response.Write("<script language='javascript'>alert('请选择你要充值的金币卡');history.go(-1);</script>")
  response.End()
  end if
%>
<%
  Randomize
  Codeabc= mid(cStr(1e+6+Int(999999 * Rnd)),2)
  dim dddd
  dddd=cStr(now())
  dddd=replace(dddd,"-","")
  dddd=replace(dddd,":","")
  dddd=replace(dddd," ","")
  dddd=dddd&codeabc
  orderId=codeabc
  'response.Write orderId
%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>易宝支付 - 网上开店 | 电子商务首选服务商，支付用易宝，方便又可靠！</title>
<meta name="description" content="易宝支付作为第三方金融服务提供商，与工商银行、招商银行、建设银行、农业银行、民生银行等多家国 内银行及VISA、MasterCard外卡组织紧密合作，为个人客户提供在线支付、电话支付、虚拟账户理财服务， 为企业商户提供银行网关支付、代收代付、委托结算以及B2B转款等多项增值业务。">
		<link href="css/yeepaytest.css" type="text/css" rel="stylesheet" />
	</head>
	<body>
		<table width="50%" border="0" align="center" cellpadding="0" cellspacing="0" style="border:solid 1px #107929">
		 <tr>
		    <td height="30" align="left"><a href="http://www.yeepay.com/"><img src="http://www.yeepay.com/new-indeximages/yeepay-indexlogo.gif" alt="YeePay易宝支付 创新的多元化支付平台 安全 可靠 便捷 自助接入" width="141" height="47" border="0" /></a></td>
		  </tr>
		  <tr>
		  	<td height="30" colspan="2" bgcolor="#6BBE18"><span style="color: #FFFFFF">感谢您使用易宝支付平台</span></td>
		  </tr>
		  <tr>
		  	<td height="24" colspan="2" bgcolor="#CEE7BD">易宝支付产品通用支付接口：充值时请您退出游戏房间，否则后果自负！</td>
		  </tr>
			<form method="get" action="req.asp" targe="_blank" name="theform" onsubmit="return check();">
		  <tr>
		  	<td width="30%" height="24" align="left">&nbsp;&nbsp;商户订单号</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
	  	    <input name="p2_Order" type="text" id="p2_Order" value="<%=orderId%>" size="50" readonly="readonly" /></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;支付金额</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
	  	    <input size="50" type="text" name="p3_Amt" id="p3_Amt" value="<%=money%>" readonly="readonly" />		  	  &nbsp;<span style="color:#FF0000;font-weight:100;">*</span></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;用户名</td>
		  	<td height="12" align="left">&nbsp;&nbsp;
	  	    <input size="50" type="text" name="p5_Pid" id="p5_Pid" /></td>
      </tr>
		  <tr>
		    <td height="24" align="left">&nbsp; 确认用户名</td>
		    <td height="12" align="left">&nbsp;&nbsp;
			<input size="50" type="text" name="p5_Pid2" id="p5_Pid2" /></td>
		    </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;商品种类</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
	  	    <input name="p6_Pcat" type="text" id="p6_Pcat" value="<%=car%>" size="50" /></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;商品描述</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
	  	    <input name="p7_Pdesc" type="text" id="p7_Pdesc" value="<%=content%>" size="50" /><input size="50" type="hidden" name="p8_Url" id="p8_Url" value="http://www.dc173.com/yibao/callback.asp" readonly /></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;商户扩展信息</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
	  	    <input size="50" type="text" name="pa_MP" id="pa_MP" /></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;应答机制</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
		  	  <select name="pr_NeedResponse" id="pr_NeedResponse">
                <option value="0">不需要</option>
              </select></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;&nbsp;选择银行文字按钮</td>
		  	<td height="24" align="left"><table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
              <tr>
                <td width="50%" height="24" align="left"><input class="radio" type="radio" name="pd_FrpId"	id="pd_FrpId" value="ICBC-NET" />
                  中国工商银行</td>
                <td width="50%" height="24" align="left"><input class="radio" type="radio" name="pd_FrpId"	id="pd_FrpId" value="ABC-NET" />
                  中国农业银行</td>
              </tr>
              <tr>
                <td height="24" align="left"><input class="radio" type="radio" name="pd_FrpId"	id="pd_FrpId" value="CMBCHINA-NET" />
                  招商银行</td>
                <td height="24" align="left"><input class="radio" type="radio" name="pd_FrpId"	id="pd_FrpId" value="CCB-NET" />
                  建设银行</td>
              </tr>
              <tr>
                <td height="24" align="left"><input class="radio" type="radio" <%if pd_FrpId="jw" then%> checked="checked" <%end if%> name="pd_FrpId"	id="pd_FrpId" value="JUNNET-NET" />
                  骏网一卡通</td>
                <td height="24" align="left"><input class="radio" type="radio" name="pd_FrpId" <%if pd_FrpId="szx" then%> checked="checked" <%end if%>	id="pd_FrpId" value="SZX-NET" />
                  神州行卡充值</td>
              </tr>
              <tr>
                <td height="24" align="left"><input class="radio" type="radio" name="pd_FrpId" <%if pd_FrpId="sd" then%> checked="checked" <%end if%>	id="pd_FrpId" value="SNDACARD-NET" />
                  盛大卡充值</td>
                <td height="24" align="left"><input class="radio" type="radio" name="pd_FrpId" <%if pd_FrpId="zt" then%> checked="checked" <%end if%>	id="pd_FrpId" value="ZHENGTU-NET" />
                  征途卡充值</td>
              </tr>
            </table></td>
      </tr>
		  <tr>
		  	<td height="24" align="left">&nbsp;</td>
		  	<td height="24" align="left">&nbsp;&nbsp;
	  	    <input type="submit" value="马上支付" /></td>
      </tr>
    </form>
      <tr>
      	<td height="5" bgcolor="#6BBE18" colspan="2"></td>
      </tr>
    </table>
		</td>
        </tr>
      </table>
	  <script language="javascript">
	  function check(){
	  if(theform.p5_Pid.value==""){
	  alert("请输入你要充值的帐号!");
	  theform.p5_Pid.focus();
	  return (false);	  
	  }
	  if(theform.p5_Pid2.value==""){
	  alert("请确认你要充值的帐号!");
	  theform.p5_Pid2.focus();
	  return (false);	  
	  }
	  if((theform.p5_Pid.value)!=(theform.p5_Pid2.value)){
	  alert("两次输入的帐号不一致!")
	  theform.p5_Pid2.focus();
	  return (false);
	  }
	  }
	  </script>
	</body>
</html>