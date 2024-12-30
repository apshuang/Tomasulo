#pragma once
#include "BasicDefine.h"
#include "Registers.h"

class LoadBuffer;
class StoreBuffer;
class ReservationStationADD;
class ReservationStationMULT;
class InstructionDecoder;

class CommonDataBus {
    // 该模块主要负责将功能单元计算出来的值传送到其它部件
    // 故每次接收一组（funcUnit, value），然后查看其它部件，如果依赖了这个funcUnit，就将这个value传送给该部件。
private:
    static queue<string> functionUnit;
    static queue<string> valueQueue;
    IntegerRegisters* integerRegisters;
    FloatRegisters* floatRegisters;
    LoadBuffer* loadBuffer;
    StoreBuffer* storeBuffer;
    ReservationStationADD* reservationAdd;
    ReservationStationMULT* reservationMult;
    InstructionDecoder* instructionDecoder;

public:
    CommonDataBus(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult, InstructionDecoder* decoder);

    static void TransformValue(string funcUnit, string value) {
        functionUnit.push(funcUnit);
        valueQueue.push(value);
    }

    void Tick(int cycle);
};