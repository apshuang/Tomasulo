#include "ReorderBuffer.h"


void ReorderBufferLine::Reset() {
	busy = 0;
	instructionType = "";
	instuctionDestination = "";
	instructionOperand1 = "";
	instructionOperand2 = "";
	state = FREE;
	moduleNum = -1;
	value = 0;
	valueString = "";
}

ReorderBufferLine::ReorderBufferLine() {
	Reset();
}

void ReorderBufferLine::SetID(int ID) {
	id = ID;
}

int ReorderBufferLine::IsBusy() {
	return busy;
}

int ReorderBufferLine::NameToNum(string RegName) {
	// ���ַ�����ʽ�ļĴ����Լ�ROB�ĳ�int��ʽ�ģ�����ֱ�Ӽ��ص���Ӧ����
	// ���罫R12���12����#2���2
	// ��Ϊ�ò�����ʵ��Ӧ���в������ڣ��������ǲ������ӵ�decoder��
	string regNumString = RegName.substr(1);
	int regNum = stoi(regNumString);
	return regNum;
}

string ReorderBufferLine::OffsetToString(int offset) {
	// ��������ʽ��offset�ĳ�string��ʽ��
	// ��Ϊ�ò�����ʵ��Ӧ���в������ڣ��������ǲ������ӵ�decoder��
	string result = "";
	if (offset == 0)return "0";
	while (offset) {
		result += (offset % 10) + '0';
		offset /= 10;
	}
	reverse(result.begin(), result.end());
	return result;
}

void ReorderBufferLine::Tick(TomasuloWithROB& tomasulo) {
	// ��ģ���Tickִֻ�з����Լ�����д�ؽ��
	int instModule;
	InstuctionDecoder decoder = tomasulo.instructionDecoder;
	switch (state) {
	case FREE:
		FetchAndIssue(tomasulo);
		break;
	case ISSUE:
		// ��ȡ�ö�Ӧ��ָ��֮�󣬸���ָ�����ͷ���ȥ��Ӧ��ģ�鴦
		instModule = decoder.GetInstructionType(instructionType);
		int result;
		switch (instModule) {
		case LOAD:
			// ��ָ���LoadBuffer��ִ��
			result = tomasulo.loadBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), id);
			moduleNum = result;
			if (result != -1) {
				state = EXEC;
				//�ɹ������Ҫ��destination�Ĵ�����Ϊbusy��������ֵ��Ϊ������ROB��Ŀ
				tomasulo.registers.SetBusy(1, NameToNum(instuctionDestination));
				tomasulo.registers.SetLineValue("#" + OffsetToString(id), NameToNum(instuctionDestination));
				tomasulo.registers.SetROBPosition(id, NameToNum(instuctionDestination));
			}
			else {
				// �������ֵΪ-1��˵��LoadBuffer�ڲ�û�п�λ������ִ��ʧ��
				// ִ��ʧ������״̬�仯����ô�´ν���������
			}
			break;
		case ADDER:
			// ��ָ���ADDERģ��ִ��
			if (moduleNum != -1)break; // ˵���Ѿ������ˣ�ֻ�ǲ�������δready
			result = tomasulo.reservationStationADD.AddExecute(instructionType, id);
			moduleNum = result;
			if (result != -1) {
				// ��һ��������
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand1))) {
					// �Ĵ�������æ������Ӧ���͵�Q������V
					tomasulo.reservationStationADD.SetQj(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1))), moduleNum);
				}
				else {
					// �Ĵ����Ѿ����ֳɵ�ֵ�ˣ�ֱ�ӷ��͵�V
					tomasulo.reservationStationADD.SetVj(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1)), moduleNum);
				}

				//�ڶ���������
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand2))) {
					tomasulo.reservationStationADD.SetQk(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2))), moduleNum);
				}
				else {
					tomasulo.reservationStationADD.SetVk(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2)), moduleNum);
				}

				//�ɹ������Ҫ��destination�Ĵ�����Ϊbusy��������ֵ��Ϊ������ROB��Ŀ
				tomasulo.registers.SetBusy(1, NameToNum(instuctionDestination));
				tomasulo.registers.SetLineValue("#" + OffsetToString(id), NameToNum(instuctionDestination));
				tomasulo.registers.SetROBPosition(id, NameToNum(instuctionDestination));
			}
			else {
				// �������ֵΪ-1��˵��LoadBuffer�ڲ�û�п�λ������ִ��ʧ��
				// ִ��ʧ������״̬�仯����ô�´ν���������
			}
			break;
		case MULTIPLIER:
			// ��ָ���ADDERģ��ִ��
			if (moduleNum != -1)break; // ˵���Ѿ������ˣ�ֻ�ǲ�������δready
			result = tomasulo.reservationStationMULT.MultExecute(instructionType, id);
			moduleNum = result;
			if (result != -1) {
				// ��һ��������
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand1))) {
					// �Ĵ�������æ������Ӧ���͵�Q������V
					tomasulo.reservationStationMULT.SetQj(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1))), moduleNum);
				}
				else {
					// �Ĵ����Ѿ����ֳɵ�ֵ�ˣ�ֱ�ӷ��͵�V
					tomasulo.reservationStationMULT.SetVj(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1)), moduleNum);
				}

				//�ڶ���������
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand2))) {
					tomasulo.reservationStationMULT.SetQk(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2))), moduleNum);
				}
				else {
					tomasulo.reservationStationMULT.SetVk(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2)), moduleNum);
				}

				//�ɹ������Ҫ��destination�Ĵ�����Ϊbusy��������ֵ��Ϊ������ROB��Ŀ
				tomasulo.registers.SetBusy(1, NameToNum(instuctionDestination));
				tomasulo.registers.SetLineValue("#" + OffsetToString(id), NameToNum(instuctionDestination));
				tomasulo.registers.SetROBPosition(id, NameToNum(instuctionDestination));
			}
			else {
				// �������ֵΪ-1��˵��LoadBuffer�ڲ�û�п�λ������ִ��ʧ��
				// ִ��ʧ������״̬�仯����ô�´ν���������
			}
			break;
		default:
			break;

		}
		break;
	case EXEC:
		// ����Ѿ��յ��˽��value���Ǿ�תΪWRITEģʽ
		if (valueString != "") {
			state = WRITE;
			// ��ֵת�����Ĵ��������ڸ���תΪcommit���ٽ��Ĵ���busyλ��0
			tomasulo.registers.SetLineValue(valueString, NameToNum(instuctionDestination));
		}
		// ��תΪWRITE֮���������ROB��Ŀ��ģ��Ϳ���ֱ��ȡ����
		// ����֮��valueд��ROB֮��ͻ�ֱ��ת��������ģ���ˣ���ΪROB����ִ��ʱ�ӣ�

	default:
		break;
	}

}

