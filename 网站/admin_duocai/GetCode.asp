<%
Option Explicit
Response.buffer=true
NumCode
Function NumCode()
	Response.Expires = -1
	Response.AddHeader "Pragma","no-cache"
	Response.AddHeader "cache-ctrol","no-cache"
	dim zNum,i,j
	dim Ados,Ados1
	Randomize timer
	zNum = cint(8999*Rnd+1000)
	response.Cookies("GetCode") = zNum
	dim zimg(4),NStr
		NStr=cstr(zNum)
		For i=0 to 3
			zimg(i)=cint(mid(NStr,i+1,1))
		Next
	dim Pos
	set Ados=Server.CreateObject("Adodb.Stream")
		Ados.Mode=3
		Ados.Type=1
		Ados.Open
	set Ados1=Server.CreateObject("Adodb.Stream")
		Ados1.Mode=3
		Ados1.Type=1
		Ados1.Open
		Ados.LoadFromFile(Server.mappath("images/body.Fix"))
		Ados1.write Ados.read(1280)
		for i=0 to 3
			Ados.Position=(9-zimg(i))*320
			Ados1.Position=i*320
			Ados1.write ados.read(320)
		next	
		Ados.LoadFromFile(Server.mappath("images/head.fix"))
		Pos=lenb(Ados.read())
		Ados.Position=Pos
		for i=0 to 9 step 1
			for j=0 to 3
				Ados1.Position=i*32+j*320
				Ados.Position=Pos+30*j+i*120
				Ados.write ados1.read(30)
			next
		next
		Response.ContentType = "image/BMP"
		Ados.Position=0
		Response.BinaryWrite Ados.read()
		Ados.Close:set Ados=nothing
		Ados1.Close:set Ados1=nothing
End Function
%>