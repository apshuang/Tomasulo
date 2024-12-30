#include "BasicDefine.h"
#include "LoadBuffer.h"
#include "CommonDataBus.h"

void LoadBufferLine::Reset() {
	busy = 0;
	base = "";
	offset = "";
	valueString = "";
	remainingTime = -1;
	arrivedTime = 0;
}

LoadBufferLine::LoadBufferLine(string name) {
	Reset();	
	unitName = name;
}

string LoadBufferLine::GetName() {
	return unitName;
}

string LoadBufferLine::GetAddress() {
	if (busy) return offset + "(" + base + ")";
	else return "";
}

int LoadBufferLine::GetRemainingTime() {
	return remainingTime;
}

void LoadBufferLine::SetExecute() {
	remainingTime = LOADCYCLE;
}

float LoadBufferLine::GetArrivedTime() {
	return arrivedTime;
}

void LoadBufferLine::SetLoadBuffer(string instBase, string instOffset, float arrived) {
	// ReorderBuffer将指令发到此处，后续再查看是否已经准备好了，且有空闲的功能单元才能执行
	busy = 1;
	base = instBase;
	offset = instOffset;
	arrivedTime = arrived;
}

int LoadBufferLine::IsBusy() {
	return busy;
}

bool LoadBufferLine::isReady() {
	return checkReady({ base, offset });
}

void LoadBufferLine::Tick() {
	// 过一个时钟
	if (remainingTime < 0) return;
	remainingTime--;
	if (remainingTime == 0) {
		valueString = "Mem[" + offset + " + " + base + "]";
		CommonDataBus::TransformValue(GetName(), valueString);  // 在exec的最后传给CDB，CDB就会在下一个周期的开始，将这个result传给其它组件
	}
	if (remainingTime == -1 && busy) {
		// 已经将值传输到其它模块去了，所以可以reset掉
		Reset();
	}
}

void LoadBufferLine::ReceiveData(string unitName, string value) {
	if (base == unitName) base = value;
	if (offset == unitName) offset = value;
}

LoadBuffer::LoadBuffer() {
	tail = LOADNUM + 1; //这样定义循环队列保证第一次访问到第0行
	for (int i = 0; i < LOADNUM; i++) {
		string s = "Load";
		s += (char)(i + 1 + '0');
		loadbuffers[i] = new LoadBufferLine(s);
	}
}

int LoadBuffer::IsFree() {
	int head = tail;
	tail += 1;
	tail %= LOADNUM;
	while (tail != head) {
		if (!loadbuffers[tail]->IsBusy()) {
			//如果有空位，就可以直接插入
			return tail;
		}
		tail += 1;
		tail %= LOADNUM;
	}
	return -1;
}

string LoadBuffer::LoadIssue(string instBase, string instOffset, float arrived) {
	int index = IsFree();
	if (index != -1) {
		loadbuffers[index]->SetLoadBuffer(instBase, instOffset, arrived);
		return loadbuffers[index]->GetName();
	}
	else return "";
}

string LoadBuffer::GetName(int index) {
	return loadbuffers[index]->GetName();
}

void LoadBuffer::Tick() {
	for (int i = 0; i < LOADNUM; i++) {
		if (loadbuffers[i]->IsBusy()) {
			loadbuffers[i]->Tick();
		}
	}


	int busyUnit = 0;
	for (int i = 0; i < LOADNUM; i++) {
		if (loadbuffers[i]->GetRemainingTime() >= 0) busyUnit++;
	}
	for (int i = 0; i < LOADUNITNUM - busyUnit; i++) {
		int earliestArrivedTime = 10007;
		int earliestIndex = -1;
		for (int j = 0; j < LOADNUM; j++) {
			if (loadbuffers[j]->IsBusy() && loadbuffers[j]->GetRemainingTime() == -1 && loadbuffers[j]->isReady()) {
				if (loadbuffers[j]->GetRemainingTime() < earliestArrivedTime) {
					earliestArrivedTime = loadbuffers[j]->GetRemainingTime();
					earliestIndex = j;
				}
			}
		}
		if (earliestIndex != -1) {
			loadbuffers[earliestIndex]->SetExecute();
		}
	}
}

void LoadBuffer::ReceiveData(string unitName, string value) {
	for (int i = 0; i < LOADNUM; i++) {
		loadbuffers[i]->ReceiveData(unitName, value);
	}
}

bool LoadBuffer::isAllFree() {
	for (int i = 0; i < LOADNUM; i++) {
		if (loadbuffers[i]->IsBusy()) return false;
	}
	return true;
}

void LoadBuffer::InsertOutput(vector<string>& table) {
	printHeader("Load Buffers", 54);

	std::cout << "|" << centerString("Line", 12)
		<< "|" << centerString("Busy", 6)
		<< "|" << centerString("Address", 20) 
		<< "|" << centerString("Remaining", 11) << "|\n";
	std::cout << std::string(55, '-') << "\n";

	// 打印每一行的内容
	for (int i = 0; i < LOADNUM; ++i) {
		std::cout << "|" << centerString(loadbuffers[i]->GetName(), 12)
			<< "|" << centerString(std::to_string(loadbuffers[i]->IsBusy()), 6)
			<< "|" << centerString(loadbuffers[i]->GetAddress(), 20) 
			<< "|" << centerString(std::to_string(loadbuffers[i]->GetRemainingTime()), 11) << "|\n";
	}

	std::cout << std::string(55, '-') << "\n";  // 打印表格下边的分隔线
	cout << endl;
}


