

// 当前模式
var sCurrMode = null;
var bEditMode = null;
// 连接对象
var oLinkField = null;

// 浏览器版本检测
var BrowserInfo = new Object() ;
BrowserInfo.MajorVer = navigator.appVersion.match(/MSIE (.)/)[1] ;
BrowserInfo.MinorVer = navigator.appVersion.match(/MSIE .\.(.)/)[1] ;
BrowserInfo.IsIE55OrMore = BrowserInfo.MajorVer >= 6 || ( BrowserInfo.MajorVer >= 5 && BrowserInfo.MinorVer >= 5 ) ;

var yToolbars = new Array();  // 工具栏数组

// 当文档完全调入时，进行初始化
var bInitialized = false;
function document.onreadystatechange(){
	if (document.readyState!="complete") return;
	if (bInitialized) return;
	bInitialized = true;

	var i, s, curr;

	// 初始每个工具栏
	for (i=0; i<document.body.all.length;i++){
		curr=document.body.all[i];
		if (curr.className == "yToolbar"){
			InitTB(curr);
			yToolbars[yToolbars.length] = curr;
		}
	}

	oLinkField = parent.document.getElementsByName(sLinkFieldName)[0];
	if (!config.License){
		try{
			eWebEditor_License.innerHTML = "";
		}
		catch(e){
		}
	}

	// IE5.5以下版本只能使用纯文本模式
	if (!BrowserInfo.IsIE55OrMore){
		config.InitMode = "TEXT";
	}
	
	if (ContentFlag.value=="0") { 
		ContentEdit.value = oLinkField.value;
		ContentLoad.value = oLinkField.value;
		ModeEdit.value = config.InitMode;
		ContentFlag.value = "1";
	}

	setMode(ModeEdit.value);
	setLinkedField() ;
}

// 初始化一个工具栏上的按钮
function InitBtn(btn) {
	btn.onmouseover = BtnMouseOver;
	btn.onmouseout = BtnMouseOut;
	btn.onmousedown = BtnMouseDown;
	btn.onmouseup = BtnMouseUp;
	btn.ondragstart = YCancelEvent;
	btn.onselectstart = YCancelEvent;
	btn.onselect = YCancelEvent;
	btn.YUSERONCLICK = btn.onclick;
	btn.onclick = YCancelEvent;
	btn.YINITIALIZED = true;
	return true;
}

//Initialize a toolbar. 
function InitTB(y) {
	// Set initial size of toolbar to that of the handle
	y.TBWidth = 0;
		
	// Populate the toolbar with its contents
	if (! PopulateTB(y)) return false;
	
	// Set the toolbar width and put in the handle
	y.style.posWidth = y.TBWidth;
	
	return true;
}


// Hander that simply cancels an event
function YCancelEvent() {
	event.returnValue=false;
	event.cancelBubble=true;
	return false;
}

// Toolbar button onmouseover handler
function BtnMouseOver() {
	if (event.srcElement.tagName != "IMG") return false;
	var image = event.srcElement;
	var element = image.parentElement;
	
	// Change button look based on current state of image.
	if (image.className == "Ico") element.className = "BtnMouseOverUp";
	else if (image.className == "IcoDown") element.className = "BtnMouseOverDown";

	event.cancelBubble = true;
}

// Toolbar button onmouseout handler
function BtnMouseOut() {
	if (event.srcElement.tagName != "IMG") {
		event.cancelBubble = true;
		return false;
	}

	var image = event.srcElement;
	var element = image.parentElement;
	yRaisedElement = null;
	
	element.className = "Btn";
	image.className = "Ico";

	event.cancelBubble = true;
}

// Toolbar button onmousedown handler
function BtnMouseDown() {
	if (event.srcElement.tagName != "IMG") {
		event.cancelBubble = true;
		event.returnValue=false;
		return false;
	}

	var image = event.srcElement;
	var element = image.parentElement;

	element.className = "BtnMouseOverDown";
	image.className = "IcoDown";

	event.cancelBubble = true;
	event.returnValue=false;
	return false;
}

// Toolbar button onmouseup handler
function BtnMouseUp() {
	if (event.srcElement.tagName != "IMG") {
		event.cancelBubble = true;
		return false;
	}

	var image = event.srcElement;
	var element = image.parentElement;

	if (element.YUSERONCLICK) eval(element.YUSERONCLICK + "anonymous()");

	element.className = "BtnMouseOverUp";
	image.className = "Ico";

	event.cancelBubble = true;
	return false;
}

