#include "InstructionDecoder.h"

InstructionDecoder::InstructionDecoder(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult) {
	operandNum.clear();
	instructionModule.clear();
	instructions.clear();
	index = 0;
	labelMap.clear();
	compareType = "";
	compareLeft = "";
	compareRight = "";
	branchLabel = "";

	integerRegisters = intRegs;
	floatRegisters = floatRegs;
	loadBuffer = LDBuffer;
	storeBuffer = SDBuffer;
	reservationAdd = RSAdd;
	reservationMult = RSMult;

	//在本实验中，全部都是三操作数的指令
	for (auto Instruction : OperandNum3Instuction) {
		operandNum[Instruction] = 3;
	}

	instructionModule["LD"] = LOAD; //Load指令类型为1
	instructionModule["fld"] = LOAD;
	instructionModule["ld"] = LOAD;
	instructionModule["SD"] = STORE; //Store指令类型为2
	instructionModule["sd"] = STORE;
	instructionModule["ADDD"] = ADDER; // 调用ADD模块的指令类型为3
	instructionModule["fadd.d"] = ADDER;
	instructionModule["addi"] = ADDER;
	instructionModule["SUBD"] = ADDER; 
	instructionModule["fsub.d"] = ADDER; 
	instructionModule["MULTD"] = MULTIPLIER; // 调用MULT模块的指令类型为4
	instructionModule["fmul.d"] = MULTIPLIER;
	instructionModule["DIVD"] = MULTIPLIER;
	instructionModule["fdiv.d"] = MULTIPLIER;
	instructionModule["bne"] = BRANCH;

	while (1) {
		string s = "";
		getline(cin, s);
		if (s == "")break;  // 已经读完所有指令了
		instructions.push_back(s);
	}

}

int InstructionDecoder::GetOperandNum(string instructionType) {
	return operandNum[instructionType];
}
int InstructionDecoder::GetInstructionType(string instructionType) {
	return instructionModule[instructionType];
}
int InstructionDecoder::GetOffset(string instructionOperand) {
	return stoi(instructionOperand);
}

string InstructionDecoder::GetRegisterValue(string originValue) {
	// 如果某字面值是寄存器，那必须是符合"F1、x2、R3"这种格式
	char label = originValue[0];
	label = tolower(label);
	if (label == 'f' || label == 'r') {
		// 浮点寄存器
		int index = stoi(originValue.substr(1));
		return floatRegisters->GetLineValue(index);
	}
	else if (label == 'x') {
		int index = stoi(originValue.substr(1));
		return integerRegisters->GetLineValue(index);
	}
	else return originValue;  // 常量直接返回
}

void InstructionDecoder::SetRegisterValue(string registerName, string funcUnit) {
	char label = registerName[0];
	label = tolower(label);
	int index = stoi(registerName.substr(1));
	if (label == 'f' || label == 'r') {
		// 浮点寄存器
		floatRegisters->SetLineValue(funcUnit, index);
	}
	else if (label == 'x') {
		integerRegisters->SetLineValue(funcUnit, index);
	}
	else {
		cout << "不应该有常量作为指令的dest！" << endl;
		abort();
	}
}

bool InstructionDecoder::ParseLoadAndStore(string opcode, string operands, float cycle) {
	string dest;
	string destValue;
	string originOperands = operands;
	size_t commaPos = operands.find(",");
	if (commaPos != std::string::npos) {
		dest = operands.substr(0, commaPos);
		operands = operands.substr(commaPos + 1);
		destValue = GetRegisterValue(dest);
	}

	string offset;
	size_t leftParenPos = operands.find("(");
	if (leftParenPos != std::string::npos) {
		offset = operands.substr(0, leftParenPos);
		operands = operands.substr(leftParenPos + 1);
		offset = GetRegisterValue(offset);
	}

	string base;
	size_t rightParenPos = operands.find(")");
	if (rightParenPos != std::string::npos) {
		base = operands.substr(0, rightParenPos);
		base = GetRegisterValue(base);
	}
	string unitName = "";
	if (instructionModule[opcode] == LOAD) {
		unitName = loadBuffer->LoadIssue(base, offset, cycle);
		if (unitName == "")return false;
		SetRegisterValue(dest, unitName);  // 设置依赖
	}
	else {
		unitName = storeBuffer->StoreIssue(base, offset, dest, cycle);
		if (unitName == "")return false;
	}

	string originInst = opcode + " " + originOperands;
	instMap[unitName] = instructionTime.size();
	instructionTime.push_back({ originInst, (int)cycle, -1 });

	return true;
}

