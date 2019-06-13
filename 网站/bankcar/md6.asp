<%
	'MD6算法开始  --------------------------------------
	Private Const BITS_TO_A_BYTE = 8
	Private Const BYTES_TO_A_WORD = 4
	Private Const BITS_TO_A_WORD = 32
	
	Private m_lOnBits(30)
	Private m_l2Power(30)
	
	Private Function LShift(lValue, iShiftBits)
		If iShiftBits = 0 Then
			LShift = lValue
			Exit Function
		ElseIf iShiftBits = 31 Then
			If lValue And 1 Then
				LShift = &H80000000
			Else
				LShift = 0
			End If
			Exit Function
		ElseIf iShiftBits < 0 Or iShiftBits > 31 Then
			Err.Raise 6
		End If
	
		If (lValue And m_l2Power(31 - iShiftBits)) Then
			LShift = ((lValue And m_lOnBits(31 - (iShiftBits + 1))) * m_l2Power(iShiftBits)) Or &H80000000
		Else
			LShift = ((lValue And m_lOnBits(31 - iShiftBits)) * m_l2Power(iShiftBits))
		End If
	End Function

	Private Function str2binold(varstr) 
		str2bin="" 
		 For i=1 To Len(varstr) 
			 varchar=mid(varstr,i,1) 
			 varasc = Asc(varchar) 
			 ' asc对中文字符求出来的值可能为负数， 
			 ' 加上65536就可求出它的无符号数值 
			 ' -1在机器内是用补码表示的0xffff, 
			 ' 其无符号值为65535,65535=-1+65536 
			 ' 其他负数依次类推。 
			 If varasc<0 Then 
				varasc = varasc + 65535 
			 End If 
			 '对中文的处理：把双字节低位和高位分开 
			 If varasc>255 Then 
				varlow = Left(Hex(Asc(varchar)),2) 
				varhigh = right(Hex(Asc(varchar)),2) 
				str2bin = str2bin & chrB("&H" & varlow) & chrB("&H" & varhigh) 
			 Else 
				str2bin = str2bin & chrB(AscB(varchar)) 
			 End If 
		 Next 
	End Function 

	Private Function str2bin(varstr) 
		str2bin="" 
		 For i=1 To Len(varstr) 
			 varchar=mid(varstr,i,1) 
			 code = Server.UrlEncode(varchar)
			 if len(code) = 1 then
			    str2bin = str2bin & chrB(AscB(code))
			 else
				codearr = split(code,"%")
				for j=1 to ubound(codearr)
				   str2bin = str2bin & chrB("&H" & codearr(j))
				next

			 End If 
		 Next 
	End Function 

	Private Function RShift(lValue, iShiftBits)
		If iShiftBits = 0 Then
			RShift = lValue
			Exit Function
		ElseIf iShiftBits = 31 Then
			If lValue And &H80000000 Then
				RShift = 1
			Else
				RShift = 0
			End If
			Exit Function
		ElseIf iShiftBits < 0 Or iShiftBits > 31 Then
			Err.Raise 6
		End If
	
		RShift = (lValue And &H7FFFFFFE) \ m_l2Power(iShiftBits)
	
		If (lValue And &H80000000) Then
			RShift = (RShift Or (&H40000000 \ m_l2Power(iShiftBits - 1)))
		End If
	End Function
	
	Private Function RotateLeft(lValue, iShiftBits)
		RotateLeft = LShift(lValue, iShiftBits) Or RShift(lValue, (32 - iShiftBits))
	End Function
	
	Private Function AddUnsigned(lX, lY)
		Dim lX4
		Dim lY4
		Dim lX8
		Dim lY8
		Dim lResult
	
		lX8 = lX And &H80000000
		lY8 = lY And &H80000000
		lX4 = lX And &H40000000
		lY4 = lY And &H40000000
		
		lResult = (lX And &H3FFFFFFF) + (lY And &H3FFFFFFF)
	
		If lX4 And lY4 Then
			lResult = lResult Xor &H80000000 Xor lX8 Xor lY8
		ElseIf lX4 Or lY4 Then
			If lResult And &H40000000 Then
				lResult = lResult Xor &HC0000000 Xor lX8 Xor lY8
			Else
				lResult = lResult Xor &H40000000 Xor lX8 Xor lY8
			End If
		Else
			lResult = lResult Xor lX8 Xor lY8
		End If
	
		AddUnsigned = lResult
	End Function
	
	Private Function MD6_F(x, y, z)
		MD6_F = (x And y) Or ((Not x) And z)
	End Function
	
	Private Function MD6_G(x, y, z)
		MD6_G = (x And z) Or (y And (Not z))
	End Function
	
	Private Function MD6_H(x, y, z)
		MD6_H = (x Xor y Xor z)
	End Function
	
	Private Function MD6_I(x, y, z)
		MD6_I = (y Xor (x Or (Not z)))
	End Function
	
	Private Sub MD6_FF(a, b, c, d, x, s, ac)
		a = AddUnsigned(a, AddUnsigned(AddUnsigned(MD6_F(b, c, d), x), ac))
		a = RotateLeft(a, s)
		a = AddUnsigned(a, b)
	End Sub
	
	Private Sub MD6_GG(a, b, c, d, x, s, ac)
		a = AddUnsigned(a, AddUnsigned(AddUnsigned(MD6_G(b, c, d), x), ac))
		a = RotateLeft(a, s)
		a = AddUnsigned(a, b)
	End Sub
	
	Private Sub MD6_HH(a, b, c, d, x, s, ac)
		a = AddUnsigned(a, AddUnsigned(AddUnsigned(MD6_H(b, c, d), x), ac))
		a = RotateLeft(a, s)
		a = AddUnsigned(a, b)
	End Sub
	
	Private Sub MD6_II(a, b, c, d, x, s, ac)
		a = AddUnsigned(a, AddUnsigned(AddUnsigned(MD6_I(b, c, d), x), ac))
		a = RotateLeft(a, s)
		a = AddUnsigned(a, b)
	End Sub
	
	Private Function ConvertToWordArray(sMessage)
		Dim lMessageLength
		Dim lNumberOfWords
		Dim lWordArray()
		Dim lBytePosition
		Dim lByteCount
		Dim lWordCount
		
		Const MODULUS_BITS = 512
		Const CONGRUENT_BITS = 448
		
		lMessageLength = LenB(sMessage)
		
		lNumberOfWords = (((lMessageLength + ((MODULUS_BITS - CONGRUENT_BITS) \ BITS_TO_A_BYTE)) \ (MODULUS_BITS \ BITS_TO_A_BYTE)) + 1) * (MODULUS_BITS \ BITS_TO_A_WORD)
		ReDim lWordArray(lNumberOfWords - 1)
		
		lBytePosition = 0
		lByteCount = 0
		Do Until lByteCount >= lMessageLength
			lWordCount = lByteCount \ BYTES_TO_A_WORD
			lBytePosition = (lByteCount Mod BYTES_TO_A_WORD) * BITS_TO_A_BYTE
			lWordArray(lWordCount) = lWordArray(lWordCount) Or LShift(AscB(MidB(sMessage, lByteCount + 1, 1)), lBytePosition)
			lByteCount = lByteCount + 1
		Loop
	
		lWordCount = lByteCount \ BYTES_TO_A_WORD
		lBytePosition = (lByteCount Mod BYTES_TO_A_WORD) * BITS_TO_A_BYTE
		
		lWordArray(lWordCount) = lWordArray(lWordCount) Or LShift(&H80, lBytePosition)
		
		lWordArray(lNumberOfWords - 2) = LShift(lMessageLength, 3)
		lWordArray(lNumberOfWords - 1) = RShift(lMessageLength, 29)
	
		ConvertToWordArray = lWordArray
	End Function
	
	Private Function WordToHex(lValue)
		Dim lByte
		Dim lCount
		
		For lCount = 0 To 3
			lByte = RShift(lValue, lCount * BITS_TO_A_BYTE) And m_lOnBits(BITS_TO_A_BYTE - 1)
			WordToHex = WordToHex & Right("0" & Hex(lByte), 2)
		Next
	End Function
	
	Public Function MD6(sMessage)
		m_lOnBits(0) = CLng(1)
		m_lOnBits(1) = CLng(3)
		m_lOnBits(2) = CLng(7)
		m_lOnBits(3) = CLng(15)
		m_lOnBits(4) = CLng(31)
		m_lOnBits(5) = CLng(63)
		m_lOnBits(6) = CLng(127)
		m_lOnBits(7) = CLng(255)
		m_lOnBits(8) = CLng(511)
		m_lOnBits(9) = CLng(1023)
		m_lOnBits(10) = CLng(2047)
		m_lOnBits(11) = CLng(4095)
		m_lOnBits(12) = CLng(8191)
		m_lOnBits(13) = CLng(16383)
		m_lOnBits(14) = CLng(32767)
		m_lOnBits(15) = CLng(65535)
		m_lOnBits(16) = CLng(131071)
		m_lOnBits(17) = CLng(262143)
		m_lOnBits(18) = CLng(524287)
		m_lOnBits(19) = CLng(1048575)
		m_lOnBits(20) = CLng(2097151)
		m_lOnBits(21) = CLng(4194303)
		m_lOnBits(22) = CLng(8388607)
		m_lOnBits(23) = CLng(16777215)
		m_lOnBits(24) = CLng(33554431)
		m_lOnBits(25) = CLng(67108863)
		m_lOnBits(26) = CLng(134217727)
		m_lOnBits(27) = CLng(268435455)
		m_lOnBits(28) = CLng(536870911)
		m_lOnBits(29) = CLng(1073741823)
		m_lOnBits(30) = CLng(2147483647)
		
		m_l2Power(0) = CLng(1)
		m_l2Power(1) = CLng(2)
		m_l2Power(2) = CLng(4)
		m_l2Power(3) = CLng(8)
		m_l2Power(4) = CLng(16)
		m_l2Power(5) = CLng(32)
		m_l2Power(6) = CLng(64)
		m_l2Power(7) = CLng(128)
		m_l2Power(8) = CLng(256)
		m_l2Power(9) = CLng(512)
		m_l2Power(10) = CLng(1024)
		m_l2Power(11) = CLng(2048)
		m_l2Power(12) = CLng(4096)
		m_l2Power(13) = CLng(8192)
		m_l2Power(14) = CLng(16384)
		m_l2Power(15) = CLng(32768)
		m_l2Power(16) = CLng(65536)
		m_l2Power(17) = CLng(131072)
		m_l2Power(18) = CLng(262144)
		m_l2Power(19) = CLng(524288)
		m_l2Power(20) = CLng(1048576)
		m_l2Power(21) = CLng(2097152)
		m_l2Power(22) = CLng(4194304)
		m_l2Power(23) = CLng(8388608)
		m_l2Power(24) = CLng(16777216)
		m_l2Power(25) = CLng(33554432)
		m_l2Power(26) = CLng(67108864)
		m_l2Power(27) = CLng(134217728)
		m_l2Power(28) = CLng(268435456)
		m_l2Power(29) = CLng(536870912)
		m_l2Power(30) = CLng(1073741824)
		
		
		Dim x
		Dim k
		Dim AA
		Dim BB
		Dim CC
		Dim DD
		Dim a
		Dim b
		Dim c
		Dim d
		
		Const S11 = 7
		Const S12 = 12
		Const S13 = 17
		Const S14 = 22
		Const S21 = 5
		Const S22 = 9
		Const S23 = 14
		Const S24 = 20
		Const S31 = 4
		Const S32 = 11
		Const S33 = 16
		Const S34 = 23
		Const S41 = 6
		Const S42 = 10
		Const S43 = 15
		Const S44 = 21
		
		x = ConvertToWordArray(str2bin(sMessage))
		
		a = &H67452301
		b = &HEFCDAB89
		c = &H98BADCFE
		d = &H10325476
		
		For k = 0 To UBound(x) Step 16
			AA = a
			BB = b
			CC = c
			DD = d
			
			MD6_FF a, b, c, d, x(k + 0), S11, &HD76AA478
			MD6_FF d, a, b, c, x(k + 1), S12, &HE8C7B756
			MD6_FF c, d, a, b, x(k + 2), S13, &H242070DB
			MD6_FF b, c, d, a, x(k + 3), S14, &HC1BDCEEE
			MD6_FF a, b, c, d, x(k + 4), S11, &HF57C0FAF
			MD6_FF d, a, b, c, x(k + 5), S12, &H4787C62A
			MD6_FF c, d, a, b, x(k + 6), S13, &HA8304613
			MD6_FF b, c, d, a, x(k + 7), S14, &HFD469501
			MD6_FF a, b, c, d, x(k + 8), S11, &H698098D8
			MD6_FF d, a, b, c, x(k + 9), S12, &H8B44F7AF
			MD6_FF c, d, a, b, x(k + 10), S13, &HFFFF5BB1
			MD6_FF b, c, d, a, x(k + 11), S14, &H895CD7BE
			MD6_FF a, b, c, d, x(k + 12), S11, &H6B901122
			MD6_FF d, a, b, c, x(k + 13), S12, &HFD987193
			MD6_FF c, d, a, b, x(k + 14), S13, &HA679438E
			MD6_FF b, c, d, a, x(k + 15), S14, &H49B40821
			
			MD6_GG a, b, c, d, x(k + 1), S21, &HF61E2562
			MD6_GG d, a, b, c, x(k + 6), S22, &HC040B340
			MD6_GG c, d, a, b, x(k + 11), S23, &H265E5A51
			MD6_GG b, c, d, a, x(k + 0), S24, &HE9B6C7AA
			MD6_GG a, b, c, d, x(k + 5), S21, &HD62F105D
			MD6_GG d, a, b, c, x(k + 10), S22, &H2441453
			MD6_GG c, d, a, b, x(k + 15), S23, &HD8A1E681
			MD6_GG b, c, d, a, x(k + 4), S24, &HE7D3FBC8
			MD6_GG a, b, c, d, x(k + 9), S21, &H21E1CDE6
			MD6_GG d, a, b, c, x(k + 14), S22, &HC33707D6
			MD6_GG c, d, a, b, x(k + 3), S23, &HF4D50D87
			MD6_GG b, c, d, a, x(k + 8), S24, &H455A14ED
			MD6_GG a, b, c, d, x(k + 13), S21, &HA9E3E905
			MD6_GG d, a, b, c, x(k + 2), S22, &HFCEFA3F8
			MD6_GG c, d, a, b, x(k + 7), S23, &H676F02D9
			MD6_GG b, c, d, a, x(k + 12), S24, &H8D2A4C8A
			
			MD6_HH a, b, c, d, x(k + 5), S31, &HFFFA3942
			MD6_HH d, a, b, c, x(k + 8), S32, &H8771F681
			MD6_HH c, d, a, b, x(k + 11), S33, &H6D9D6122
			MD6_HH b, c, d, a, x(k + 14), S34, &HFDE5380C
			MD6_HH a, b, c, d, x(k + 1), S31, &HA4BEEA44
			MD6_HH d, a, b, c, x(k + 4), S32, &H4BDECFA9
			MD6_HH c, d, a, b, x(k + 7), S33, &HF6BB4B60
			MD6_HH b, c, d, a, x(k + 10), S34, &HBEBFBC70
			MD6_HH a, b, c, d, x(k + 13), S31, &H289B7EC6
			MD6_HH d, a, b, c, x(k + 0), S32, &HEAA127FA
			MD6_HH c, d, a, b, x(k + 3), S33, &HD4EF3085
			MD6_HH b, c, d, a, x(k + 6), S34, &H4881D05
			MD6_HH a, b, c, d, x(k + 9), S31, &HD9D4D039
			MD6_HH d, a, b, c, x(k + 12), S32, &HE6DB99E5
			MD6_HH c, d, a, b, x(k + 15), S33, &H1FA27CF8
			MD6_HH b, c, d, a, x(k + 2), S34, &HC4AC5665
			
			MD6_II a, b, c, d, x(k + 0), S41, &HF4292244
			MD6_II d, a, b, c, x(k + 7), S42, &H432AFF97
			MD6_II c, d, a, b, x(k + 14), S43, &HAB9423A7
			MD6_II b, c, d, a, x(k + 5), S44, &HFC93A039
			MD6_II a, b, c, d, x(k + 12), S41, &H655B59C3
			MD6_II d, a, b, c, x(k + 3), S42, &H8F0CCC92
			MD6_II c, d, a, b, x(k + 10), S43, &HFFEFF47D
			MD6_II b, c, d, a, x(k + 1), S44, &H85845DD1
			MD6_II a, b, c, d, x(k + 8), S41, &H6FA87E4F
			MD6_II d, a, b, c, x(k + 15), S42, &HFE2CE6E0
			MD6_II c, d, a, b, x(k + 6), S43, &HA3014314
			MD6_II b, c, d, a, x(k + 13), S44, &H4E0811A1
			MD6_II a, b, c, d, x(k + 4), S41, &HF7537E82
			MD6_II d, a, b, c, x(k + 11), S42, &HBD3AF235
			MD6_II c, d, a, b, x(k + 2), S43, &H2AD7D2BB
			MD6_II b, c, d, a, x(k + 9), S44, &HEB86D391
			
			a = AddUnsigned(a, AA)
			b = AddUnsigned(b, BB)
			c = AddUnsigned(c, CC)
			d = AddUnsigned(d, DD)
		Next
		
		MD6 = LCase(WordToHex(a) & WordToHex(b) & WordToHex(c) & WordToHex(d))
	End Function

	'MD6算法结束  --------------------------------------

%>
