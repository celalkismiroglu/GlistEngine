/*
 * gGUIGrid.cpp
 *
 *  Created on: 5 Aug 2022
 *      Author: murat
 */

#include "gGUIGrid.h"
//const int gGUIGrid::SELECTEDBOX_X = 0;
//const int gGUIGrid::SELECTEDBOX_Y = 1;
#include "gBaseApp.h"

gGUIGrid::gGUIGrid() {
//	gridsizer.setSize(10,10);
//	gridsizer.enableBorders(true);
	isselected = false;
	isrowselected = false;
	iscolumnselected = false;
	isdoubleclicked = false;
	shiftpressed = false;
	ctrlpressed = false;
	ctrlcpressed = false;
	ctrlvpressed = false;
	ctrlzpressed = false;
	ctrlypressed = false;
	selectedbox = 0;
	selectedtitle = 0;
	rownum = 50;
	columnnum = 10;
	rowtitle = 1;
	columntitle = 65; // 'A' char in ASCII
	gridboxw = 80.0f;
	gridboxh = 30.0f;
	newgridboxw = gridboxw;
	gridx = 0.0f;
	gridy = 0.0f;
	gridw = gridboxw * columnnum;
	gridh = gridboxh * rownum;
	clicktimediff = 250;
	titlediff = 20;
	clicktime = gGetSystemTimeMillis();
	previousclicktime = clicktime - 2 * clicktimediff;
	firstclicktime = previousclicktime - 2 * clicktimediff;
	strflag = "";
	totalw = columnnum * gridboxw;
	totalh = rownum * gridboxh;
	enableScrollbars(true, false);
	Cell tempcell;
	setMargin(tempcell.cellw / 2, tempcell.cellh);
}

gGUIGrid::~gGUIGrid() {

}

void gGUIGrid::set(gBaseApp* root, gBaseGUIObject* topParentGUIObject, gBaseGUIObject* parentGUIObject, int parentSlotLineNo, int parentSlotColumnNo, int x, int y, int w, int h) {
	totalw = columnnum * gridboxw;
	totalh = h;
	totalh = rownum * gridboxh;
	gGUIScrollable::set(root, topParentGUIObject, parentGUIObject, parentSlotLineNo, parentSlotColumnNo, x, y, w, h);
	setDimensions(w, h);
	textbox.set(root, this, this, 0, 0, gridx + (gridboxw / 2) + 1, gridy + gridboxh - 5, gridboxw - 6, gridboxh);
	textbox.setSize(gridboxw - 6, gridboxh - 2);
	textbox.enableBackground(false);
	manager = root->getAppManager()->getGUIManager();
//	gLogi("Grid") << "Textbox:" << textbox.left << " " << textbox.top << " " << textbox.right << " " << textbox.bottom;
}

void gGUIGrid::setGrid(int rowNum, int columnNum) {
	if(rowNum > maxrownum) rownum = maxrownum;
	else rownum = rowNum;
	if(columnNum > maxcolumnnum) columnnum = maxcolumnnum;
	else columnnum = columnNum;
	gridw = gridboxw * columnnum;
	gridh = gridboxh * rownum;
}

void gGUIGrid::setRowNum(int rowNum) {
	rownum = rowNum;
}

void gGUIGrid::setColumnNum(int columnNum) {
	columnnum = columnNum;
}

void gGUIGrid::update() {
	textbox.update();
}

void gGUIGrid::createCell(int rowNo, int columnNo) {
	Cell tempcell;
	tempcell.cellx = gridx + (gridboxw / 2) + (columnNo * gridboxw);
	tempcell.celly = gridy + gridboxh + (rowNo * gridboxh);
	tempcell.cellrowno = rowNo;
	tempcell.cellcolumnno = columnNo;
	allcells.push_back(tempcell);
}

void gGUIGrid::createTextBox() {
	//allcells.at(selectedbox).cellx + 1
	//allcells.at(cellindexcounter).cellx + (allcells.at(cellindexcounter).cellw - font->getStringWidth(allcells.at(cellindexcounter).showncontent)) * textbox.getTextMoveAmount() - textbox.getInitX() * textbox.getTextAlignment() - firstx
	int newwamount = font->getStringWidth(allcells.at(selectedbox).cellcontent) / gridboxw + 1;
	newgridboxw = gridboxw * newwamount;
	if(allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_LEFT) {
		while(newgridboxw > gridw - allcells.at(selectedbox).cellx + allcells.at(selectedbox).cellw / 2)
			newgridboxw -= gridboxw;
			//Should go to a new line and the width of the textbox should be "gridw - allcells.at(selectedbox).cellx + allcells.at(selectedbox).cellw / 2"
	}
	else if(allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_RIGHT) {
		while(newgridboxw > allcells.at(selectedbox).cellx - allcells.at(selectedbox).cellw / 2 + gridboxw)
			newgridboxw -= gridboxw;
			//Should go to a new line and the width of the textbox should be "allcells.at(selectedbox).cellx - allcells.at(selectedbox).cellw / 2 + gridboxw"
	}
	textbox.set(root, this, this, 0, 0, allcells.at(selectedbox).cellx + 1, allcells.at(selectedbox).celly - 2, newgridboxw - 10, gridboxh - 2);
	textbox.setTextFont(manager->getFont(allcells.at(selectedbox).fontnum, allcells.at(selectedbox).fontstate));
	textbox.setTextAlignment(allcells.at(selectedbox).cellalignment, allcells.at(selectedbox).cellw, textbox.getInitX());
	textbox.setTextColor(&allcells.at(selectedbox).cellfontcolor);
	if(allcells.at(selectedbox).cellcontent != "") {
		textbox.setText(allcells.at(selectedbox).cellcontent);
		int length = allcells.at(selectedbox).cellcontent.length();
		if(allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_LEFT || allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_RIGHT)
			textbox.setCursorPosX(font->getStringWidth(allcells.at(selectedbox).cellcontent), length);
		else {
			std::string mid;
			int middle;
			if(length % 2 == 0) middle = length / 2;
			else middle = length / 2 + 1;
			mid = allcells.at(selectedbox).cellcontent.substr(0, middle);
			textbox.setCursorPosX(font->getStringWidth(mid), middle);
		}
		allcells.at(selectedbox).showncontent = "";
	}
}