// Populate a toolbar with the elements within it
function PopulateTB(y) {
	var i, elements, element;

	// Iterate through all the top-level elements in the toolbar
	elements = y.children;
	for (i=0; i<elements.length; i++) {
		element = elements[i];
		if (element.tagName == "SCRIPT" || element.tagName == "!") continue;
		
		switch (element.className) {
		case "Btn":
			if (element.YINITIALIZED == null) {
				if (! InitBtn(element)) {
					alert("Problem initializing:" + element.id);
					return false;
				}
			}
			
			element.style.posLeft = y.TBWidth;
			y.TBWidth += element.offsetWidth + 1;
			break;
			
		case "TBGen":
			element.style.posLeft = y.TBWidth;
			y.TBWidth += element.offsetWidth + 1;
			break;
			
		case "TBSep":
			element.style.posLeft = y.TBWidth + 2;
			y.TBWidth += 5;
			break;
			
		case "TBHandle":
			element.style.posLeft = 2;
			y.TBWidth += element.offsetWidth + 7;
			break;
			
		default:
			alert("Invalid class: " + element.className + " on Element: " + element.id + " <" + element.tagName + ">");
			return false;
		}
	}

	y.TBWidth += 1;
	return true;
}


// 设置所属表单的提交或reset事件
function setLinkedField() {
	if (! oLinkField) return ;
	var oForm = oLinkField.form ;
	if (!oForm) return ;
	// 附加submit事件
	oForm.attachEvent("onsubmit", AttachSubmit) ;
	if (! oForm.submitEditor) oForm.submitEditor = new Array() ;
	oForm.submitEditor[oForm.submitEditor.length] = AttachSubmit ;
	if (! oForm.originalSubmit) {
		oForm.originalSubmit = oForm.submit ;
		oForm.submit = function() {
			if (this.submitEditor) {
				for (var i = 0 ; i < this.submitEditor.length ; i++) {
					this.submitEditor[i]() ;
				}
			}
			this.originalSubmit() ;
		}
	}
	// 附加reset事件
	oForm.attachEvent("onreset", AttachReset) ;
	if (! oForm.resetEditor) oForm.resetEditor = new Array() ;
	oForm.resetEditor[oForm.resetEditor.length] = AttachReset ;
	if (! oForm.originalReset) {
		oForm.originalReset = oForm.reset ;
		oForm.reset = function() {
			if (this.resetEditor) {
				for (var i = 0 ; i < this.resetEditor.length ; i++) {
					this.resetEditor[i]() ;
				}
			}
			this.originalReset() ;
		}
	}
}

// 附加submit提交事件,大表单数据提交,远程文件获取,保存eWebEditor中的内容
var bDoneAutoRemote = false;
function AttachSubmit() { 
	var oForm = oLinkField.form ;
	if (!oForm) return;
	
	if ((config.AutoRemote=="1")&&(!bDoneAutoRemote)){
		parent.event.returnValue = false;
		bDoneAutoRemote = true;
		remoteUpload();
	} else {
		var html = getHTML();
		ContentEdit.value = html;
		if (sCurrMode=="TEXT"){
			html = HTMLEncode(html);
		}
		splitTextField(oLinkField, html);
	}
} 

// 提交表单
function doSubmit(){
	var oForm = oLinkField.form ;
	if (!oForm) return ;
	oForm.submit();
}

// 附加Reset事件
function AttachReset() {
	if(bEditMode){
		eWebEditor.document.body.innerHTML = ContentLoad.value;
	}else{
		eWebEditor.document.body.innerText = ContentLoad.value;
	}
}

// 显示帮助
function onHelp(){
	ShowDialog('dialog/help.htm','400','300');
	return false;
}

// 粘贴时自动检测是否来源于Word格式
function onPaste() {
	if (sCurrMode=="VIEW") return false;

	if (sCurrMode=="EDIT"){
		if (config.AutoDetectPasteFromWord && BrowserInfo.IsIE55OrMore) {
			var sHTML = GetClipboardHTML() ;
			var re = /<\w[^>]* class="?MsoNormal"?/gi ;
			if ( re.test(sHTML)){
				if ( confirm( "你要粘贴的内容好象是从Word中拷出来的，是否要先清除Word格式再粘贴？" ) ){
					cleanAndPaste( sHTML ) ;
					return false ;
				}
			}
		}else{
			return true ;
		}
	}else{
		eWebEditor.document.selection.createRange().pasteHTML(HTMLEncode( clipboardData.getData("Text"))) ;
		return false;
	}
	
}

