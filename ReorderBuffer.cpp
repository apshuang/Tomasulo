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
	// 将字符串形式的寄存器以及ROB改成int形式的，方便直接加载到对应的行
	// 比如将R12变成12，把#2变成2
	// 因为该部分在实际应用中并不存在，所以我们不将它加到decoder中
	string regNumString = RegName.substr(1);
	int regNum = stoi(regNumString);
	return regNum;
}

string ReorderBufferLine::OffsetToString(int offset) {
	// 将数字形式的offset改成string形式的
	// 因为该部分在实际应用中并不存在，所以我们不将它加到decoder中
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
	// 该模块的Tick只执行发射以及接收写回结果
	int instModule;
	InstuctionDecoder decoder = tomasulo.instructionDecoder;
	switch (state) {
	case FREE:
		FetchAndIssue(tomasulo);
		break;
	case COMMIT:
		FetchAndIssue(tomasulo);
		break;
	case ISSUE:
		// 在取得对应的指令之后，根据指令类型发射去对应的模块处
		instModule = decoder.GetInstructionType(instructionType);
		int result;
		switch (instModule) {
		case LOAD:
			// 将指令发到LoadBuffer处执行
			result = tomasulo.loadBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), id);
			moduleNum = result;
			if (result != -1) {
				state = EXEC;
				//成功发射后，要将destination寄存器置为busy，并将其值设为依赖本ROB条目
				tomasulo.registers.SetBusy(1, NameToNum(instuctionDestination));
				tomasulo.registers.SetLineValue("#" + OffsetToString(id), NameToNum(instuctionDestination));
				tomasulo.registers.SetROBPosition(id, NameToNum(instuctionDestination));
			}
			else {
				// 如果返回值为-1，说明LoadBuffer内部没有空位，所以执行失败
				// 执行失败则不做状态变化，那么下次将继续尝试
			}
			break;
		case ADDER:
			// 将指令发到ADDER模块执行
			if (moduleNum != -1)break; // 说明已经发射了，只是操作数还未ready
			result = tomasulo.reservationStationADD.AddExecute(instructionType, id);
			moduleNum = result;
			if (result != -1) {
				// 第一个操作数
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand1))) {
					// 寄存器还在忙，所以应发送到Q而不是V
					tomasulo.reservationStationADD.SetQj(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1))), moduleNum);
				}
				else {
					// 寄存器已经有现成的值了，直接发送到V
					tomasulo.reservationStationADD.SetVj(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1)), moduleNum);
				}

				//第二个操作数
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand2))) {
					tomasulo.reservationStationADD.SetQk(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2))), moduleNum);
				}
				else {
					tomasulo.reservationStationADD.SetVk(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2)), moduleNum);
				}

				//成功发射后，要将destination寄存器置为busy，并将其值设为依赖本ROB条目
				tomasulo.registers.SetBusy(1, NameToNum(instuctionDestination));
				tomasulo.registers.SetLineValue("#" + OffsetToString(id), NameToNum(instuctionDestination));
				tomasulo.registers.SetROBPosition(id, NameToNum(instuctionDestination));
			}
			else {
				// 如果返回值为-1，说明LoadBuffer内部没有空位，所以执行失败
				// 执行失败则不做状态变化，那么下次将继续尝试
			}
			break;
		case MULTIPLIER:
			// 将指令发到ADDER模块执行
			if (moduleNum != -1)break; // 说明已经发射了，只是操作数还未ready
			result = tomasulo.reservationStationMULT.MultExecute(instructionType, id);
			moduleNum = result;
			if (result != -1) {
				// 第一个操作数
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand1))) {
					// 寄存器还在忙，所以应发送到Q而不是V
					tomasulo.reservationStationMULT.SetQj(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1))), moduleNum);
				}
				else {
					// 寄存器已经有现成的值了，直接发送到V
					tomasulo.reservationStationMULT.SetVj(tomasulo.registers.GetLineValue(NameToNum(instructionOperand1)), moduleNum);
				}

				//第二个操作数
				if (tomasulo.registers.IsBusy(NameToNum(instructionOperand2))) {
					tomasulo.reservationStationMULT.SetQk(NameToNum(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2))), moduleNum);
				}
				else {
					tomasulo.reservationStationMULT.SetVk(tomasulo.registers.GetLineValue(NameToNum(instructionOperand2)), moduleNum);
				}

				//成功发射后，要将destination寄存器置为busy，并将其值设为依赖本ROB条目
				tomasulo.registers.SetBusy(1, NameToNum(instuctionDestination));
				tomasulo.registers.SetLineValue("#" + OffsetToString(id), NameToNum(instuctionDestination));
				tomasulo.registers.SetROBPosition(id, NameToNum(instuctionDestination));
			}
			else {
				// 如果返回值为-1，说明LoadBuffer内部没有空位，所以执行失败
				// 执行失败则不做状态变化，那么下次将继续尝试
			}
			break;
		case STORE:
			if (moduleNum != -1)break; // 说明已经发射了，只是操作数还未ready
			result = tomasulo.storeBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), tomasulo.registers.GetLineValue(NameToNum(instuctionDestination)), id);
			moduleNum = result;
			break;
		default:
			break;

		}
		break;
	case EXEC:
		// 如果已经收到了结果value，那就转为WRITE模式
		if (valueString != "") {
			state = WRITE;
			// 将值转发到寄存器，但在该行转为commit后，再将寄存器busy位置0
			tomasulo.registers.SetLineValue(valueString, NameToNum(instuctionDestination));
		}
		// 在转为WRITE之后，依赖这个ROB条目的模块就可以直接取数了
		// 换言之，value写到ROB之后就会直接转发到其他模块了（因为ROB最先执行时钟）

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
	Reset();
	busy = 1;
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
	// 改为提交，修改状态，更改寄存器busy位
	state = COMMIT;
	busy = 0;
	// 需要验证，确保是当前条目掌控这个寄存器，才能够修改寄存器的状态
	int t = tomasulo.registers.GetROBPosition(NameToNum(instuctionDestination));
	if (tomasulo.registers.GetROBPosition(NameToNum(instuctionDestination)) == id) {
		tomasulo.registers.SetBusy(0, NameToNum(instuctionDestination));
		tomasulo.registers.SetLineValue(valueString, NameToNum(instuctionDestination));
	}
}