void StoreBufferLine::Reset() {
	busy = 0;
	base = "";
	offset = "";
	valueString = "";
	remainingTime = -1;
	arrivedTime = 0;
}

StoreBufferLine::StoreBufferLine(string name) {
	Reset();
	unitName = name;
}

string StoreBufferLine::GetName() {
	return unitName;
}

string StoreBufferLine::GetAddress() {
	if (busy) return offset + "(" + base + ")";
	else return "";
}

int StoreBufferLine::GetRemainingTime() {
	return remainingTime;
}

void StoreBufferLine::SetExecute() {
	remainingTime = STORECYCLE;
}

float StoreBufferLine::GetArrivedTime() {
	return arrivedTime;
}

void StoreBufferLine::SetStoreBuffer(string instBase, string instOffset, string instValue, float arrived) {
	// ReorderBuffer将指令发到此处，后续再查看是否已经准备好了，且有空闲的功能单元才能执行
	busy = 1;
	base = instBase;
	offset = instOffset;
	valueString = instValue;
	arrivedTime = arrived;
}

int StoreBufferLine::IsBusy() {
	return busy;
}

bool StoreBufferLine::isReady() {
	return checkReady({ base, offset, valueString });
}

void StoreBufferLine::Tick() {
	// 过一个时钟
	if (remainingTime < 0) return;
	remainingTime--;
	if (remainingTime == 0) {
		CommonDataBus::TransformValue(GetName(), valueString);  // 实际上不占用CDB，只是将这个消息告诉ROB（如果采用ROB）
	}
	if (remainingTime == -1) {
		Reset();
	}
}

void StoreBufferLine::ReceiveData(string unitName, string value) {
	if (base == unitName) base = value;
	if (offset == unitName) offset = value;
	if (valueString == unitName) valueString = unitName;
}

StoreBuffer::StoreBuffer() {
	tail = STORENUM + 1; //这样定义循环队列保证第一次访问到第0行
	for (int i = 0; i < STORENUM; i++) {
		string s = "Store";
		s += (char)(i + 1 + '0');
		storebuffers[i] = new StoreBufferLine(s);
	}
}

int StoreBuffer::IsFree() {
	int head = tail;
	tail += 1;
	tail %= STORENUM;
	while (tail != head) {
		if (!storebuffers[tail]->IsBusy()) {
			//如果有空位，就可以直接插入
			return tail;
		}
		tail += 1;
		tail %= STORENUM;
	}
	return -1;
}

string StoreBuffer::GetName(int index) {
	return storebuffers[index]->GetName();
}

string StoreBuffer::StoreIssue(string instBase, string instOffset, string instValue, float arrived) {
	int index = IsFree();
	if (index != -1) {
		storebuffers[index]->SetStoreBuffer(instBase, instOffset, instValue, arrived);
		return storebuffers[index]->GetName();
	}
	else return "";
}

void StoreBuffer::Tick() {
	for (int i = 0; i < LOADNUM; i++) {
		if (storebuffers[i]->IsBusy()) {
			storebuffers[i]->Tick();
		}
	}


	int busyUnit = 0;
	for (int i = 0; i < STORENUM; i++) {
		if (storebuffers[i]->GetRemainingTime() >= 0) busyUnit++;
	}
	for (int i = 0; i < STOREUNITNUM - busyUnit; i++) {
		int earliestArrivedTime = 10007;
		int earliestIndex = -1;
		for (int j = 0; j < STORENUM; j++) {
			if (storebuffers[j]->IsBusy() && storebuffers[j]->GetRemainingTime() == -1 && storebuffers[j]->isReady()) {
				if (storebuffers[j]->GetRemainingTime() < earliestArrivedTime) {
					earliestArrivedTime = storebuffers[j]->GetRemainingTime();
					earliestIndex = j;
				}
			}
		}
		if (earliestIndex != -1) {
			storebuffers[earliestIndex]->SetExecute();
		}
	}
}

bool StoreBuffer::isAllFree() {
	for (int i = 0; i < STORENUM; i++) {
		if (storebuffers[i]->IsBusy()) return false;
	}
	return true;
}

void StoreBuffer::ReceiveData(string unitName, string value) {
	for (int i = 0; i < STORENUM; i++) {
		storebuffers[i]->ReceiveData(unitName, value);
	}
}

void StoreBuffer::InsertOutput(vector<string>& table) {
	printHeader("Store Buffers", 53);

	std::cout << "|" << centerString("Line", 10)
		<< "|" << centerString("Busy", 6)
		<< "|" << centerString("Address", 20)
		<< "|" << centerString("Remaining", 11) << "|\n";
	std::cout << std::string(53, '-') << "\n";

	// 打印每一行的内容
	for (int i = 0; i < STORENUM; ++i) {
		std::cout << "|" << centerString(storebuffers[i]->GetName(), 10)
			<< "|" << centerString(std::to_string(storebuffers[i]->IsBusy()), 6)
			<< "|" << centerString(storebuffers[i]->GetAddress(), 20)
			<< "|" << centerString(std::to_string(storebuffers[i]->GetRemainingTime()), 11) << "|\n";
	}

	std::cout << std::string(53, '-') << "\n";  // 打印表格下边的分隔线
	cout << endl;
}