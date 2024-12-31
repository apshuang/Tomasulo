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
	instructionDecoder->Tick(cycle);  // 放在一个周期的最后才进行这个取指/译码，用来模拟这一整个周期该部件都在取指和译码（而不是一开头就issue出去了）
	commonDataBus->Tick(cycle);  // 放在一个周期的最后，接收所有数据之后，在这个周期的最后将其转发出去，这样就能让各部件在下一个周期开始工作
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
	// 这个方法可以折叠相同内容的各周期的打印， 比如cycle12-27的打印是一样的，就可以直接只打印一次（并用cycle12-27表示）
	// 但该方法目前在本项目中不生效，因为本项目会打印remainingTime，它每个周期必然是不一样的

	int sameStart = 0; //如果是相同的，就置为非0
	int allSize = outputTable.size();
	for (int i = 0; i < allSize - 1; i++) {
		int nowCheck = 1; //1代表是相同的，如果有不同的话再置为零即可
		for (int j = 0; j < outputTable[i].size(); j++) {
			if (outputTable[i][j] != outputTable[i + 1][j]) {
				nowCheck = 0;
				break;
			}
		}
		if (nowCheck == 1) {
			//如果是相同的，就置为起始的cycle
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