<HTML>
<HEAD>
<TITLE>eWebEditor在线编辑器 - 使用例子</TITLE>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<style>
body,td,input,textarea {font-size:9pt}
</style>
</HEAD>
<BODY>



<script language=JavaScript>

// 表单提交检测
function doCheck(){

	// 检测表单的有效性
	// 如：标题不能为空，内容不能为空，等等....
	if (eWebEditor1.getHTML()=="") {
		alert("内容不能为空！");
		return false;
	}

	// 表单有效性检测完后，自动上传远程文件
	// 函数：remoteUpload(strEventUploadAfter)
	// 参数：strEventUploadAfter ; 上传完后，触发的函数名，如果上传完后不需动作可不填参数
	eWebEditor1.remoteUpload("doSubmit()");
	// 当一个表单中有多个编辑区时，你可以指定上传某个编辑区，如：
	// eWebEditor1.remoteUpload();
	// eWebEditor2.remoteUpload();
	// eWebEditor4.remoteUpload("doSubmit()");    这是最后一个，执行完后提交表单
	return false;

}

// 表单提交（当远程上传完成后，触发此函数）
function doSubmit(){
	document.myform1.submit();
}

</script>



<p><b>eWebEditor 远程文件上传示例：</b></p>

<FORM method="POST" name="myform1" action="submit.asp" onsubmit="return doCheck();">
<TABLE border="0" cellpadding="2" cellspacing="1">
<TR>
	<TD>编辑内容：</TD>
	<TD>
		<INPUT type="hidden" name="content1" value="&lt;P&gt;&lt;IMG src=&quot;http://ewebeditor.webasp.net/images/ewebeditor.gif&quot; border=0&gt;&lt;/P&gt;&lt;P&gt;以上图片地址为：&lt;A href=&quot;http://ewebeditor.webasp.net/images/ewebeditor.gif&quot;&gt;http://ewebeditor.webasp.net/images/ewebeditor.gif&lt;/A&gt;&lt;/P&gt;&lt;P&gt;1。点击&lt;IMG src=&quot;http://ewebeditor.webasp.net/ewebeditor/buttonimage/standard/remoteupload.gif&quot; border=0&gt;按钮，然后转到“代码”状态看一下，以上图片的地址已经到本地服务器了；&lt;/P&gt;&lt;P&gt;2。或点此表单的“提交”，提交后查看源文件看一下，图片的地址也到本地服务器了；&lt;/P&gt;&lt;P&gt;要看其使用说明，请见压缩包中，example/test2.asp 中的注释。&lt;/P&gt;">
		<IFRAME ID="eWebEditor1" src="../ewebeditor.asp?id=content1&style=s_exampleremote" frameborder="0" scrolling="no" width="550" height="350"></IFRAME>
	</TD>
</TR>
<TR>
	<TD colspan=2 align=right>
	<INPUT type=submit name=b1 value="提交"> 
	<INPUT type=reset name=b2 value="重填"> 
	<INPUT type=button name=b3 value="查看源文件" onclick="location.replace('view-source:'+location)"> 
	</TD>
</TR>
</TABLE>
</FORM>






</BODY>
</HTML>
