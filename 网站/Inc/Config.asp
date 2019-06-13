<%
Option explicit
Response.Buffer=True
Response.Charset="gb2312" 
Session.CodePage=936
dim startime
startime=timer()


Dim flashack_Post,flashack_Get,flashack_In,flashack_Inf,flashack_Xh,flashack_db,flashack_dbstr 
	flashack_In = "'※;※and※exec※insert※select※delete※update※count※*※%※chr※mid※master※truncate※char※declare" 
	flashack_Inf = split(flashack_In,"※") 
	
	
	If Request.QueryString<>"" Then 
	For Each flashack_Get In Request.QueryString 
	For flashack_Xh=0 To Ubound(flashack_Inf) 
	If Instr(LCase(Request.QueryString(flashack_Get)),flashack_Inf(flashack_Xh))<>0 Then 
	Response.Write "<Script Language=JavaScript>alert('flashack防注入系统提示你↓\n\n请不要在参数中包含非法字符尝试注入,QQ:200160！\n\nHTTP://www.dc173.com');</Script>" 
	Response.Write "非法操作！flashack已经给你做了如下记录↓<br>" 
	Response.Write "操作IP："&Request.ServerVariables("REMOTE_ADDR")&"<br>" 
	Response.Write "操作时间："&Now&"<br>" 
	Response.Write "操作页面："&Request.ServerVariables("URL")&"<br>" 
	Response.Write "提交方式：GET<br>" 
	Response.Write "提交参数："&flashack_Get&"<br>" 
	Response.Write "提交数据："&Request.QueryString(flashack_Get)
	Response.End 
	End If 
	Next 
	Next 
	End If 


Dim Conn,SqlConnectionString,Rs,MD5,Maxpage,page,i,RLWebDBPrefix
Rem 数据库头名称
RLWebDBPrefix = "QP"
'Response.write chr(-24159)
Rem 游戏网页操作类开始


