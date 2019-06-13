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

// 表格相关全局变量
var selectedTD
var selectedTR
var selectedTBODY
var selectedTable

// 插入表格
function TableInsert(){
	if (!isTableSelected()){
		ShowDialog('dialog/table.htm', 350, 380, true);
	}
}

// 修改表格属性
function TableProp(){
	if (isTableSelected()||isCursorInTableCell()){
		ShowDialog('dialog/table.htm?action=modify', 350, 380, true);
	}
}

// 修改单元格属性
function TableCellProp(){
	if (isCursorInTableCell()){
		ShowDialog('dialog/tablecell.htm', 350, 280, true);
	}
}

// 拆分单元格
function TableCellSplit(){
	if (isCursorInTableCell()){
		ShowDialog('dialog/tablecellsplit.htm', 200, 150, true);
	}
}

// 修改表格行属性
function TableRowProp(){
	if (isCursorInTableCell()){
		ShowDialog('dialog/tablecell.htm?action=row', 350, 280, true);
	}
}

// 插入行（在上方）
function TableRowInsertAbove() {

	if (isCursorInTableCell()){
		var numCols = 0

		allCells = selectedTR.cells
		for (var i=0;i<allCells.length;i++) {
		 	numCols = numCols + allCells[i].getAttribute('colSpan')
		}

		var newTR = selectedTable.insertRow(selectedTR.rowIndex)
	
		for (i = 0; i < numCols; i++) {
		 	newTD = newTR.insertCell()
			newTD.innerHTML = "&nbsp;"

			if (borderShown == "yes") {
				newTD.runtimeStyle.border = "1px dotted #BFBFBF"
			}
		}
	}	
}

// 插入行（在下方）
function TableRowInsertBelow() {

	if (isCursorInTableCell()){
		
		var numCols = 0

		allCells = selectedTR.cells
		for (var i=0;i<allCells.length;i++) {
		 	numCols = numCols + allCells[i].getAttribute('colSpan')
		}

		var newTR = selectedTable.insertRow(selectedTR.rowIndex+1)

		for (i = 0; i < numCols; i++) {
		 	newTD = newTR.insertCell()
			newTD.innerHTML = "&nbsp;"
			
			if (borderShown == "yes") {
				newTD.runtimeStyle.border = "1px dotted #BFBFBF"
			}
		}
	}
}

// 合并行（向下方）
function TableRowMerge() {
	if (isCursorInTableCell()) {

		var rowSpanTD = selectedTD.getAttribute('rowSpan')
		allRows = selectedTable.rows
		if (selectedTR.rowIndex +1 != allRows.length) {

			var allCellsInNextRow = allRows[selectedTR.rowIndex+selectedTD.rowSpan].cells
			var addRowSpan = allCellsInNextRow[selectedTD.cellIndex].getAttribute('rowSpan')
			var moveTo = selectedTD.rowSpan

			if (!addRowSpan) addRowSpan = 1;

			selectedTD.rowSpan = selectedTD.rowSpan + addRowSpan
			allRows[selectedTR.rowIndex + moveTo].deleteCell(selectedTD.cellIndex)
		}
	}

}

// 拆分行
function TableRowSplit(nRows){
	if (!isCursorInTableCell()) return;
	if (nRows<2) return;

	var addRows = nRows - 1;
	var addRowsNoSpan = addRows;

	var nsLeftColSpan = 0;
	for (var i=0; i<selectedTD.cellIndex; i++){
		nsLeftColSpan += selectedTR.cells[i].colSpan;
	}

	var allRows = selectedTable.rows;

	// rowspan>1时
	while (selectedTD.rowSpan > 1 && addRowsNoSpan > 0){
		var nextRow = allRows[selectedTR.rowIndex+selectedTD.rowSpan-1];
		selectedTD.rowSpan -= 1;

		var ncLeftColSpan = 0;
		var position = -1;
		for (var n=0; n<nextRow.cells.length; n++){
			ncLeftColSpan += nextRow.cells[n].getAttribute('colSpan');
			if (ncLeftColSpan>nsLeftColSpan){
				position = n;
				break;
			}
		}

		var newTD=nextRow.insertCell(position);
		newTD.innerHTML = "&nbsp;";

		if (borderShown == "yes") {
			newTD.runtimeStyle.border = "1px dotted #BFBFBF";
		}
			
		addRowsNoSpan -= 1;
	}

	// rowspan=1时
	for (var n=0; n<addRowsNoSpan; n++){
		var numCols = 0

		allCells = selectedTR.cells
		for (var i=0;i<allCells.length;i++) {
			numCols = numCols + allCells[i].getAttribute('colSpan')
		}

		var newTR = selectedTable.insertRow(selectedTR.rowIndex+1)

		// 上方行的rowspan达到这行
		for (var j=0; j<selectedTR.rowIndex; j++){
			for (var k=0; k<allRows[j].cells.length; k++){
				if ((allRows[j].cells[k].rowSpan>1)&&(allRows[j].cells[k].rowSpan>=selectedTR.rowIndex-allRows[j].rowIndex+1)){
					allRows[j].cells[k].rowSpan += 1;
				}
			}
		}
		// 当前行
		for (i = 0; i < allCells.length; i++) {
			if (i!=selectedTD.cellIndex){
				selectedTR.cells[i].rowSpan += 1;
			}else{
				newTD = newTR.insertCell();
				newTD.colSpan = selectedTD.colSpan;
				newTD.innerHTML = "&nbsp;";

				if (borderShown == "yes") {
					newTD.runtimeStyle.border = "1px dotted #BFBFBF";
				}
			}
		}
	}

}

