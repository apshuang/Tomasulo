#include "ReservationStation.h"
#include "ReorderBuffer.h"

void ReservationStationLine::Reset() {
	busy = 0;
	Op = "";
	Vj = 0;
	Vk = 0;
	Vj2 = "";
	Vk2 = "";
	Qj = -1;
	Qk = -1;
	value = 0;
	valueString = "";
	state = FREE;
	destination = -1;
	remainingTime = -1;
}

void ReservationStationLine::WriteBack(TomasuloWithROB& tomasulo) {
	tomasulo.reorderBuffer.WriteResult(destination, valueString);
}

ReservationStationLine::ReservationStationLine() {
	Reset();
}

int ReservationStationLine::IsBusy() {
	return busy;
}

void ReservationStationLine::SetRSLine(string instOp, int instDest) {
	Op = instOp;
	destination = instDest;
	busy = 1;
	remainingTime = ExecTime.at(Op) + 1;
}

void ReservationStationLine::SetVj(string instVj) {
	Vj2 = instVj;
}

void ReservationStationLine::SetVk(string instVk) {
	Vk2 = instVk;
}

void ReservationStationLine::SetQj(int instQj) {
	Qj = instQj;
}

void ReservationStationLine::SetQk(int instQk) {
	Qk = instQk;
}

void ReservationStationLine::Tick(TomasuloWithROB& tomasulo) {
	// 过一个时钟
	if (Qj != -1) {
		Vj2 = tomasulo.reorderBuffer.GetValue(Qj);
		if (Vj2 != "")Qj = -1;
	}
	if (Qk != -1) {
		Vk2 = tomasulo.reorderBuffer.GetValue(Qk);
		if (Vj2 != "")Qk = -1;
	}

	if (Vj2 != "" && Vk2 != "") {
		// 说明操作数已ready
		if (remainingTime > 0) {
			tomasulo.reorderBuffer.SetExecState(destination);
		}
		remainingTime--;
	}
	else return;
	if (remainingTime == 0) {
		valueString = Vj2 + " " + OperatorSign.at(Op) + " " + Vk2;
		WriteBack(tomasulo);
	}
	if (remainingTime == -1) {
		// 已经将值写到ROB里了，所以重置这个模块
		Reset();
	}
}

void ReservationStationLine::InsertOutput(vector<string>& table, int id, int moduleDistinguish) {
	string line;
	line += (moduleDistinguish == 0 ? (string)"Add" : (string)"Mult") + ((char)(id + 1 + '0')) + (string)" : " + (busy ? (string)"Yes" : (string)"No");
	if (Op == "") {
		line += (string)",,,,,;";
		table.push_back(line);
		return;
	}
	line += (string)"," + Op + (string)"," + Vj2 + (string)"," + Vk2 + (string)",";
	if (Qj != -1) {
		line += (string)"#" + ((char)(Qj + 1 + '0'));
	}
	line += (string)",";
	if (Qk != -1) {
		line += (string)"#" + ((char)(Qk + 1 + '0'));
	}
	line += (string)"," + '#' + ((char)(destination + 1 + '0')) + ';';
	table.push_back(line);
}

ReservationStationADD::ReservationStationADD() {
	tail = ADDNUM - 1; //这样定义循环队列保证第一次访问到第0行
	for (int i = 0; i < ADDNUM; i++) {
		addModule[i] = ReservationStationLine();
	}
}

int ReservationStationADD::IsFree() {
	int head = tail;
	tail += 1;
	tail %= ADDNUM;
	while (tail != head) {
		if (!addModule[tail].IsBusy()) {
			//如果有空位，就可以直接插入
			return tail;
		}
		tail += 1;
		tail %= ADDNUM;
	}
	return -1;
}

int ReservationStationADD::AddExecute(string instOp, int instDest) {
	int index = IsFree();
	if (index != -1) {
		addModule[index].SetRSLine(instOp, instDest);
		return index;
	}
	else return -1;
}

void ReservationStationADD::SetVj(string instVj, int moduleNum) {
	addModule[moduleNum].SetVj(instVj);
}

void ReservationStationADD::SetVk(string instVk, int moduleNum) {
	addModule[moduleNum].SetVk(instVk);
}

void ReservationStationADD::SetQj(int instQj, int moduleNum) {
	addModule[moduleNum].SetQj(instQj);
}

void ReservationStationADD::SetQk(int instQk, int moduleNum) {
	addModule[moduleNum].SetQk(instQk);
}

void ReservationStationADD::Tick(TomasuloWithROB& tomasulo) {
	for (int i = 0; i < ADDNUM; i++) {
		if (addModule[i].IsBusy()) {
			addModule[i].Tick(tomasulo);
		}
	}
}

void ReservationStationADD::InsertOutput(vector<string>& table) {
	for (int i = 0; i < ADDNUM; i++) {
		addModule[i].InsertOutput(table, i, 0);
	}
}



ReservationStationMULT::ReservationStationMULT() {
	tail = MULTNUM - 1; //这样定义循环队列保证第一次访问到第0行 
	for (int i = 0; i < MULTNUM; i++) {
		multModule[i] = ReservationStationLine();
	}
}

int ReservationStationMULT::IsFree() {
	int head = tail;
	tail += 1;
	tail %= MULTNUM;
	while (tail != head) {
		if (!multModule[tail].IsBusy()) {
			//如果有空位，就可以直接插入
			return tail;
		}
		tail += 1;
		tail %= MULTNUM;
	}
	return -1;
}

int ReservationStationMULT::MultExecute(string instOp, int instDest) {
	int index = IsFree();
	if (index != -1) {
		multModule[index].SetRSLine(instOp, instDest);
		return index;
	}
	else return -1;
}

void ReservationStationMULT::SetVj(string instVj, int moduleNum) {
	multModule[moduleNum].SetVj(instVj);
}

void ReservationStationMULT::SetVk(string instVk, int moduleNum) {
	multModule[moduleNum].SetVk(instVk);
}

void ReservationStationMULT::SetQj(int instQj, int moduleNum) {
	multModule[moduleNum].SetQj(instQj);
}

void ReservationStationMULT::SetQk(int instQk, int moduleNum) {
	multModule[moduleNum].SetQk(instQk);
}

void ReservationStationMULT::Tick(TomasuloWithROB& tomasulo) {
	for (int i = 0; i < MULTNUM; i++) {
		if (multModule[i].IsBusy()) {
			multModule[i].Tick(tomasulo);
		}
	}
}

void ReservationStationMULT::InsertOutput(vector<string>& table) {
	for (int i = 0; i < MULTNUM; i++) {
		multModule[i].InsertOutput(table, i, 1);
	}
}