void ReorderBufferLine::FetchAndIssue(TomasuloWithROB& tomasulo) {
	// �����һ��decoder��ָ�룬�������ö�Ӧ�Ľ��뺯��
	InstuctionDecoder decoder = tomasulo.instructionDecoder;
	string s = "";
	cin >> s;
	if (s == "") {
		// �Ѿ�û���µ�ָ����
		tomasulo.reorderBuffer.BackTrack(); // ���û�ж���ָ�Ӧ�ý���λ�����ڳ�����Ҳ��tail����һ��
		return;
	}
	tomasulo.reorderBuffer.SetEmpty(0); // ������ָ���ROB�ڿ϶���Ϊ��
	instructionType = s;
	int operandNum = decoder.GetOperandNum(instructionType);
	switch (operandNum) {
	case 1:
		cin >> instuctionDestination;
		break;
	case 2:
		cin >> instuctionDestination;
		cin >> instructionOperand1;
		break;
	case 3:
		cin >> instuctionDestination;
		cin >> instructionOperand1;
		cin >> instructionOperand2;
		break;
	default:
		break;
	}
	state = ISSUE;
	busy = 1;
}

void ReorderBufferLine::WriteResult(string value) {
	valueString = value;
	state = WRITE;
}

string ReorderBufferLine::GetValue() {
	return valueString;
}

