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
	// ReorderBuffer将指令发到此处来执行
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
	// 将数字形式的offset改成string形式的
	// 因为该部分在实际应用中并不存在，所以我们不将它加到decoder中
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

void LoadBufferLine::Tick() {
	// 过一个时钟
	remainingTime--;
	if (remainingTime == 0) {
		valueString = "Mem[" + OffsetToString(offset) + " + Regs[" + base + "]]";

	}
}


LoadBuffer::LoadBuffer() {
	head = LOADNUM - 1;
	tail = LOADNUM - 1; //这样定义循环队列保证第一次访问到第0行
	for (int i = 0; i < LOADNUM; i++) {
		loadbuffers[i] = LoadBufferLine();
	}
}
int LoadBuffer::IsFree() {
	if (head == ((tail + 1) % LOADNUM)) {
		return -1; //队列已满
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