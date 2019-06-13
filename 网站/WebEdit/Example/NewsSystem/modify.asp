<!--#include file = "Startup.asp"-->

<%

' ======================
' 功能：修改新闻
' 描述：提供一个修改表单，包括新闻标题和新闻内容，新闻内容使用eWebEditor进行编辑；
'       同时接收上传文件，以便删除新闻时，同时删除上传文件；
'       并由编辑区中上传的文件，提供标题新闻的图片选择；
'		功能与新增相同，不同在于只是修改表单具有初始值。
' ======================

Call Header("修改新闻")
Call Content()
Call Footer()


' 本页内容区
Sub Content()
	
	' 传入参数：新闻ID
	Dim sNewsID
	sNewsID = Trim(Request("id"))

	' 新闻ID有效性验证，防止有些人恶意的破坏此演示程序
	If IsNumeric(sNewsID) = False Then
		GoError "请通过页面上的链接进行操作，不要试图破坏此演示系统。"
	End If

	' 从数据库中取初始值
	Dim sTitle, sContent, sPicture, sOriginalFileName, sSaveFileName, sSavePathFileName
	sSql = "SELECT * FROM NewsData WHERE D_ID=" & sNewsID
	oRs.Open sSql, oConn, 0, 1
	If Not oRs.Eof Then
		sTitle = oRs("D_Title")
		sContent = oRs("D_Content")
		sPicture = oRs("D_Picture")
		sOriginalFileName = oRs("D_OriginalFileName")
		sSaveFileName = oRs("D_SaveFileName")
		sSavePathFileName = oRs("D_SavePathFileName")
	Else
		GoError "无效的新闻ID，请点页面上的链接进行操作！"
	End If
	oRs.Close

	' 把带"|"的字符串转为数组，用于初始下拉框表单
	Dim aSavePathFileName
	aSavePathFileName = Split(sSavePathFileName, "|")

	' 根据新闻内容中相关上传文件为数据，初始下拉框表单
	' 函数InitSelect，根据数组值及初始值返回下拉框输出字串，具体请见startup.asp文件中函数的说明部分
	Dim sOptionSavePath
	sOptionSavePath = InitSelect("d_picture", aSavePathFileName, aSavePathFileName, sPicture, "", "无")

	%>

	<Script Language=JavaScript>
	// 当上传图片等文件时，往下拉框中填入图片路径，可根据实际需要更改此函数
	function doChange(objText, objDrop){
		if (!objDrop) return;
		var str = objText.value;
		var arr = str.split("|");
		var nIndex = objDrop.selectedIndex;
		objDrop.length=1;
		for (var i=0; i<arr.length; i++){
			objDrop.options[objDrop.length] = new Option(arr[i], arr[i]);
		}
		objDrop.selectedIndex = nIndex;
	}

	// 表单提交客户端检测
	function doCheck(){

		if (document.myform.d_title.value==""){
			alert("新闻标题不能为空！");
			return false;
		}
		// getHTML()为eWebEditor自带的接口函数，功能为取编辑区的内容
		if (eWebEditor1.getHTML()==""){
			alert("新闻内容不能为空！");
			return false;
		}

		// 表单有效性检测完后，自动上传远程文件
		// 函数：remoteUpload(strEventUploadAfter)
		// 参数：strEventUploadAfter ; 上传完后，触发的函数名，如果上传完后不需动作可不填参数
		eWebEditor1.remoteUpload("doSubmit()");
		return false;

	}

	// 表单提交（当远程上传完成后，触发此函数）
	function doSubmit(){
		document.myform.submit();
	}
	
	</Script>
	
	<form action="modifysave.asp?id=<%=sNewsID%>" method="post" name="myform" onsubmit="return doCheck();">
	<% '取源文件名 %>
	<input type=hidden name=d_originalfilename value="<%=sOriginalFileName%>">
	<% '取保存的方件名（带路径），如果不要带路径的填充下拉框，可以在下面的表单项加入onchange事件 %>
	<input type=hidden name=d_savefilename value="<%=sSaveFileName%>">
	<% '取保存的文件名，使用带路径的填充下拉框 %>
	<input type=hidden name=d_savepathfilename onchange="doChange(this,document.myform.d_picture)" value="<%=sSavePathFileName%>">

	<table cellspacing=3 align=center>
	<tr>
		<td>新闻标题：</td>
		<% ' 函数inHTML：去除Html格式，用于从数据库中取出值填入输入框时，具体请见startup.asp文件中函数的说明 %>
		<td><input type="text" name="d_title" value="<%=inHTML(sTitle)%>" size="90"></td>
	</tr>
	<tr>
		<td>标题图片：</td>
		<td><%=sOptionSavePath%> 当编辑区有插入图片时，将自动填充此下拉框</td>
	</tr>
	<tr>
		<td>新闻内容：</td>
		<td>
			<%
			' ewebeditor.asp文件调用的参数：
			'	id：下面表单项textarea的名称，在此表单中是d_content，注意大小写
			'	style：编辑器的样式名称，可在eWebEditor的后台设置
			'	originalfilename：用于获取源文件名的表单项名，在此表单中是d_originalfilename
			'	savefilename：用于获取保存文件名的表单项名，在此表单中是d_savefilename
			'	savepathfilename：用于获取保存带路径文件名的表单项名，在此表单中是d_savepathfilename

			' 请一定要用Server.HtmlEncode()函数对内容进行转换，否则将有可以出现某些内容出了编辑区的情况
			%>
			<textarea name="d_content" style="display:none"><%=Server.HtmlEncode(sContent)%></textarea>
			<iframe ID="eWebEditor1" src="../../ewebeditor.asp?id=d_content&style=s_newssystem&originalfilename=d_originalfilename&savefilename=d_savefilename&savepathfilename=d_savepathfilename" frameborder="0" scrolling="no" width="550" HEIGHT="350"></iframe>
		</td>
	</tr>
	</table>
	<p align=center><input type=submit name=btnSubmit value=" 提 交 "> <input type=reset name=btnReset value=" 重 填 "></p>
	</form>

	<%
End Sub

%>