// 删除行
function TableRowDelete() {
	if (isCursorInTableCell()) {
		selectedTable.deleteRow(selectedTR.rowIndex)
	}
}

// 插入列（在左侧）
function TableColInsertLeft() {
   	if (isCursorInTableCell()) {
		moveFromEnd = (selectedTR.cells.length-1) - (selectedTD.cellIndex)
		allRows = selectedTable.rows
		for (i=0;i<allRows.length;i++) {
			rowCount = allRows[i].cells.length - 1
			position = rowCount - moveFromEnd
			if (position < 0) {
				position = 0
			}
			newCell = allRows[i].insertCell(position)
			newCell.innerHTML = "&nbsp;"

			if (borderShown == "yes") {
				newCell.runtimeStyle.border = "1px dotted #BFBFBF"
			}
		}
   	}
}

// 插入列（在右侧）
function TableColInsertRight() {
   	if (isCursorInTableCell()) {
		moveFromEnd = (selectedTR.cells.length-1) - (selectedTD.cellIndex)
		allRows = selectedTable.rows
		for (i=0;i<allRows.length;i++) {
			rowCount = allRows[i].cells.length - 1
			position = rowCount - moveFromEnd
			if (position < 0) {
				position = 0
			}
			newCell = allRows[i].insertCell(position+1)
			newCell.innerHTML = "&nbsp;"

			if (borderShown == "yes") {
				newCell.runtimeStyle.border = "1px dotted #BFBFBF"
			}
		}	
   	}
}

// 合并列
function TableColMerge() {
	if (isCursorInTableCell()) {

		var colSpanTD = selectedTD.getAttribute('colSpan')
		allCells = selectedTR.cells

		if (selectedTD.cellIndex + 1 != selectedTR.cells.length) {
			var addColspan = allCells[selectedTD.cellIndex+1].getAttribute('colSpan')
			selectedTD.colSpan = colSpanTD + addColspan
			selectedTR.deleteCell(selectedTD.cellIndex+1)
		}	
	}

}

// 删除列
function TableColDelete() {
   	if (isCursorInTableCell()) {
		moveFromEnd = (selectedTR.cells.length-1) - (selectedTD.cellIndex)
		allRows = selectedTable.rows
		for (var i=0;i<allRows.length;i++) {
			endOfRow = allRows[i].cells.length - 1
			position = endOfRow - moveFromEnd
			if (position < 0) {
				position = 0
			}
				

			allCellsInRow = allRows[i].cells
				
			if (allCellsInRow[position].colSpan > 1) {
				allCellsInRow[position].colSpan = allCellsInRow[position].colSpan - 1
			} else { 
				allRows[i].deleteCell(position)
			}

		}

	}

}

// 拆分列
function TableColSplit(nCols){
	if (!isCursorInTableCell()) return;
	if (nCols<2) return;

	var addCols = nCols - 1;
	var addColsNoSpan = addCols;
	var newCell;

	var nsLeftColSpan = 0;
	var nsLeftRowSpanMoreOne = 0;
	for (var i=0; i<selectedTD.cellIndex; i++){
		nsLeftColSpan += selectedTR.cells[i].colSpan;
		if (selectedTR.cells[i].rowSpan > 1){
			nsLeftRowSpanMoreOne += 1;
		}
	}

	var allRows = selectedTable.rows
	// colSpan>1时
	while (selectedTD.colSpan > 1 && addColsNoSpan > 0) {
		newCell = selectedTR.insertCell(selectedTD.cellIndex+1);
		newCell.innerHTML = "&nbsp;"
		if (borderShown == "yes") {
			newCell.runtimeStyle.border = "1px dotted #BFBFBF"
		}
		selectedTD.colSpan -= 1;
		addColsNoSpan -= 1;
	}
	// colSpan=1时
	for (i=0;i<allRows.length;i++) {
		var ncLeftColSpan = 0;
		var position = -1;
		for (var n=0; n<allRows[i].cells.length; n++){
			ncLeftColSpan += allRows[i].cells[n].getAttribute('colSpan');
			if (ncLeftColSpan+nsLeftRowSpanMoreOne>nsLeftColSpan){
				position = n;
				break;
			}
		}
		
		if (selectedTR.rowIndex!=i){
			if (position!=-1){
				allRows[i].cells[position+nsLeftRowSpanMoreOne].colSpan += addColsNoSpan;
			}
		}else{
			for (var n=0; n<addColsNoSpan; n++){
				newCell = allRows[i].insertCell(selectedTD.cellIndex+1)
				newCell.innerHTML = "&nbsp;"
				newCell.rowSpan = selectedTD.rowSpan;

				if (borderShown == "yes") {
					newCell.runtimeStyle.border = "1px dotted #BFBFBF"
				}
			}
		}
	}
}

// 是否选中表格
function isTableSelected() {
	if (eWebEditor.document.selection.type == "Control") {
		var oControlRange = eWebEditor.document.selection.createRange();
		if (oControlRange(0).tagName.toUpperCase() == "TABLE") {
			selectedTable = eWebEditor.document.selection.createRange()(0);
			return true;
		}	
	}
} 

// 光标是否在表格中
function isCursorInTableCell() {
	if (eWebEditor.document.selection.type != "Control") {
		var elem = eWebEditor.document.selection.createRange().parentElement()
		while (elem.tagName.toUpperCase() != "TD" && elem.tagName.toUpperCase() != "TH"){
			elem = elem.parentElement
			if (elem == null)
                break
		}
		if (elem) {
			selectedTD = elem
			selectedTR = selectedTD.parentElement
			selectedTBODY =  selectedTR.parentElement
			selectedTable = selectedTBODY.parentElement
			return true
		}
	}
}
