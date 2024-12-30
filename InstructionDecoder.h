#pragma once
#include "BasicDefine.h"
#include "LoadBuffer.h"
#include "ReservationStation.h"
#include "Registers.h"

class InstructionDecoder {
private:
    unordered_map<string, int> operandNum; // ����ĳ��ָ��Ĳ���������
    unordered_map<string, int> instructionModule; // ����ĳ��ָ��ĵ��õ�ģ�飬LoadΪ1��StoreΪ2��AdderΪ3��MultiplierΪ4
    vector<string> instructions;
    int index;  // �����ڡ���ǰָ���ַ����������ڵ���instructions.size()���Ǿ�˵���Ѿ�����������ָ��
    unordered_map<string, int> labelMap;

    IntegerRegisters* integerRegisters;
    FloatRegisters* floatRegisters;
    LoadBuffer* loadBuffer;
    StoreBuffer* storeBuffer;
    ReservationStationADD* reservationAdd;
    ReservationStationMULT* reservationMult;
    
    bool ParseLoadAndStore(string opcode, string operands, float cycle);
    bool ParseAddAndMult(string opcode, string operands, float cycle);
    string GetRegisterValue(string originValue);
    void SetRegisterValue(string registerName, string funcUnit);
public:
    InstructionDecoder(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult);
    int GetOperandNum(string instructionType);
    int GetInstructionType(string instructionType);
    int GetOffset(string instructionOperand);
    bool isAllFree();
    void Tick(int cycle);
};
