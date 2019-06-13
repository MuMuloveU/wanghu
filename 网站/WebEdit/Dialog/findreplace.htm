<HTML>
<HEAD>
<TITLE>查找 / 替换</TITLE>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<style type="text/css">
body, a, table, div, span, td, th, input, select{font-size:9pt;font-family: "宋体", Verdana, Arial, Helvetica, sans-serif;}
body {padding:5px}
</style>

<script language="JavaScript">
// 初始选定对象
var oSelection;
oSelection = dialogArguments.document.selection.createRange();

// 返回匹配条件
function searchtype(){
    var retval = 0;
    var matchcase = 0;
    var matchword = 0;
    if (document.frmSearch.blnMatchCase.checked) matchcase = 4;
    if (document.frmSearch.blnMatchWord.checked) matchword = 2;
    retval = matchcase + matchword;
    return(retval);
}

// 检测输入
function checkInput(){
    if (document.frmSearch.strSearch.value.length < 1) {
        alert("请输入查找内容");
        return false;
    } else {
        return true;
    }
}

// 查找
function findtext(){
    if (checkInput()) {
        var searchval = document.frmSearch.strSearch.value;
        oSelection.collapse(false);
        if (oSelection.findText(searchval, 1000000000, searchtype())) {
            oSelection.select();
        } else {
            var startfromtop = confirm("搜索完成，是否要从顶部开始重新搜索？");
            if (startfromtop) {
                oSelection.expand("textedit");
                oSelection.collapse();
                oSelection.select();
                findtext();
            }
        }
    }
}

// 在选中的文本中替换
function replacetext(){
    if (checkInput()) {
        if (document.frmSearch.blnMatchCase.checked){
            if (oSelection.text == document.frmSearch.strSearch.value) oSelection.text = document.frmSearch.strReplace.value
        } else {
            if (oSelection.text.toLowerCase() == document.frmSearch.strSearch.value.toLowerCase()) oSelection.text = document.frmSearch.strReplace.value
        }
        findtext();
    }
}

// 在所有内容中替换
function replacealltext(){
    if (checkInput()) {
        var searchval = document.frmSearch.strSearch.value;
        var wordcount = 0;
        var msg = "";
        oSelection.expand("textedit");
        oSelection.collapse();
        oSelection.select();
        while (oSelection.findText(searchval, 1000000000, searchtype())){
            oSelection.select();
            oSelection.text = document.frmSearch.strReplace.value;
            wordcount++;
        }
        if (wordcount == 0) msg = "要查找的内容没有找到"
        else msg = wordcount + " 处文本被替换成功";
        alert(msg);
    }
}
</script>

</HEAD>
<BODY bgcolor="menu">
<FORM NAME="frmSearch" method="post" action="">
<TABLE CELLSPACING="0" cellpadding="5" border="0">
<TR><TD VALIGN="top" align="left" nowrap>
    <label for="strSearch">查找内容</label><br>
    <INPUT TYPE=TEXT SIZE=40 NAME=strSearch id="strSearch" style="width : 200px;"><br>
    <label for="strReplace">替换内容</label><br>
    <INPUT TYPE=TEXT SIZE=40 NAME=strReplace id="strReplace" style="width : 200px;"><br>
    <INPUT TYPE=Checkbox SIZE=40 NAME=blnMatchCase ID="blnMatchCase"><label for="blnMatchCase">区分大小写</label><br>
    <INPUT TYPE=Checkbox SIZE=40 NAME=blnMatchWord ID="blnMatchWord"><label for="blnMatchWord">全部匹配</label>
</td>
<td rowspan="2" valign="top">
    <input type=button style="width:80px;margin-top:15px" name="btnFind" onClick="findtext();" value="查找下一个"><br>
    <input type=button style="width:80px;margin-top:5px" name="btnCancel" onClick="window.close();" value="关闭"><br>
    <input type=button style="width:80px;margin-top:5px" name="btnReplace" onClick="replacetext();" value="替换"><br>
    <input type=button style="width:80px;margin-top:5px" name="btnReplaceall" onClick="replacealltext();" value="全部替换"><br>
</td>
</tr>
</table>
</FORM>
</BODY>
</HTML>