void ReorderBufferLine::Commit(TomasuloWithROB& tomasulo) {
	// ��Ϊ�ύ���޸�״̬�����ļĴ���busyλ
	state = COMMIT;

	// ��Ҫ��֤��ȷ���ǵ�ǰ��Ŀ�ƿ�����Ĵ��������ܹ��޸ļĴ�����״̬
	int t = tomasulo.registers.GetROBPosition(NameToNum(instuctionDestination));
	if (tomasulo.registers.GetROBPosition(NameToNum(instuctionDestination)) == id) {
		tomasulo.registers.SetBusy(0, NameToNum(instuctionDestination));
		tomasulo.registers.SetLineValue(valueString, NameToNum(instuctionDestination));
	}
}

int ReorderBufferLine::GetState() {
	return state;
}


ReorderBuffer::ReorderBuffer() {
	head = 0;
	tail = ENTRYNUM - 1; //��������ѭ�����б�֤��һ�η��ʵ���0��
	empty = 1;
	for (int i = 0; i < ENTRYNUM; i++) {
		entry[i] = ReorderBufferLine();
		entry[i].SetID(i);
	}
}

int ReorderBuffer::IsFree() {
	if (!IsEmpty() && IsFull()) {
		return -1; //��������
	}
	else {
		tail += 1;
		tail %= ENTRYNUM;
		return tail;
	}
}

int ReorderBuffer::IsEmpty() {
	return empty;
}

int ReorderBuffer::IsFull() {
	if (head == ((tail + 1) % ENTRYNUM)) return 1;
	return 0;
}

void ReorderBuffer::SetEmpty(int emptyValue) {
	empty = emptyValue;
}

void ReorderBuffer::WriteResult(int entryLine, string value) {
	entry[entryLine].WriteResult(value);
}

void ReorderBuffer::Tick(TomasuloWithROB& tomasulo) {
	// �Ƚ���ǰ���ROB�ύ�ˣ�������Ѿ���WRITE״̬��
	if (entry[head].GetState() == WRITE) {
		entry[head].Commit(tomasulo);
		head += 1;
		head %= ENTRYNUM;
	}


	int index = head;
	while (!IsEmpty() && index != ((tail + 1) % ENTRYNUM)) {
		// ����Ҫ��Ϊ��
		if (entry[index].IsBusy()) {
			entry[index].Tick(tomasulo);
		}
		index++;
		index %= ENTRYNUM;
	}
	int index_free = IsFree();
	if (index_free != -1) {
		entry[index_free].Tick(tomasulo); // ���Զ����µ�ָ��

	}
}

void ReorderBuffer::BackTrack() {
	// ���û�ж���ָ�Ӧ�ý���λ�����ڳ�����Ҳ��tail����һ��
	tail += ENTRYNUM - 1;
	tail %= ENTRYNUM;
}

string ReorderBuffer::GetValue(int entryLine) {
	return entry[entryLine].GetValue();
}




InstuctionDecoder::InstuctionDecoder() {
	OperandNum.clear();
	InstructionModule.clear();

	//�ڱ�ʵ���У�ȫ����������������ָ��
	for (auto Instruction : OperandNum3Instuction) {
		OperandNum[Instruction] = 3;
	}

	InstructionModule["LD"] = LOAD; //Loadָ������Ϊ1
	InstructionModule["SD"] = STORE; //Storeָ������Ϊ2
	InstructionModule["ADDD"] = ADDER;
	InstructionModule["SUBD"] = ADDER; // ����ADDģ���ָ������Ϊ3
	InstructionModule["MULTD"] = MULTIPLIER;
	InstructionModule["DIVD"] = MULTIPLIER; // ����MULTģ���ָ������Ϊ4
}

int InstuctionDecoder::GetOperandNum(string instructionType) {
	return OperandNum[instructionType];
}
int InstuctionDecoder::GetInstructionType(string instructionType) {
	return InstructionModule[instructionType];
}
int InstuctionDecoder::GetOffset(string instructionOperand) {
	return stoi(instructionOperand);
}


TomasuloWithROB::TomasuloWithROB() {
	reorderBuffer = ReorderBuffer();
	loadBuffer = LoadBuffer();
	reservationStationADD = ReservationStationADD();
	reservationStationMULT = ReservationStationMULT();
	registers = Registers();
	instructionDecoder = InstuctionDecoder();
	cycle = 0;
}

void TomasuloWithROB::Tick() {
	reorderBuffer.Tick(*this);
	loadBuffer.Tick(*this);
	reservationStationADD.Tick(*this);
	reservationStationMULT.Tick(*this);
}