int ReorderBufferLine::GetState() {
	return state;
}

void ReorderBufferLine::InsertOutput(vector<string>& table) {
	string line;
	line += (string)"entry" + ((char)(id + 1 + '0')) + (string)" : " + (busy ? (string)"Yes" : (string)"No");
	if (instructionType == "") {
		line += (string)",,,,,;";
		table.push_back(line);
		return;
	}
	line += (string)"," + instructionType + ' ' + instuctionDestination + ' ' + instructionOperand1 + ' ' + instructionOperand2 + (string)",";
	line += StateOutput[state] + (string)"," + instuctionDestination + (string)"," + valueString + ';';
	table.push_back(line);
}

void ReorderBufferLine::SetExecState() {
	state = EXEC;
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
	// 先将最前面的ROB提交了（如果它已经是WRITE状态）
	if (entry[head].GetState() == WRITE) {
		entry[head].Commit(tomasulo);
		head += 1;
		head %= ENTRYNUM;
	}


	int index = head;
	while (!IsEmpty() && index != ((tail + 1) % ENTRYNUM)) {
		// 首先要求不为空
		if (entry[index].IsBusy()) {
			entry[index].Tick(tomasulo);
		}
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

string ReorderBuffer::GetValue(int entryLine) {
	return entry[entryLine].GetValue();
}

void ReorderBuffer::InsertOutput(vector<string>& table) {
	for (int i = 0; i < ENTRYNUM; i++) {
		entry[i].InsertOutput(table);
	}
}

int ReorderBuffer::CheckStop() {
	int stop = 1;
	for (int i = 0; i < ENTRYNUM; i++) {
		if (entry[i].IsBusy() || entry[i].GetState() != COMMIT) {
			stop = 0;
			break;
		}
	}
	return stop;
}

void ReorderBuffer::SetExecState(int entryLine) {
	entry[entryLine].SetExecState();
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
	storeBuffer = StoreBuffer();
	reservationStationADD = ReservationStationADD();
	reservationStationMULT = ReservationStationMULT();
	registers = Registers();
	instructionDecoder = InstuctionDecoder();
	outputTable.clear();
	cycle = 0;
}

int TomasuloWithROB::Tick() {
	if (reorderBuffer.CheckStop())return 1;
	reorderBuffer.Tick(*this);
	loadBuffer.Tick(*this);
	reservationStationADD.Tick(*this);
	reservationStationMULT.Tick(*this);
	storeBuffer.Tick(*this);
	GetOutput();
	return 0;
}

void TomasuloWithROB::GetOutput() {
	vector<string> table;
	reorderBuffer.InsertOutput(table);
	loadBuffer.InsertOutput(table);
	reservationStationADD.InsertOutput(table);
	reservationStationMULT.InsertOutput(table);
	registers.InsertOutput(table);
	outputTable.push_back(table);
}

void TomasuloWithROB::OutputAll() {
	int sameStart = 0; //如果是相同的，就置为非0
	int allSize = outputTable.size();
	for (int i = 0; i < allSize -1; i++) {
		int nowCheck = 1; //1代表是相同的，如果有不同的话再置为零即可
		for (int j = 0; j < outputTable[i].size(); j++) {
			if (outputTable[i][j] != outputTable[i + 1][j]) {
				nowCheck = 0;
				break;
			}
		}
		if (nowCheck == 1) {
			//如果是相同的，就置为起始的cycle
			if (sameStart == 0)sameStart = i + 1;
		}
		else {
			cout << "cycle_";
			if (sameStart) {
				cout << sameStart << "-";
			}
			cout << i + 1 << ":" << endl;
			for (int j = 0; j < outputTable[i].size(); j++) {
				cout << outputTable[i][j] << endl;
			}
			cout << endl << endl;
			sameStart = 0;
		}
	}
	cout << "cycle_";
	if (sameStart) {
		cout << sameStart << "-";
	}
	cout << allSize << ":" << endl;
	for (int j = 0; j < outputTable[allSize - 1].size(); j++) {
		cout << outputTable[allSize - 1][j] << endl;
	}
}

