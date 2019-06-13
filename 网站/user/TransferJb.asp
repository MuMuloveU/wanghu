<!--#include file="../Inc/Config.asp" -->
<!--#include file="../Inc/md5VB.asp" -->
<%
  CxGame.IsCheckLogin()
%>
<%
  dim login
  login=replace(trim(request.Form("login")),"'","")
  if login="true" then
     dim username,money,BankPassWord,MemberOrder,BMoney,userid,Money1,Cip,sql,getdate
	 'getdate=datetime()
	 'response.Write getdate
	 'response.End()
	     CIP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
		If CIP = "" Then CIP = Request.ServerVariables("REMOTE_ADDR")
	 username=replace(trim(request.Form("username")),"'","")
	 money=replace(trim(request.Form("money2")),"'","")
	 BankPassWord=replace(trim(request.Form("BankPassWord")),"'","")
	 
	 if Session("UserName")=username then
	    response.Write("<script language='javascript'>alert('不能对自己转帐!');history.go(-1);</script>")
		response.End()
	 end if	 
	 
	 CxGame.DbConn("QPGameUserDB")
	 Set md5= New MD5obj
	 BankPassWord=md5.calcMD5(BankPassWord)
	 set rs=conn.execute("select * from AccountsInfo where UserID="&Session("UserID")&" and  InsurePass='"& BankPassWord &"'")
	   if rs.eof then
		  response.Write("<script language='javascript'>alert('您的银行密码错误!不能进行取银子操作');history.go(-1);</script>")
		  response.End()
	   else
	      MemberOrder=clng(rs("MemberOrder"))		  
	   end if
   	 
	 set rs=conn.execute("select * from AccountsInfo where Accounts='"& username &"'")
	 if rs.eof then
	    response.Write("<script language='javascript'>alert('没有此用户，请确认用户名是否正确！');history.go(-1);</script>")
		response.End()
	 else
	    userid=rs("userid")
	 end if
	 
	 if clng(session("money")) < 10001 then
	    response.Write("<script language='javascript'>alert('你没有太多的银子，不能转帐！');history.go(-1);</script>")
		response.End()
	 end if
	 
	 if 100000 > clng(money) then
	    response.Write("<script language='javascript'>alert('每次最少转帐100000！');history.go(-1);</script>")
		response.End()
	 end if
	 
	 if clng(session("money"))-clng(money)<10000 then
	    response.Write("<script language='javascript'>alert('保底银子为10000！');history.go(-1);</script>")
		response.End()
	 end if
	 
	 if MemberOrder < 1 then
	    BMoney=clng(money)*0.02
	 end if
	 if MemberOrder = 1 then
	    BMoney=clng(money)*0.01
	 end if
	 if MemberOrder > 1 then
	    BMoney=clng(money)
	 end if
	 Money1=clng(money)-clng(BMoney)
	 'response.Write Money
	 
	 CxGame.DbConn(RLWebDBPrefix&"TreasureDb")
	 'sql="insert into GameScoreTransfersLog (UserID,GameID,UserName,GameName,Money,BMoney,DateTime,IP) values ("& Session("UserID") &","& Session("GameID") &",'"& Session("UserName") &"','"& username &"',"& Money &","& BMoney &",'"& getdate() &"','"& Cip &"')"
	 'response.Write sql
	 'response.End()
	 'set rs=conn.execute("insert into GameScoreTransfersLog (UserID,GameID,UserName,GameName,Money,BMoney,DateTime,IP) values ("&Session("UserID")&","& Session("GameID") &",'"& Session("UserName") &"','"& username &"',"& Money &","& BMoney &",'"& date() &"','"& Cip &"')")
	 
	 set rs=conn.execute("update GameScoreInfo set score=score-"& money &" where userid="&Session("UserID")&"")
	 set rs=conn.execute("update GameScoreInfo set score=score+"& Money1 &" where userid="& userid &"")
	 session("money")=clng(session("money"))-clng(money)
	 response.Write("<script language='javascript'>alert('转帐成功！');location.href='TransferJb.asp';</script>")
  end if
%>
<link href="images/css.css" rel="stylesheet" type="text/css">
<table width="770" border="0" cellspacing="0" cellpadding="0" align="center">
  <tr>
    <td><!--#include file="Top.asp" --></td>
  </tr>
