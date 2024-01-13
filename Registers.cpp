#include "BasicDefine.h"
#include "Registers.h"


void RegisterLine::Reset() {
    busy = 0;
	ROBPosition = -1;
    value = 0;
    valueString = "";
}

RegisterLine::RegisterLine() {
	Reset();
}

void RegisterLine::SetValue(string value) {
	valueString = value;
}

string RegisterLine::GetValue() {
	return valueString;
}

int RegisterLine::IsBusy() {
	return busy;
}

void RegisterLine::SetBusy(int busyValue) {
	busy = busyValue;
}

string RegisterLine::OffsetToString(int offset) {
	// 将数字形式的offset改成string形式的
	// 因为该部分在实际应用中并不存在，所以我们不将它加到decoder中
	offset *= 2;
	string result = "";
	if (offset == 0)return "0";
	while (offset) {
		result += (offset % 10) + '0';
		offset /= 10;
	}
	reverse(result.begin(), result.end());
	return result;
}

int RegisterLine::GetROBPosition() {
	return ROBPosition;
}

void RegisterLine::SetROBPosition(int position) {
	ROBPosition = position;
}


Registers::Registers() {
	for (int i = 0; i < REGNUM; i++) {
		registers[i] = RegisterLine();
		string initValue;
		initValue = "Regs[F" + registers[i].OffsetToString(i) + "]";
		registers[i].SetValue(initValue);
	}
}

void Registers::SetLineValue(string value, int reg) {
	// 寄存器F0对应reg0，F2对应reg1，F4对应reg2……所以要转码
	registers[reg / 2].SetValue(value);
}

int Registers::IsBusy(int reg) {
	return registers[reg / 2].IsBusy();
}

string Registers::GetLineValue(int reg) {
	return registers[reg / 2].GetValue();
}

void Registers::SetBusy(int busyValue, int reg) {
	registers[reg / 2].SetBusy(busyValue);
}

int Registers::GetROBPosition(int reg) {
	return registers[reg / 2].GetROBPosition();
}

void Registers::SetROBPosition(int position,int reg) {
	registers[reg / 2].SetROBPosition(position);
}

void Registers::InsertOutput(vector<string>& table) {
	string line1;
	line1 += (string)"Reorder:";
	for (int i = 0; i < REGNUM; i++) {
		line1 += 'F';
		line1 += registers[i].OffsetToString(i);
		line1 += (string)":";
		if (registers[i].IsBusy())line1 += (char)(registers[i].GetROBPosition() + 1 + '0');
		line1 += ';';
	}
	table.push_back(line1);

	string line2;
	line2 += (string)"Busy:";
	for (int i = 0; i < REGNUM; i++) {
		line2 += 'F';
		line2 += registers[i].OffsetToString(i);
		line2 += (string)":";
		if (registers[i].IsBusy())line2 += (string)"Yes";
		else line2 += (string)"No";
		line2 += ';';
	}
	table.push_back(line2);
}