<HTML>
<HEAD>
<TITLE>eWebEditor - eWebSoft在线编辑器 - 使用例子</TITLE>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<style>
body,td,input,textarea {font-size:9pt}
</style>
</HEAD>
<BODY>
<FORM method="POST" name="myform" action="submit.asp">
<TABLE border="0" cellpadding="2" cellspacing="1">
<TR>
	<TD>编辑内容：</TD>
	<TD>
		<INPUT type="hidden" name="content1" value="<p>&nbsp;</p><p><font color=#ff0000>本样式为自带默认样式，最佳调用宽度550px，高度350px！</font></p><p>下面为一些高级调用功能的例子，你可以用脚本方便的编辑进行一些操作。</p><p>看到这些内容，且没有错误提示，说明安装已经正确完成！</p><p>你可以先到后台管理中，进行必要的设置，然后再调用！</p><p><b><font color=#0000ff>提示：</font>别忘了改用户和密码、数据库文件名。</b></p>">
		<IFRAME ID="eWebEditor1" src="../ewebeditor.asp?id=content1&style=s_coolblue" frameborder="0" scrolling="no" width="550" height="350"></IFRAME>
	</TD>
</TR>
<TR>
	<TD colspan=2 align=right>
	<INPUT type=submit name=b1 value="提交"> 
	<INPUT type=reset name=b2 value="重填"> 
	<INPUT type=button name=b3 value="查看源文件" onclick="location.replace('view-source:'+location)"> 
	</TD>
</TR>
<TR>
	<TD>取到内容：</TD>
	<TD><TEXTAREA cols=50 rows=5 id=myTextArea style="width:550px">点击“取值”按钮，看一下效果！</TEXTAREA></TD>
</TR>
<TR>
	<TD colspan=2 align=right>
	<INPUT type=button name=b4 value="取值" onclick="myTextArea.value=eWebEditor1.getHTML()"> 
	<INPUT type=button name=b5 value="赋值" onclick="eWebEditor1.setHTML('<b>Hello My World!</b>')">
	<INPUT type=button name=b6 value="代码状态" onclick="eWebEditor1.setMode('CODE')">
	<INPUT type=button name=b7 value="设计状态" onclick="eWebEditor1.setMode('EDIT')">
	<INPUT type=button name=b8 value="文本状态" onclick="eWebEditor1.setMode('TEXT')">
	<INPUT type=button name=b8 value="预览状态" onclick="eWebEditor1.setMode('VIEW')">
	<INPUT type=button name=b9 value="当前位置插入" onclick="eWebEditor1.insertHTML('This is Insert Function!')">
	<INPUT type=button name=b10 value="尾部追加" onclick="eWebEditor1.appendHTML('This is Append Function!')">
	</TD>
</TR>
</TABLE>
</FORM>


</BODY>
</HTML>
