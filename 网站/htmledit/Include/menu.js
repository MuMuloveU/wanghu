/*
*☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆
*★                                                                  ★
*☆                eWebEditor - eWebSoft在线文本编辑器               ☆
*★                                                                  ★
*☆  版权所有: eWebSoft.com                                          ☆
*★                                                                  ★
*☆  程序制作: eWeb开发团队                                          ☆
*★            email:webmaster@webasp.net                            ★
*☆            QQ:589808                                             ☆
*★                                                                  ★
*☆  相关网址: [产品介绍]http://www.eWebSoft.com/Product/eWebEditor/ ☆
*★            [支持论坛]http://bbs.eWebSoft.com/                    ★
*☆                                                                  ☆
*★  主页地址: http://www.eWebSoft.com/   eWebSoft团队及产品         ★
*☆            http://www.webasp.net/     WEB技术及应用资源网站      ☆
*★            http://bbs.webasp.net/     WEB技术交流论坛            ★
*★                                                                  ★
*☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆
*/


// 菜单常量
var sMenuHr="<tr><td align=center valign=middle height=2><TABLE border=0 cellpadding=0 cellspacing=0 width=128 height=2><tr><td height=1 class=HrShadow><\/td><\/tr><tr><td height=1 class=HrHighLight><\/td><\/tr><\/TABLE><\/td><\/tr>";
var sMenu1="<TABLE border=0 cellpadding=0 cellspacing=0 class=Menu width=150><tr><td width=18 valign=bottom align=center style='background:url(sysimage/contextmenu.gif);background-position:bottom;'><\/td><td width=132 class=RightBg><TABLE border=0 cellpadding=0 cellspacing=0>";
var sMenu2="<\/TABLE><\/td><\/tr><\/TABLE>";
// 菜单
var oPopupMenu = null;
if (BrowserInfo.IsIE55OrMore){
	oPopupMenu = window.createPopup();
}

// 取菜单行
function getMenuRow(s_Disabled, s_Event, s_Image, s_Html) {
	var s_MenuRow = "";
	s_MenuRow = "<tr><td align=center valign=middle><TABLE border=0 cellpadding=0 cellspacing=0 width=132><tr "+s_Disabled+"><td valign=middle height=20 class=MouseOut onMouseOver=this.className='MouseOver'; onMouseOut=this.className='MouseOut';";
	if (s_Disabled==""){
		s_MenuRow += " onclick=\"parent."+s_Event+";parent.oPopupMenu.hide();\"";
	}
	s_MenuRow += ">"
	if (s_Image !=""){
		s_MenuRow += "&nbsp;<img border=0 src='ButtonImage/"+config.StyleDir+"/"+s_Image+"' width=20 height=20 align=absmiddle "+s_Disabled+">&nbsp;";
	}else{
		s_MenuRow += "&nbsp;";
	}
	s_MenuRow += s_Html+"<\/td><\/tr><\/TABLE><\/td><\/tr>";
	return s_MenuRow;

}

// 取标准的format菜单行
function getFormatMenuRow(menu, html, image){
	var s_Disabled = "";
	if (!eWebEditor.document.queryCommandEnabled(menu)){
		s_Disabled = "disabled";
	}
	var s_Event = "format('"+menu+"')";
	var s_Image = menu+".gif";
	if (image){
		s_Image = image;
	}
	return getMenuRow(s_Disabled, s_Event, s_Image, html)
}

