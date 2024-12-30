#include "InstructionDecoder.h"

InstructionDecoder::InstructionDecoder(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult) {
	operandNum.clear();
	instructionModule.clear();
	instructions.clear();
	index = 0;
	labelMap.clear();

	integerRegisters = intRegs;
	floatRegisters = floatRegs;
	loadBuffer = LDBuffer;
	storeBuffer = storeBuffer;
	reservationAdd = RSAdd;
	reservationMult = RSMult;

	//�ڱ�ʵ���У�ȫ����������������ָ��
	for (auto Instruction : OperandNum3Instuction) {
		operandNum[Instruction] = 3;
	}

	instructionModule["LD"] = LOAD; //Loadָ������Ϊ1
	instructionModule["fld"] = LOAD;
	instructionModule["SD"] = STORE; //Storeָ������Ϊ2
	instructionModule["ADDD"] = ADDER; // ����ADDģ���ָ������Ϊ3
	instructionModule["fadd.d"] = ADDER;
	instructionModule["SUBD"] = ADDER; 
	instructionModule["fsub.d"] = ADDER; 
	instructionModule["MULTD"] = MULTIPLIER; // ����MULTģ���ָ������Ϊ4
	instructionModule["fmul.d"] = MULTIPLIER;
	instructionModule["DIVD"] = MULTIPLIER;
	instructionModule["fdiv.d"] = MULTIPLIER;

	while (1) {
		string s = "";
		getline(cin, s);
		if (s == "")break;  // �Ѿ���������ָ����
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
	// ���ĳ����ֵ�ǼĴ������Ǳ����Ƿ���"F1��x2��R3"���ָ�ʽ
	char label = originValue[0];
	label = tolower(label);
	int index = stoi(originValue.substr(1));
	if (label == 'f' || label == 'r') {
		// ����Ĵ���
		return floatRegisters->GetLineValue(index);
	}
	else if (label == 'x') {
		return integerRegisters->GetLineValue(index);
	}
	else return originValue;  // ����ֱ�ӷ���
}

void InstructionDecoder::SetRegisterValue(string registerName, string funcUnit) {
	char label = registerName[0];
	label = tolower(label);
	int index = stoi(registerName.substr(1));
	if (label == 'f' || label == 'r') {
		// ����Ĵ���
		floatRegisters->SetLineValue(funcUnit, index);
	}
	else if (label == 'x') {
		integerRegisters->SetLineValue(funcUnit, index);
	}
	else {
		cout << "��Ӧ���г�����Ϊָ���dest��" << endl;
		abort();
	}
}

bool InstructionDecoder::ParseLoadAndStore(string opcode, string operands, float cycle) {
	string dest;
	string destValue;

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

	if (instructionModule[opcode] == LOAD) {
		string unitName = loadBuffer->LoadIssue(base, offset, cycle);
		if (unitName == "")return false;
		SetRegisterValue(dest, unitName);  // ��������
	}
	else {
		string unitName = storeBuffer->StoreIssue(base, offset, dest, cycle);
		if (unitName == "")return false;
	}
	return true;
}

bool InstructionDecoder::ParseAddAndMult(string opcode, string operands, float cycle) {
	string dest;

	size_t commaPos = operands.find(",");
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

	if (instructionModule[opcode] == ADDER) {
		string unitName = reservationAdd->AddIssue(opcode, src1, src2, cycle);
		if (unitName == "")return false;
		SetRegisterValue(dest, unitName);
	}
	else {
		string unitName = reservationMult->MultIssue(opcode, src1, src2, cycle);
		if (unitName == "")return false;
		SetRegisterValue(dest, unitName);
	}
	return true;
}

void InstructionDecoder::Tick(int cycle) {
	/*
	��ע�⣡��decoderֻ�ܴ�����ϸ�ʽ��inst����ʽ���£�
	(label:)    opcode operands  (// comments)
	���������ڵ�label��comments���ǿ�ѡ��
	��operands����ָ�����ģ�����addָ����"dest,src1,src2"����ldָ����"dest,offset(base)"�����parse��������ȥ����
	*/

	for (int i = 0; i < ISSUENUM; i++) {
		float exactCycle = cycle + (float)i / ISSUENUM;
		if (index >= instructions.size()) return;  // �Ѷ�������ָ��
		string inst = instructions[index];

		// ���������ĩβ��ע���Լ�ͷβ�Ŀ��ַ�
		size_t commentPos = inst.find("//");
		if (commentPos != std::string::npos) {
			inst = inst.substr(0, commentPos);
		}
		inst.erase(0, inst.find_first_not_of(" \t"));
		inst.erase(inst.find_last_not_of(" \t") + 1);

		// ��label������еĻ���mark����
		size_t labelPos = inst.find(":");
		if (labelPos != std::string::npos) {
			labelMap[inst.substr(0, labelPos)] = index;
			inst = inst.substr(labelPos + 1);
			inst.erase(0, inst.find_first_not_of(" \t"));
		}

		// ����opcode�����ʱ��ʣ�µ�inst��������"opcode operands"�ĸ�ʽ��
		string opcode;
		string operands;
		size_t opcodePos = inst.find(" ");
		if (opcodePos != std::string::npos) {
			opcode = inst.substr(0, opcodePos);
			operands = inst.substr(opcodePos + 1);
		}

		if (instructionModule.count(opcode)) {
			if (instructionModule[opcode] == LOAD || instructionModule[opcode] == STORE) {
				// LD/SD��operands�ĸ�ʽΪ"dest,offset(base)"
				if (ParseLoadAndStore(opcode, operands, exactCycle)) index++;
				else {
					// ��ǰָ���ʧ�ܣ���˵���������޷���������ָ���ֱ������ѭ��
					break;
				}
			}
			else if (instructionModule[opcode] == ADDER || instructionModule[opcode] == MULTIPLIER) {
				// ADDER��MULTIPLIER��operands�ĸ�ʽΪ"dest,src1,src2"
				if (ParseAddAndMult(opcode, operands, exactCycle)) index++;
				else {
					// ��ǰָ���ʧ�ܣ���˵���������޷���������ָ���ֱ������ѭ��
					break;
				}
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