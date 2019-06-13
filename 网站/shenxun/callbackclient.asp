<!--#include file="vnetchar.asp"-->
<!--#include file="vnetsettings.asp"-->
<!--#include file="../inc/config.asp"-->
<%
dim result1,result2,result3,result4,result5,result6,payAmount,youxibi,payAmount1,UserID,SpreaderID,username
result1=request.QueryString("tt")  '盈华讯方订单号码 如“T100353166”
result2=request.QueryString("cc")  '成功状态码  “0”表示成功,其它数据表示失败
result3=request.QueryString("mm")  '返回充值金额 如01,02,05,10,20 的字符， 请接收后，MD5比较后再改位数字(int) 
result4=request.QueryString("pp")  '返回验证密码 32位 此码每次都不一样。
result5=request.QueryString("af")  '返回自己定义字段内容 custom 可以是订你方单号号码 18字符内
result6=request.QueryString("ka")'  返回15位的充值卡号
   
            payAmount=clng(result3)
			youxibi=payAmount*5000
			payAmount1=youxibi/10
 
 if result2 ="0"  then 
'response.Write("充值成功!"&"<br>")
   
   '执行状态码的更新为"1"
' conn.Execute("update order2 set otype='1' where orderid='"&orderId&"'")
  
response.Write("卡号是："&result6&"<br>")

response.Write("订单是："&result1&"<br>")
  
response.Write("金额是："&result3&"<br>")

'response.Write("密码是："&result4&"<br>")
 
response.Write("自己的订单是："&result5&"<br>")

yyyy=Ucase(trim(md6(agentID&agentPWD&result2&result3&result1&result5))) 'MD5加密

'response.Write("商户端密码是："&yyyy&"<br>")

			if yyyy=result4  then
			
			CxGame.DbConn("CxGameTreasureDb")
			    set rs=conn.execute("select username from yinghua where agentself='"& result5 &"'")
				if not rs.eof then				
				  
				  username=rs(0)
				
			      CxGame.DbConn("QPGameUserDB")
				  set rs=conn.execute("select top 1 UserID,SpreaderID from AccountsInfo where Accounts='"& username &"'")
			      if not rs.eof then
					 UserID=rs(0)
					 SpreaderID=clng(rs(1))
			      end if
				  
				  CxGame.DbConn("CxGameTreasureDb")
				  set rs=conn.execute("select * from PostLog where PostCode='"& result1 &"'")
				  if rs.eof then
					  set rs=conn.execute("insert into PostLog (UserID,PostCode,Price,PostDate) values ("& UserID &","& result1 &","& payAmount &","& date() &")")
					  CxGame.DbConn("QPTreasureDB")
					  set rs=conn.execute("update GameScoreInfo set score=score+"& youxibi &" where userid="&UserID)
					  
					  if SpreaderID > 0 then
						 set rs=conn.execute("update set GameScoreInfo score=score+"& payAmount1 &" where userid="&SpreaderID)
					  end if					  
				  end if
			      response.Write("<script language='javascript'>alert('充值成功！');location.href='../index.asp';</script>")
			  
			     end if
			'response.Write("成功") '表示成功了。
			'response.Write("<a href="+agentclient+">"  +"继续充值"+"</a>")
			'''''''''''''''''''''''
			
			
			else
			response.Write("0")  '表示失败的错误的
			response.Write("<a href="+agentclient+">"  +"继续充值"+"</a>")
            end if

else

response.Write("失败")  '表示失败的错误的

end if 

 response.End
 Response.Expires=0

%>