// 取表格菜单行
function getTableMenuRow(what){
	var s_Menu = "";
	var s_Disabled = "disabled";
	switch(what){
	case "TableInsert":
		if (!isTableSelected()) s_Disabled="";
		s_Menu += getMenuRow(s_Disabled, "TableInsert()", "TableInsert.gif", "插入表格...")
		break;
	case "TableProp":
		if (isTableSelected()||isCursorInTableCell()) s_Disabled="";
		s_Menu += getMenuRow(s_Disabled, "TableProp()", "TableProp.gif", "表格属性...")
		break;
	case "TableCell":
		if (isCursorInTableCell()) s_Disabled="";
		s_Menu += getMenuRow(s_Disabled, "TableCellProp()", "TableCellProp.gif", "单元格属性...")
		s_Menu += getMenuRow(s_Disabled, "TableCellSplit()", "TableCellSplit.gif", "拆分单元格...")
		s_Menu += sMenuHr;
		s_Menu += getMenuRow(s_Disabled, "TableRowProp()", "TableRowProp.gif", "表格行属性...")
		s_Menu += getMenuRow(s_Disabled, "TableRowInsertAbove()", "TableRowInsertAbove.gif", "插入行（在上方）");
		s_Menu += getMenuRow(s_Disabled, "TableRowInsertBelow()", "TableRowInsertBelow.gif", "插入行（在下方）");
		s_Menu += getMenuRow(s_Disabled, "TableRowMerge()", "TableRowMerge.gif", "合并行（向下方）");
		s_Menu += getMenuRow(s_Disabled, "TableRowSplit(2)", "TableRowSplit.gif", "拆分行");
		s_Menu += getMenuRow(s_Disabled, "TableRowDelete()", "TableRowDelete.gif", "删除行");
		s_Menu += sMenuHr;
		s_Menu += getMenuRow(s_Disabled, "TableColInsertLeft()", "TableColInsertLeft.gif", "插入列（在左侧）");
		s_Menu += getMenuRow(s_Disabled, "TableColInsertRight()", "TableColInsertRight.gif", "插入列（在右侧）");
		s_Menu += getMenuRow(s_Disabled, "TableColMerge()", "TableColMerge.gif", "合并列（向右侧）");
		s_Menu += getMenuRow(s_Disabled, "TableColSplit(2)", "TableColSplit.gif", "拆分列");
		s_Menu += getMenuRow(s_Disabled, "TableColDelete()", "TableColDelete.gif", "删除列");
		break;
	}
	return s_Menu;
}

// 右键菜单
function showContextMenu(event){
	if (!bEditMode) return false;

	var width = 150;
	var height = 0;
	var lefter = event.clientX;
	var topper = event.clientY;

	var oPopDocument = oPopupMenu.document;
	var oPopBody = oPopupMenu.document.body;

	var sMenu="";
	
	sMenu += getFormatMenuRow("cut", "剪切");
	sMenu += getFormatMenuRow("copy", "复制");
	sMenu += getFormatMenuRow("paste", "常规粘贴");
	sMenu += getFormatMenuRow("delete", "删除");
	sMenu += getFormatMenuRow("selectall", "全选");
	sMenu += sMenuHr;
	height += 102;

	if (isCursorInTableCell()){
		sMenu += getTableMenuRow("TableProp");
		sMenu += getTableMenuRow("TableCell");
		sMenu += sMenuHr;
		height += 286;
	}

	if (isControlSelected("TABLE")){
		sMenu += getTableMenuRow("TableProp");
		sMenu += sMenuHr;
		height += 22;
	}

	if (isControlSelected("IMG")){
		sMenu += getMenuRow("", "ShowDialog('dialog/img.htm', 350, 315, true)", "img.gif", "图片属性...");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "zIndex('forward')", "forward.gif", "上移一层");
		sMenu += getMenuRow("", "zIndex('backward')", "backward.gif", "下移一层");
		sMenu += sMenuHr;
		height += 64;
	}

	sMenu += getMenuRow("", "findReplace()", "findreplace.gif", "查找替换...");
	height += 20;

	sMenu = sMenu1 + sMenu + sMenu2;

	oPopDocument.open();
	oPopDocument.write(config.StyleMenuHeader+sMenu);
	oPopDocument.close();

	height+=2;
	if(lefter+width > document.body.clientWidth) lefter=lefter-width;
	//if(topper+height > document.body.clientHeight) topper=topper-height;

	oPopupMenu.show(lefter, topper, width, height, eWebEditor.document.body);
	return false;

}

