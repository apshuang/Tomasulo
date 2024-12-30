#include "BasicDefine.h"
#include "Registers.h"


void RegisterLine::Reset() {
    busy = 0;
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

void RegisterLine::ReceiveData(string unitName, string value) {
	if (valueString == unitName) valueString = value;
}


FloatRegisters::FloatRegisters() {
	for (int i = 0; i < REGNUM; i++) {
		registers[i] = new RegisterLine();
		string initValue;
		initValue = "Regs[F" + OffsetToString(i * 2) + "]";
		registers[i]->SetValue(initValue);
	}
}

void FloatRegisters::SetLineValue(string value, int reg) {
	// �Ĵ���F0��Ӧreg0��F2��Ӧreg1��F4��Ӧreg2��������Ҫת��
	registers[reg / 2]->SetValue(value);
}

int FloatRegisters::IsBusy(int reg) {
	return registers[reg / 2]->IsBusy();
}

string FloatRegisters::GetLineValue(int reg) {
	return registers[reg / 2]->GetValue();
}

void FloatRegisters::SetBusy(int busyValue, int reg) {
	registers[reg / 2]->SetBusy(busyValue);
}

void FloatRegisters::ReceiveData(string unitName, string value) {
	for (int i = 0; i < REGNUM; i++) {
		registers[i]->ReceiveData(unitName, value);
	}
}

void FloatRegisters::InsertOutput(vector<string>& table) {
	printHeader("Float Registers", 94);


	std::cout << "|" << centerString("Register", 14) << "|";
	std::cout << centerString("Busy", 6) << "|";
	std::cout << centerString("Value", 70) << "|\n";
	std::cout << std::string(94, '-') << "\n";  // ��ӡ�ָ���

	// ��ӡÿ���Ĵ��������֡�ֵ�� busy ״̬
	for (int i = 0; i < REGNUM; ++i) {
		std::cout << "|" << centerString("R" + std::to_string(i * 2), 14) << "|";
		std::cout << centerString(std::to_string(registers[i]->IsBusy()), 6) << "|";
		std::cout << centerString(registers[i]->GetValue(), 70) << "|\n";

	}
	std::cout << std::string(94, '-') << "\n";  // ��ӡ�ָ���
	cout << endl;
}



IntegerRegisters::IntegerRegisters() {
	for (int i = 0; i < REGNUM; i++) {
		registers[i] = new RegisterLine();
		string initValue;
		initValue = "Regs[x" + OffsetToString(i) + "]";
		registers[i]->SetValue(initValue);
	}
}

void IntegerRegisters::SetLineValue(string value, int reg) {
	// �Ĵ���x0�Ͷ�Ӧreg0��x1�Ͷ�Ӧreg1��x2��Ӧreg2���븡��Ĵ�����һ��
	registers[reg]->SetValue(value);
}

int IntegerRegisters::IsBusy(int reg) {
	return registers[reg]->IsBusy();
}

string IntegerRegisters::GetLineValue(int reg) {
	return registers[reg]->GetValue();
}

void IntegerRegisters::SetBusy(int busyValue, int reg) {
	registers[reg]->SetBusy(busyValue);
}

void IntegerRegisters::ReceiveData(string unitName, string value) {
	for (int i = 0; i < REGNUM; i++) {
		registers[i]->ReceiveData(unitName, value);
	}
}

void IntegerRegisters::InsertOutput(vector<string>& table) {
	printHeader("Integer Registers", 94);

	std::cout << "|" << centerString("Register", 14) << "|";
	std::cout << centerString("Busy", 6) << "|";
	std::cout << centerString("Value", 70) << "|\n";
	std::cout << std::string(94, '-') << "\n";  // ��ӡ�ָ���

	// ��ӡÿ���Ĵ��������֡�ֵ�� busy ״̬
	for (int i = 0; i < REGNUM; ++i) {
		std::cout << "|" << centerString("x" + std::to_string(i), 14) << "|";
		std::cout << centerString(std::to_string(registers[i]->IsBusy()), 6) << "|";
		std::cout << centerString(registers[i]->GetValue(), 70) << "|\n";

	}
	std::cout << std::string(94, '-') << "\n";  // ��ӡ�ָ���
	cout << endl;
}