// 快捷键
function onKeyDown(event){
	var key = String.fromCharCode(event.keyCode).toUpperCase();

	// F2:显示或隐藏指导方针
	if (event.keyCode==113){
		showBorders();
		return false;
	}
	if (event.ctrlKey){
		// Ctrl+Enter:提交
		if (event.keyCode==10){
			doSubmit();
			return false;
		}
		// Ctrl++:增加编辑区
		if (key=="+"){
			sizeChange(300);
			return false;
		}
		// Ctrl+-:减小编辑区
		if (key=="-"){
			sizeChange(-300);
			return false;
		}
		// Ctrl+1:代码模式
		if (key=="1"){
			setMode("CODE");
			return false;
		}
		// Ctrl+2:设计模式
		if (key=="2"){
			setMode("EDIT");
			return false;
		}
		// Ctrl+3:纯文本
		if (key=="3"){
			setMode("TEXT");
			return false;
		}
		// Ctrl+4:预览
		if (key=="4"){
			setMode("VIEW");
			return false;
		}
	}

	switch(sCurrMode){
	case "VIEW":
		return true;
		break;
	case "EDIT":
		if (event.ctrlKey){
			// Ctrl+D:从Word粘贴
			if (key == "D"){
				PasteWord();
				return false;
			}
			// Ctrl+R:查找替换
			if (key == "R"){
				findReplace();
				return false;
			}
			// Ctrl+Z:Undo
			if (key == "Z"){
				goHistory(-1);
				return false;
			}
			// Ctrl+Y:Redo
			if (key == "Y"){
				goHistory(1);
				return false;
			}
		}
		if(!event.ctrlKey && event.keyCode != 90 && event.keyCode != 89) {
			if (event.keyCode == 32 || event.keyCode == 13){
				saveHistory()
			}
		}
		return true;
		break;
	default:
		if (event.keyCode==13){
			var sel = eWebEditor.document.selection.createRange();
			sel.pasteHTML("<BR>");
			event.cancelBubble = true;
			event.returnValue = false;
			sel.select();
			sel.moveEnd("character", 1);
			sel.moveStart("character", 1);
			sel.collapse(false);
			return false;
		}
		// 屏蔽事件
		if (event.ctrlKey){
			// Ctrl+B,I,U
			if ((key == "B")||(key == "I")||(key == "U")){
				return false;
			}
		}

	}
}

// 取剪粘板中的HTML格式数据
function GetClipboardHTML() {
	var oDiv = document.getElementById("eWebEditor_Temp_HTML")
	oDiv.innerHTML = "" ;
	
	var oTextRange = document.body.createTextRange() ;
	oTextRange.moveToElementText(oDiv) ;
	oTextRange.execCommand("Paste") ;
	
	var sData = oDiv.innerHTML ;
	oDiv.innerHTML = "" ;
	
	return sData ;
}

// 清除WORD冗余格式并粘贴
function cleanAndPaste( html ) {
	// Remove all SPAN tags
	html = html.replace(/<\/?SPAN[^>]*>/gi, "" );
	// Remove Class attributes
	html = html.replace(/<(\w[^>]*) class=([^ |>]*)([^>]*)/gi, "<$1$3") ;
	// Remove Style attributes
	html = html.replace(/<(\w[^>]*) style="([^"]*)"([^>]*)/gi, "<$1$3") ;
	// Remove Lang attributes
	html = html.replace(/<(\w[^>]*) lang=([^ |>]*)([^>]*)/gi, "<$1$3") ;
	// Remove XML elements and declarations
	html = html.replace(/<\\?\?xml[^>]*>/gi, "") ;
	// Remove Tags with XML namespace declarations: <o:p></o:p>
	html = html.replace(/<\/?\w+:[^>]*>/gi, "") ;
	// Replace the &nbsp;
	html = html.replace(/&nbsp;/, " " );
	// Transform <P> to <DIV>
	var re = new RegExp("(<P)([^>]*>.*?)(<\/P>)","gi") ;	// Different because of a IE 5.0 error
	html = html.replace( re, "<div$2</div>" ) ;
	
	insertHTML( html ) ;
}

// 在当前文档位置插入.
function insertHTML(html) {
	if (isModeView()) return false;
	if (eWebEditor.document.selection.type.toLowerCase() != "none"){
		eWebEditor.document.selection.clear() ;
	}
	if (sCurrMode!="EDIT"){
		html=HTMLEncode(html);
	}
	eWebEditor.document.selection.createRange().pasteHTML(html) ; 
}