void gGUIGrid::showCells() {
	int cellindexcounter = 0;
	for(int i = 0; i < rownum; i++) {
		for(int j = 0; j < columnnum; j++) {
//			gLogi("Grid") << "cellx: "<< allcells.at(cellindexcounter).cellx
//				<< " celly: " << allcells.at(cellindexcounter).celly
//				<< " cellrowno: " << allcells.at(cellindexcounter).cellrowno
//				<< " cellcolumnno: " << allcells.at(cellindexcounter).cellcolumnno
//				<< " cellcontent: " << allcells.at(cellindexcounter).cellcontent
//				<< " celltype:" << allcells.at(cellindexcounter).celltype;
			cellindexcounter++;
		}
	}
}

void gGUIGrid::showCell(int rowNo , int columnNo) {
	int cellindex = columnNo + (rowNo  * columnnum);
//	gLogi("Grid") << "cellindex:" << cellindex;
//	gLogi("Grid") << "cellx: "<< allcells.at(cellindex).cellx
//		<< " celly: " << allcells.at(cellindex).celly
//		<< " cellrowno: " << allcells.at(cellindex).cellrowno
//		<< " cellcolumnno: " << allcells.at(cellindex).cellcolumnno
//		<< " cellcontent: " << allcells.at(cellindex).cellcontent
//		<< " celltype: " << allcells.at(cellindex).celltype;
}

void gGUIGrid::checkCellType(int cellIndex) {
	if(allcells.at(cellIndex).showncontent == "") return;
	bool isnegative = false;
	bool isfractional = false;
	bool doubledot = false;
	if(allcells.at(cellIndex).showncontent.at(0) == '-') isnegative = true;
	for(int i = 1 + isnegative; i < allcells.at(cellIndex).showncontent.length(); i++) {
		if(allcells.at(cellIndex).showncontent.at(i) == '.') {
			for(int j = i + 1; j < allcells.at(cellIndex).showncontent.length(); j++) {
				if(!isdigit(allcells.at(cellIndex).showncontent.at(j))) {
					isfractional = false;
					doubledot = true;
					break;
				}
				else isfractional = true;
			}
			if(doubledot) break;
		}
	}
	for(int i = 0 + isnegative; i < allcells.at(cellIndex).showncontent.length(); i++) {
		if(!isdigit(allcells.at(cellIndex).showncontent.at(i)) && !(isfractional && allcells.at(cellIndex).showncontent.at(i) == '.')) {
			if(allcells.at(cellIndex).celltype == Cell::TYPE_DIGIT && !allcells.at(cellIndex).iscellaligned) setCellAlignment(gBaseGUIObject::TEXTALIGNMENT_LEFT, false);
			allcells.at(cellIndex).celltype = Cell::TYPE_STRING;
			break;
		}
		else allcells.at(cellIndex).celltype = Cell::TYPE_DIGIT;
	}
	if(allcells.at(cellIndex).celltype == Cell::TYPE_DIGIT && !allcells.at(cellIndex).iscellaligned) setCellAlignment(gBaseGUIObject::TEXTALIGNMENT_RIGHT, false);
}

void gGUIGrid::setCellFont(int fontNum) {
	undocellstack.push(allcells.at(selectedbox));
	undocellstack.top().showncontent = fixTextFunction(undocellstack.top().cellcontent, selectedbox);
	allcells.at(selectedbox).fontnum = fontNum;
	allcells.at(selectedbox).fontstate = gGUIManager::FONTTYPE_REGULAR;
	if(allcells.at(selectedbox).isbold) allcells.at(selectedbox).fontstate += gGUIManager::FONTTYPE_BOLD;
	if(allcells.at(selectedbox).isitalic) allcells.at(selectedbox).fontstate += gGUIManager::FONTTYPE_ITALIC;
	textbox.setTextFont(manager->getFont(allcells.at(selectedbox).fontnum, allcells.at(selectedbox).fontstate));
}

void gGUIGrid::setCellFontBold() {
	if(allcells.at(selectedbox).isbold) allcells.at(selectedbox).isbold = false;
	else allcells.at(selectedbox).isbold = true;
	setCellFont(allcells.at(selectedbox).fontnum);
}

void gGUIGrid::setCellFontItalic() {
	if(allcells.at(selectedbox).isitalic) allcells.at(selectedbox).isitalic = false;
	else allcells.at(selectedbox).isitalic = true;
	setCellFont(allcells.at(selectedbox).fontnum);
}

void gGUIGrid::setCellAlignment(int cellAlignment, bool clicked) {
	if(clicked) {
		undocellstack.push(allcells.at(selectedbox));
		undocellstack.top().showncontent = fixTextFunction(undocellstack.top().cellcontent, selectedbox);
	}
	if(clicked && !allcells.at(selectedbox).iscellaligned) allcells.at(selectedbox).iscellaligned = true;
	else if(clicked && allcells.at(selectedbox).iscellaligned && allcells.at(selectedbox).celltype == Cell::TYPE_DIGIT) {
		allcells.at(selectedbox).iscellaligned = false;
		cellAlignment = gBaseGUIObject::TEXTALIGNMENT_RIGHT;
	}
	else if(clicked && allcells.at(selectedbox).cellalignment == cellAlignment) {
		allcells.at(selectedbox).iscellaligned = false;
		cellAlignment = gBaseGUIObject::TEXTALIGNMENT_LEFT;
	}
	allcells.at(selectedbox).cellalignment = cellAlignment;
	allcells.at(selectedbox).textmoveamount = 0.5f * cellAlignment;
	textbox.setTextAlignment(cellAlignment, allcells.at(selectedbox).cellw, textbox.getInitX());
}

void gGUIGrid::setCellFontColor(gColor *fontColor) {
	undocellstack.push(allcells.at(selectedbox));
	undocellstack.top().showncontent = fixTextFunction(undocellstack.top().cellcontent, selectedbox);
	allcells.at(selectedbox).cellfontcolor = fontColor;
	textbox.setTextColor(fontColor);
}

