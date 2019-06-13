<!--#include file="Inc/Config.asp" -->
<!--#include file="Inc/md5VB.asp" -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<style type="text/css">
<!--
body,td,th {
	font-size: 12px;
}
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #56cce6;
}
-->
</style>
<link href="images/css.css" rel="stylesheet" type="text/css">
</head>

<body>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td><!--#include file="top.asp"--></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td width="6" valign="top"><img src="images/L_33.jpg" width="6" height="384"></td>
    <td width="207" align="center" valign="top" bgcolor="#56CCE6"><table width="207" border="0" cellpadding="0" cellspacing="0" background="images/left_bj.jpg">
        <tr>
          <td align="center"><!--#include file="left.asp"--></td>
        </tr>
      </table></td>
    <td width="701" valign="top"><table width="699" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td height="39" valign="bottom" background="images/l_35.jpg"><table width="198" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="198" height="37" align="right" valign="bottom" class="k_16">游戏积分排行</td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td height="1198" align="center" valign="top" background="images/l_36.jpg"><br>
          <table width="600" height="24" border="0" cellpadding="0" cellspacing="0">
            <tr>
              <td width="120" align="center"><a href="?GameDB=LLShow" class="ju_link">连连看</a></td>
              <td width="120" align="center"><a href="?GameDB=Hoe" class="ju_link">锄大地</a></td>
              <td width="78" align="center"><a href="?GameDB=UpGrade" class="ju_link">升级</a></td>
              <td width="85" align="center"><a href="?GameDB=ChinaChess" class="ju_link">中国象棋</a></td>
              <td width="91" align="center"><a href="?GameDB=LandCrazyD" class="ju_link">疯狂斗地主</a></td>
              <td width="106" align="center"><a href="?GameDB=LandSC" class="ju_link">四川斗地主</a></td>
            </tr>
          </table>
        <br>
          <table width="600" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#C0E9FF">
            <tr>
              <td width="52" height="24" align="center" bgcolor="#FFFFFF">排名</td>
              <td width="148" align="center" bgcolor="#FFFFFF">游戏昵称</td>
              <td width="96" align="center" bgcolor="#FFFFFF">积分</td>
              <td align="center" bgcolor="#FFFFFF">胜局</td>
              <td align="center" bgcolor="#FFFFFF">输局</td>
              <td align="center" bgcolor="#FFFFFF">和局</td>
            </tr>
            <%CxGame.paihang()%>
     
          </table></td>
      </tr>
      <tr>
        <td><img src="images/l_43.jpg" width="699" height="23"></td>
      </tr>
    </table></td>
    <td width="88" align="center" valign="top"><!--#include file="Right.asp"--></td>
  </tr>
</table>
<table width="1002" border="0" align="center" cellpadding="0" cellspacing="0">
  <tr>
    <td align="center"><!--#include file="copy.asp"--></td>
  </tr>
</table>
</body>
</html>
