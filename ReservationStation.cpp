#include "ReservationStation.h"
#include "CommonDataBus.h"

void ReservationStationLine::Reset() {
	busy = 0;
	Op = "";
	Vj = "";
	Vk = "";
	valueString = "";
	remainingTime = -1;
}

ReservationStationLine::ReservationStationLine(string name) {
	Reset();
	unitName = name;
}

int ReservationStationLine::IsBusy() {
	return busy;
}

string ReservationStationLine::GetName() {
	return unitName;
}

string ReservationStationLine::GetOp() {
	return Op;
}

string ReservationStationLine::GetVj() {
	return Vj;
}
string ReservationStationLine::GetVk() {
	return Vk;
}

int ReservationStationLine::GetRemainingTime() {
	return remainingTime;
}

void ReservationStationLine::SetExecute() {
	remainingTime = ExecTime.at(Op);
}

float ReservationStationLine::GetArrivedTime() {
	return arrivedTime;
}

void ReservationStationLine::SetRSLine(string instOp, string instVj, string instVk, float arrived) {
	busy = 1;
	Op = instOp;
	Vj = instVj;
	Vk = instVk;
	arrivedTime = arrived;
}

bool ReservationStationLine::isReady() {
	return checkReady({ Vj, Vk });
}

void ReservationStationLine::ReceiveData(string unitName, string value) {
	if (Vj == unitName) Vj = value;
	if (Vk == unitName) Vk = value;
}

void ReservationStationLine::Tick() {
	// 过一个时钟
	if (remainingTime < 0)return;
	remainingTime--;
	if (remainingTime == 0) {
		valueString = Vj + " " + OperatorSign.at(Op) + " " + Vk;
		CommonDataBus::TransformValue(GetName(), valueString);
	}
	if (remainingTime == -1) {
		// 已经将值写到ROB里了，所以重置这个模块
		Reset();
	}
}

ReservationStationADD::ReservationStationADD() {
	tail = ADDNUM - 1; //这样定义循环队列保证第一次访问到第0行
	for (int i = 0; i < ADDNUM; i++) {
		string s = "Add";
		s += (char)(i + 1 + '0');
		addModule[i] = new ReservationStationLine(s);
	}
}

int ReservationStationADD::IsFree() {
	int head = tail;
	tail += 1;
	tail %= ADDNUM;
	while (tail != head) {
		if (!addModule[tail]->IsBusy()) {
			//如果有空位，就可以直接插入
			return tail;
		}
		tail += 1;
		tail %= ADDNUM;
	}
	return -1;
}

string ReservationStationADD::AddIssue(string instOp, string instVj, string instVk, float arrived) {
	int index = IsFree();
	if (index != -1) {
		addModule[index]->SetRSLine(instOp, instVj, instVk, arrived);
		return addModule[index]->GetName();
	}
	else return "";
}


void ReservationStationADD::Tick() {
	for (int i = 0; i < ADDNUM; i++) {
		if (addModule[i]->IsBusy()) {
			addModule[i]->Tick();
		}
	}

	int busyUnit = 0;
	for (int i = 0; i < ADDNUM; i++) {
		if (addModule[i]->GetRemainingTime() >= 0) busyUnit++;
	}
	for (int i = 0; i < ADDERNUM - busyUnit; i++) {
		int earliestArrivedTime = 10007;
		int earliestIndex = -1;
		for (int j = 0; j < ADDNUM; j++) {
			if (addModule[j]->IsBusy() && addModule[j]->GetRemainingTime() == -1 && addModule[j]->isReady()) {
				if (addModule[j]->GetRemainingTime() < earliestArrivedTime) {
					earliestArrivedTime = addModule[j]->GetRemainingTime();
					earliestIndex = j;
				}
			}
		}
		if (earliestIndex != -1) {
			addModule[earliestIndex]->SetExecute();
		}
	}
}

void ReservationStationADD::ReceiveData(string unitName, string value) {
	for (int i = 0; i < ADDNUM; i++) {
		addModule[i]->ReceiveData(unitName, value);
	}
}

bool ReservationStationADD::isAllFree() {
	for (int i = 0; i < ADDNUM; i++) {
		if (addModule[i]->IsBusy()) return false;
	}
	return true;
}

