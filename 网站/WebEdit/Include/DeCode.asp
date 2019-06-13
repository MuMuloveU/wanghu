<%
'☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆
'★                                                                  ★
'☆                eWebEditor - eWebSoft在线文本编辑器               ☆
'★                                                                  ★
'☆  版权所有: eWebSoft.com                                          ☆
'★                                                                  ★
'☆  程序制作: eWeb开发团队                                          ☆
'★            email:webmaster@webasp.net                            ★
'☆            QQ:589808                                             ☆
'★                                                                  ★
'☆  相关网址: [产品介绍]http://www.eWebSoft.com/Product/eWebEditor/ ☆
'★            [支持论坛]http://bbs.eWebSoft.com/                    ★
'☆                                                                  ☆
'★  主页地址: http://www.eWebSoft.com/   eWebSoft团队及产品         ★
'☆            http://www.webasp.net/     WEB技术及应用资源网站      ☆
'★            http://bbs.webasp.net/     WEB技术交流论坛            ★
'★                                                                  ★
'☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆
%>

<%
'================================================
' 显示解释函数，返回根据参数允许显示的格式字符串，具体调用方法可从后台管理获得
' 输入参数：
'	s_Content	:	要转换的数据字符串
'	s_Filters	:	要过滤掉的格式集，用逗号分隔多个
'================================================
Function eWebEditor_DeCode(s_Content, sFilters)
	Dim a_Filter, i, s_Result, s_Filters
	eWebEditor_Decode = s_Content
	If IsNull(s_Content) Then Exit Function
	If s_Content = "" Then Exit Function
	s_Result = s_Content
	s_Filters = sFilters

	' 设置默认过滤
	If sFilters = "" Then s_Filters = "script,object"

	a_Filter = Split(s_Filters, ",")
	For i = 0 To UBound(a_Filter)
		s_Result = eWebEditor_DecodeFilter(s_Result, a_Filter(i))
	Next
	eWebEditor_DeCode = s_Result
End Function

%>

<Script Language=JavaScript RunAt=Server>
//===============================================
// 单个过滤
// 输入参数：
//	s_Content	:	要转换的数据字符串
//	s_Filter	:	要过滤掉的单个格式
//===============================================
function eWebEditor_DecodeFilter(html, filter){
	switch(filter.toUpperCase()){
	case "SCRIPT":		// 去除所有客户端脚本javascipt,vbscript,jscript,js,vbs,event,...
		html = eWebEditor_execRE("</?script[^>]*>", "", html);
		html = eWebEditor_execRE("(javascript|jscript|vbscript|vbs):", "$1：", html);
		html = eWebEditor_execRE("on(mouse|exit|error|click|key)", "<I>on$1</I>", html);
		html = eWebEditor_execRE("&#", "<I>&#</I>", html);
		break;
	case "TABLE":		// 去除表格<table><tr><td><th>
		html = eWebEditor_execRE("</?table[^>]*>", "", html);
		html = eWebEditor_execRE("</?tr[^>]*>", "", html);
		html = eWebEditor_execRE("</?th[^>]*>", "", html);
		html = eWebEditor_execRE("</?td[^>]*>", "", html);
		break;
	case "CLASS":		// 去除样式类class=""
		html = eWebEditor_execRE("(<[^>]+) class=[^ |^>]*([^>]*>)", "$1 $2", html) ;
		break;
	case "STYLE":		// 去除样式style=""
		html = eWebEditor_execRE("(<[^>]+) style=\"[^\"]*\"([^>]*>)", "$1 $2", html);
		break;
	case "XML":			// 去除XML<?xml>
		html = eWebEditor_execRE("<\\?xml[^>]*>", "", html);
		break;
	case "NAMESPACE":	// 去除命名空间<o:p></o:p>
		html = eWebEditor_execRE("<\/?[a-z]+:[^>]*>", "", html);
		break;
	case "FONT":		// 去除字体<font></font>
		html = eWebEditor_execRE("</?font[^>]*>", "", html);
		break;
	case "MARQUEE":		// 去除字幕<marquee></marquee>
		html = eWebEditor_execRE("</?marquee[^>]*>", "", html);
		break;
	case "OBJECT":		// 去除对象<object><param><embed></object>
		html = eWebEditor_execRE("</?object[^>]*>", "", html);
		html = eWebEditor_execRE("</?param[^>]*>", "", html);
		html = eWebEditor_execRE("</?embed[^>]*>", "", html);
		break;
	default:
	}
	return html;
}

// ============================================
// 执行正则表达式替换
// ============================================
function eWebEditor_execRE(re, rp, content) {
	oReg = new RegExp(re, "ig");
	r = content.replace(oReg, rp);
	return r; 
}

</Script>