void gGUIGrid::setCellLine(int lineNo, bool clicked) {
	undocellstack.push(allcells.at(selectedbox));
	undocellstack.top().showncontent = fixTextFunction(undocellstack.top().cellcontent, selectedbox);
	if(clicked && lineNo == allcells.at(selectedbox).lineno) lineNo = 0;
	else allcells.at(selectedbox).lineno = lineNo;
}

void gGUIGrid::setCopiedCell(Cell* cell) {
	cell->cellcontent = appmanager->getClipboardString();
	cell->cellx = allcells.at(selectedbox).cellx;
	cell->celly = allcells.at(selectedbox).celly;
	cell->cellrowno = allcells.at(selectedbox).cellrowno;
	cell->cellcolumnno = allcells.at(selectedbox).cellcolumnno;
	cell->overflowcontent = "";
}

int gGUIGrid::getCell(int rowNo, int columnNo) {
	for(int i = 0; i < allcells.size(); i++)
		if(allcells.at(i).cellrowno == rowNo && allcells.at(i).cellcolumnno == columnNo) return i;
	return -1;
}

std::string gGUIGrid::fixTextFunction(std::string text, int index) {
	if(text == "") return text;
	std::string tempstr = text;
	bool function = (tempstr[0] == '=');
	if(int(tempstr[0]) == 39) tempstr.erase(0, 1);
	else if(function) {
		tempstr.erase(0, function);
		bool isnegative = false;
		std::transform(tempstr.begin(), tempstr.end(), tempstr.begin(), ::toupper);
		tempstr = fixNumeric(tempstr);
		isnegative = (tempstr[0] == '-');
		tempstr.erase(0, isnegative);
		bool hasdigit = isNumeric(tempstr);

		if(!isdigit(tempstr[0])) {
			int parentheses1 = tempstr.find('(');
			int parentheses2 = tempstr.find(')');
			if(parentheses1 != std::string::npos && parentheses2 != std::string::npos) {
				std::string functionstr = tempstr.substr(0, parentheses1);
				std::string parenthesesstr = tempstr.substr(parentheses1 + 1, parentheses2 - parentheses1 - 1);
				parenthesesstr = fixNumeric(parenthesesstr);
				bool isparenthesesnegative = (parenthesesstr[0] == '-');
				parenthesesstr.erase(0, isparenthesesnegative);
				if(functionstr == "SUM") {
					int doubledat = parenthesesstr.find(':');
					if(doubledat == std::string::npos) {
						if(!isNumeric(parenthesesstr)) {
							std::string column = getTextColumn(parenthesesstr);
							std::string row = parenthesesstr.substr(column.size(), parenthesesstr.size() - column.size());
							int columnindex = int(column[column.size() - 1]) % 65;
							for(int i = 0; i < column.size() - 1; i++) columnindex += int((column[i] % 65 + 1) * (26 * ((column.size() - 1) - i)));
							int copiedindex = getCell(std::stoi(row), columnindex);
							if(copiedindex == -1) {
								createCell(std::stoi(row) - 1, columnindex);
								copiedindex = allcells.size() - 1;
							}
							tempstr = allcells.at(copiedindex).showncontent;
							functionindexes.push_back(index);
							if(text != strflag) addFunction(FUNCTION_COPY, copiedindex);
						}
					}
					else {
						std::string str1 = parenthesesstr.substr(0, doubledat);
						std::string str2 = parenthesesstr.substr(str1.size() + 1, parenthesesstr.size());
						int columnno1 = int(str1[0]) % 65;
						int columnno2 = int(str2[0]) % 65;
						int rowno1 = std::stoi(str1.substr(1, str1.size())) - 1;
						int rowno2 = std::stoi(str2.substr(1, str2.size())) - 1;

						tempstr = std::to_string(makeSum(columnno1, rowno1, columnno2, rowno2));
						while(tempstr[tempstr.size() - 1] == '0') tempstr.erase(tempstr.size() - 1, 1);
						if(tempstr[tempstr.size() - 1] == '.') tempstr.erase(tempstr.size() - 1, 1);
						if(text != strflag) addFunction(FUNCTION_SUM, index);
					}
					if((isnegative && !isparenthesesnegative) || (!isnegative && isparenthesesnegative)) tempstr = "-" + tempstr;
				}
			}
			else if(int(tempstr[0]) >= 65 && int(tempstr[0]) < 91) {
				if(tempstr.size() > 1) {
					std::string tstr = tempstr.substr(1, tempstr.size());
					int found = tstr.find_first_not_of("0123456789");
					if(found == std::string::npos) {
						int copiedindex = getCell(std::stoi(tstr) - 1, int(tempstr[0]) % 65);
						if(copiedindex == -1) {
							createCell(std::stoi(tstr) - 1, int(tempstr[0]) % 65);
							copiedindex = allcells.size() - 1;
						}
						tempstr = allcells.at(copiedindex).showncontent;
						if(isnegative) tempstr = "-" + tempstr;
						functionindexes.push_back(index);
						if(text != strflag) addFunction(FUNCTION_COPY, copiedindex);
					}
				}
			}
		}
	}
	return tempstr;
}

std::string gGUIGrid::fixNumeric(std::string text) {
	std::string tempstr = text;
	std::stack<int> unnecessaryindexes;
	if(tempstr[0] == '+') {
		unnecessaryindexes.push(0);
		for(int i = 1; i < tempstr.size(); i++) {
			if(tempstr[i] != '+' && tempstr[i] != '-') break;
			if(tempstr[i] == '+') unnecessaryindexes.push(i);
			else if(tempstr[i] != '-') continue;
		}
		while(!unnecessaryindexes.empty()) {
			tempstr.erase(unnecessaryindexes.top(), 1);
			unnecessaryindexes.pop();
		}
	}
	if(tempstr[0] == '-') {
		int minuscount = 1;
		for(int i = 1; i < tempstr.size(); i++) {
			if(tempstr[i] == '-') minuscount++;
			else if(tempstr[i] == '+') unnecessaryindexes.push(i);
			else break;
		}
		while(!unnecessaryindexes.empty()) {
			tempstr.erase(unnecessaryindexes.top(), 1);
			unnecessaryindexes.pop();
		}
		if(minuscount % 2 == 1) tempstr.erase(0, minuscount - 1);
		else tempstr.erase(0, minuscount);
	}
	return tempstr;
}

