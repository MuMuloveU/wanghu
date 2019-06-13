var floor = document.all;
function channeTableB(type1){
	floor.tableB1.className="icoGreen13px";
	floor.tableB2.className="icoGreen13px";
	floor.tableB3.className="icoGreen13px";
	floor.tableB4.className="icoGreen13px";
	floor.tableB5.className="icoGreen13px";
	floor.tableB6.className="icoGreen13px";
	floor.tableB7.className="icoGreen13px";
	floor["tableB"+type1].className="icoWhite14px";
	
	floor.tableB1.background="";
	floor.tableB2.background="";
	floor.tableB3.background="";
	floor.tableB4.background="";
	floor.tableB5.background="";
	floor.tableB6.background="";
	floor.tableB7.background="";
	floor["tableB"+type1].background="image/bg_buy.jpg";
	
	
	document.getElementById("b1").style.display= "none";
	document.getElementById("b2").style.display= "none";
	document.getElementById("b3").style.display= "none";
	document.getElementById("b4").style.display= "none";
	document.getElementById("b5").style.display= "none";
	document.getElementById("b6").style.display= "none";
	document.getElementById("b7").style.display= "none";
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
