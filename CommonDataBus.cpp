#include "CommonDataBus.h"
#include "LoadBuffer.h"
#include "ReservationStation.h"

queue<string> CommonDataBus::functionUnit;
queue<string> CommonDataBus::valueQueue;

CommonDataBus::CommonDataBus(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult) {
	while (functionUnit.size())functionUnit.pop();
	while (valueQueue.size())valueQueue.pop();
	integerRegisters = intRegs;
	floatRegisters = floatRegs;
	loadBuffer = LDBuffer;
	storeBuffer = SDBuffer;
	reservationAdd = RSAdd;
	reservationMult = RSMult;
}


void CommonDataBus::Tick(int cycle) {
	for (int i = 1; i <= ISSUENUM; i++) {
		if (functionUnit.empty())break;
		string unitName = functionUnit.front();
		functionUnit.pop();
		string value = valueQueue.front();
		valueQueue.pop();
		while (unitName.substr(0, 5) == "Store") {
			// Storeָ�����֮�󲢲���Ҫ������ת����ȥ������Ҳ�Ͳ�ռ������
			// ������Ȼ����������������Ϊ����InstructionDecoderͳ�Ƹ���ָ������ʱ��
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
	}
}