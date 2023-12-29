#include "BasicDefine.h"
#include "Registers.h"


void RegisterLine::Reset() {
    busy = 0;
    ROBPosition = 0;
    value = 0;
    valueString = "";
}

RegisterLine::RegisterLine() {
	Reset();
}


Registers::Registers() {
	for (int i = 0; i < REGNUM; i++) {
		registers[i] = RegisterLine();
	}
}