Class GameObj
	Private Money,ErrInfo,BPassWord,AMoney,UserName,PassWordTo,BankPassWordTo,BankPassWord
	Private BMoney,CMoney,DMoney,EMoney,CheckPassWordArr,CheckPassWord,CheckUserNameArr,GetPassWord
	Private GameID,GameName,PassWord,ProCode2,ProCodes,CheckI,Ip,Sex,F,CheckUserName,CardCode,CardPass
	Private Ncode,Nmail,Nadd,PassW,PassD,CIP
	
	Private Sub Class_Terminate()

	End Sub 
	
	Private Sub Class_Initialize() 
		BMoney=1000'银行必须存底数额
		CMoney=0.002'转帐服务费百分比
		DMoney=1000'低于多少的不收服务费
		EMoney=1000000000'一天最多能转多少银子
		CheckPassWord="112233,444444,123456,222222,111111,aaaaaa,bbbbbb,888888,666666,123123,555555,cccccc,aabbcc,333333,999999,12345678,654321"
		CheckUserName="管理员,客服,系统,乱泡,乱虫,乱子,日,草,fuck,中生,死全家,操你妈,浆糊,畜生,妈的,垃圾,贱人,慈溪网,熬浆,做爱,性交,糨糊,笑傲,射精,狗屁,操,婊,娘个,屁眼,插,奸,中生,草你妈,婊子生,干你妈,干你全家,手淫,卖淫,找小姐,打炮,精液,儿子,孙子,系统,管理,公告,GM,系統,提示,消息,狗,杂种,工作人员,公布,巡逻,收购,出售,奖,抽奖,zxip,www,买,卖,银,性,热线,电话,手机,售,9334,九三三四,7238,出,262,0806,yin,0566,6276,7687,网通,8270,5885,5808,充值,收,-,_,—,+,=,*,',~,`,!,#,@,$,%,^,&,(,),|,\,?,/,136,139,130,135,134,133,信息,公告,告示,⑻,⑴,⑶,⑹,⑤,⑨,①,⑼,卵子,卵炮,比赛,巡查,【,】,告示,网管,死全家,～,☆,游戏"
		CIP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
		If CIP = "" Then CIP = Request.ServerVariables("REMOTE_ADDR") 
	End Sub 
	
	Private Function IsCheckPassWord(Str)
		CheckPassWordArr=Split(CheckPassWord,",")
		IsCheckPassWord=True
		For CheckI=0 To Ubound(CheckPassWordArr)
			IF Str=CheckPassWordArr(CheckI) Then
				IsCheckPassWord=False
				EXIT Function
			End IF
		Next
	End Function
	
	Private Function IsCheckUserName(Str)
		CheckUserNameArr=Split(CheckUserName,",")
		IsCheckUserName=True
		For CheckI=0 To Ubound(CheckUserNameArr)
			IF instr(Str,CheckUserNameArr(CheckI))>0 Then
				IsCheckUserName=False
				EXIT Function
			End IF
		Next

                if instr(str,chr(32))>0 Or instr(str,chr(34))>0 Then
	        IsCheckUserName=False
	        EXIT Function
		End IF

                if instr(str,",")>0 Or instr(str,",")>0 Then
	        IsCheckUserName=False
	        EXIT Function
		End IF
	End Function
	
	Public Sub BakErr(Info,Url)
		IF Url="" Then
			Url=Request.ServerVariables("HTTP_REFERER")
		End IF
		IF Info="" Then
			Info="不可知错误,返回!"
		End IF
		Response.Write "<script language=JavaScript1.1>alert('"& Info &"');" 
		Response.Write " document.location='"& Url &"';</script>" 
		Response.End
	End Sub
	
	Public Function CheckSqlstr(Str)
		If Isnull(Str) Then
			CheckSqlstr = ""
			Exit Function 
		End If
		Str = Replace(Str,Chr(0),"")
		CheckSqlstr = Replace(Str,"'","''")
		CheckSqlStr=Replace(CheckSqlstr,"　","")
		CheckSqlStr=Replace(CheckSqlstr," ","")
	End Function
	
	
	Public Function GetInfo(Typ,GetType,FormName)'[1|0][form|session|app]
		GetInfo=CheckSqlstr(Trim(Request(FormName)))
		If Typ=1 Then
			IF Not Isnumeric(GetInfo) Then
				GetInfo=1
			Else
				GetInfo=Clng(GetInfo)
			End IF
		End IF	
	End Function
	
	
	Public Sub DbConn(GameDb)
		SqlConnectionString="DRIVER={SQL Server};SERVER=.;UID=sa;PWD=123456789zxcvbnm;DATABASE="& GameDb &";"
		Set Conn= Server.CreateObject("ADODB.Connection")
		Conn.Open SqlConnectionString
	End Sub
	
	Public Sub DbConn60(GameDb)
		Set Conn= Server.CreateObject("ADODB.Connection")
		Conn.Open "DRIVER={SQL Server};SERVER=.;UID=sa;PWD=123456789zxcvbnm;DATABASE="& GameDb &";"
	End Sub
	
	Public Sub DbClose()
		IF IsObject(Conn) Then
			Conn.Close:Set Conn=Nothing
		End IF
	End Sub

	Public Sub RsClose()
		IF IsObject(Rs) Then
			Rs.Close:Set Rs=Nothing
		End IF
	End Sub
	
	rem 输出验证码
    Public Function Vcode()
        Vcode = "<img src=""Inc/GetCode.asp"" onclick=""this.src='Inc/GetCode.asp';"" alt=""看不清或没看到请点这里！"" />"
    	Response.write Vcode
    End Function
	
    Public Function Vcode2()
        Vcode2 = "<img src=""Inc/GetCode2.asp"" onclick=""this.src='Inc/GetCode2.asp';"" alt=""看不清或没看到请点这里！"" />"
    	Response.write Vcode2
    End Function
	
	rem 验证码验证
	Public Function CodeIsTrue()
		Dim CodeStr
		CodeStr=Lcase(Trim(Request("getcode")))
		If CStr(Lcase(Session("GetCode")))=CStr(CodeStr) And CodeStr<>""  Then
			CodeIsTrue=True
			Session("GetCode")=empty
		Else
			CodeIsTrue=False
			Session("GetCode")=empty
		End If	
	End Function 
	
	rem 登陆验证
	Public Sub CheckLogin()
		IF GetInfo(0,"form","login")="true" Then
			IF Not CodeIsTrue Then
				Response.Write "<script>err.innerHTML='<font class=""blue"">验证码错误!</font>'</script>"
				Exit Sub
			End IF
			DbConn(RLWebDBPrefix&"GameUserDb")
			IF GetInfo(0,"form","id")="true" Then
				Set Rs=Conn.Execute("Select UserID,GameID,Accounts,LogonPass,Nullity,MemberOrder,IsBoxPassWord From AccountsInfo Where UserID="& Left(GetInfo(1,"form","username"),10) &"")
			Else
				Set Rs=Conn.Execute("Select UserID,GameID,Accounts,LogonPass,Nullity,MemberOrder,IsBoxPassWord From AccountsInfo Where Accounts='"& Left(GetInfo(0,"form","username"),20) &"'")
			End IF
			
			IF Rs.Eof Then
				Response.Write "<script>err.innerHTML='<font class=""blue"">用户名或密码错误!</font>'</script>"
				RsClose:DbClose
				Exit Sub
			Else
				Set md5= New MD5obj
				If md5.calcMD5(GetInfo(0,"form","password"))<>Rs(3) Then
					Response.Write "<script>err.innerHTML='<font class=""blue"">用户名或密码错误!</font>'</script>"
					Set Md5=Nothing
					RsClose:DbClose
					Exit Sub
				Else
					Set Md5=Nothing
					IF Rs(4)=True Then
						Response.Write "<script>err.innerHTML='<font class=""blue"">此帐号已被管理员冻结!</font>'</script>"
						RsClose:DbClose
						Exit Sub
					Else
						Session("UserID")=Rs(0)
						Session("GameID")=Rs(1)
						Session("UserName")=Rs(2)
						Session("PassWord")=Rs(3)
						Session("Vip")=Rs(5)
						Session("IsBox")=Rs(6)
						
						RsClose:DbClose
						Response.Redirect "user/"
					End IF
				End IF
			End IF
		End IF
		IF GetInfo(0,"form","exit")="true" Then
			Session("UserID")=Empty
			Session("GameID")=Empty
			Session("UserName")=Empty
			Session("PassWord")=Empty
			Session("Vip")=Empty
			Session("Deposits")=Empty	
			Session("money")=Empty
			Session("IsBox")=Empty
			Response.Redirect "/index.asp"
		End IF
	End Sub
	
	rem 检测是否登陆
	Public Function IsLogin()
		IsLogin=False
		
		if Session("Userid")="" then Session("Userid")=Request.Cookies("UserID")
		IF Not IsEmpty(Session("UserID")) And Session("UserID")<>"" Then
			IsLogin=True
		End IF
	End Function
	
	rem 用户登陆
	Public Sub LoginInfo()
		Response.Write "<table   width=""94%"" border=""0"" cellpadding=""0"" cellspacing=""0"">"
		IF IsLogin Then
			Dim Vip
			IF Session("Vip")=0 Then
				Vip="普通"
			Else
				Vip="Vip"
			End IF
			CheckLogin()
			Response.Write "<tr><td valign=""top"">"
			Response.Write("&nbsp;您好!<strong><font color=#ff0000>"& Session("UserName") &"</font></strong><br><br>&nbsp;您的ID号为:<font color=#ff0000><strong>"& Session("GameID") &"</strong></font>,<br>&nbsp;你是本站的"& Vip &"用户,您已成功登陆! <br><br>[<a href=""?exit=true"">退出登入</a>] [<a href=""user/"">个人管理</a>]<br><br><br><br><br><br>")
			Response.Write "</td></tr>"
		Else
			Response.Write "<form name=""form1"" method=""post"" action="""">"
			Response.Write "<tr><td width=""40%"" height=28 align=""center"" class=""lan"">游戏账号:</td><td width=""60%""><input name=""UserName"" type=""text"" class=""texform"" id=""UserName"" size=""10""></td>"
			Response.Write "</tr><tr><td align=""center"" class=""lan"">密　　码:</td>"
			Response.Write "<td><input name=""PassWord"" type=""password"" class=""texform"" id=""PassWord"" size=""8""></td>"
			Response.Write "</tr><tr><td align=""center"" class=""lan"">附 加 码:</td>"
			Response.Write "<td><input height=""29"" name=""getcode"" type=""text"" class=""yanzheng"" id=""getcode"" size=""6""><input name=""login"" type=""hidden"" id=""login"" value=""true""><input name=""id"" type=""hidden"" id=""login"" value=""false"">"
			Vcode()
			Response.Write "</td></tr><tr><td  colspan=""2""  id=""err"" align=""center"" style=""height:1px""></td><td >"
			Response.Write "</td></tr><tr><td colspan=""2"" height=""50px"" align=""center""><input name=""imageField"" type=""image"" src=""images/deng.gif"" border=""0"">&nbsp;&nbsp;<a href=""reg.asp""><img src=""images/reg.gif"" width=78 height=33 border=0></a>"
			CheckLogin()				
			Response.Write "</td></tr><tr><td colspan=""2""><table width=""100%"" border=""0"" align=""center"" cellpadding=""1"" cellspacing=""0"">"
			Response.Write "<tr><td  align=""center""><a href=""SSPassWord.asp""><img src=""images/fing.gif"" border=0></a>&nbsp;&nbsp;<a href=""Passwordprotection.Asp""><img src=""images/baohu.jpg"" border=0></a></td><td>"
			
			Response.Write "</tr></table></td></tr>"
			Response.Write "</form>"
		End iF
		rem Response.Write "<tr><td colspan=""2""><div align=""center""><img src=""img/showbb.gif""><a href=""payhelp.asp""><img src=""img/logo_buyitem.gif"" width=""100"" height=""60"" border=""0""></a></div></td></tr>"
		Response.Write "</table>"
	End Sub
	
	rem 检测是否登陆
	Public Function IsCheckLogin()
		IF Not IsLogin Then
		'response.Write IsLogin
			BakErr "您还没有登入,请登入后再行操作!","/Login.asp"
		End IF
	End Function

	rem 修改密码
	Public Sub UpdatePassWord(Typ)'[LogonPass|InsurePass]
	IsCheckLogin()
		IF GetInfo(0,"form","UpPass")="true" Then
			IF Not CodeIsTrue Then
				Response.Write "<script>alert('验证码错误!');</script>"
				Exit Sub
			End IF
			
			IF GetInfo(0,"form","password")<>GetInfo(0,"form","password2") Then
				Response.Write "<script>err.innerHTML='<strong><font color=#000000>两次输入的密码不相同!</font></strong>'</script>"
				Exit Sub
			Else
				DbConn(RLWebDBPrefix&"GameUserDb")
				IF Typ="InsurePass" And Session("IsBox")=False Then
					Set md5= New MD5obj
					GetPassWord=SetPassword(GetInfo(0,"form","UserName"))
					Set Rs=Conn.Execute("Exec GSP_GP_UpdatePassWord2 "& Session("UserID") &",'"& GetPassWord &"','"& md5.calcMD5(GetInfo(0,"form","PassWord")) &"','"&Typ&"'")
					Response.Write "<script>err.innerHTML='<strong><font color=#000000>"& Rs(0) &"</font></strong>'</script>"
					IF Left(Rs(0),2)="成功" Then
						Session("IsBox")=True
					End IF
					Set Md5=Nothing
				Else
					Set md5= New MD5obj
					Set Rs=Conn.Execute("Exec GSP_GP_UpdatePassWord "& Session("UserID") &",'"& md5.calcMD5(GetInfo(0,"form","UserName")) &"','"& md5.calcMD5(GetInfo(0,"form","PassWord")) &"','"&Typ&"'")
					Response.Write "<script>err.innerHTML='<strong><font color=#000000>"& Rs(0) &"</font></strong>'</script>"
					Set Md5=Nothing
				End IF
				RsClose:DBClose:Exit Sub
			End IF
		End IF
	End Sub
	
	rem 检测账号是否在游戏中
	Private Function IsWealthGame(UserID)
		DbConn(RLWebDBPrefix&"TreasureDb")
		Set Rs=Conn.Execute("Select UserID From GameScoreLocker Where UserID="& UserID &"")
		IF Not Rs.Eof Then
			BakErr "对不起,您目前正在银子类游戏房间(如梭哈,麻奖等)中!不能进行银行操作!要继续进行之前操作请退出游戏.","index.Asp"
		End IF
		RsClose:DbClose
	End Function
	
	Public Sub ServeWealthShow(UserID)
		IsWealthGame(UserID)
		IP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
		If IP = "" Then IP = Request.ServerVariables("REMOTE_ADDR")
		
		IF Session("Deposits")="" Or IsEmpty(Session("Deposits")) Then
			DbConn(RLWebDBPrefix&"TreasureDb")
			Set Rs=Conn.Execute("Select Score,Bnak From GameScoreInfo Where UserID="& UserID &"")
			IF Not Rs.Eof Then
				Session("Deposits")=Rs(1)	
				Session("money")=Rs(0)
			Else 
				Conn.Execute("INSERT INTO GameScoreInfo(UserID,RegisterIP,LastLogonIP) VALUES ("&UserID&",'"& IP &"','"& IP &"')")
				Session("Deposits")=0	
				Session("money")=0	
			End IF
			RsClose:DbClose
		End IF
	End Sub
	
	Public Sub ServeWealth()
		IsCheckLogin()
		IF GetInfo(0,"form","login")="true" Then
			IF Not Isnumeric(GetInfo(0,"form","money2")) Or instr(GetInfo(0,"form","money2"),".")>0 Then
				BakErr "存入的银子必须为整数,并且不能大于现有银子!",""
				Exit Sub
			End IF
			if clng(Session("Deposits")) > 990000000 then 
			BakErr "您的存款超过9900000000!不能进行存银子操作",""
				Exit Sub
			End IF
			money=GetInfo(1,"form","money2")
			IF Money<1 Then
				BakErr "您的现金为零!不能进行存银子操作",""
				Exit Sub
			End IF
			if int(GetInfo(0,"form","money2"))<100000 then
			    bakerr "存入的银子必须大于100000！",""
				exit sub
			end if
			DbConn(RLWebDBPrefix&"TreasureDb")
			Set Rs=Conn.Execute("Select Score From GameScoreInfo Where UserID="& session("userid") &"")
			dim sheng
			sheng=clng(rs("Score"))-int(GetInfo(0,"form","money2"))
			if clng(sheng) < 10000 then
			   BakErr "保底银子不能少于10000",""
			   exit sub
			end if
			DbConn(RLWebDBPrefix&"TreasureDb")
			Set Rs=Conn.Execute("Exec GSP_GP_ServeWealth "& Session("UserID") &","& money &",'"& Cip &"'")
			Errinfo=RS(0)
			RsClose:DbClose
			IF Left(Errinfo,4)="成功信息" Then
				Session("Deposits")=Empty	
				Session("money")=Empty	
			End IF
			BakErr Errinfo,""
			Exit Sub
		End IF
		ServeWealthShow(Session("UserID"))
	End Sub	
	
	Public Sub ServeWealth222()
		IsCheckLogin()
		IF GetInfo(0,"form","login")="true" Then
			IF Not CodeIsTrue Then
				BakErr "验证码错误!",""
				Exit Sub
			End IF
			IF Not Isnumeric(GetInfo(0,"form","money2")) Or instr(GetInfo(0,"form","money2"),".")>0 Then
				BakErr "存入的银子必须为整数,并且不能大于现有银子!",""
				Exit Sub
			End IF
			money=GetInfo(1,"form","money2")
			IF Money<1 Then
				BakErr "您的现金为零!不能进行存银子操作",""
				Exit Sub
			End IF
			DbConn(RLWebDBPrefix&"TreasureDb")
			Set Rs=Conn.Execute("Exec Wh_Serve "& Session("UserID") &","& money &"")
			Errinfo=RS(0)
			RsClose:DbClose
			IF Left(Errinfo,4)="成功信息" Then
				Session("Deposits")=Empty	
				Session("money")=Empty	
			End IF
			BakErr Errinfo,""
			Exit Sub
		End IF
		ServeWealthShow(Session("UserID"))
	End Sub
	
	rem 银行存取
	Public Sub ReceiveWealth()
		IsCheckLogin()
		IF GetInfo(0,"form","login")="true" Then
			IF Not Isnumeric(GetInfo(0,"form","money2")) Or instr(GetInfo(0,"form","money2"),".")>0 Then
				BakErr "存入的银子必须为整数,并且不能大于现有银子!",""
				Exit Sub
			End IF
			money=GetInfo(1,"form","money2")
			IF Money<1 Then
				BakErr "您的取的银子为零!不能进行取银子操作",""
				Exit Sub
			End IF
			DbConn(RLWebDBPrefix&"TreasureDb")
			Set md5= New MD5obj
			BPassWord=md5.calcMD5(GetInfo(0,"form","PassWord"))
			Set Md5=Nothing
			
			IF Session("IsBox")=False Then
				GetPassWord=SetPassword(GetInfo(0,"form","PassWord"))			
				Set Rs=Conn.Execute("Exec GSP_GP_ReceiveWealth2 "& Session("UserID") &","& money &",'"& BPassWord &"','"& GetPassWord &"'")
			Else
				Set Rs=Conn.Execute("Exec GSP_GP_ReceiveWealth "& Session("UserID") &","& money &",'"& BPassWord &"','"& Cip &"'")
			End IF
			
			Errinfo=RS(0)
			RsClose:DbClose
			IF Left(Errinfo,4)="成功信息" Then
				Session("Deposits")=Empty	
				Session("money")=Empty
				Session("IsBox")=True	
			End IF
			BakErr Errinfo,""
			Exit Sub
		End IF
		ServeWealthShow(Session("UserID"))
	End Sub
	
	Public Sub OutBank()
	    IsCheckLogin()
		dim sql
		IF GetInfo(0,"form","login")="true" Then
		   Set md5= New MD5obj
		   money=GetInfo(0,"form","money2")
		   GetPassWord=GetInfo(0,"form","PassWord")
	       GetPassWord=md5.calcMD5(GetPassWord)
		   money=GetInfo(0,"form","money2")
		   'response.Write(GetPassWord)
		   DbConn("QPGameUserDB")
		   set rs=conn.execute("select * from AccountsInfo where UserID="&Session("UserID")&" and  InsurePass='"& GetPassWord &"'")
		   'sql="select * from AccountsInfo where UserID="&Session("UserID")&" and  InsurePass='"& GetPassWord &"'"
'		   response.Write sql
'		   response.End()
		   if rs.eof then
		      BakErr "您的银行密码错误!不能进行取银子操作",""
			  exit sub
		   end if
		   if int(money)<1 then
		      BakErr "您的取的银子为零!不能进行取银子操作",""
			  Exit Sub
		   End IF
		   if int(money)>int(clng(Session("Deposits"))) then
		      BakErr "取出的银两不能大于银行的存款！",""
			  exit sub
		   end if
		   'dim score
			    DbConn(RLWebDBPrefix&"TreasureDb")
				Set Rs=Conn.Execute("Select Score From GameScoreInfo Where UserID="& session("userid") &"")
				dim score,bnak
				bnak=clng(Session("Deposits"))-clng(money)
				score=clng(rs("Score"))+clng(GetInfo(0,"form","money2"))


'sql="insert into MoneyLog (userid,type,money,datetime,bnak,score,ip) values ("& Session("UserID") &",'取出',"& money &",'"& date() &"',"& bnak &","& score &",'"& CIP &"')"
'response.Write sql
'response.End()
		   set rs=conn.execute("insert into MoneyLog (userid,type,money,datetime,bnak,score,ip) values ("& Session("UserID") &",'取出',"& money &",'"& date() &"',"& bnak &","& score &",'"& CIP &"')")
		   
		   set rs=conn.execute("update GameScoreInfo set score=score+"& money &",bnak=bnak-"& money &" where UserID="& session("userid") &"")
		   
		   Set Rs=Conn.Execute("Select Score,bnak From GameScoreInfo Where UserID="& session("userid") &"")
		       session("money")=rs("Score")
			   Session("Deposits")=rs("bnak")			  
		   'RsClose:DbClose
		   'Exit Sub
		   response.Write("<script language='javascript'>alert('取出银子成功');location.href='outbank.asp';</script>")
		end if		
		ServeWealthShow(Session("UserID"))
	end sub
	
	rem 转账
	Public Sub Transfers()
		IsCheckLogin()
		IF GetInfo(0,"form","login")="true" Then
			GameID=GetInfo(1,"form","UserID")
			GameName=GetInfo(0,"form","UserName")
			
			IF Session("UserID")=GameID Then
				BakErr "不能对自已转帐!",""
				Exit Sub
			End If
			
			IF Not Isnumeric(GetInfo(0,"form","money2")) Or instr(GetInfo(0,"form","money2"),".")>0 Then
				BakErr "转帐的银子必须为整数,并且不能大于现有银子!",""
				Exit Sub
			End IF
			
			Money=GetInfo(1,"form","money2")
			IF Money<1 Then
				BakErr "您的转的银子为零或负数!不能进行取银子操作",""
				Exit Sub
			End IF
			IF Clng(Session("Deposits"))<Money Or (Clng(Session("Deposits"))-Money)<BMoney Then
				BakErr "银行必须有"& BMoney &"两银子留底!",""
				Exit Sub
			End IF
			
			IF Money>EMoney Then
				BakErr "对不起,您一天最多只能转"& EMoney &" 两银子!",""
				Exit Sub
			End IF
			
			
			IF Money>DMoney Then
				AMoney=Clng(Money*CMoney)
			Else
				AMoney=0
			End IF
			DbConn(RLWebDBPrefix&"TreasureDb")
			Set md5= New MD5obj
			BPassWord=md5.calcMD5(GetInfo(0,"form","BankPassWord"))
			Set Md5=Nothing
			IF Session("IsBox")=False Then
				GetPassWord=SetPassword(GetInfo(0,"form","PassWord"))
				Set Rs=Conn.Execute("Exec GSP_GP_Transfers2 "&Session("UserID")&",'"&Session("UserName")&"','"& BPassWord &"',"&GameID&",'"&GameName&"',"& BMoney &","& AMoney &","& Money &","& EMoney &",'"& GetPassWord &"'")
			Else
				Set Rs=Conn.Execute("Exec GSP_GP_Transfers "&Session("UserID")&",'"&Session("UserName")&"','"& BPassWord &"',"&GameID&",'"&GameName&"',"& BMoney &","& AMoney &","& Money &","& EMoney &",'"&Cip&"'")
			End IF
			Errinfo=RS(0)
			RsClose:DbClose
			Errinfo=Replace(Errinfo,Chr(13),"")
			Errinfo=Replace(Errinfo,Chr(10),"")
			Errinfo=Replace(Errinfo,"@DMoney",EMoney)
			Errinfo=Replace(Errinfo,"@GameName",GameName)
			Errinfo=Replace(Errinfo,"@money",Money)
			Errinfo=Replace(Errinfo,"@amoney",AMoney)
			Errinfo=Replace(Errinfo,"@Bmoney",BMoney)
			IF Left(Errinfo,4)="成功信息" Then
				Session("Deposits")=Empty	
				Session("money")=Empty
				Session("IsBox")=True	
			End IF
			BakErr Errinfo,""
			Exit Sub
		End IF
		ServeWealthShow(Session("UserID"))
	End Sub
	
	rem 密码保护
	Public Sub ProCode()
		IsCheckLogin()
		DbConn(RLWebDBPrefix&"GameUserDb")
		Set Rs=Conn.Execute("Select PassWordCode From AccountsInfo Where UserID="& Session("UserID") &"")
		IF Not Rs.Eof Then
			If Not IsNull(Rs(0)) And Rs(0)<>"" Then
				Errinfo="对不起,您已经申请了密码保护!"
				BakErr Errinfo,""
				RsClose:DbClose
			End IF
		End IF
		IF GetInfo(0,"form","loginsev")="true" Then
			IF Not CodeIsTrue Then
				BakErr "验证码错误!",""
				Exit Sub
			End IF
			
			IF GetInfo(0,"form","Code")="" Or Len(GetInfo(0,"form","Code"))<6 Then
				BakErr "对不起!安全码不能为空,并且字数不能少于6位",""
				Exit Sub
			End IF
			
			IF GetInfo(0,"form","Code")=Session("UserName") Then
				BakErr "对不起,安全码不能和用户名相同!",""
				Exit Sub
			End IF
			IF Isnumeric(GetInfo(0,"form","Code")) Then
				BakErr "安全码不能为纯数字!建议用字母和数字混合,并且记在相关笔记本上!",""
				Exit Sub
			END if
			IF GetInfo(0,"form","Code")<>GetInfo(0,"form","Code2") Then
				BakErr "两次输入的安全码不正确!",""
				Exit Sub
			END if
			DbConn(RLWebDBPrefix&"GameUserDb")
			Set md5= New MD5obj
			PassWord=md5.calcMD5(GetInfo(0,"form","PassWord"))
			BPassWord=md5.calcMD5(GetInfo(0,"form","BankPassWord"))
			ProCode2=md5.calcMD5(GetInfo(0,"form","Code"))
			ProCodes=GetInfo(0,"form","Code")
			Set Md5=Nothing
			Set Rs=Conn.Execute("Exec GSP_GP_ProCode "& Session("UserID") &",'"&PassWord&"','"&BPassWord&"','"&ProCodes&"','"& ProCode2 &"'")
			Errinfo=Rs(0)
			RsClose:DbClose
			BakErr Errinfo,"Index.asp"
			Exit Sub
		End IF
	End Sub
	
	rem 找回密码
	Public Sub FindPassWord()
		IF GetInfo(0,"form","rlogin")="true" Then
			IF Session("FindPassWordCount")="" Or IsEmpty(Session("FindPassWordCount")) Then
				Session("FindPassWordCount")=0
			End IF
			Session("FindPassWordCount")=Session("FindPassWordCount")+1
			IF Not CodeIsTrue Then
				BakErr "验证码错误!",""
				Exit Sub
			End IF
			IF Session("FindPassWordCount")>=6 Then
				BakErr "您操作的次数过多!","index.asp"
				Exit Sub
			End IF
			
			ProCodes=GetInfo(0,"form","Code")
			IF ProCodes="" Or Len(ProCodes)<6 Or IsNull(ProCodes) Then
				BakErr "安全码错误!",""
				Exit Sub
			End IF
			IF GetInfo(0,"form","PassWord")=ProCodes Or GetInfo(0,"form","BankPassWord")=ProCodes Then
				BakErr "重设的密码不能和安全码相同!","index.asp"
				Exit Sub
			End IF
			Set md5= New MD5obj
			PassWord=md5.calcMD5(GetInfo(0,"form","PassWord"))
			BPassWord=md5.calcMD5(GetInfo(0,"form","BankPassWord"))
			Set Md5=Nothing
			UserName=GetInfo(0,"form","UserName")
			DbConn(RLWebDBPrefix&"GameUserDb")
			Set Rs=Conn.Execute("Exec GSP_GP_FindPassWord '"& UserName &"','"&PassWord&"','"&BPassWord&"','"&ProCodes&"'")
			Errinfo=Rs(0)
			RsClose:DbClose
			BakErr Errinfo,"Index.asp"
		End IF
	End Sub
	
	rem 规则跳转
	Public Sub GameGuanze()
		DbConn(RLWebDBPrefix&"ServerInfoDb")
		Set Rs=Conn.Execute("Select GzUrl From GameKindItem Where KindID="& GetInfo(1,"form","KindID") &"")
		IF Not Rs.Eof Then
			Response.Redirect("guizhi/"&Rs(0))
			RsClose:DbClose
			Response.End
		End IF
		RsClose:DbClose
	End Sub
	
	rem 下载跳转
	Public Sub GameDown()
		IF GetInfo(1,"form","KindID")=0 Then
			Response.Redirect("down/Plaza.exe")
			exit sub
			Response.End
		end if
		DbConn(RLWebDBPrefix&"ServerInfoDb")
		Set Rs=Conn.Execute("Select ProcessName From GameKindItem Where KindID="& GetInfo(1,"form","KindID") &"")
		IF Not Rs.Eof Then
			Response.Redirect("down/"&Rs(0))
			RsClose:DbClose
			Response.End
		End IF
		RsClose:DbClose
	End Sub
	
	Public Sub PassWordSS ()
		IF GetInfo(0,"form","sspassword")="true" Then
			IF Not CodeIsTrue Then
				Response.Write "<script>codeerr.className='box4';codeerr.innerHTML='错误:验证码错误,请重新输入!';</script>"
				Exit Sub
			End IF
			Dim UTel,Txt,NNCODE
			UserName=GetInfo(0,"form","UserName")
			Ncode=GetInfo(0,"form","Ncode")
			Nmail=GetInfo(0,"form","Nmail")
			Nadd=GetInfo(0,"form","Nadd")
			PassW=GetInfo(0,"form","PassW")
			PassD=GetInfo(0,"form","PassD")
			UTel=GetInfo(0,"form","Tel")
			Txt=Left(GetInfo(0,"form","Txt"),350)
			IF UserName="" Or Ncode="" Or Nmail="" Or Nadd="" Or PassW="" Or PassD="" Or UTel="" Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='下面资料为必填,请详细填写你注册时的资料';</script>"
				Exit Sub
			End IF
			Randomize Timer
			NNCODE=Int(1000000 * Rnd * Rnd * Rnd)
            NNCODE=Left(NNCODE,6)
			DbConn(RLWebDBPrefix&"GameUserDb")
			Set Rs=Conn.Execute("Select UserID From AccountsInfo Where Accounts='"& UserName &"'")
			IF Rs.Eof Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='您输入的用户名不存在!';</script>"
				RsClose:DbClose
				Exit Sub
			End IF
			Conn.Execute("Exec GetPassWord '"& UserName &"','"& Nmail &"','"& PassW &"','"& PassD &"','"& Ncode &"','"& Nadd &"','"& UTel &"','"& Txt &"','"& NNCODE &"'")
			IF Request.Cookies("passwordsh")("reg")="abb" Then
				Response.Cookies("passwordsh")("reg")="abbabb"
			Else
				Response.Cookies("passwordsh")("reg")="abb"
			End IF
			Response.Cookies("passwordsh").Expires=date+1
			Response.Write "<form id=""forma"" action=""Ssshow.Asp"" method=""post"">"
			Response.Write "<input type=""hidden"" name=""nncode"" id=""nncode"" value="& nncode &"></form>"
			Response.Write "<script type=""text/javascript"">"
			Response.Write "forma.submit();"
			Response.Write "</script>"
		End IF
	End Sub
	
	Public Sub SSSelect()
		IF GetInfo(0,"form","ssselect")="true" Then
			IF Not CodeIsTrue Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:验证码错误,请重新输入!';</script>"
				Exit Sub
			End IF
			UserName=GetInfo(0,"form","UserName")
			Ncode=GetInfo(0,"form","code")
			DbConn(RLWebDBPrefix&"GameUserDb")
			Set Rs=Conn.Execute("Select UserID,PassWord From PassWordList Where UserID='"& UserName &"' Order By ID Desc")
			IF RS.EOF THEN
				RsClose:DbClose
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:此用户没有申诉或您的申诉码不正确!';</script>"
				Exit Sub
			Else
				IF Rs(1)<>Ncode Then
					RsClose:DbClose
					Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:此用户没有申诉或您的申诉码不正确!';</script>"
					Exit Sub
				End IF
				'Session("UserNameUbb")=UserName
			End IF
			RsClose
			Dim U
			Response.Write "<table width=""100%"" border=""0"" cellpadding=""5"" cellspacing=""0"" class=""box"">"
        	Response.Write "<tr><td width=""13%"" background=""img/q03.jpg""> <div align=""center""><font color=""#FFFFFF"">用户名</font></div></td>"
            Response.Write "<td width=""19%"" background=""img/q03.jpg""> <div align=""center""><font color=""#FFFFFF"">申诉时间</font></div></td>"
            Response.Write "<td width=""68%"" background=""img/q03.jpg""> <div align=""center""><font color=""#FFFFFF"">申诉结果</font></div></td></tr>"
            Set Rs=Conn.Execute("Select Top 1 UserID,IsCut,SSDate,PassWord2 From PassWordList Where UserID='"& UserName &"' Order By ID Desc")
            Do While Not Rs.Eof
			IF Rs(1)=0 Then
				U="等待处理..."
			End IF
			IF Rs(1)=1 Then
				U="申诉成功:"
			End IF
			IF Rs(1)=2 Then
				U="申诉失败:"
			End IF
          	Response.Write "<tr><td><div align=""center"">"& Rs(0) &"</div></td>"
			Response.Write "<td><div align=""center"">"& Rs(2) &"</div></td>"
			Response.Write "<td><div align=""center"">"& U & Rs(3) &"</div></td></tr>"
          	Rs.MoveNext
			Loop
			RsClose:DbClose
		    Response.Write "</table>"
		End IF
	End Sub
	
		rem 用户注册
	Public Sub UserReg()
	dim tuiguang,SpreaderID
		IF GetInfo(0,"form","reg")="true" Then
			IF Not CodeIsTrue Then
				Response.Write "<script>codeerr.className='box4';codeerr.innerHTML='错误:验证码错误,请重新输入!';</script>"
				Exit Sub
			End IF
			UserName=GetInfo(0,"form","UserName")
			PassWord=Cstr(GetInfo(0,"form","PassWord"))
			PassWordTo=Cstr(GetInfo(0,"form","PassWord2"))
			BankPassWordTo=Cstr(GetInfo(0,"form","BankPassWord2"))
			BankPassWord=Cstr(GetInfo(0,"form","BankPassWord"))
			tuiguang=Cstr(GetInfo(0,"form","tuiguang"))
			
			DbConn("QPGameUserDB")
			set rs=conn.execute("select userid from AccountsInfo where Accounts='"& tuiguang &"'")
			if not rs.eof then
			   SpreaderID=rs(0)
			end if
			
			
			IF UserName="" Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:用户名不能为空!';</script>"
				Exit Sub
			End IF
			IF Not IsCheckUserName(UserName) Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:用户名中有非法的字符请换个用户名!';</script>"
				Exit Sub
			End IF
			
			IF PassWord="" Or Len(PassWord)<6 Or Not IsCheckPassWord(PassWord) Then
				Response.Write "<script>passworderr.className='box4';passworderr.innerHTML='错误:您的登入密码过于简单,为了防止密码被盗,长度必须超过6位,并且不要以123456,111111,888888,AAAAAA等简单字符做为您的密码!';</script>"
				Exit Sub
			End IF
			IF PassWord=UserName Then
				Response.Write "<script>passworderr.className='box4';passworderr.innerHTML='错误:登入密码不能与用户名相同!';</script>"
				Exit Sub
			End IF
			IF PassWord<>PassWordTo Then
				Response.Write "<script>passworderr.className='box4';passworderr.innerHTML='错误:您两次输入的登入密码不一样!';</script>"
				Exit Sub
			End IF
			
			IF BankPassWord="" Or Len(BankPassWord)<6 Or Not IsCheckPassWord(BankPassWord) Then
				Response.Write "<script>BankPassWorderr.className='box4';BankPassWorderr.innerHTML='错误:您的银行密码过于简单,为了防止密码被盗,长度必须超过6位,并且不要以123456,111111,888888,AAAAAA等简单字符做为您的密码!';</script>"
				Exit Sub
			End IF
			
			IF BankPassWord=UserName Then
				Response.Write "<script>BankPassWorderr.className='box4';BankPassWorderr.innerHTML='错误:银行密码不能与用户名相同!';</script>"
				Exit Sub
			End IF
			
			IF BankPassWord=PassWord Then
				Response.Write "<script>BankPassWorderr.className='box4';BankPassWorderr.innerHTML='错误:银行密码不能与登入密码相同!';</script>"
				Exit Sub
			End IF
			
			IF BankPassWord<>BankPassWordTo Then
				Response.Write "<script>BankPassWorderr.className='box4';BankPassWorderr.innerHTML='错误:您两次输入的银行密码不一样!';</script>"
				Exit Sub
			End IF
			Sex=GetInfo(1,"form","Sex")
			F=GetInfo(1,"form","FF")
			Ncode=GetInfo(0,"form","Ncode")
			Nmail=GetInfo(0,"form","Nmail")
			Nadd=GetInfo(0,"form","Nadd")
			PassW=GetInfo(0,"form","PassW")
			PassD=GetInfo(0,"form","PassD")
			IF Ncode="" Or Nmail="" Or Nadd="" Or PassW="" Or PassD="" Then
				Response.Write "<script>getpass.className='box4';getpass.innerHTML='错误:下面的资料为必填,请认址填写,可能会对您找回密码有关键作用';</script>"
				Exit Sub
			End IF
			
			
			F=F-1
			IP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
			If IP = "" Then IP = Request.ServerVariables("REMOTE_ADDR")
			Set md5= New MD5obj
			PassWord=md5.calcMD5(PassWord)
			BankPassWord=md5.calcMD5(BankPassWord)
			Set Md5=Nothing
			DbConn(RLWebDBPrefix&"GameUserDb")
			set rs=conn.execute("select * from AccountsInfo where Accounts='"& username &"'")
			if not rs.eof then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:用户名已被注册!';</script>"
				RsClose:DbClose
				Exit Sub					                  
				
			Else
			   DIM RS1
			   Set Rs=Conn.Execute("insert into AccountsInfo (Accounts,LogonPass,InsurePass,Gender,FaceID,RegisterIP,C_IDNO,C_EMAIL,C_ADDRESS,C_PROTECTQUES,C_PROTECTANSW,SpreaderID,RegAccounts,LastLogonIP) values('"& UserName &"','"& PassWord &"','"& BankPassWord &"',"& Sex &","& F &",'"& IP &"','"& Ncode &"','"& Nmail &"','"& Nadd &"','"& PassW &"','"& PassD &"','"& SpreaderID &"','"& UserName &"','"& IP &"')")
			   SET rs1=conn.execute("update AccountsInfo set GameID=(userid-1)+100000 where Accounts='"& username &"'")	
			   set rs1=conn.execute("select * from AccountsInfo where Accounts='"& username &"'")
			   dim ssd_id
			   ssd_id=rs1("userid")
			   CxGame.DbConn("QPTreasureDB")
			   dim rs2
			   set rs2=conn.execute("insert into GameScoreInfo (userID,score,RegisterIP,LastLogonIP) values ('"& ssd_id &"',10000,'"& IP &"','"& IP &"')")
				Response.Cookies("cxgame")("reg")="reok00"
				Response.Cookies("cxgame").Expires=date+1
				response.Write("<script language='javascript'>alert('注册成功!');location.href='EndReg.asp';</script>")
				'BakErr "您已成功注册,您的ID号是:"&Rs(0),"EndReg.Asp?ID="&Rs(0)
				'RsClose:DbClose
				Exit Sub
			End IF
			
		End IF
	End Sub
	
	Public Sub EyUserReg()
		IF GetInfo(0,"form","login")="true" Then
			IF Not CodeIsTrue Then
				Response.Write "<script>codeerr.className='box4';codeerr.innerHTML='错误:验证码错误,请重新输入!';</script>"
				Exit Sub
			End IF
			UserName=GetInfo(0,"form","UserName")
			PassWord=Cstr(GetInfo(0,"form","PassWord"))
			PassWordTo=Cstr(GetInfo(0,"form","PassWord2"))
			IF UserName="" Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:用户名不能为空!';</script>"
				Exit Sub
			End IF
			IF Not IsCheckUserName(UserName) Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:用户名中不能有管理员,游戏中心,客服等字符!';</script>"
				Exit Sub
			End IF
			
			IF PassWord="" Or Len(PassWord)<6 Or Not IsCheckPassWord(PassWord) Then
				Response.Write "<script>passworderr.className='box4';passworderr.innerHTML='错误:您的登入密码过于简单,为了防止密码被盗,长度必须超过6位,并且不要以123456,111111,888888,AAAAAA等简单字符做为您的密码!';</script>"
				Exit Sub
			End IF
			IF PassWord=UserName Then
				Response.Write "<script>passworderr.className='box4';passworderr.innerHTML='错误:登入密码不能与用户名相同!';</script>"
				Exit Sub
			End IF
			IF PassWord<>PassWordTo Then
				Response.Write "<script>passworderr.className='box4';passworderr.innerHTML='错误:您两次输入的登入密码不一样!';</script>"
				Exit Sub
			End IF
			
			Sex=GetInfo(1,"form","Sex")
			F=GetInfo(1,"form","FF")
			F=F-1
			IP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
			If IP = "" Then IP = Request.ServerVariables("REMOTE_ADDR")
			Set md5= New MD5obj
			PassWord=md5.calcMD5(PassWord)
			BankPassWord=md5.calcMD5(BankPassWord)
			Set Md5=Nothing
			DbConn(RLWebDBPrefix&"GameUserDb")
			Set Rs=Conn.Execute("Exec GSP_GP_UserReg '"& UserName &"','"& PassWord &"','"& PassWord &"',"& Sex &","& F &",'"& IP &"'")
			IF Rs(0)=0 Then
				Response.Write "<script>usernameerr.className='box4';usernameerr.innerHTML='错误:用户名已被注册!';</script>"
				RsClose:DbClose
				Exit Sub
			Else
				BakErr "您已成功注册,您的ID号是:"&Rs(2),"EndReg.Asp?ID="&Rs(2)
				RsClose:DbClose
				Exit Sub
			End IF
			
		End IF
	End Sub
	
	
	rem 充值
	Public Sub Pay()
		
			IF GetInfo(0,"form","login")="true" Then
				IF Not CodeIsTrue Then
					BakErr "您输入的验证码不正确!",""
					Exit Sub
				End IF
				dim userid,username1
				CardCode=GetInfo(0,"form","CardCode")
				CardPass=GetInfo(0,"form","CardPass")
				UserID=GetInfo(0,"form","userid")
				username1=GetInfo(0,"form","username1")
				IsWealthGame(UserID)
				IF Not Isnumeric(CardCode) Or Not Isnumeric(CardPass) Then
					BakErr "对不起,你的卡号或密码错误!","pay1.asp?username1="& username1 &""
					Exit Sub
				End IF
				DbConn("CxGameTreasureDb")
				Set Rs=Conn.Execute("Exec CxGamePty "& CardCode &",'"& CardPass &"','"& userid &"'")
				Errinfo=Rs(0)
				RsClose:DbClose
				IF Left(Errinfo,4)="成功信息" Then
					Session("Deposits")=Empty	
					Session("money")=Empty
				End IF
				BakErr Errinfo,""
				Exit Sub
			End IF
	End Sub
	
	rem 更改头像
	Public Sub UpdateFF()
		IsCheckLogin()
		IF GetInfo(0,"form","login")="true" Then
			IF Not CodeIsTrue Then
				BakErr "您输入的验证码不正确!",""
				Exit Sub
			End IF
			F=GetInfo(1,"form","FF")
			F=F-1
			DbConn(RLWebDBPrefix&"GameUserDb")
			Conn.Execute ("Update AccountsInfo Set FaceID="& F &" Where UserID="& Session("UserID") &"")
			DbClose
			BakErr "更换头像成功!",""
		End IF
	End Sub
	
	rem 头条新闻
	Rem 调用方式 NewsObj.NewsTop "100",10,15,"CssName"
	Public Sub NewsTop(ClassCode,Top,TitleLen,Css)
		Response.Write "<ul>"
		DBConn("News")
		Dim Title
		
		Set Rs=Conn.Execute("Select Top "& Top &" Id,Newstitle,Newsdate From News Where ClassCode Like '"& ClassCode &"%' Order By ID DESC")
		Do While Not Rs.Eof
			Title=Rs(1)
			IF Len(Title)>TitleLen Then
				Title=Left(Title,TitleLen) & ".."
			End IF
			Response.Write "<li>.<a class="""& Css &""" target=""_blank"" href=""show.asp?id="& Rs(0) &""" title="""& Rs(1) &""">"& Title &"</a></li>"
		Rs.MoveNext
		Loop
		Rs.Close:Set Rs=Nothing
		Conn.Close:Set Conn=Nothing
		Response.Write "</ul>"
	End Sub
	
	rem 显示新闻内容
	Rem 调用方式 NewsObj.NewsShow
	Public Sub NewsShow()
		Dim ID,Show
		ID=Request("ID")
		IF Not Isnumeric(ID) then
			Response.Write "非法ID号!"
			Response.End
		End if
		DBConn("News")
		Set Rs=Conn.Execute("Select Newstitle,Newsinfo,Newsdate From News Where ID="& ID &"")
		IF Not Rs.Eof Then
			Show=Rs("Newsinfo")
			Response.write "<h1>"&Rs("newstitle")&"</h1>"
			Response.write "<div id=""show"">"& Show &"</div>"
		End if
		Rs.Close:Set Rs=Nothing
		Conn.Close:Set Conn=Nothing	
	End Sub
	
	rem 分页
	Public Sub goPage(pagecount,page)
		Dim query, a, x, temp,action,Table_style,font_style
		action = Request.ServerVariables("SCRIPT_NAME")
		query = Split(Request.ServerVariables("QUERY_STRING"), "&")
		For Each x In query
			a = Split(x, "=")
			If StrComp(a(0), "page", vbTextCompare) <> 0 Then
				temp = temp & a(0) & "=" & a(1) & "&"
			End If
		Next
		Response.Write("<table " & Table_style & ">" & vbCrLf )		
		Response.Write("<form method=get onsubmit=""document.location = '" & action & "?" & temp & "Page='+ this.page.value;return false;""><TR>" & vbCrLf )
		Response.Write("<TD align=right>" & vbCrLf )
		Response.Write(font_style & vbCrLf )	
		if page<=1 then
			Response.Write ("[首页] " & vbCrLf)		
			Response.Write ("[上页] " & vbCrLf)
		else		
			Response.Write("[<A HREF=" & action & "?" & temp & "Page=1>首页</A>] " & vbCrLf)
			Response.Write("[<A HREF=" & action & "?" & temp & "Page=" & (Page-1) & ">上页</A>] " & vbCrLf)
		end if
		if page>=pagecount then
			Response.Write ("[下页] " & vbCrLf)
			Response.Write ("[尾页]" & vbCrLf)			
		else
			Response.Write("[<A HREF=" & action & "?" & temp & "Page=" & (Page+1) & ">下页</A>] " & vbCrLf)
			Response.Write("[<A HREF=" & action & "?" & temp & "Page=" & pagecount & ">尾页</A>]" & vbCrLf)			
		end if
		Response.Write(" 第" & "<INPUT TYEP=TEXT NAME=page SIZE=2 Maxlength=5 VALUE=" & page & " style=""font-size: 9pt"">" & "页"  & vbCrLf & "<INPUT type=submit style=""font-size: 7pt"" value=GO>")
		Response.Write(" 共 " & pageCount & " 页" &  vbCrLf)			'pageCount是调用页面中赋值总页数
		Response.Write("</TD>" & vbCrLf )				
		Response.Write("</TR></form>" & vbCrLf )		
		Response.Write("</table>" & vbCrLf )		
	End Sub
	
	Rem 新闻列表
	Rem 调用方式 NewsObj.NewsList "100",40
	Public Sub NewsList(ClassCode,Top)
		DBConn("News")
		IF ClassCode="" Then
			ClassCODE=Request("ClassCode")
		End IF
		IF Not Isnumeric(ClassCODE) Then
			ClassCODE=1
		End IF
		
		Set Rs=Server.Createobject("Adodb.Recordset")
		Rs.Open "Select ID,ClassCode,NewsTitle,NewsDate From News where ClassCode in(select ClassCode from Class where classname<>'活动') Order By ID Desc",Conn,1,1
		Rs.PageSize =Top
		Dim Result_n
		Result_n=Rs.RecordCount 
		Response.Write "<table width=""96%"" border=""0"" align=""center"" cellpadding=""0"" cellspacing=""1"">"
		If Result_n>0 then
			Maxpage=Rs.PageCount 
			Page=Request("page")	
			If Not IsNumeric(Page) or Page="" then
				Page=1
			Else
				Page=cint(Page)
			End if
			'Response.write Result_n
			If page<1 then
				page=2
			Elseif  page>maxpage then
				page=maxpage
			End if	
			Rs.AbsolutePage=Page

			Response.Write "<tr><td height=""26""><table width=""98%"" border=""0"" align=""center"" cellpadding=""0"" cellspacing=""1"">"
		Else
			Rs.Close:Set Rs=Nothing
			Conn.Close:Set Conn=Nothing	
			Response.Write "<tr><td height=""26"" bgcolor=""ffffff"">对不起,没有任何信息!</td></tr></table>"

			Response.End
		End IF
		For i=1 to Top
			Response.Write "<tr>"
			Response.Write "<td width=""5%"" height=26><div align=""center"">.</div></td>"
			Response.Write "<td width=""95%""><a  target=""_blank"" class=""hui_link"" href=show.asp?id="& Rs("id") &">"& Rs("newstitle") & "</a>  ["& Rs("newsdate")& "]</td>"
			Response.Write "</tr>"
			Rs.MoveNext
			IF Rs.Eof Then Exit For
		Next
		Response.Write "</table>"
		Rs.Close:Set Rs=Nothing
		Conn.Close:Set Conn=Nothing	
		Response.Write "</td></tr>"
		Response.Write "<tr>"
		Response.Write "<td height=""26""><div align=""right"">"
                goPage maxpage,page
		Response.Write "</div></td>"
		Response.Write "</tr></table>"
	End Sub
	
	Public Sub ReOk()
		IF GetInfo(0,"form","add")="ok" Then
			IF Not CodeIsTrue Then
				BakErr "验证码错误!",""
				Exit Sub
			End IF
		Dim Ip
		IP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
		If IP = "" Then IP = Request.ServerVariables("REMOTE_ADDR") 
		IP=Replace(ip,"'","")
		
			DBConn("News")
			Conn.Execute("INSERT INTO [Re]([UserName],[ATxt],[Act]) VALUES ('"& IP &"','"& GetInfo(0,"form","txt") &"',0)")
			DbClose
			BakErr "提问成功,管理员会在一段时间后给于你回复!",""
		End IF
	End Sub
	
	Rem 活动列表
	Rem 调用方式 huodongObj.huodongList "100",40
	Public Sub huodongList(ClassCode,Top)
		DBConn("News")
		IF ClassCode="" Then
			ClassCODE=Request("ClassCode")
		End IF
		IF Not Isnumeric(ClassCODE) Then
			ClassCODE=1
		End IF
		
		Set Rs=Server.Createobject("Adodb.Recordset")
		Rs.Open "Select ID,ClassCode,NewsTitle,NewsDate From News where ClassCode in(select ClassCode from Class where classname='活动') Order By ID Desc",Conn,1,1
		Rs.PageSize =Top
		Dim Result_n
		Result_n=Rs.RecordCount 
		Response.Write "<table width=""96%"" border=""0"" align=""center"" cellpadding=""0"" cellspacing=""1"">"
		If Result_n>0 then
			Maxpage=Rs.PageCount 
			Page=Request("page")	
			If Not IsNumeric(Page) or Page="" then
				Page=1
			Else
				Page=cint(Page)
			End if
			'Response.write Result_n
			If page<1 then
				page=2
			Elseif  page>maxpage then
				page=maxpage
			End if	
			Rs.AbsolutePage=Page

			Response.Write "<tr><td height=""26""><table width=""98%"" border=""0"" align=""center"" cellpadding=""0"" cellspacing=""1"">"
		Else
			Rs.Close:Set Rs=Nothing
			Conn.Close:Set Conn=Nothing	
			Response.Write "<tr><td height=""26"" bgcolor=""ffffff"">对不起,没有任何信息!</td></tr></table>"

			Response.End
		End IF
		For i=1 to Top
			Response.Write "<tr>"
			Response.Write "<td width=""5%"" height=26><div align=""center"">.</div></td>"
			Response.Write "<td width=""95%""><a  target=""_blank"" class=""hui_link"" href=show.asp?id="& Rs("id") &">"& Rs("newstitle") & "</a>  ["& Rs("newsdate")& "]</td>"
			Response.Write "</tr>"
			Rs.MoveNext
			IF Rs.Eof Then Exit For
		Next
		Response.Write "</table>"
		Rs.Close:Set Rs=Nothing
		Conn.Close:Set Conn=Nothing	
		Response.Write "</td></tr>"
		Response.Write "<tr>"
		Response.Write "<td height=""26""><div align=""right"">"
                goPage maxpage,page
		Response.Write "</div></td>"
		Response.Write "</tr></table>"
	End Sub
	
	Public Sub ReOkh()
		IF GetInfo(0,"form","add")="ok" Then
			IF Not CodeIsTrue Then
				BakErr "验证码错误!",""
				Exit Sub
			End IF
		Dim Ip
		IP = Left(Replace(Request.ServerVariables("HTTP_X_FORWARDED_FOR"),"'",""),18)
		If IP = "" Then IP = Request.ServerVariables("REMOTE_ADDR") 
		IP=Replace(ip,"'","")
		
			DBConn("News")
			Conn.Execute("INSERT INTO [Re]([UserName],[ATxt],[Act]) VALUES ('"& IP &"','"& GetInfo(0,"form","txt") &"',0)")
			DbClose
			BakErr "提问成功,管理员会在一段时间后给于你回复!",""
		End IF
	End Sub
	
	
	rem 举报
	Public Sub JBOk()
		IF GetInfo(0,"form","add")="ok" Then
			IF Not CodeIsTrue Then
				BakErr "验证码错误!",""
				Exit Sub
			End IF
			DBConn("News")
			Conn.Execute("INSERT INTO [Re]([UserName],[ATxt],[Act]) VALUES ('提问人','"& GetInfo(0,"form","txt") &"',1)")
			DbClose
			BakErr "举报成功,等待管理员审查!",""
		End IF
	End Sub
	
	rem 客服
	Public Sub ReList()
		DBConn("News")
		Set Rs=Server.Createobject("Adodb.Recordset")
		Rs.Open "Select * From Re Where Act=0 And not Btxt Is Null Order By ID DESC",Conn,1,1
		Rs.PageSize =10
		Dim Result_n,Maxpage,page,i
		Result_n=Rs.RecordCount 
		If Result_n>0 then
			Maxpage=Rs.PageCount 
			Page=Request("page")	
			If Not IsNumeric(Page) or Page="" then
				Page=1
			Else
				Page=cint(Page)
			End if
			If page<1 then
				page=1
			Elseif  page>maxpage then
				page=maxpage
			End if	
			Rs.AbsolutePage=Page
		Else
		   Response.Write "没有任何信息!"
		   Response.END
		End IF
		For i=1 to 10
			Response.Write "<table width=100% border=0 cellpadding=5 cellspacing=1 bgcolor=#CCCCCC>"
			Response.Write "<tr><td bgcolor=#FFFFFF class=tw><p>问题:"& Rs("Atxt") &" </p></td></tr>"
			IF Rs("Btxt")<>"" Then
				Response.Write "<tr><td bgcolor=""#FFFFFF"" class=re>回复:"& Rs("Btxt") &"</td></tr>"
			End IF
			Response.Write "</table>"
			Response.Write "<br>"
			Rs.MoveNext
			IF Rs.Eof Then Exit For
		Next
		'Response.write MaxPage
		goPage maxpage,page
		RsClose:DbClose
	End Sub
	
	Public Sub GameCls()
		IsCheckLogin()
		IF GetInfo(0,"form","cls")="true" Then
			IF Not CodeIsTrue Then
				BakErr "您输入的验证码不正确!",""
				Exit Sub
			End IF
			Dim DbName,ErrCls,IsGame
			DbName=Replace(GetInfo(0,"form","GameDb"),RLWebDBPrefix&"TreasureDB","")
			Select Case LCase(DbName)
				case LCase(RLWebDBPrefix&"CXLandDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"LandDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"ChinaChessDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"GoBangDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"UpGradeDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"PlaneDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"UncoverPigDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"WeiQiDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"FourEnsignDB"):
					IsGame=1
				case LCase(RLWebDBPrefix&"DouShouQiDB"):
					IsGame=1
				case LCase("llshow_cx"):
					IsGame=1
				case else:
					IsGame=0
			End Select
			
			IF DbName<>"" And IsGame=1 Then
				DbConn60(DbName)
				Set Rs=Conn.Execute("Exec Gsp_Cls "& Session("UserID") &"")
				ErrCls=Rs(0)
				RsClose:DbClose
				BakErr ErrCls,""
			End IF
		End IF
	End Sub
		
		
		
	Public Sub AddCahe(CaheName,CaheInfo)
        Application.Lock
        Application(CaheName)=CaheInfo
        Application.unLock
	End Sub
	Public Sub EmptyCahe(CaheName)
        Application.Lock
        Application(CaheName)=Empty
        Application.unLock
	End Sub
	
    Public Sub DelCahe(CaheName)
		Dim Arr,U
		Arr=Split(CaheName,"|")
        Application.Lock
			For U=0 To UBound(Arr)
				Application.Contents.Remove (Arr(U))
			Next
        Application.unLock
    End Sub
	
    Public Function Cahe(CaheName)
        Cahe=Application(CaheName)
    End Function
	
    Public Function IsCaheEmpty(CaheName)
		IsCaheEmpty=True
		IF IsEmpty(Application(CaheName)) Then
			IsCaheEmpty=False
		End IF
    End Function
	
    Public Function IsCaheArr(CaheName)
		IsCaheArr=False
		IF IsArray(Application(CaheName)) Then
			IsCaheArr=True
		End IF
    End Function

	'Public Sub ScoreTop()
