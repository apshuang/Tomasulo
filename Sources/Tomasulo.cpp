#include "../Headers/Tomasulo.h"


Tomasulo::Tomasulo() {
	loadBuffer = new LoadBuffer();
	storeBuffer = new StoreBuffer();
	reservationStationADD = new ReservationStationADD();
	reservationStationMULT = new ReservationStationMULT();
	integerRegisters = new IntegerRegisters();
	floatRegisters = new FloatRegisters();
	instructionDecoder = new InstructionDecoder(integerRegisters, floatRegisters, loadBuffer, storeBuffer, reservationStationADD, reservationStationMULT);
	commonDataBus = new CommonDataBus(integerRegisters, floatRegisters, loadBuffer, storeBuffer, reservationStationADD, reservationStationMULT, instructionDecoder);
	outputTable.clear();
	cycle = 0;
}

bool Tomasulo::CheckStop() {
	if (!loadBuffer->isAllFree()) return false;
	if (!storeBuffer->isAllFree())return false;
	if (!reservationStationADD->isAllFree())return false;
	if (!reservationStationMULT->isAllFree())return false;
	if (!instructionDecoder->isAllFree()) return false;
	return true;
}


bool Tomasulo::Tick() {
	cycle++;
	loadBuffer->Tick();
	storeBuffer->Tick();
	reservationStationADD->Tick();
	reservationStationMULT->Tick();
	instructionDecoder->Tick(cycle);  // ����һ�����ڵ����Ž������ȡָ/���룬����ģ����һ�������ڸò�������ȡָ�����루������һ��ͷ��issue��ȥ�ˣ�
	commonDataBus->Tick(cycle);  // ����һ�����ڵ���󣬽�����������֮����������ڵ������ת����ȥ�����������ø���������һ�����ڿ�ʼ����
	GetOutput();
	if (CheckStop())return true;
	cout << string(80, '-') << "\n\n\n";
	return false;
}


void Tomasulo::GetOutput() {
	vector<string> table;
	cout << "cycle" << cycle << ":\n";
	loadBuffer->InsertOutput(table);
	storeBuffer->InsertOutput(table);
	reservationStationADD->InsertOutput(table);
	reservationStationMULT->InsertOutput(table);
	integerRegisters->InsertOutput(table);
	floatRegisters->InsertOutput(table);
	outputTable.push_back(table);
	cout << "\n";
}


void Tomasulo::FoldingOutput() {
	// ������������۵���ͬ���ݵĸ����ڵĴ�ӡ�� ����cycle12-27�Ĵ�ӡ��һ���ģ��Ϳ���ֱ��ֻ��ӡһ�Σ�����cycle12-27��ʾ��
	// ���÷���Ŀǰ�ڱ���Ŀ�в���Ч����Ϊ����Ŀ���ӡremainingTime����ÿ�����ڱ�Ȼ�ǲ�һ����

	int sameStart = 0; //�������ͬ�ģ�����Ϊ��0
	int allSize = outputTable.size();
	for (int i = 0; i < allSize - 1; i++) {
		int nowCheck = 1; //1��������ͬ�ģ�����в�ͬ�Ļ�����Ϊ�㼴��
		for (int j = 0; j < outputTable[i].size(); j++) {
			if (outputTable[i][j] != outputTable[i + 1][j]) {
				nowCheck = 0;
				break;
			}
		}
		if (nowCheck == 1) {
			//�������ͬ�ģ�����Ϊ��ʼ��cycle
			if (sameStart == 0)sameStart = i + 1;
		}
		else {
			cout << "cycle_";
			if (sameStart) {
				cout << sameStart << "-";
			}
			cout << i + 1 << ":" << endl;
			for (int j = 0; j < outputTable[i].size(); j++) {
				cout << outputTable[i][j] << endl;
			}
			cout << endl << endl;
			sameStart = 0;
		}
	}
	cout << "cycle_";
	if (sameStart) {
		cout << sameStart << "-";
	}
	cout << allSize << ":" << endl;
	for (int j = 0; j < outputTable[allSize - 1].size(); j++) {
		cout << outputTable[allSize - 1][j] << endl;
	}
}

void Tomasulo::OutputInstructionTime() {
	instructionDecoder->OutputInstructionTime();
}