</table>
<table width="889" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
    <td width="204" height="362" align="center" valign="top"><table width="180" height="4" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td></td>
      </tr>
    </table>
      <table width="170" height="4" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td bgcolor="#FFFFFF"></td>
        </tr>
    </table><!--#include file="left.asp"--></td>
    <td align="center" valign="top"><table width="583" height="24" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
      <tr>
        <td class="14_px">&nbsp;&nbsp;个人管理中心</td>
      </tr>
    </table>
      <table width="583" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
        <tr>
          <td height="11" colspan="3" align="center"><img src="images/xuzhi.jpg" width="555" height="11"></td>
        </tr>
        <tr>
          <td width="14" height="104" class="bg_hui">&nbsp;</td>
          <td width="565" align="left" class="hang">&nbsp; 1、单笔转帐金额不低于100000币。<BR>
         &nbsp; 2、转帐交易中系统将从您帐号中扣除转帐金额的(普通用户2%，VIP1用户1%，VIP2用户不收)作为交易<br>
        &nbsp;&nbsp;&nbsp;&nbsp; 手续费。<br>
         &nbsp; 3、保底金币不得少于10000币。</td>
          <td width="14" class="bg_huir">&nbsp;</td>
        </tr>
        <tr>
          <td colspan="3" align="center"><img src="images/middle.jpg" width="555" height="6"></td>
        </tr>
      </table>
      <table width="583" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
    
        <tr>
          <td width="14" height="238" class="bg_hui">&nbsp;</td>
          <td width="565" align="center">
		  
		  
		  <table width="530" border="0" cellspacing="0" cellpadding="0">
		  <form action="" method="post" name="myform" onSubmit="return check();">
            <tr>
              <td width="98" height="35" align="right" class="12_pxcu">当前余额：</td>
              <td width="432" height="30" class="red_12px">&nbsp;<%=session("money")%></td>
            </tr>
            <tr>
              <td height="35" align="right" class="12_pxcu">转入游戏帐号：</td>
              <td height="30" class="red_12px">&nbsp;<input name="username" type="text" class="box" id="username">
                *请输入对方的游戏帐号</td>
            </tr>
            <tr>
              <td height="35" align="right" class="12_pxcu">确认游戏帐号：</td>
              <td height="30" class="red_12px">&nbsp;<input name="username2" type="text" class="box" id="username2">
                *请确认对方的游戏帐号</td>
            </tr>
            <tr>
              <td height="35" align="right" class="12_pxcu">转帐金额：</td>
              <td height="30" class="red_12px">&nbsp;<input name="money2" type="text" class="box" id="money2" onKeyPress	= "return regInput(this,	/^[0-9]*$/,		String.fromCharCode(event.keyCode))" value="0" size="10"
		onpaste		= "return regInput(this,	/^[0-9]*$/,		window.clipboardData.getData('Text'))"
		ondrop		= "return regInput(this,	/^[0-9]*$/,		event.dataTransfer.getData('Text'))">
                *请输入转出金额</td>
            </tr>
            <tr>
              <td height="35" align="right" class="12_pxcu">银行密码：</td>
              <td height="30" class="red_12px">&nbsp;<input name="BankPassWord" type="password" class="box" id="BankPassWord">
                *请输您的游戏银行密码
                  <input name="login" type="hidden" id="login4" value="true"></td>
            </tr>
            <tr>
              <td height="35" align="right" class="12_pxcu">&nbsp;</td>
              <td height="30" class="red_12px"><input type="submit" value="确  定">
               &nbsp; <input name="submit" type="reset" value="重  填"></td>
            </tr>
			</form>
          </table>
		  
		  
		  </td>
          <td width="14" class="bg_huir">&nbsp;</td>
        </tr>
        <tr>
          <td colspan="3" align="center"><img src="images/down.gif" width="555" height="3" /></td>
        </tr>
      </table>    </td>
  </tr>
</table>

<table width="913" height="85" border="0" align="center" cellpadding="0" cellspacing="0" class="bj">
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
<table width="770" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><!--#include file="../copy.asp"--></td>
  </tr>
</table>
<script>
	function regInput(obj, reg, inputStr)
	{
		var docSel	= document.selection.createRange()
		if (docSel.parentElement().tagName != "INPUT")	return false
		oSel = docSel.duplicate()
		oSel.text = ""
		var srcRange	= obj.createTextRange()
		oSel.setEndPoint("StartToStart", srcRange)
		var str = oSel.text + inputStr + srcRange.text.substr(oSel.text.length)
		return reg.test(str)
	}
	function check(){
	if(myform.username.value==""){
	alert("请输入转入的帐户！");
	myform.username.focus();
	return (false);
	}
	if(myform.username2.value==""){
	alert("请确认转入帐户！");
	myform.username2.focus();
	return (false);
	}
	if(myform.username.value != myform.username2.value){
	alert("两次输入的帐户不一致！");
	myform.username2.focus();
	return (false);
	}
	if((myform.money2.value=="")||(myform.money2.value==0)){
	alert("请输入你要存入的数目!");
	myform.money2.focus();
	return (false);
	}
	if(myform.BankPassWord.value==""){
	alert("请输入游戏银行密码！");
	myform.BankPassWord.focus();
	return (false);
	}
	}
</script>