'		Response.Write "<table width=""100%"" border=""0"" align=""center"" cellpadding=""5"" cellspacing=""0"" class=""boxlogin"">"
'		Response.Write "<tr valign=""middle""><td height=""35"" colspan=""6"" bgcolor=""#FFFFFF"" background=""img/index_title_bg.gif""><strong>" & Request.QueryString("GameName") & " 积分排行榜(前50强)</strong> 积分排行榜并不是实时的,大约两三天左右会更新一次</td>"
'		Response.Write "</tr><tr bgcolor=""#efefef""><td width=""66"" valign=""top"">名次</td><td width=""101"" valign=""top"">用户名</td><td width=""89"" valign=""top"">积分</td>"
'		Response.Write "<td width=""90"" valign=""top"">胜局</td><td width=""89"" valign=""top"">输局</td><td width=""90"" valign=""top"">和局</td></tr>"
'		Dim GameDb,IsGame
'		GameDB=GetInfo(0,"form","Game")
'		GameDB=Replace(GameDB,"'","")
'		GameDB=Replace(GameDB,";","")
'		GameDB=Replace(GameDB,":","")
'		Select Case LCase(GameDB)
'			case LCase("CXLand"):
'				IsGame=1
'			case LCase("Land"):
'				IsGame=1
'			case LCase("ChinaChess"):
'				IsGame=1
'			case LCase("GoBang"):
'				IsGame=1
'			case LCase("UpGrade"):
'				IsGame=1
'			case LCase("Plane"):
'				IsGame=1
'			case LCase("UncoverPig"):
'				IsGame=1
'			case LCase("WeiQi"):
'				IsGame=1
'			case LCase("FourEnsign"):
'				IsGame=1
'			case LCase("DouShouQi"):
'				IsGame=1
'			case LCase("llshow_cx"):
'				IsGame=1
'			case else:
'				IsGame=0
'		End Select
'		
'		IF GameDB<>"" And IsGame=1 Then
'			IF LCase(GameDB)<>"llshow_cx" Then
'				GameDB=RLWebDBPrefix&""& GameDB &"DB"
'			End IF
'			IF Not IsCaheEmpty(GameDB) Then
'				DbConn60(GameDB)
'				Set Rs=Conn.Execute("Select Top 50 us.Accounts,GameScoreInfo.Score,GameScoreInfo.WinCount,GameScoreInfo.LostCount,GameScoreInfo.DrawCount From "&RLWebDBPrefix&"GameUserDBLink."&RLWebDBPrefix&"GameUserDB.dbo.AccountsInfo As Us,GameScoreInfo Where Us.UserID=GameScoreInfo.UserID Order by Score desc")
'				IF NOT RS.EOF Then
'					AddCahe GameDB,Rs.GetRows()
'				End IF
'				Rs.Close:Set Rs=Nothing
'				Conn.Close:Set Conn=Nothing
'			End IF
'			
'			IF IsCaheArr(GameDB) Then
'				Dim J
'				For J=0 to Ubound(Cahe(GameDB),2)
'					Response.Write "<tr>"
'					Response.Write "<td  bgcolor=""#FFFFFF"" class=xbian>"& J+1 &"</td>"
'					Response.Write "<td  bgcolor=""#FFFFFF"" class=xbian>"& Cahe(GameDB)(0,j) &"</td>"
'					Response.Write "<td  bgcolor=""#FFFFFF"" class=xbian>"& Cahe(GameDB)(1,j) &"</td>"
'					Response.Write "<td  bgcolor=""#FFFFFF"" class=xbian>"& Cahe(GameDB)(2,j) &"</td>"
'					Response.Write "<td  bgcolor=""#FFFFFF"" class=xbian>"& Cahe(GameDB)(3,j) &"</td>"
'					Response.Write "<td  bgcolor=""#FFFFFF"" class=xbian>"& Cahe(GameDB)(4,j) &"</td>"
'					Response.Write "</tr>"		
'				Next
'			End IF
'			End IF
'			Response.Write "</table>"
'	End Sub
	
	public sub paihang()
	
	DIM GameDB,IsGame
			   GameDB=replace(trim(request.QueryString("GameDB")),"","'")
			   if GameDB="" then GameDB="LLShow"
			   
			   select case LCase(GameDB)
			   
			   case LCase("LLShow")
			        IsGame=1
			   case LCase("Hoe")
			        IsGame=1
			   case LCase("UpGrade")
			        IsGame=1
			   case LCase("ChinaChes")
			        IsGame=1
			   case LCase("LandCrazyD")
			        IsGame=1
			   case LCase("LandSC")
			        IsGame=1
			   end select
			   
			   
			   IF GameDB<>"" And IsGame=1 Then
			      IF LCase(GameDB)<>"llshow_cx" Then
				     GameDB=RLWebDBPrefix&""& GameDB &"DB"
			      End IF
			      IF Not IsCaheEmpty(GameDB) Then
					  DbConn60(GameDB)
					  Set Rs=Conn.Execute("Select Top 50 us.Accounts,GameScoreInfo.Score,GameScoreInfo.WinCount,GameScoreInfo.LostCount,GameScoreInfo.DrawCount From "&RLWebDBPrefix&"GameUserDBLink."&RLWebDBPrefix&"GameUserDB.dbo.AccountsInfo As Us,GameScoreInfo Where Us.UserID=GameScoreInfo.UserID Order by Score desc")
						  IF NOT RS.EOF Then
							AddCahe GameDB,Rs.GetRows()
						  End IF
					  Rs.Close:Set Rs=Nothing
					  Conn.Close:Set Conn=Nothing
			      End IF
			

            
			  IF IsCaheArr(GameDB) Then
			  Dim J
			  For J=0 to Ubound(Cahe(GameDB),2)
			
            
			  response.Write"<tr>"
              response.Write"<td height=""24"" align=""center"" bgcolor=""#FFFFFF""><span class=""hui"">"& J+1 &"</span></td>"
              response.Write"<td align=""center"" bgcolor=""#FFFFFF""><span class=""hui"">"& Cahe(GameDB)(0,j) &"</span></td>"
              response.Write"<td align=""center"" bgcolor=""#FFFFFF""><span class=""hui"">"& Cahe(GameDB)(1,j) &"</span></td>"
              response.Write"<td align=""center"" bgcolor=""#FFFFFF""><span class=""hui"">"& Cahe(GameDB)(2,j) &"</span></td>"
              response.Write"<td align=""center"" bgcolor=""#FFFFFF""><span class=""hui"">"& Cahe(GameDB)(3,j) &"</span></td>"
              response.Write"<td align=""center"" bgcolor=""#FFFFFF""><span class=""hui"">"& Cahe(GameDB)(4,j) &"</span></td>"
              response.Write"</tr>"
			
			 Next
			 end if
			 end if
			
	
	end sub
	
	Public Sub YYToCixi()
		IF GetInfo(0,"form","reg")="true" Then
			Dim NewUserName
			UserName=GetInfo(0,"form","UserName")
			NewUserName=GetInfo(0,"form","NewUserName")
			PassWord=Cstr(GetInfo(0,"form","PassWord"))
			BankPassWord=Cstr(GetInfo(0,"form","BnakPass"))
			IF UserName="" Then
				Response.Write "<script>username.className='box4';err.innerHTML='对不起,用户名不能为空!';</script>"
				Exit Sub
			End IF
			
			IF NewUserName="" Then
				Response.Write "<script>newu.className='box4';err.innerHTML='对不起,呢称不能为空!';</script>"
				Exit Sub
			End IF
			
			Ncode=GetInfo(0,"form","Ncode")
			Nmail=GetInfo(0,"form","Nmail")
			Nadd=GetInfo(0,"form","Nadd")
			PassW=GetInfo(0,"form","PassW")
			PassD=GetInfo(0,"form","PassD")
			IF Ncode="" Or Nmail="" Or Nadd="" Or PassW="" Or PassD="" Then
				Response.Write "<script>pbox.className='box4';err.innerHTML='请认真填写密码保护资料,它将为您的号码被盗后提供身份证明!';</script>"
				Exit Sub
			End IF
			
			IF Not IsCheckUserName(NewUserName) Then
				Response.Write "<script>newu.className='box4';err.innerHTML='您设置的呢称中有非法字符,请重新设置您的呢称!';</script>"
				Exit Sub
			End IF
			Set md5= New MD5obj
			PassWord=md5.calcMD5(PassWord)
			BankPassWord=md5.calcMD5(BankPassWord)
			'Response.WRITE BankPassWord
			'Response.End
			Set Md5=Nothing
			DbConn(RLWebDBPrefix&"GameUserDb")
			Set Rs=Conn.Execute("Exec GSP_GP_YyToCixiGame '"& UserName &"','"& NewUserName &"','"& PassWord &"','"& BankPassWord &"','"& Ncode &"','"& Nmail &"','"& Nadd &"','"& PassW &"','"& PassD &"'")
			IF Rs(0)<>"true" Then
				IF Rs(1)="此用户已经补填了密码保护资料!" Then
					Response.Redirect("Downdd.Asp")
					Response.End
				End IF
				Response.Write "<script>"& Rs(0) &".className='box4';err.innerHTML='"& Rs(1) &"';</script>"
				RsClose:DbClose
				Exit Sub
			Else
				BakErr "填写密码保护成功!您的新的ID号是:"&Rs(2),"YYtoCixiOK.asp?ID="&Rs(2)&"&U="&NewUserName
				RsClose:DbClose
				Exit Sub
			End IF
		End IF
	End Sub
	
End Class

Rem 建立类对象
Dim CxGame
Set CxGame=New GameObj
%>       