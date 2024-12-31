#pragma once
#include "InstructionDecoder.h"
#include "LoadBuffer.h"
#include "ReservationStation.h"
#include "CommonDataBus.h"

class Tomasulo {
private:
    // �˴�������ǰ������ģ�飬���һ��ϵͳ����������Ӧ��cycle
    LoadBuffer* loadBuffer;
    StoreBuffer* storeBuffer;
    ReservationStationADD* reservationStationADD;
    ReservationStationMULT* reservationStationMULT;
    IntegerRegisters* integerRegisters;
    FloatRegisters* floatRegisters;
    InstructionDecoder* instructionDecoder;
    CommonDataBus* commonDataBus = nullptr;
    vector<vector<string>> outputTable;
    int cycle;

    bool CheckStop();
    
public:
    Tomasulo();
    bool Tick();
    void GetOutput();
    void FoldingOutput();
    void OutputInstructionTime();
};