std::string gGUIGrid::fixOverflowText(Cell& thisCell, Cell& otherCell) {
	std::string tmpstr = "";
	int diff = 0;
	int charindex = 0;
	bool middlealignment = (thisCell.cellalignment == gBaseGUIObject::TEXTALIGNMENT_MIDDLE);

	if(otherCell.cellx > thisCell.cellx)
		diff = otherCell.cellx - (thisCell.cellx + (middlealignment * thisCell.cellw / 2) + (!middlealignment * (textbox.getInitX() + 2) * 2));
	else
		diff = (thisCell.cellx + thisCell.cellw - (middlealignment * thisCell.cellw / 2)) - (otherCell.cellx + otherCell.cellw + (!middlealignment * (textbox.getInitX() + 2) * 2));

	while(manager->getFont(thisCell.fontnum)->getStringWidth(tmpstr) < diff) {
		tmpstr += thisCell.showncontent[charindex];
		charindex++;
	}
	return tmpstr;
}

std::string gGUIGrid::getTextColumn(std::string text) {
    int found = text.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (found != std::string::npos && std::isalpha(text[0])) return text.substr(0, found);
    return text;
}

int gGUIGrid::getNearestFilledCell(int index) {
	int nearestindex = -1;
	for(int i = 0; i < allcells.size(); i++) {
		if(i != index && allcells.at(i).celly == allcells.at(index).celly && !allcells.at(i).showncontent.empty()) {
			if(allcells.at(index).cellalignment != gBaseGUIObject::TEXTALIGNMENT_RIGHT && allcells.at(index).cellx + manager->getFont(allcells.at(index).fontnum)->getStringWidth(allcells.at(index).showncontent) > allcells.at(i).cellx && allcells.at(index).cellx < allcells.at(i).cellx) {
				if(nearestindex != -1 && allcells.at(nearestindex).cellx > allcells.at(i).cellx) nearestindex = i;
				else if(nearestindex == -1) nearestindex = i;
			}
			else if(allcells.at(index).cellalignment == gBaseGUIObject::TEXTALIGNMENT_RIGHT && allcells.at(index).cellx - (manager->getFont(allcells.at(index).fontnum)->getStringWidth(allcells.at(index).showncontent) - allcells.at(index).cellw) < allcells.at(i).cellx + allcells.at(i).cellw && allcells.at(index).cellx > allcells.at(i).cellx) {
				if(nearestindex != -1 && allcells.at(nearestindex).cellx < allcells.at(i).cellx) nearestindex = i;
				else if(nearestindex == -1) nearestindex = i;
			}
		}
	}
	return nearestindex;
}

float gGUIGrid::makeSum(int c1, int r1, int c2, int r2) {
	if(c1 > c2) {
		int flag = c1;
		c1 = c2;
		c2 = flag;
	}
	if(r1 > r2) {
		int flag = r1;
		r1 = r2;
		r2 = flag;
	}
	float result = 0;
	for(int row = r1; row <= r2; row++) {
		for(int column = c1; column <= c2; column++) {
			int currentindex = getCell(row, column);
			if(currentindex == -1) {
				createCell(row, column);
				currentindex = allcells.size() - 1;
			}
			functionindexes.push_back(currentindex);
			if(allcells.at(currentindex).celltype == Cell::TYPE_DIGIT) {
				result += std::stof(allcells.at(currentindex).showncontent);
			}
		}
	}
	return result;
}

bool gGUIGrid::isNumeric(std::string text) {
	bool hasdigit = true;
	for(int i = 0; i < text.size(); i++) {
		if(!isdigit(text[i])) {
			hasdigit = false;
			break;
		}
	}
	return hasdigit;
}

void gGUIGrid::addFunction(int functionType, int functionSender) {
	int same = -1;
	functionindexes.insert(functionindexes.begin(), functionSender);
	functionindexes.insert(functionindexes.begin(), functionType);
	for(int i = 0; i < functions.size(); i++) {
		if(functionSender == functions[i][FUNCTION_SENDER] && functions[i][FUNCTION_TYPE] != FUNCTION_COPY) {
			same = i;
			break;
		}
	}
	if(same != -1) functions[same] = functionindexes;
	else functions.push_back(functionindexes);
	functionindexes.clear();
	operateFunction(functions.size() - 1);
}

void gGUIGrid::removeFunction(int functionNum) {
	functionindexes.clear();
	functions.erase(functions.begin() + functionNum);
}

void gGUIGrid::operateFunction(int functionNum) {
	switch(functions[functionNum][FUNCTION_TYPE]) {
	case FUNCTION_COPY:
		allcells.at(functions[functionNum][FUNCTION_FIRSTINDEX]).showncontent = allcells.at(functions[functionNum][FUNCTION_SENDER]).showncontent;
		break;
	case FUNCTION_SUM:
		std::string result = std::to_string(makeSum(functions[functionNum][FUNCTION_FIRSTINDEX] % columnnum, functions[functionNum][FUNCTION_FIRSTINDEX] / columnnum, functions[functionNum][functions[functionNum].size() - 1] % columnnum, functions[functionNum][functions[functionNum].size() - 1] / columnnum));
		while(result[result.size() - 1] == '0') result.erase(result.size() - 1, 1);
		if(result[result.size() - 1] == '.') result.erase(result.size() - 1, 1);
		allcells.at(functions[functionNum][FUNCTION_SENDER]).showncontent = result;
		break;
	}
}