bool InstructionDecoder::ParseAddAndMult(string opcode, string operands, float cycle) {
	string dest;

	size_t commaPos = operands.find(",");
	string originOperands = operands;
	if (commaPos != std::string::npos) {
		dest = operands.substr(0, commaPos);
		operands = operands.substr(commaPos + 1);
	}

	string src1, src2;
	commaPos = operands.find(",");
	if (commaPos != std::string::npos) {
		src1 = operands.substr(0, commaPos);
		src1 = GetRegisterValue(src1);
		src2 = operands.substr(commaPos + 1);
		src2 = GetRegisterValue(src2);
	}

	string unitName = "";
	if (instructionModule[opcode] == ADDER) {
		unitName = reservationAdd->AddIssue(opcode, src1, src2, cycle);
	}
	else {
		unitName = reservationMult->MultIssue(opcode, src1, src2, cycle);
	}

	if (unitName == "")return false;
	SetRegisterValue(dest, unitName);  // 设置寄存器依赖

	string originInst = opcode + " " + originOperands;
	instMap[unitName] = instructionTime.size();
	instructionTime.push_back({ originInst, (int)cycle, -1 });

	return true;
}

void InstructionDecoder::ParseBranch(string opcode, string operands, float cycle) {
	compareType = opcode;
	size_t commaPos = operands.find(",");
	string originOperands = operands;
	if (commaPos != std::string::npos) {
		compareLeft = operands.substr(0, commaPos);
		compareLeft = GetRegisterValue(compareLeft);
		operands = operands.substr(commaPos + 1);
	}

	commaPos = operands.find(",");
	if (commaPos != std::string::npos) {
		compareRight = operands.substr(0, commaPos);
		compareRight = GetRegisterValue(compareRight);
		branchLabel = operands.substr(commaPos + 1);
	}

	string originInst = opcode + " " + originOperands;
	instructionTime.push_back({ originInst, (int)cycle, -1 });
}

int InstructionDecoder::isBranch() {
	if(!checkReady({compareLeft, compareRight})) return -1;
	if (compareType == "bne") {
		if (compareLeft != compareRight) {
			return 1;
		}
		else return 0;
	}
	else {
		cout << "Unknown branch type" << endl;
		abort();
	}
}

void InstructionDecoder::ResetBranch() {
	compareType = "";
	compareLeft = "";
	compareRight = "";
	branchLabel = "";

}

