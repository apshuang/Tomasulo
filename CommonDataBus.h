#pragma once
#include "BasicDefine.h"
#include "Registers.h"

class LoadBuffer;
class StoreBuffer;
class ReservationStationADD;
class ReservationStationMULT;

class CommonDataBus {
    // ��ģ����Ҫ���𽫹��ܵ�Ԫ���������ֵ���͵���������
    // ��ÿ�ν���һ�飨funcUnit, value����Ȼ��鿴����������������������funcUnit���ͽ����value���͸��ò�����
private:
    static queue<string> functionUnit;
    static queue<string> valueQueue;
    IntegerRegisters* integerRegisters;
    FloatRegisters* floatRegisters;
    LoadBuffer* loadBuffer;
    StoreBuffer* storeBuffer;
    ReservationStationADD* reservationAdd;
    ReservationStationMULT* reservationMult;

public:
    CommonDataBus(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult);

    static void TransformValue(string funcUnit, string value) {
        functionUnit.push(funcUnit);
        valueQueue.push(value);
    }

    void Tick(int cycle);
};