void gGUIGrid::makeDefaultCell() {
	allcells.at(selectedbox).iscellselected = false;
	allcells.at(selectedbox).iscellaligned = false;
	allcells.at(selectedbox).fontnum = gGUIManager::FONTTYPE_REGULAR;
	allcells.at(selectedbox).cellalignment = gBaseGUIObject::TEXTALIGNMENT_LEFT;
	allcells.at(selectedbox).lineno = TEXTLINE_NONE;
	allcells.at(selectedbox).cellh = 30.0f;
	allcells.at(selectedbox).cellw = 80.0f;
	allcells.at(selectedbox).textmoveamount = 0;
	allcells.at(selectedbox).cellcontent = "";
	allcells.at(selectedbox).showncontent = "";
	allcells.at(selectedbox).overflowcontent = "";
	allcells.at(selectedbox).celltype = Cell::TYPE_STRING;
	allcells.at(selectedbox).cellfontcolor = fontcolor;
	for(int i = 0; i < functions.size(); i++) {
		if(selectedbox == functions[i][FUNCTION_SENDER]) {
			removeFunction(selectedbox);
			break;
		}
	}
}

void gGUIGrid::fillCell(int cellNo, std::string tempstr) { //when rowNo = 1, columnNO = 4; tempstr = "happyyyy";
	if(cellNo > rownum * columnnum - 1) return;
	int cellindex = cellNo;
	if(strflag != tempstr && !ctrlzpressed && !ctrlypressed && !ctrlvpressed) {
		undocellstack.push(allcells.at(cellindex));
		undocellstack.top().cellcontent = strflag;
		undocellstack.top().showncontent = fixTextFunction(strflag, cellindex);
	}
	allcells.at(cellindex).cellcontent = tempstr;
	allcells.at(cellindex).showncontent = fixTextFunction(tempstr, cellindex);

	checkCellType(cellindex);

	int nearestindex = -1;
	for(int i = 0; i < allcells.size(); i++) {
		if(allcells.at(i).celly == allcells.at(cellindex).celly) {
			nearestindex = getNearestFilledCell(i);
			if(nearestindex != -1) allcells.at(i).overflowcontent = fixOverflowText(allcells.at(i), allcells.at(nearestindex));
			else allcells.at(i).overflowcontent = "";
		}
	}
	if(allcells.at(cellindex).cellw < manager->getFont(allcells.at(cellindex).fontnum)->getStringWidth(allcells.at(cellindex).showncontent)) {
		nearestindex = getNearestFilledCell(cellindex);
		if(nearestindex != -1) allcells.at(cellindex).overflowcontent = fixOverflowText(allcells.at(cellindex), allcells.at(nearestindex));
		else allcells.at(cellindex).overflowcontent = "";
	}
}

void gGUIGrid::changeCell() {
	int index = selectedbox;
	if(ctrlzpressed) index = getCell(undocellstack.top().cellrowno, undocellstack.top().cellcolumnno);
	else if(ctrlypressed) index = getCell(redocellstack.top().cellrowno, redocellstack.top().cellcolumnno);

	std::stack<int> removefunctionindexstack;
	for(int function = 0; function < functions.size(); function++) {
		if(index == functions[function][FUNCTION_SENDER] && functions[function][FUNCTION_TYPE] == FUNCTION_SUM) {
			removefunctionindexstack.push(function);
			continue;
		}
		else if(index == functions[function][FUNCTION_FIRSTINDEX] && functions[function][FUNCTION_TYPE] == FUNCTION_COPY) {
			removefunctionindexstack.push(function);
			continue;
		}
	}
	while(!removefunctionindexstack.empty()) {
		removeFunction(removefunctionindexstack.top());
		removefunctionindexstack.pop();
	}

	if(ctrlvpressed) {
		undocellstack.push(allcells.at(index));
		allcells.at(index) = copiedcell;
	}
	else if(ctrlzpressed) allcells.at(index) = undocellstack.top();
	else if(ctrlypressed) allcells.at(index) = redocellstack.top();
	if(istextboxactive) {
		fillCell(index, textbox.getText());
		textbox.cleanText();
		istextboxactive = false;
	}
	else {
		fillCell(index, allcells.at(index).cellcontent);
		ctrlvpressed = false;
		ctrlzpressed = false;
		ctrlypressed = false;
	}

	for(int function = 0; function < functions.size(); function++) {
		if(selectedbox == functions[function][FUNCTION_SENDER] && functions[function][FUNCTION_TYPE] == FUNCTION_COPY) {
			operateFunction(function);
			continue;
		}
		for(int functionindex = FUNCTION_FIRSTINDEX; functionindex < functions[function].size(); functionindex++) {
			if(selectedbox == functions[function][functionindex]) {
				operateFunction(function);
				break;
			}
		}
	}
}

void gGUIGrid::drawContent() {
	gColor oldcolor = renderer->getColor();
	drawCellBackground();
	if(isselected) drawSelectedBox();
	else if(isrowselected) drawSelectedRow();
	else if(iscolumnselected) drawSelectedColumn();
	drawCellContents();
	textbox.setFirstX(firstx);
	textbox.setFirstY(firsty);
	if(istextboxactive)textbox.draw();
	drawTitleRowBackground();
	drawRowContents();
	drawTitleColumnBackground();
	drawColumnContents();
	drawTitleLines();
	renderer->setColor(oldcolor);
}

void gGUIGrid::drawCellBackground() {
	renderer->setColor(*textbackgroundcolor);
	gDrawRectangle(gridx + (gridboxw / 2) - firstx, gridy + gridboxh - firsty, gridw, gridh, true);
}

void gGUIGrid::drawSelectedBox() {
	renderer->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	gDrawRectangle(allcells.at(selectedbox).cellx + 1 - firstx, (allcells.at(selectedbox).celly + 1) - firsty, gridboxw - 2, gridboxh - 2, false);
	gDrawRectangle(allcells.at(selectedbox).cellx + (gridboxw - 2) - 6 - firstx, allcells.at(selectedbox).celly + (gridboxh - 2) - 4 - firsty, 6, 6, true); // FLAG
}

void gGUIGrid::drawSelectedRow() {
	renderer->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	gDrawRectangle(gridx + (gridboxw / 2) + 1 - firstx, gridy + gridboxh + gridboxh * allcells.at(selectedtitle).cellrowno + 1 - firsty, gridw - 2, gridboxh - 2, false);
	gDrawRectangle(gridx + (gridboxw / 2) + gridw - 2 - 6 - firstx, gridy + gridboxh + gridboxh * allcells.at(selectedtitle).cellrowno + (gridboxh - 2) - 4 - firsty, 6, 6, true); // FLAG
}

