#include "BasicDefine.h"
#include "LoadBuffer.h"


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
	string result = "";
	while (offset) {
		result += (offset % 10) + '0';
		offset /= 10;
	}
	reverse(result.begin(), result.end());
	return result;
}

void LoadBufferLine::Tick() {
	// ��һ��ʱ��
	remainingTime--;
	if (remainingTime == 0) {
		valueString = "Mem[" + OffsetToString(offset) + " + Regs[" + base + "]]";
	}
}


LoadBuffer::LoadBuffer() {
	head = 0;
	tail = 0;
	for (int i = 0; i < LOADNUM; i++) {
		loadbuffers[i] = LoadBufferLine();
	}
}
int LoadBuffer::IsFree() {
	if (head == ((tail + 1) % LOADNUM)) {
		return -1; //��������
	}
	else {
		tail += 1;
		tail %= LOADNUM;
		return tail;
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
void LoadBuffer::Tick() {
	for (int i = 0; i < LOADNUM; i++) {
		loadbuffers[i].Tick();
	}
}