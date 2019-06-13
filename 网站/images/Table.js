var floor = document.all;
function channeTableB(type1){
	floor.tableB1.className="icoWhite13px";
	floor.tableB2.className="icoWhite13px";
	floor.tableB3.className="icoWhite13px";
	floor.tableB4.className="icoWhite13px";
	floor["tableB"+type1].className="icoWhite14px";
	
	floor.tableB1.background="";
	floor.tableB2.background="";
	floor.tableB3.background="";
	floor.tableB4.background="";
	floor["tableB"+type1].background="images/n_bj.jpg";
	
	
	document.getElementById("b1").style.display= "none";
	document.getElementById("b2").style.display= "none";
	document.getElementById("b3").style.display= "none";
	document.getElementById("b4").style.display= "none";
	document.getElementById("b"+type1).style.display= "";
//	for (i=1;i<7;i++) {
//		document.getElementById(i).style.display = "none";
//		document.getElementById("b"+i).style.display = "none";
//	}
//	document.getElementById(type1).style.display = "";
//	document.getElementById("b"+type1).style.display = "";

}
// ÈÝ´í´úÂë
function killErrors() {
return true;
}
