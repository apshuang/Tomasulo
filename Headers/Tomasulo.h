#pragma once
#include "InstructionDecoder.h"
#include "LoadBuffer.h"
#include "ReservationStation.h"
#include "CommonDataBus.h"

class Tomasulo {
private:
    // 此处整合了前面所有模块，组成一个系统，并包括对应的cycle
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