void gGUIGrid::drawSelectedColumn() {
	renderer->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	gDrawRectangle(gridx + (gridboxw / 2) + 1 + (gridboxw * allcells.at(selectedtitle).cellcolumnno) - firstx, gridy + gridboxh + 1 - firsty, gridboxw - 2, gridh - 2, false);
	gDrawRectangle(gridx + (gridboxw / 2) + (gridboxw * allcells.at(selectedtitle).cellcolumnno) + gridboxw - 2 - 6 - firstx, gridy + gridboxh + gridh - 2 - 4 - firsty, 6, 6, true); // FLAG
}

void gGUIGrid::drawTitleRowBackground() {
	renderer->setColor(*buttoncolor);
	gDrawRectangle(gridx, gridy - firsty, gridboxw / 2, gridh + gridboxh, true);
}

void gGUIGrid::drawTitleColumnBackground() {
	renderer->setColor(*buttoncolor);
	gDrawRectangle(gridx - firstx, gridy, gridw + (gridboxw / 2), gridboxh , true);
}

void gGUIGrid::drawRowContents() {
	int temprow = rowtitle;
	for(int i = 1; i <= rownum; i++) {
		if(gridy + i * gridboxh < gridy + firsty || gridy + i * gridboxh > boxh + firsty) continue;
		if(gridy + i * gridboxh >= gridy + firsty && gridy + i * gridboxh <= boxh + firsty) {
			std::string rowtitlestring = std::to_string(i);
			renderer->setColor(*fontcolor);
			font->drawText(rowtitlestring, gridx + (gridboxw / 4) - (font->getStringWidth(rowtitlestring) / 2), gridy + (gridboxh / 2) + (i * gridboxh)  + (font->getStringHeight(rowtitlestring) / 2) - firsty);
			renderer->setColor(*pressedbuttoncolor);
			gDrawLine(gridx - firstx, gridy + ((i + 1) * gridboxh) - firsty, gridx + gridw + (gridboxw / 2) - firstx, gridy + ((i + 1) * gridboxh) - firsty);
		}
		temprow++;
	}
}

void gGUIGrid::drawColumnContents() {
	std::string columntitlestring = "";
	for(int i = 0; i < columnnum; i++) {
		if(gridx + (i + 1) * gridboxw < gridx + firstx || gridx + ((i + 1) * gridboxw) > boxw + firstx) continue;
		if(gridx + (i + 1) * gridboxw >= gridx + firstx && gridx + ((i + 1) * gridboxw) <= boxw + firstx) {
			if(i / 26 > 26) {
				columntitlestring = (char)(columntitle + i / (26 * 26) - 1);
				columntitlestring += (char)(columntitle + (i / 26 - 1) % 26);
				columntitlestring += (char)(columntitle + i % 26);
			}
			else if(i / 26 > 0) {
				columntitlestring = (char)(columntitle + i / 26 - 1);
				columntitlestring += (char)(columntitle + i % 26);
			}
			else columntitlestring = (char)(columntitle + i);
			renderer->setColor(*fontcolor);
			font->drawText(columntitlestring, gridx + ((i + 1) * gridboxw) - (font->getStringWidth(columntitlestring) / 2) - firstx, gridy + (gridboxh / 2) + (font->getStringHeight(columntitlestring) / 2));
			renderer->setColor(*pressedbuttoncolor);
			gDrawLine(gridx - (gridboxw / 2) + ((i + 2) * gridboxw) - firstx, gridy - firsty, gridx - (gridboxw / 2) + ((i + 2) * gridboxw) - firstx, gridy + gridboxh + gridh - firsty);
		}
	}
	Cell emptycell;
	gDrawRectangle(gridx, gridy, emptycell.cellw / 2, emptycell.cellh, true);
}

void gGUIGrid::drawTitleLines() {
	renderer->setColor(*backgroundcolor);
	gDrawLine(gridx + (gridboxw / 2) + 1 - firstx, gridy - firsty, gridx + (gridboxw / 2) + 1 - firstx, gridy + gridboxh + gridh - firsty);
	gDrawLine(gridx - firstx, gridy + (gridboxh) - firsty, gridx + gridboxw / 2 + gridw - firstx, gridy + (gridboxh) - firsty);
}