void InstructionDecoder::Tick(int cycle) {
	/*
	请注意！该decoder只能处理符合格式的inst，格式如下：
	(label:)    opcode operands  (// comments)
	其中括号内的label和comments都是可选项
	而operands是因指令而异的，比如add指令是"dest,src1,src2"，而ld指令是"dest,offset(base)"，这个parse交给下游去进行
	*/
	for (int i = 0; i < ISSUENUM; i++) {
		if (branchLabel != "") {
			int branchResult = isBranch();
			if (branchResult == -1)return;  // 尚未准备好，阻塞
			else if (branchResult == 0) {
				instructionTime[instructionTime.size() - 1].executeComplete = cycle;
				ResetBranch();
				index++;
			}
			else if (branchResult == 1) {
				instructionTime[instructionTime.size() - 1].executeComplete = cycle;
				ResetBranch();
				index = labelMap[branchLabel];
			}
		}
		float exactCycle = cycle + (float)i / ISSUENUM;
		if (index >= instructions.size()) return;  // 已读完所有指令
		string inst = instructions[index];

		// 必须先清掉末尾的注释以及头尾的空字符
		size_t commentPos = inst.find("//");
		if (commentPos != std::string::npos) {
			inst = inst.substr(0, commentPos);
		}
		inst.erase(0, inst.find_first_not_of(" \t"));
		inst.erase(inst.find_last_not_of(" \t") + 1);

		// 找label，如果有的话就mark下来
		size_t labelPos = inst.find(":");
		if (labelPos != std::string::npos) {
			labelMap[inst.substr(0, labelPos)] = index;
			inst = inst.substr(labelPos + 1);
			inst.erase(0, inst.find_first_not_of(" \t"));
		}

		// 先找opcode（这个时候剩下的inst必须满足"opcode operands"的格式了
		string opcode;
		string operands;
		size_t opcodePos = inst.find(" ");
		if (opcodePos != std::string::npos) {
			opcode = inst.substr(0, opcodePos);
			operands = inst.substr(opcodePos + 1);
		}

		if (instructionModule.count(opcode)) {
			if (instructionModule[opcode] == LOAD || instructionModule[opcode] == STORE) {
				// LD/SD的operands的格式为"dest,offset(base)"
				if (ParseLoadAndStore(opcode, operands, exactCycle)) index++;
				else {
					// 当前指令发射失败，就说明该周期无法继续发射指令，故直接跳出循环
					break;
				}
			}
			else if (instructionModule[opcode] == ADDER || instructionModule[opcode] == MULTIPLIER) {
				// ADDER和MULTIPLIER的operands的格式为"dest,src1,src2"
				if (ParseAddAndMult(opcode, operands, exactCycle)) index++;
				else {
					// 当前指令发射失败，就说明该周期无法继续发射指令，故直接跳出循环
					break;
				}
			}
			else if (instructionModule[opcode] == BRANCH) {
				// BRANC的Hoperands的格式为"compareLeft,compareRight,label"
				ParseBranch(opcode, operands, cycle);
				
			}
		}
		else {
			cout << "You have not registered " << opcode << " instruction yet!!" << endl;
			abort();
		}
	}
}

bool InstructionDecoder::isAllFree() {
	return index >= instructions.size();
}

void InstructionDecoder::ReceiveData(string unitName, string value, int cycle) {
	if (instMap.count(unitName)) {
		int idx = instMap[unitName];
		instMap.erase(unitName);
		instructionTime[idx].executeComplete = cycle;
	}

	if (branchLabel != "") {
		// 必须当前是在阻塞状态的
		if (compareLeft == unitName) compareLeft = value;
		if (compareRight == unitName) compareRight = value;

	}
}


void InstructionDecoder::OutputInstructionTime() {
	const int tableWidth = 70;
	const int colWidth[] = { 20, 15, 15, 15 };
	printHeader("Instruction Complete Time", tableWidth);

	// 打印列标题
	cout << "|"
		<< centerString("Instruction", colWidth[0]) << "|"
		<< centerString("Issue", colWidth[1]) << "|"
		<< centerString("Execute", colWidth[2]) << "|"
		<< centerString("WriteBack", colWidth[3]) << "|\n";
	cout << string(tableWidth, '-') << "\n";

	// 打印指令数据
	for (const auto& instr : instructionTime) {
		cout << "|"
			<< centerString(instr.inst, colWidth[0]) << "|"
			<< centerString(to_string(instr.issueComplete), colWidth[1]) << "|"
			<< centerString(to_string(instr.executeComplete), colWidth[2]) << "|"
			<< centerString(to_string(instr.executeComplete + 1), colWidth[3]) << "|\n";
	}

	// 打印表尾线
	cout << string(tableWidth, '-') << "\n";
}