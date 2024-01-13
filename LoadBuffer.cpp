#include "BasicDefine.h"
#include "LoadBuffer.h"
#include "ReorderBuffer.h"

void LoadBufferLine::Reset() {
	busy = 0;
	base = "";
	offset = 0;
	value = 0;
	valueString = "";
	remainingTime = -1;
}

LoadBufferLine::LoadBufferLine() {
	Reset();
}

void LoadBufferLine::SetLoadBuffer(string instBase, int instOffset, int instDestination) {
	// ReorderBuffer��ָ����˴���ִ��
	busy = 1;
	base = instBase;
	offset = instOffset;
	remainingTime = LOADCYCLE + 1;
	destination = instDestination;
}

int LoadBufferLine::IsBusy() {
	return busy;
}

string LoadBufferLine::OffsetToString(int offset) {
	// ��������ʽ��offset�ĳ�string��ʽ��
	// ��Ϊ�ò�����ʵ��Ӧ���в������ڣ��������ǲ������ӵ�decoder��
	if (offset == 0)return "0";
	string result = "";
	while (offset) {
		result += (offset % 10) + '0';
		offset /= 10;
	}
	reverse(result.begin(), result.end());
	return result;
}

void LoadBufferLine::WriteBack(TomasuloWithROB& tomasulo) {
	tomasulo.reorderBuffer.WriteResult(destination, valueString);
}

void LoadBufferLine::Tick(TomasuloWithROB& tomasulo) {
	// ��һ��ʱ��
	remainingTime--;
	if (remainingTime == 0) {
		valueString = "Mem[" + OffsetToString(offset) + " + Regs[" + base + "]]";
		WriteBack(tomasulo);
	}
	if (remainingTime == -1) {
		// �Ѿ���ֵд��ROB���ˣ������������ģ��
		Reset();
	}
}

void LoadBufferLine::InsertOutput(vector<string>& table, int id) {
	string line;
	line += (string)"Load" + ((char)(id + 1 + '0')) + (string)" : " + (busy ? (string)"Yes" : (string)"No");
	if (base == "") {
		line += (string)",,,,,;";
		table.push_back(line);
		return;
	}
	line += (string)"," +(string)"LD," + (string)"," + base + (string)"," + (string)"," + '#' + ((char)(destination + 1 + '0')) + ';';
	table.push_back(line);
}


LoadBuffer::LoadBuffer() {
	tail = LOADNUM + 1; //��������ѭ�����б�֤��һ�η��ʵ���0��
	for (int i = 0; i < LOADNUM; i++) {
		loadbuffers[i] = LoadBufferLine();
	}
}

int LoadBuffer::IsFree() {
	int head = tail;
	tail += 1;
	tail %= LOADNUM;
	while (tail != head) {
		if (!loadbuffers[tail].IsBusy()) {
			//����п�λ���Ϳ���ֱ�Ӳ���
			return tail;
		}
		tail += 1;
		tail %= LOADNUM;
	}
}

int LoadBuffer::LoadExecute(string instBase, int instOffset, int instDestination) {
	int index = IsFree();
	if (index != -1) {
		loadbuffers[index].SetLoadBuffer(instBase, instOffset, instDestination);
		return index;
	}
	else return -1;
}

void LoadBuffer::Tick(TomasuloWithROB& tomasulo) {
	for (int i = 0; i < LOADNUM; i++) {
		if (loadbuffers[i].IsBusy()) {
			loadbuffers[i].Tick(tomasulo);
		}
	}
}

void LoadBuffer::InsertOutput(vector<string>& table) {
	for (int i = 0; i < LOADNUM; i++) {
		loadbuffers[i].InsertOutput(table, i);
	}
}



void StoreBufferLine::Reset() {
	busy = 0;
	base = "";
	offset = 0;
	valueString = "";
	remainingTime = -1;
	ROBPosition = -1;
	valueROB = -1;
}

StoreBufferLine::StoreBufferLine() {
	Reset();
}

void StoreBufferLine::SetStoreBuffer(string instBase, int instOffset, string instValue, int instROBPosition) {
	// ReorderBuffer��ָ����˴���ִ��
	busy = 1;
	base = instBase;
	offset = instOffset;
	if (instValue[0] == '#') {
		valueROB = instValue[1] - '0';
	}
	else valueString = instValue;
	remainingTime = STORECYCLE + 1;
	ROBPosition = instROBPosition;
}

int StoreBufferLine::IsBusy() {
	return busy;
}


void StoreBufferLine::Tick(TomasuloWithROB& tomasulo) {
	// ��һ��ʱ��
	if (valueROB != -1) {
		valueString = tomasulo.reorderBuffer.GetValue(valueROB);
		if (valueString != "") {
			valueROB = -1; //ȡ������
			tomasulo.reorderBuffer.SetExecState(ROBPosition);
		}
		return;
	}
	remainingTime--;
	if (remainingTime == 0) {
		tomasulo.reorderBuffer.WriteResult(ROBPosition, valueString);
	}
	if (remainingTime == -1) {
		Reset();
	}
}

StoreBuffer::StoreBuffer() {
	tail = STORENUM + 1; //��������ѭ�����б�֤��һ�η��ʵ���0��
	for (int i = 0; i < STORENUM; i++) {
		storebuffers[i] = StoreBufferLine();
	}
}

int StoreBuffer::IsFree() {
	int head = tail;
	tail += 1;
	tail %= STORENUM;
	while (tail != head) {
		if (!storebuffers[tail].IsBusy()) {
			//����п�λ���Ϳ���ֱ�Ӳ���
			return tail;
		}
		tail += 1;
		tail %= STORENUM;
	}
	return -1;
}

int StoreBuffer::LoadExecute(string instBase, int instOffset, string instValue, int instDestination) {
	int index = IsFree();
	if (index != -1) {
		storebuffers[index].SetStoreBuffer(instBase, instOffset, instValue, instDestination);
		return index;
	}
	else return -1;
}

void StoreBuffer::Tick(TomasuloWithROB& tomasulo) {
	for (int i = 0; i < LOADNUM; i++) {
		if (storebuffers[i].IsBusy()) {
			storebuffers[i].Tick(tomasulo);
		}
	}
}