void gGUIGrid::drawCellContents() {
	for(int i = 0; i < allcells.size(); i++) {
		Cell& currentcell = allcells.at(i);
		float currentstringwidth;
		float currentstringheight;
		if(currentcell.overflowcontent.empty()) {
			currentstringwidth = manager->getFont(currentcell.fontnum, currentcell.fontstate)->getStringWidth(currentcell.showncontent);
			currentstringheight = manager->getFont(currentcell.fontnum, currentcell.fontstate)->getStringHeight(currentcell.showncontent);
		}
		else {
			currentstringwidth = manager->getFont(currentcell.fontnum, currentcell.fontstate)->getStringWidth(currentcell.overflowcontent);
			currentstringheight = manager->getFont(currentcell.fontnum, currentcell.fontstate)->getStringHeight(currentcell.overflowcontent);
		}
		if(currentcell.cellx + currentstringwidth * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment < gridx + firstx || currentcell.cellx + currentstringwidth * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment > boxw + firstx || currentcell.celly < gridy + firsty || currentcell.celly > boxh + firsty) continue;
		if(currentcell.cellx + currentstringwidth * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment >= gridx + firstx && currentcell.cellx + currentstringwidth * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment <= boxw + firstx && currentcell.celly >= gridy + firsty && currentcell.celly <= boxh + firsty) {
			renderer->setColor(currentcell.cellfontcolor);
			if(currentcell.overflowcontent.empty()) manager->getFont(currentcell.fontnum, currentcell.fontstate)->drawText(currentcell.showncontent, currentcell.cellx + (currentcell.cellw - currentstringwidth) * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment - firstx, currentcell.celly + (gridboxh / 2) + (currentstringheight / 2) - firsty);
			else manager->getFont(currentcell.fontnum, currentcell.fontstate)->drawText(currentcell.overflowcontent, currentcell.cellx + (currentcell.cellw - currentstringwidth) * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment - firstx, currentcell.celly + (gridboxh / 2) + (currentstringheight / 2) - firsty);
			switch(currentcell.lineno) {
			case TEXTLINE_UNDER:
				gDrawLine(currentcell.cellx + (currentcell.cellw - currentstringwidth) * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment - firstx, currentcell.celly + (gridboxh / 2) + currentstringheight - firsty,
						currentcell.cellx + currentstringwidth + ((currentcell.cellw - currentstringwidth) * currentcell.textmoveamount) - firstx, currentcell.celly + (gridboxh / 2) + currentstringheight - firsty);
				break;
			case TEXTLINE_DOUBLEUNDER:
				gDrawLine(currentcell.cellx + (currentcell.cellw - currentstringwidth) * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment - firstx, currentcell.celly + (gridboxh / 2) + currentstringheight - firsty,
						currentcell.cellx + currentstringwidth + ((currentcell.cellw - currentstringwidth) * currentcell.textmoveamount) - firstx, currentcell.celly + (gridboxh / 2) + currentstringheight - firsty);
				gDrawLine(currentcell.cellx + (currentcell.cellw - currentstringwidth) * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment - firstx, currentcell.celly + (gridboxh / 2) + currentstringheight - firsty + 2,
						currentcell.cellx + currentstringwidth + ((currentcell.cellw - currentstringwidth) * currentcell.textmoveamount) - firstx, currentcell.celly + (gridboxh / 2) + currentstringheight - firsty + 2);
				break;
			case TEXTLINE_STRIKE:
				gDrawLine(currentcell.cellx + (currentcell.cellw - currentstringwidth) * currentcell.textmoveamount - textbox.getInitX() * currentcell.cellalignment - firstx, currentcell.celly + currentcell.cellh / 2 + textbox.getInitX() - firsty,
						currentcell.cellx + currentstringwidth + ((currentcell.cellw - currentstringwidth) * currentcell.textmoveamount) - firstx, currentcell.celly + currentcell.cellh / 2 + textbox.getInitX() - firsty);
				break;
			default:
				break;
			}
		}
	}
}

void gGUIGrid::mousePressed(int x, int y, int button) {
	gGUIScrollable::mousePressed(x, y, button);
	int pressedx = x - left - firstx;
	int pressedy = y - top - firsty - titledy;
	if(!(pressedy < gridy + gridboxh - firsty && pressedx < gridx + (gridboxw / 2) - firstx) && pressedx >= gridx - firstx && pressedx <= gridx + (gridboxw / 2) + gridw - firstx && pressedy >= gridy - firsty && pressedy <= gridy + gridboxh + gridh - firsty) {
		if(pressedx >= gridx + (gridboxw / 2) - firstx && pressedx <= gridx + (gridboxw / 2) + gridw - firstx && pressedy >= gridy + gridboxh - firsty && pressedy <= gridy + gridboxh + gridh - firsty) {
			isselected = true;
			isrowselected = false;
			iscolumnselected = false;
			int index = getCell((int)((pressedy - gridboxh + firsty * 2) / gridboxh), (int)((pressedx - (gridboxw / 2) + firstx * 2) / gridboxw));
			if(index == -1) {
				createCell((int)((pressedy - gridboxh + firsty * 2) / gridboxh), (int)((pressedx - (gridboxw / 2) + firstx * 2) / gridboxw));
				index = allcells.size() - 1;
			}
			if(istextboxactive) changeCell();
			allcells.at(selectedbox).iscellselected = false;
			allcells.at(index).iscellselected = true;
			selectedbox = index;
		}
		else if(pressedx >= gridx - firstx && pressedx < gridx + (gridboxw / 2) - firstx && pressedy >= gridy + gridboxh - firsty && pressedy <= gridy + gridboxh + gridh - firsty) {
			int index = getCell((int)((pressedy - gridboxh + firsty * 2) / gridboxh), 0);
			if(index == -1) {
				createCell((int)((pressedy - gridboxh + firsty * 2) / gridboxh), 0);
				index = allcells.size() - 1;
			}
			if(istextboxactive) changeCell();
			allcells.at(selectedbox).iscellselected = false;
			allcells.at(index).iscellselected = true;
			selectedtitle = index;
			selectedbox = index;
			isselected = false;
			isrowselected = true;
			iscolumnselected = false;
		}
		else if(pressedx >= gridx + (gridboxw / 2) - firstx && pressedx < gridx + (gridboxw / 2) + gridw - firstx && pressedy >= gridy - firsty && pressedy <= gridy + gridboxh - firsty) {
			int index = getCell(0, (int)((pressedx - (gridboxw / 2) + firstx * 2) / gridboxw));
			if(index == -1) {
				createCell(0, (int)((pressedx - (gridboxw / 2) + firstx * 2) / gridboxw));
				index = allcells.size() - 1;
			}
			if(istextboxactive) changeCell();
			allcells.at(selectedbox).iscellselected = false;
			allcells.at(index).iscellselected = true;
			selectedtitle = index;
			selectedbox = index;
			isselected = false;
			isrowselected = false;
			iscolumnselected = true;
		}

		previousclicktime = clicktime;
		clicktime = gGetSystemTimeMillis();
		if(clicktime - previousclicktime <= clicktimediff) {
			isdoubleclicked = true;
//			gLogi("Grid") << "doubleclicked.";
		} else isdoubleclicked = false;
		if(isdoubleclicked) {
			strflag = allcells.at(selectedbox).cellcontent;
			textbox.cleanText();
			createTextBox();
			textbox.mousePressed(pressedx, pressedy, button);
			istextboxactive = true;
		} else istextboxactive = false;
	}
}

void gGUIGrid::mouseReleased(int x, int y, int button) {
	if(istextboxactive) textbox.mouseReleased((x - left), (y - top - firsty - titledy), button);
	gGUIScrollable::mouseReleased(x, y, button);
}

void gGUIGrid::mouseDragged(int x, int y, int button) {
//	gLogi("Grid") << x << " " << y;
	if(istextboxactive) textbox.mouseDragged((x - left), (y - top - firsty), button);
	gGUIScrollable::mouseDragged(x, y, button);
}