// 工具栏菜单
function showToolMenu(menu){
	if (!bEditMode) return false;
	var sMenu = ""
	var width = 150;
	var height = 0;

	var lefter = event.clientX;
	var leftoff = event.offsetX
	var topper = event.clientY;
	var topoff = event.offsetY;

	var oPopDocument = oPopupMenu.document;
	var oPopBody = oPopupMenu.document.body;

	switch(menu){
	case "font":		// 字体菜单
		sMenu += getFormatMenuRow("bold", "粗体", "bold.gif");
		sMenu += getFormatMenuRow("italic", "斜体", "italic.gif");
		sMenu += getFormatMenuRow("underline", "下划线", "underline.gif");
		sMenu += getFormatMenuRow("strikethrough", "中划线", "strikethrough.gif");
		sMenu += sMenuHr;
		sMenu += getFormatMenuRow("superscript", "上标", "superscript.gif");
		sMenu += getFormatMenuRow("subscript", "下标", "subscript.gif");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "ShowDialog('dialog/selcolor.htm?action=forecolor', 280, 250, true)", "forecolor.gif", "字体颜色");
		sMenu += getMenuRow("", "ShowDialog('dialog/selcolor.htm?action=backcolor', 280, 250, true)", "backcolor.gif", "字体背景色");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "insert('big')", "tobig.gif", "字体增大");
		sMenu += getMenuRow("", "insert('small')", "tosmall.gif", "字体减小");
		height = 206;
		break;
	case "paragraph":	// 段落菜单
		sMenu += getFormatMenuRow("JustifyLeft", "左对齐", "JustifyLeft.gif");
		sMenu += getFormatMenuRow("JustifyCenter", "居中对齐", "JustifyCenter.gif");
		sMenu += getFormatMenuRow("JustifyRight", "右对齐", "JustifyRight.gif");
		sMenu += getFormatMenuRow("JustifyFull", "两端对齐", "JustifyFull.gif");
		sMenu += sMenuHr;
		sMenu += getFormatMenuRow("insertorderedlist", "编号", "insertorderedlist.gif");
		sMenu += getFormatMenuRow("insertunorderedlist", "项目符号", "insertunorderedlist.gif");
		sMenu += getFormatMenuRow("indent", "增加缩进量", "indent.gif");
		sMenu += getFormatMenuRow("outdent", "减少缩进量", "outdent.gif");
		sMenu += sMenuHr;
		sMenu += getFormatMenuRow("insertparagraph", "插入段落", "insertparagraph.gif");
		sMenu += getMenuRow("", "insert('br')", "br.gif", "插入换行符");
		height = 204;
		break;
	case "edit":		// 编辑菜单
		var s_Disabled = "";
		if (history.data.length <= 1 || history.position <= 0) s_Disabled = "disabled";
		sMenu += getMenuRow(s_Disabled, "goHistory(-1)", "undo.gif", "撤消")
		if (history.position >= history.data.length-1 || history.data.length == 0) s_Disabled = "disabled";
		sMenu += getMenuRow(s_Disabled, "goHistory(1)", "redo.gif", "恢复")
		sMenu += sMenuHr;
		sMenu += getFormatMenuRow("Cut", "剪切", "cut.gif");
		sMenu += getFormatMenuRow("Copy", "复制", "copy.gif");
		sMenu += getFormatMenuRow("Paste", "常规粘贴", "paste.gif");
		sMenu += getMenuRow("", "PasteText()", "pastetext.gif", "纯文本粘贴");
		sMenu += getMenuRow("", "PasteWord()", "pasteword.gif", "从Word中粘贴");
		sMenu += sMenuHr;
		sMenu += getFormatMenuRow("delete", "删除", "delete.gif");
		sMenu += getFormatMenuRow("RemoveFormat", "删除文字格式", "removeformat.gif");
		sMenu += sMenuHr;
		sMenu += getFormatMenuRow("SelectAll", "全部选中", "selectall.gif");
		sMenu += getFormatMenuRow("Unselect", "取消选择", "unselect.gif");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "findReplace()", "findreplace.gif", "查找替换");
		height = 248;
		break;
	case "object":		// 对象效果菜单
		sMenu += getMenuRow("", "ShowDialog('dialog/selcolor.htm?action=bgcolor', 280, 250, true)", "bgcolor.gif", "对象背景颜色");
		sMenu += getMenuRow("", "ShowDialog('dialog/backimage.htm', 350, 210, true)", "bgpic.gif", "背景图片");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "absolutePosition()", "abspos.gif", "绝对或相对位置");
		sMenu += getMenuRow("", "zIndex('forward')", "forward.gif", "上移一层");
		sMenu += getMenuRow("", "zIndex('backward')", "backward.gif", "下移一层");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "showBorders()", "showborders.gif", "显示隐藏指导方针");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "insert('quote')", "quote.gif", "引用样式");
		sMenu += getMenuRow("", "insert('code')", "code.gif", "代码样式");
		height = 166;
		break;
	case "component":	// 组件菜单
		sMenu += getMenuRow("", "ShowDialog('dialog/img.htm', 350, 315, true)", "img.gif", "插入或修改图片");
		sMenu += getMenuRow("", "ShowDialog('dialog/flash.htm', 350, 200, true)", "flash.gif", "插入Flash动画");
		sMenu += getMenuRow("", "ShowDialog('dialog/media.htm', 350, 200, true)", "media.gif", "插入自动播放媒体");
		sMenu += getMenuRow("", "ShowDialog('dialog/file.htm', 350, 150, true)", "file.gif", "插入其他文件");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "remoteUpload()", "remoteupload.gif", "远程自动上传");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "ShowDialog('dialog/fieldset.htm', 350, 170, true)", "fieldset.gif", "插入或修改栏目框");
		sMenu += getMenuRow("", "ShowDialog('dialog/iframe.htm', 350, 200, true)", "iframe.gif", "插入或修改网页帧");
		sMenu += getFormatMenuRow("InsertHorizontalRule", "插入水平尺", "inserthorizontalrule.gif");
		sMenu += getMenuRow("", "ShowDialog('dialog/marquee.htm', 395, 150, true)", "marquee.gif", "插入或修改字幕");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "createLink()", "createlink.gif", "插入或修改超链接");
		sMenu += getMenuRow("", "ShowDialog('dialog/anchor.htm', 270, 220, true)", "anchor.gif", "书签管理");
		sMenu += getMenuRow("", "mapEdit()", "map.gif", "图形热点链接");
		sMenu += getFormatMenuRow("UnLink", "取消超链接或标签", "unlink.gif");
		height = 266;
		break;
	case "tool":		// 工具菜单
		sMenu += getMenuRow("", "ShowDialog('dialog/symbol.htm', 350, 220, true)", "symbol.gif", "插入特殊字符");
		sMenu += getMenuRow("", "insert('excel')", "excel.gif", "插入Excel表格");
		sMenu += getMenuRow("", "ShowDialog('dialog/emot.htm', 300, 180, true)", "emot.gif", "插入表情图标");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "insert('nowdate')", "date.gif", "插入当前日期");
		sMenu += getMenuRow("", "insert('nowtime')", "time.gif", "插入当前时间");
		height = 102;
		break;
	case "file":		// 文件视图菜单
		sMenu += getMenuRow("", "format('Refresh')", "refresh.gif", "新建");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "setMode('CODE')", "modecodebtn.gif", "代码状态");
		sMenu += getMenuRow("", "setMode('EDIT')", "modeeditbtn.gif", "编辑状态");
		sMenu += getMenuRow("", "setMode('TEXT')", "modetextbtn.gif", "文本状态");
		sMenu += getMenuRow("", "setMode('VIEW')", "modeviewbtn.gif", "预览状态");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "sizeChange(300)", "sizeplus.gif", "增高编辑区");
		sMenu += getMenuRow("", "sizeChange(-300)", "sizeminus.gif", "减小编辑区");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "format('Print')", "print.gif", "打印");
		sMenu += sMenuHr;
		sMenu += getMenuRow("", "ShowDialog('dialog/help.htm','400','300')", "help.gif", "查看使用帮助");
		sMenu += getMenuRow("", "ShowDialog('dialog/about.htm','400','220')", "about.gif", "关于eWebEditor");
		sMenu += getMenuRow("", "window.open('http://ewebeditor.webasp.net')", "site.gif", "eWebEditor站点");
		height = 228;
		break;
	case "table":		// 表格菜单
		sMenu += getTableMenuRow("TableInsert");
		sMenu += getTableMenuRow("TableProp");
		sMenu += sMenuHr;
		sMenu += getTableMenuRow("TableCell");
		height = 306;
		break;
	case "form":		// 表单菜单
		sMenu += getFormatMenuRow("InsertInputText", "插入输入框", "FormText.gif");
		sMenu += getFormatMenuRow("InsertTextArea", "插入输入区", "FormTextArea.gif");
		sMenu += getFormatMenuRow("InsertInputRadio", "插入单选钮", "FormRadio.gif");
		sMenu += getFormatMenuRow("InsertInputCheckbox", "插入复选钮", "FormCheckBox.gif");
		sMenu += getFormatMenuRow("InsertSelectDropdown", "插入下拉框", "FormDropdown.gif");
		sMenu += getFormatMenuRow("InsertButton", "插入按钮", "FormButton.gif");
		height = 120;
		break;
	case "zoom":		// 缩放菜单
		for (var i=0; i<aZoomSize.length; i++){
			if (aZoomSize[i]==nCurrZoomSize){
				sMenu += getMenuRow("", "doZoom("+aZoomSize[i]+")", "checked.gif", aZoomSize[i]+"%");
			}else{
				sMenu += getMenuRow("", "doZoom("+aZoomSize[i]+")", "space.gif", aZoomSize[i]+"%");
			}
			height += 20;
		}
		break;
	}
	
	sMenu = sMenu1 + sMenu + sMenu2;
	
	oPopDocument.open();
	oPopDocument.write(config.StyleMenuHeader+sMenu);
	oPopDocument.close();

	height+=2;
	if(lefter+width > document.body.clientWidth) lefter=lefter-width;
	//if(topper+height > document.body.clientHeight) topper=topper-height;

	oPopupMenu.show(lefter - leftoff - 2, topper - topoff + 22, width, height, document.body);

	return false;
}

