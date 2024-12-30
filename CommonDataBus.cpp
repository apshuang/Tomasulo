#include "CommonDataBus.h"
#include "LoadBuffer.h"
#include "ReservationStation.h"
#include "InstructionDecoder.h"

queue<string> CommonDataBus::functionUnit;
queue<string> CommonDataBus::valueQueue;

CommonDataBus::CommonDataBus(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult, InstructionDecoder* decoder) {
	while (functionUnit.size())functionUnit.pop();
	while (valueQueue.size())valueQueue.pop();
	integerRegisters = intRegs;
	floatRegisters = floatRegs;
	loadBuffer = LDBuffer;
	storeBuffer = SDBuffer;
	reservationAdd = RSAdd;
	reservationMult = RSMult;
	instructionDecoder = decoder;
}


void CommonDataBus::Tick(int cycle) {
	for (int i = 1; i <= ISSUENUM; i++) {
		if (functionUnit.empty()) return;
		string unitName = functionUnit.front();
		functionUnit.pop();
		string value = valueQueue.front();
		valueQueue.pop();
		while (unitName.substr(0, 5) == "Store") {
			// Store指令完成之后并不需要将数据转发出去，所以也就不占据总线
			// 这里仍然让它发到总线来是为了让InstructionDecoder统计各条指令的完成时间
			if (functionUnit.empty()) return;
			unitName = functionUnit.front();
			functionUnit.pop();
			value = valueQueue.front();
			valueQueue.pop();
		}

		integerRegisters->ReceiveData(unitName, value);
		floatRegisters->ReceiveData(unitName, value);
		loadBuffer->ReceiveData(unitName, value);
		storeBuffer->ReceiveData(unitName, value);
		reservationAdd->ReceiveData(unitName, value);
		reservationMult->ReceiveData(unitName, value);
		instructionDecoder->ReceiveData(unitName, value);
	}
}