// 设置编辑器的内容
function setHTML(html) {
	ContentEdit.value = html;
	switch (sCurrMode){
	case "CODE":
		eWebEditor.document.designMode="On";
		eWebEditor.document.open();
		eWebEditor.document.write(config.StyleEditorHeader);
		eWebEditor.document.body.innerText=html;
		eWebEditor.document.body.contentEditable="true";
		eWebEditor.document.close();
		bEditMode=false;
		break;
	case "EDIT":
		eWebEditor.document.designMode="On";
		eWebEditor.document.open();
		eWebEditor.document.write(config.StyleEditorHeader+html);
		eWebEditor.document.body.contentEditable="true";
		eWebEditor.document.execCommand("2D-Position",true,true);
		eWebEditor.document.execCommand("MultipleSelection", true, true);
		eWebEditor.document.execCommand("LiveResize", true, true);
		eWebEditor.document.close();
		doZoom(nCurrZoomSize);
		bEditMode=true;
		eWebEditor.document.onselectionchange = function () { doToolbar();}
		break;
	case "TEXT":
		eWebEditor.document.designMode="On";
		eWebEditor.document.open();
		eWebEditor.document.write(config.StyleEditorHeader);
		eWebEditor.document.body.innerText=html;
		eWebEditor.document.body.contentEditable="true";
		eWebEditor.document.close();
		bEditMode=false;
		break;
	case "VIEW":
		eWebEditor.document.designMode="off";
		eWebEditor.document.open();
		eWebEditor.document.write(config.StyleEditorHeader+html);
		eWebEditor.document.body.contentEditable="false";
		eWebEditor.document.close();
		bEditMode=false;
		break;
	}

	eWebEditor.document.body.onpaste = onPaste ;
	eWebEditor.document.body.onhelp = onHelp ;
	eWebEditor.document.onkeydown = new Function("return onKeyDown(eWebEditor.event);");
	eWebEditor.document.oncontextmenu=new Function("return showContextMenu(eWebEditor.event);");

	if ((borderShown != "0")&&bEditMode) {
		borderShown = "0";
		showBorders();
	}

	initHistory();
}

// 取编辑器的内容
function getHTML() {
	var html;
	if((sCurrMode=="EDIT")||(sCurrMode=="VIEW")){
		html = eWebEditor.document.body.innerHTML;
	}else{
		html = eWebEditor.document.body.innerText;
	}
	if (sCurrMode!="TEXT"){
		if ((html.toLowerCase()=="<p>&nbsp;</p>")||(html.toLowerCase()=="<p></p>")){
			html = "";
		}
	}
	return html;
}

// 在尾部追加内容
function appendHTML(html) {
	if (isModeView()) return false;
	if(sCurrMode=="EDIT"){
		eWebEditor.document.body.innerHTML += html;
	}else{
		eWebEditor.document.body.innerText += html;
	}
}

// 从Word中粘贴，去除格式
function PasteWord(){
	if (!validateMode()) return;
	eWebEditor.focus();
	if (BrowserInfo.IsIE55OrMore)
		cleanAndPaste( GetClipboardHTML() ) ;
	else if ( confirm( "此功能要求IE5.5版本以上，你当前的浏览器不支持，是否按常规粘贴进行？" ) )
		format("paste") ;
	eWebEditor.focus();
}

// 粘贴纯文本
function PasteText(){
	if (!validateMode()) return;
	eWebEditor.focus();
	var sText = HTMLEncode( clipboardData.getData("Text") ) ;
	insertHTML(sText);
	eWebEditor.focus();
}

// 检测当前是否允许编辑
function validateMode() {
	if (sCurrMode=="EDIT") return true;
	alert("需转换为编辑状态后才能使用编辑功能！");
	eWebEditor.focus();
	return false;
}

// 检测当前是否在预览模式
function isModeView(){
	if (sCurrMode=="VIEW"){
		alert("预览时不允许设置编辑区内容。");
		return true;
	}
	return false;
}

// 格式化编辑器中的内容
function format(what,opt) {
	if (!validateMode()) return;
	eWebEditor.focus();
	if (opt=="RemoveFormat") {
		what=opt;
		opt=null;
	}
	if (opt==null) eWebEditor.document.execCommand(what);
	else eWebEditor.document.execCommand(what,"",opt);
	eWebEditor.focus();
}

// 确保焦点在 eWebEditor 内
function VerifyFocus() {
	if ( eWebEditor )
		eWebEditor.focus();
}