void ReservationStationADD::InsertOutput(vector<string>& table) {
	printHeader("Reservation Stations", 124);

	std::cout << "|" << centerString("Line", 10)
		<< "|" << centerString("Busy", 6)
		<< "|" << centerString("Op", 6)
		<< "|" << centerString("Vj", 42)
		<< "|" << centerString("Vk", 42)
		<< "|" << centerString("Remaining", 11) << "|\n";
	std::cout << std::string(124, '-') << "\n";

	for (int i = 0; i < ADDNUM; ++i) {
		std::cout << "|" << centerString(addModule[i]->GetName(), 10)
			<< "|" << centerString(std::to_string(addModule[i]->IsBusy()), 6)
			<< "|" << centerString(addModule[i]->GetOp(), 6)
			<< "|" << centerString(addModule[i]->GetVj(), 42)
			<< "|" << centerString(addModule[i]->GetVk(), 42)
			<< "|" << centerString(std::to_string(addModule[i]->GetRemainingTime()), 11) << "|\n";
	}

	std::cout << std::string(124, '-') << endl;
	cout << endl;
}



ReservationStationMULT::ReservationStationMULT() {
	tail = MULTNUM - 1; //这样定义循环队列保证第一次访问到第0行 
	for (int i = 0; i < MULTNUM; i++) {
		string s = "Mult";
		s += (char)(i + 1 + '0');
		multModule[i] = new ReservationStationLine(s);
	}
}

int ReservationStationMULT::IsFree() {
	int head = tail;
	tail += 1;
	tail %= MULTNUM;
	while (tail != head) {
		if (!multModule[tail]->IsBusy()) {
			//如果有空位，就可以直接插入
			return tail;
		}
		tail += 1;
		tail %= MULTNUM;
	}
	return -1;
}

string ReservationStationMULT::MultIssue(string instOp, string instVj, string instVk, float arrived) {
	int index = IsFree();
	if (index != -1) {
		multModule[index]->SetRSLine(instOp, instVj, instVk, arrived);
		return multModule[index]->GetName();
	}
	else return "";
}


void ReservationStationMULT::Tick() {
	for (int i = 0; i < MULTNUM; i++) {
		if (multModule[i]->IsBusy()) {
			multModule[i]->Tick();
		}
	}

	int busyUnit = 0;
	for (int i = 0; i < MULTNUM; i++) {
		if (multModule[i]->GetRemainingTime() >= 0) busyUnit++;
	}
	for (int i = 0; i < MULTIPLIERNUM - busyUnit; i++) {
		int earliestArrivedTime = 10007;
		int earliestIndex = -1;
		for (int j = 0; j < MULTNUM; j++) {
			if (multModule[j]->IsBusy() && multModule[j]->GetRemainingTime() == -1 && multModule[j]->isReady()) {
				if (multModule[j]->GetRemainingTime() < earliestArrivedTime) {
					earliestArrivedTime = multModule[j]->GetRemainingTime();
					earliestIndex = j;
				}
			}
		}
		if (earliestIndex != -1) {
			multModule[earliestIndex]->SetExecute();
		}
	}
}

void ReservationStationMULT::ReceiveData(string unitName, string value) {
	for (int i = 0; i < MULTNUM; i++) {
		multModule[i]->ReceiveData(unitName, value);
	}
}

bool ReservationStationMULT::isAllFree() {
	for (int i = 0; i < MULTNUM; i++) {
		if (multModule[i]->IsBusy()) return false;
	}
	return true;
}

void ReservationStationMULT::InsertOutput(vector<string>& table) {
	printHeader("Reservation Stations", 124);

	std::cout << "|" << centerString("Line", 10)
		<< "|" << centerString("Busy", 6)
		<< "|" << centerString("Op", 6)
		<< "|" << centerString("Vj", 42)
		<< "|" << centerString("Vk", 42)
		<< "|" << centerString("Remaining", 11) << "|\n";
	std::cout << std::string(124, '-') << "\n";

	for (int i = 0; i < MULTNUM; ++i) {
		std::cout << "|" << centerString(multModule[i]->GetName(), 10)
			<< "|" << centerString(std::to_string(multModule[i]->IsBusy()), 6)
			<< "|" << centerString(multModule[i]->GetOp(), 6)
			<< "|" << centerString(multModule[i]->GetVj(), 42)
			<< "|" << centerString(multModule[i]->GetVk(), 42)
			<< "|" << centerString(std::to_string(multModule[i]->GetRemainingTime()), 11) << "|\n";
	}

	std::cout << std::string(124, '-') << endl;
	cout << endl;
}