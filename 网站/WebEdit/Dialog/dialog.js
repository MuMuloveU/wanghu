// 取通过URL传过来的参数 (格式如 ?Param1=Value1&Param2=Value2)
var URLParams = new Object() ;
var aParams = document.location.search.substr(1).split('&') ;
for (i=0 ; i < aParams.length ; i++) {
	var aParam = aParams[i].split('=') ;
	URLParams[aParam[0]] = aParam[1] ;
}

// 具有主窗口相同的配置信息
var config;
try{
	config = dialogArguments.config;
}
catch(e){
}


// 去空格，left,right,all可选
function BaseTrim(str){
	  lIdx=0;rIdx=str.length;
	  if (BaseTrim.arguments.length==2)
	    act=BaseTrim.arguments[1].toLowerCase()
	  else
	    act="all"
      for(var i=0;i<str.length;i++){
	  	thelStr=str.substring(lIdx,lIdx+1)
		therStr=str.substring(rIdx,rIdx-1)
        if ((act=="all" || act=="left") && thelStr==" "){
			lIdx++
        }
        if ((act=="all" || act=="right") && therStr==" "){
			rIdx--
        }
      }
	  str=str.slice(lIdx,rIdx)
      return str
}

// 基本信息提示，得到焦点并选定
function BaseAlert(theText,notice){
	alert(notice);
	theText.focus();
	theText.select();
	return false;
}

// 是否有效颜色值
function IsColor(color){
	var temp=color;
	if (temp=="") return true;
	if (temp.length!=7) return false;
	return (temp.search(/\#[a-fA-F0-9]{6}/) != -1);
}

// 只允许输入数字
function IsDigit(){
  return ((event.keyCode >= 48) && (event.keyCode <= 57));
}

// 选颜色
function SelectColor(what){
	var dEL = document.all("d_"+what);
	var sEL = document.all("s_"+what);
	var url = "selcolor.htm?color="+encodeURIComponent(dEL.value);
	var arr = showModalDialog(url,window,"dialogWidth:280px;dialogHeight:250px;help:no;scroll:no;status:no");
	if (arr) {
		dEL.value=arr;
		sEL.style.backgroundColor=arr;
	}
}

// 选背景图
function SelectImage(){
	showModalDialog("backimage.htm?action=other",window,"dialogWidth:350px;dialogHeight:210px;help:no;scroll:no;status:no");
}

// 搜索下拉框值与指定值匹配，并选择匹配项
function SearchSelectValue(o_Select, s_Value){
	for (var i=0;i<o_Select.length;i++){
		if (o_Select.options[i].value == s_Value){
			o_Select.selectedIndex = i;
			return true;
		}
	}
	return false;
}

// 转为数字型，并无前导0，不能转则返回""
function ToInt(str){
	str=BaseTrim(str);
	if (str!=""){
		var sTemp=parseFloat(str);
		if (isNaN(sTemp)){
			str="";
		}else{
			str=sTemp;
		}
	}
	return str;
}

// 是否有效的链接
function IsURL(url){
	var sTemp;
	var b=true;
	sTemp=url.substring(0,7);
	sTemp=sTemp.toUpperCase();
	if ((sTemp!="HTTP://")||(url.length<10)){
		b=false;
	}
	return b;
}

// 是否有效的扩展名
function IsExt(url, opt){
	var sTemp;
	var b=false;
	var s=opt.toUpperCase().split("|");
	for (var i=0;i<s.length ;i++ ){
		sTemp=url.substr(url.length-s[i].length-1);
		sTemp=sTemp.toUpperCase();
		s[i]="."+s[i];
		if (s[i]==sTemp){
			b=true;
			break;
		}
	}
	return b;
}

// 相对路径转为根形式路径
function relativePath2rootPath(url){
	if (url.substring(0,1)=="/") return url;

	var sWebEditorPath = getWebEditorRootPath();
	while(url.substr(0,3)=="../"){
		url = url.substr(3);
		sWebEditorPath = sWebEditorPath.substring(0,sWebEditorPath.lastIndexOf("/"));
	}
	return sWebEditorPath + "/" + url;
}

// 相对路径按路径模式转为相应的格式
function relativePath2setPath(url){
	switch(config.BaseUrl){
	case "0":
		return baseHref2editorPath(url);
		break;
	case "1":
		return relativePath2rootPath(url);
		break;
	case "2":
		return getSitePath() + relativePath2rootPath(url);
		break;
	}
}

// 显示路径到编辑器路的相对形式
function baseHref2editorPath(url){
	var editorPath = getWebEditorRootPath() + "/";
	var baseHref = config.BaseHref;
	//baseHref = baseHref.substr(0, baseHref.length-1);
	var editorLen = editorPath.length;
	var baseLen = baseHref.length;
	var nMinLen = 0;
	if (editorLen>baseLen){
		nMinLen = baseLen;
	}else{
		nMinLen = editorLen;
	}
	var n = 0;
	for (var i=1; i<=nMinLen; i++){
		n = i;
		if (editorPath.substr(0,i).toLowerCase()!=baseHref.substr(0,i).toLowerCase()){
			break;
		}
	}	
	var str = editorPath.substr(0, n);
	str = str.substring(0,str.lastIndexOf("/")+1);
	n = str.length;
	editorPath = editorPath.substr(n);
	baseHref = baseHref.substr(n);
	var arr = baseHref.split("/");
	var result = "";
	for (var i=1; i<=arr.length-1; i++){
		result += "../";
	}
	result = result + editorPath + url;
	return result;
}

// 取编辑所在跟路径
function getWebEditorRootPath(){
	var url = "/" + document.location.pathname;
	return url.substring(0,url.lastIndexOf("/dialog/"));
}

// 取站点路径
function getSitePath(){
	var sSitePath = document.location.protocol + "//" + document.location.host;
	if (sSitePath.substr(sSitePath.length-3) == ":80"){
		sSitePath = sSitePath.substring(0,sSitePath.length-3);
	}
	return sSitePath;
}