// 改变模式：代码、编辑、文本、预览
function setMode(NewMode){
	if (NewMode!=sCurrMode){
		
		if (!BrowserInfo.IsIE55OrMore){
			if ((NewMode=="CODE") || (NewMode=="EDIT") || (NewMode=="VIEW")){
				alert("HTML编辑模式需要IE5.5版本以上的支持！");
				return false;
			}
		}

		if (NewMode=="TEXT"){
			if (sCurrMode==ModeEdit.value){
				if (!confirm("警告！切换到纯文本模式会丢失您所有的HTML格式，您确认切换吗？")){
					return false;
				}
			}
		}

		var sBody = "";
		switch(sCurrMode){
		case "CODE":
			if (NewMode=="TEXT"){
				eWebEditor_Temp_HTML.innerHTML = eWebEditor.document.body.innerText;
				sBody = eWebEditor_Temp_HTML.innerText;
			}else{
				sBody = eWebEditor.document.body.innerText;
			}
			break;
		case "TEXT":
			sBody = eWebEditor.document.body.innerText;
			sBody = HTMLEncode(sBody);
			break;
		case "EDIT":
		case "VIEW":
			if (NewMode=="TEXT"){
				sBody = eWebEditor.document.body.innerText;
			}else{
				sBody = eWebEditor.document.body.innerHTML;
			}
			break;
		default:
			sBody = ContentEdit.value;
			break;
		}

		// 换图片
		try{
			document.all["eWebEditor_CODE"].className = "StatusBarBtnOff";
			document.all["eWebEditor_EDIT"].className = "StatusBarBtnOff";
			document.all["eWebEditor_TEXT"].className = "StatusBarBtnOff";
			document.all["eWebEditor_VIEW"].className = "StatusBarBtnOff";
			document.all["eWebEditor_"+NewMode].className = "StatusBarBtnOn";
			}
		catch(e){
			}
		
		sCurrMode = NewMode;
		ModeEdit.value = NewMode;
		setHTML(sBody);
		disableChildren(eWebEditor_Toolbar);

	}
}

// 使工具栏无效
function disableChildren(obj){
	if (obj){
		obj.disabled=(!bEditMode);
		for (var i=0; i<obj.children.length; i++){
			disableChildren(obj.children[i]);
		}
	}
}



// 显示无模式对话框
function ShowDialog(url, width, height, optValidate) {
	if (optValidate) {
		if (!validateMode()) return;
	}
	eWebEditor.focus();
	var arr = showModalDialog(url, window, "dialogWidth:" + width + "px;dialogHeight:" + height + "px;help:no;scroll:no;status:no");
	eWebEditor.focus();
}

// 全屏编辑
function Maximize() {
	if (!validateMode()) return;
	window.open("dialog/fullscreen.htm?style="+config.StyleName, 'FullScreen'+sLinkFieldName, 'toolbar=no,location=no,directories=no,status=yes,menubar=no,scrollbars=yes,resizable=yes,fullscreen=yes');
}

// 创建或修改超级链接
function createLink(){
	if (!validateMode()) return;
	
	if (eWebEditor.document.selection.type == "Control") {
		var oControlRange = eWebEditor.document.selection.createRange();
		if (oControlRange(0).tagName.toUpperCase() != "IMG") {
			alert("链接只能是图片或文本");
			return;
		}
	}
	
	ShowDialog("dialog/hyperlink.htm", 350, 170, true);
}