void gGUIGrid::keyPressed(int key){
	if(istextboxactive) textbox.keyPressed(key);
	else if(key == G_KEY_C && ctrlpressed) {
		ctrlcpressed = true;
		copiedcell = allcells.at(selectedbox);
		appmanager->setClipboardString(allcells.at(selectedbox).cellcontent);
	}
	else if(key == G_KEY_V && ctrlpressed && (&copiedcell != NULL || appmanager->getClipboardString() != "")) {
		ctrlvpressed = true;
		setCopiedCell(&copiedcell);
		changeCell();
	}
	else if(key == G_KEY_X && ctrlpressed) {
		ctrlcpressed = false;
		copiedcell = allcells.at(selectedbox);
		appmanager->setClipboardString(allcells.at(selectedbox).cellcontent);
		textbox.cleanText();
		makeDefaultCell();
	}
	else if(key == G_KEY_Z && ctrlpressed) {
		if(undocellstack.empty()) return;
		ctrlzpressed = true;
		int zindex = getCell(undocellstack.top().cellrowno, undocellstack.top().cellcolumnno);
		redocellstack.push(allcells.at(zindex));
		redocellstack.top().showncontent = fixTextFunction(redocellstack.top().cellcontent, zindex);
		changeCell();
		undocellstack.pop();
	}
	else if(key == G_KEY_Y && ctrlpressed) {
		if(redocellstack.empty()) return;
		ctrlypressed = true;
		int yindex = getCell(redocellstack.top().cellrowno, redocellstack.top().cellcolumnno);
		undocellstack.push(allcells.at(yindex));
		undocellstack.top().showncontent = fixTextFunction(undocellstack.top().cellcontent, yindex);
		changeCell();
		redocellstack.pop();
	}
	else if(key == G_KEY_B && ctrlpressed) setCellFontBold();
	else if(key == G_KEY_I && ctrlpressed) setCellFontItalic();
	else if((isselected || isrowselected || iscolumnselected) && (key == G_KEY_LEFT_CONTROL || key == G_KEY_RIGHT_CONTROL)) ctrlpressed = true;
	else if((isselected || isrowselected || iscolumnselected) && (key == G_KEY_LEFT_SHIFT || key == G_KEY_RIGHT_SHIFT)) shiftpressed = true;
	else if((isselected || isrowselected || iscolumnselected) && key != G_KEY_ENTER && key != G_KEY_UP && key != G_KEY_DOWN && key != G_KEY_RIGHT && key != G_KEY_LEFT && key != G_KEY_ESC && key != G_KEY_LEFT_SHIFT && key != G_KEY_RIGHT_SHIFT && key != G_KEY_F2) {
		textbox.cleanText();
		strflag = allcells.at(selectedbox).cellcontent;
		allcells.at(selectedbox).cellcontent = "";
		allcells.at(selectedbox).showncontent = "";
		createTextBox();
		textbox.mousePressed(allcells.at(selectedbox).cellx + textbox.getInitX(), allcells.at(selectedbox).celly + textbox.getInitX(), 0);
		textbox.keyPressed(key);
		istextboxactive = true;
	}
}

void gGUIGrid::keyReleased(int key) {
	if(istextboxactive) textbox.keyReleased(key);
	if((key == G_KEY_ENTER && istextboxactive)) {
		changeCell();
		istextboxactive = false;
		textbox.setEditable(false);
	}
	else if ((key == G_KEY_ENTER || key == G_KEY_DOWN) && !istextboxactive) {
		if(allcells.at(selectedbox).cellrowno + 1 < rownum) {
			int index = getCell(allcells.at(selectedbox).cellrowno + 1, allcells.at(selectedbox).cellcolumnno);
			if(index == -1) {
				createCell(allcells.at(selectedbox).cellrowno + 1, allcells.at(selectedbox).cellcolumnno);
				index = allcells.size() - 1;
			}
			selectedbox = index;
		}
		if(isrowselected) {
			isrowselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_UP && !istextboxactive) {
		if(allcells.at(selectedbox).cellrowno - 1 >= 0) {
			int index = getCell(allcells.at(selectedbox).cellrowno - 1, allcells.at(selectedbox).cellcolumnno);
			if(index == -1) {
				createCell(allcells.at(selectedbox).cellrowno - 1, allcells.at(selectedbox).cellcolumnno);
				index = allcells.size() - 1;
			}
			selectedbox = index;
		}
		if(isrowselected) {
			isrowselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_RIGHT && !istextboxactive) {
		if(allcells.at(selectedbox).cellcolumnno + 1 < columnnum) {
			int index = getCell(allcells.at(selectedbox).cellrowno, allcells.at(selectedbox).cellcolumnno + 1);
			if(index == -1) {
				createCell(allcells.at(selectedbox).cellrowno, allcells.at(selectedbox).cellcolumnno + 1);
				index = allcells.size() - 1;
			}
			selectedbox = index;
		}
		if(iscolumnselected) {
			iscolumnselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_LEFT && !istextboxactive) {
		if(allcells.at(selectedbox).cellcolumnno - 1 >= 0) {
			int index = getCell(allcells.at(selectedbox).cellrowno, allcells.at(selectedbox).cellcolumnno - 1);
			if(index == -1) {
				createCell(allcells.at(selectedbox).cellrowno, allcells.at(selectedbox).cellcolumnno - 1);
				index = allcells.size() - 1;
			}
			selectedbox = index;
		}
		if(iscolumnselected) {
			iscolumnselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_F2) {
		strflag = allcells.at(selectedbox).cellcontent;
		textbox.cleanText();
		createTextBox();
		istextboxactive = true;
		textbox.setEditable(true);
	}
	else if(key == G_KEY_LEFT_CONTROL || key == G_KEY_RIGHT_CONTROL) ctrlpressed = false;
	else if(key == G_KEY_LEFT_SHIFT || key == G_KEY_RIGHT_SHIFT) shiftpressed = false;
}

void gGUIGrid::charPressed(unsigned int codepoint) {
	if(istextboxactive) textbox.charPressed(codepoint);
}

void gGUIGrid::mouseScrolled(int x, int y) {
	gGUIScrollable::mouseScrolled(x, y);
}
