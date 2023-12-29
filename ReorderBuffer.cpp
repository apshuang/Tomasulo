#include "ReorderBuffer.h"


void ReorderBufferLine::Reset() {
	busy = 0;
	instructionType = "";
	instuctionDestination = "";
	instructionOperand1 = "";
	instructionOperand2 = "";
	state = FREE;
	value = 0;
	valueString = "";
}

ReorderBufferLine::ReorderBufferLine() {
	Reset();
}
void ReorderBufferLine::SetID(int ID) {
	id = ID;
}
void ReorderBufferLine::Tick(TomasuloWithROB& tomasulo) {
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
		case 1:
			result = tomasulo.loadBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), id);
			if (result != -1) {
				state = EXEC;
			}
			else {
				// �������ֵΪ-1��˵��LoadBuffer�ڲ�û�п�λ������ִ��ʧ��
				// ִ��ʧ������״̬�仯����ô�´ν���������
			}
			break;
		default:
			break;

		}
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
}

void ReorderBufferLine::WriteResult(string value) {
	valueString = value;
	state = WRITE;
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
	int index = head;
	while (index != tail) {
		entry[index].Tick(tomasulo);
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
	loadBuffer.Tick();
	reorderBuffer.Tick(*this);
}