// 替换特殊字符
function HTMLEncode(text){
	text = text.replace(/&/g, "&amp;") ;
	text = text.replace(/"/g, "&quot;") ;
	text = text.replace(/</g, "&lt;") ;
	text = text.replace(/>/g, "&gt;") ;
	text = text.replace(/'/g, "&#146;") ;
	text = text.replace(/\ /g,"&nbsp;");
	text = text.replace(/\n/g,"<br>");
	text = text.replace(/\t/g,"&nbsp;&nbsp;&nbsp;&nbsp;");
	return text;
}

// 插入特殊对象
function insert(what) {
	if (!validateMode()) return;
	eWebEditor.focus();
	var sel = eWebEditor.document.selection.createRange();

	switch(what){
	case "excel":		// 插入EXCEL表格
		insertHTML("<object classid='clsid:0002E510-0000-0000-C000-000000000046' id='Spreadsheet1' codebase='file:\\Bob\software\office2000\msowc.cab' width='100%' height='250'><param name='HTMLURL' value><param name='HTMLData' value='&lt;html xmlns:x=&quot;urn:schemas-microsoft-com:office:excel&quot;xmlns=&quot;http://www.w3.org/TR/REC-html40&quot;&gt;&lt;head&gt;&lt;style type=&quot;text/css&quot;&gt;&lt;!--tr{mso-height-source:auto;}td{black-space:nowrap;}.wc4590F88{black-space:nowrap;font-family:宋体;mso-number-format:General;font-size:auto;font-weight:auto;font-style:auto;text-decoration:auto;mso-background-source:auto;mso-pattern:auto;mso-color-source:auto;text-align:general;vertical-align:bottom;border-top:none;border-left:none;border-right:none;border-bottom:none;mso-protection:locked;}--&gt;&lt;/style&gt;&lt;/head&gt;&lt;body&gt;&lt;!--[if gte mso 9]&gt;&lt;xml&gt;&lt;x:ExcelWorkbook&gt;&lt;x:ExcelWorksheets&gt;&lt;x:ExcelWorksheet&gt;&lt;x:OWCVersion&gt;9.0.0.2710&lt;/x:OWCVersion&gt;&lt;x:Label Style='border-top:solid .5pt silver;border-left:solid .5pt silver;border-right:solid .5pt silver;border-bottom:solid .5pt silver'&gt;&lt;x:Caption&gt;Microsoft Office Spreadsheet&lt;/x:Caption&gt; &lt;/x:Label&gt;&lt;x:Name&gt;Sheet1&lt;/x:Name&gt;&lt;x:WorksheetOptions&gt;&lt;x:Selected/&gt;&lt;x:Height&gt;7620&lt;/x:Height&gt;&lt;x:Width&gt;15240&lt;/x:Width&gt;&lt;x:TopRowVisible&gt;0&lt;/x:TopRowVisible&gt;&lt;x:LeftColumnVisible&gt;0&lt;/x:LeftColumnVisible&gt; &lt;x:ProtectContents&gt;False&lt;/x:ProtectContents&gt; &lt;x:DefaultRowHeight&gt;210&lt;/x:DefaultRowHeight&gt; &lt;x:StandardWidth&gt;2389&lt;/x:StandardWidth&gt; &lt;/x:WorksheetOptions&gt; &lt;/x:ExcelWorksheet&gt;&lt;/x:ExcelWorksheets&gt; &lt;x:MaxHeight&gt;80%&lt;/x:MaxHeight&gt;&lt;x:MaxWidth&gt;80%&lt;/x:MaxWidth&gt;&lt;/x:ExcelWorkbook&gt;&lt;/xml&gt;&lt;![endif]--&gt;&lt;table class=wc4590F88 x:str&gt;&lt;col width=&quot;56&quot;&gt;&lt;tr height=&quot;14&quot;&gt;&lt;td&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;/body&gt;&lt;/html&gt;'> <param name='DataType' value='HTMLDATA'> <param name='AutoFit' value='0'><param name='DisplayColHeaders' value='-1'><param name='DisplayGridlines' value='-1'><param name='DisplayHorizontalScrollBar' value='-1'><param name='DisplayRowHeaders' value='-1'><param name='DisplayTitleBar' value='-1'><param name='DisplayToolbar' value='-1'><param name='DisplayVerticalScrollBar' value='-1'> <param name='EnableAutoCalculate' value='-1'> <param name='EnableEvents' value='-1'><param name='MoveAfterReturn' value='-1'><param name='MoveAfterReturnDirection' value='0'><param name='RightToLeft' value='0'><param name='ViewableRange' value='1:65536'></object>");
		break;
	case "nowdate":		// 插入当前系统日期
		var d = new Date();
		insertHTML(d.toLocaleDateString());
		break;
	case "nowtime":		// 插入当前系统时间
		var d = new Date();
		insertHTML(d.toLocaleTimeString());
		break;
	case "br":			// 插入换行符
		insertHTML("<br>")
		break;
	case "code":		// 代码片段样式
		insertHTML('<table width=95% border="0" align="Center" cellpadding="6" cellspacing="0" style="border: 1px Dotted #CCCCCC; TABLE-LAYOUT: fixed"><tr><td bgcolor=#FDFDDF style="WORD-WRAP: break-word"><font style="color: #990000;font-weight:bold">以下是代码片段：</font><br>'+HTMLEncode(sel.text)+'</td></tr></table>');
		break;
	case "quote":		// 引用片段样式
		insertHTML('<table width=95% border="0" align="Center" cellpadding="6" cellspacing="0" style="border: 1px Dotted #CCCCCC; TABLE-LAYOUT: fixed"><tr><td bgcolor=#F3F3F3 style="WORD-WRAP: break-word"><font style="color: #990000;font-weight:bold">以下是引用片段：</font><br>'+HTMLEncode(sel.text)+'</td></tr></table>');
		break;
	case "big":			// 字体变大
		insertHTML("<big>" + sel.text + "</big>");
		break;
	case "small":		// 字体变小
		insertHTML("<small>" + sel.text + "</small>");
		break;
	default:
		alert("错误参数调用！");
		break;
	}
	sel=null;
}

// 显示或隐藏指导方针
var borderShown = config.ShowBorder;
function showBorders() {
	if (!validateMode()) return;
	
	var allForms = eWebEditor.document.body.getElementsByTagName("FORM");
	var allInputs = eWebEditor.document.body.getElementsByTagName("INPUT");
	var allTables = eWebEditor.document.body.getElementsByTagName("TABLE");
	var allLinks = eWebEditor.document.body.getElementsByTagName("A");

	// 表单
	for (a=0; a < allForms.length; a++) {
		if (borderShown == "0") {
			allForms[a].runtimeStyle.border = "1px dotted #FF0000"
		} else {
			allForms[a].runtimeStyle.cssText = ""
		}
	}

	// Input Hidden类
	for (b=0; b < allInputs.length; b++) {
		if (borderShown == "0") {
			if (allInputs[b].type.toUpperCase() == "HIDDEN") {
				allInputs[b].runtimeStyle.border = "1px dashed #000000"
				allInputs[b].runtimeStyle.width = "15px"
				allInputs[b].runtimeStyle.height = "15px"
				allInputs[b].runtimeStyle.backgroundColor = "#FDADAD"
				allInputs[b].runtimeStyle.color = "#FDADAD"
			}
		} else {
			if (allInputs[b].type.toUpperCase() == "HIDDEN")
				allInputs[b].runtimeStyle.cssText = ""
		}
	}

	// 表格
	for (i=0; i < allTables.length; i++) {
			if (borderShown == "0") {
				allTables[i].runtimeStyle.border = "1px dotted #BFBFBF"
			} else {
				allTables[i].runtimeStyle.cssText = ""
			}

			allRows = allTables[i].rows
			for (y=0; y < allRows.length; y++) {
			 	allCellsInRow = allRows[y].cells
					for (x=0; x < allCellsInRow.length; x++) {
						if (borderShown == "0") {
							allCellsInRow[x].runtimeStyle.border = "1px dotted #BFBFBF"
						} else {
							allCellsInRow[x].runtimeStyle.cssText = ""
						}
					}
			}
	}

	// 链接 A
	for (a=0; a < allLinks.length; a++) {
		if (borderShown == "0") {
			if (allLinks[a].href.toUpperCase() == "") {
				allLinks[a].runtimeStyle.borderBottom = "1px dashed #000000"
			}
		} else {
			allLinks[a].runtimeStyle.cssText = ""
		}
	}

	if (borderShown == "0") {
		borderShown = "1"
	} else {
		borderShown = "0"
	}

	scrollUp()
}

// 返回页面最上部
function scrollUp() {
	eWebEditor.scrollBy(0,0);
}

// 缩放操作
var nCurrZoomSize = 100;
var aZoomSize = new Array(10, 25, 50, 75, 100, 150, 200, 500);
function doZoom(size) {
	eWebEditor.document.body.runtimeStyle.zoom = size + "%";
	nCurrZoomSize = size;
}

// 拼写检查
function spellCheck(){
	ShowDialog('dialog/spellcheck.htm', 300, 220, true)
}

// 查找替换
function findReplace(){
	ShowDialog('dialog/findreplace.htm', 320, 165, true)
}

// 相对(absolute)或绝对位置(static)
function absolutePosition(){
	var objReference	= null;
	var RangeType		= eWebEditor.document.selection.type;
	if (RangeType != "Control") return;
	var selectedRange	= eWebEditor.document.selection.createRange();
	for (var i=0; i<selectedRange.length; i++){
		objReference = selectedRange.item(i);
		if (objReference.style.position != 'absolute') {
			objReference.style.position='absolute';
		}else{
			objReference.style.position='static';
		}
	}
	eWebEditor.content = false;
	eWebEditor.setActive();
}

// 上移(forward)或下移(backward)一层
function zIndex(action){
	var objReference	= null;
	var RangeType		= eWebEditor.document.selection.type;
	if (RangeType != "Control") return;
	var selectedRange	= eWebEditor.document.selection.createRange();
	for (var i=0; i<selectedRange.length; i++){
		objReference = selectedRange.item(i);
		if (action=='forward'){
			objReference.style.zIndex  +=1;
		}else{
			objReference.style.zIndex  -=1;
		}
		objReference.style.position='absolute';
	}
	eWebEditor.content = false;
	eWebEditor.setActive();
}

// 是否选中指定类型的控件
function isControlSelected(tag){
	if (eWebEditor.document.selection.type == "Control") {
		var oControlRange = eWebEditor.document.selection.createRange();
		if (oControlRange(0).tagName.toUpperCase() == tag) {
			return true;
		}	
	}
	return false;
}

// 改变编辑区高度
function sizeChange(size){
	if (!BrowserInfo.IsIE55OrMore){
		alert("此功能需要IE5.5版本以上的支持！");
		return false;
	}
	for (var i=0; i<parent.frames.length; i++){
		if (parent.frames[i].document==self.document){
			var obj=parent.frames[i].frameElement;
			var height = parseInt(obj.offsetHeight);
			if (height+size>=300){
				obj.height=height+size;
			}
			break;
		}
	}
}

// 热点链接
function mapEdit(){
	if (!validateMode()) return;
	
	var b = false;
	if (eWebEditor.document.selection.type == "Control") {
		var oControlRange = eWebEditor.document.selection.createRange();
		if (oControlRange(0).tagName.toUpperCase() == "IMG") {
			b = true;
		}
	}
	if (!b){
		alert("热点链接只能作用于图片");
		return;
	}

	window.open("dialog/map.htm", 'mapEdit'+sLinkFieldName, 'toolbar=no,location=no,directories=no,status=not,menubar=no,scrollbars=no,resizable=yes,width=450,height=300');
}

// 上传文件成功返回原文件名、保存后的文件名、保存后的路径文件名，提供接口
function addUploadFile(originalFileName, saveFileName, savePathFileName){
	doInterfaceUpload(sLinkOriginalFileName, originalFileName);
	doInterfaceUpload(sLinkSaveFileName, saveFileName);
	doInterfaceUpload(sLinkSavePathFileName, savePathFileName);
}

// 文件上传成功接口操作
function doInterfaceUpload(strLinkName, strValue){
	if (strValue=="") return;

	if (strLinkName){
		var objLinkUpload = parent.document.getElementsByName(strLinkName)[0];
		if (objLinkUpload){
			if (objLinkUpload.value!=""){
				objLinkUpload.value = objLinkUpload.value + "|";
			}
			objLinkUpload.value = objLinkUpload.value + strValue;
			objLinkUpload.fireEvent("onchange");
		}
	}
}

// 大文件内容自动拆分
function splitTextField(objField, html) { 
	var strFieldName = objField.name;
	var objForm = objField.form;
	var objDocument = objField.document;
	objField.value = html;

	//表单限制值设定，限制值是102399，考虑到中文设为一半
	var FormLimit = 50000 ;

	// 再次处理时，先赋空值
	for (var i=1;i<objDocument.getElementsByName(strFieldName).length;i++) {
		objDocument.getElementsByName(strFieldName)[i].value = "";
	}

	//如果表单值超过限制，拆成多个对象
	if (html.length > FormLimit) { 
		objField.value = html.substr(0, FormLimit) ;
		html = html.substr(FormLimit) ;

		while (html.length > 0) { 
			var objTEXTAREA = objDocument.createElement("TEXTAREA") ;
			objTEXTAREA.name = strFieldName ;
			objTEXTAREA.style.display = "none" ;
			objTEXTAREA.value = html.substr(0, FormLimit) ;
			objForm.appendChild(objTEXTAREA) ;

			html = html.substr(FormLimit) ;
		} 
	} 
} 

// 远程上传
function remoteUpload() { 
	if (sCurrMode=="TEXT") return;
	
	var objField = document.getElementsByName("eWebEditor_UploadText")[0];
	splitTextField(objField, getHTML());

	divProcessing.style.top = (document.body.clientHeight-parseFloat(divProcessing.style.height))/2;
	divProcessing.style.left = (document.body.clientWidth-parseFloat(divProcessing.style.width))/2;
	divProcessing.style.display = "";
	eWebEditor_UploadForm.submit();
} 

// 远程上传完成
function remoteUploadOK() {
	divProcessing.style.display = "none";
	if (bDoneAutoRemote){
		doSubmit();
	}
}

// 修正Undo/Redo
var history = new Object;
history.data = [];
history.position = 0;
history.bookmark = [];

// 保存历史
function saveHistory() {
	if (bEditMode){
		if (history.data[history.position] != eWebEditor.document.body.innerHTML){
			var nBeginLen = history.data.length;
			var nPopLen = history.data.length - history.position;
			for (var i=1; i<nPopLen; i++){
				history.data.pop();
				history.bookmark.pop();
			}

			history.data[history.data.length] = eWebEditor.document.body.innerHTML;

			if (eWebEditor.document.selection.type != "Control"){
				history.bookmark[history.bookmark.length] = eWebEditor.document.selection.createRange().getBookmark();
			} else {
				var oControl = eWebEditor.document.selection.createRange();
				history.bookmark[history.bookmark.length] = oControl(0);
			}

			if (nBeginLen!=0){
				history.position++;
			}
		}
	}
}

// 初始历史
function initHistory() {
	history.data.length = 0;
	history.bookmark.length = 0;
	history.position = 0;
}

// 返回历史
function goHistory(value) {
	saveHistory();
	// undo
	if (value == -1){
		if (history.position > 0){
			eWebEditor.document.body.innerHTML = history.data[--history.position];
			setHistoryCursor();
		}
	// redo
	} else {
		if (history.position < history.data.length -1){
			eWebEditor.document.body.innerHTML = history.data[++history.position];
			setHistoryCursor();
		}
	}
}

// 设置当前书签
function setHistoryCursor() {
	if (history.bookmark[history.position]){
		r = eWebEditor.document.body.createTextRange()
		if (history.bookmark[history.position] != "[object]"){
			if (r.moveToBookmark(history.bookmark[history.position])){
				r.collapse(false);
				r.select();
			}
		}
	}
}
// End Undo / Redo Fix

// 工具栏事件发生
function doToolbar(){
	if (bEditMode){
		saveHistory();
	}
}