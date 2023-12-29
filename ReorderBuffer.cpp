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
		// 在取得对应的指令之后，根据指令类型发射去对应的模块处
		instModule = decoder.GetInstructionType(instructionType);
		int result;
		switch (instModule) {
		case 1:
			result = tomasulo.loadBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), id);
			if (result != -1) {
				state = EXEC;
			}
			else {
				// 如果返回值为-1，说明LoadBuffer内部没有空位，所以执行失败
				// 执行失败则不做状态变化，那么下次将继续尝试
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
	// 需接受一个decoder的指针，用来调用对应的解码函数
	InstuctionDecoder decoder = tomasulo.instructionDecoder;
	string s = "";
	cin >> s;
	if (s == "") {
		// 已经没有新的指令了
		tomasulo.reorderBuffer.BackTrack(); // 如果没有读到指令，应该将空位重新腾出来，也即tail回退一格
		return;
	}
	tomasulo.reorderBuffer.SetEmpty(0); // 若读到指令，则ROB内肯定不为空
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
	tail = ENTRYNUM - 1; //这样定义循环队列保证第一次访问到第0行
	empty = 1;
	for (int i = 0; i < ENTRYNUM; i++) {
		entry[i] = ReorderBufferLine();
		entry[i].SetID(i);
	}
}

int ReorderBuffer::IsFree() {
	if (!IsEmpty() && IsFull()) {
		return -1; //队列已满
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
		entry[index_free].Tick(tomasulo); // 尝试读入新的指令

	}
}

void ReorderBuffer::BackTrack() {
	// 如果没有读到指令，应该将空位重新腾出来，也即tail回退一格
	tail += ENTRYNUM - 1;
	tail %= ENTRYNUM;
}


InstuctionDecoder::InstuctionDecoder() {
	OperandNum.clear();
	InstructionModule.clear();

	//在本实验中，全部都是三操作数的指令
	for (auto Instruction : OperandNum3Instuction) {
		OperandNum[Instruction] = 3;
	}

	InstructionModule["LD"] = LOAD; //Load指令类型为1
	InstructionModule["SD"] = STORE; //Store指令类型为2
	InstructionModule["ADDD"] = ADDER;
	InstructionModule["SUBD"] = ADDER; // 调用ADD模块的指令类型为3
	InstructionModule["MULTD"] = MULTIPLIER;
	InstructionModule["DIVD"] = MULTIPLIER; // 调用MULT模块的指令类型为4
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