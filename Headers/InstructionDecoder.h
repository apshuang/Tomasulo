#pragma once
#include "BasicDefine.h"
#include "LoadBuffer.h"
#include "ReservationStation.h"
#include "Registers.h"

class InstructionDecoder {
private:
    unordered_map<string, int> operandNum; // 定义某条指令的操作数数量
    unordered_map<string, int> instructionModule; // 定义某条指令的调用的模块，Load为1，Store为2，Adder为3，Multiplier为4
    vector<string> instructions;
    int index;  // 类似于“当前指令地址”，如果大于等于instructions.size()，那就说明已经读完了所有指令
    unordered_map<string, int> labelMap;
    vector<Instruction> instructionTime;  // 记录每条instruction发射和执行完的时钟
    unordered_map<string, int> instMap;  // 记录某条指令依赖于什么值，在数据发回CDB的时候，就能同时获得执行完成的时钟
    string compareType;
    string compareLeft, compareRight;
    string branchLabel;

    IntegerRegisters* integerRegisters;
    FloatRegisters* floatRegisters;
    LoadBuffer* loadBuffer;
    StoreBuffer* storeBuffer;
    ReservationStationADD* reservationAdd;
    ReservationStationMULT* reservationMult;
    
    bool ParseLoadAndStore(string opcode, string operands, float cycle);
    bool ParseAddAndMult(string opcode, string operands, float cycle);
    int isBranch();  // 返回-1说明未准备好（继续阻塞），返回0说明不跳转（index向下），返回1说明跳转（index回到对应label处）
    void ParseBranch(string opcode, string operands, float cycle);
    void ResetBranch();
    string GetRegisterValue(string originValue);
    void SetRegisterValue(string registerName, string funcUnit);
public:
    InstructionDecoder(IntegerRegisters* intRegs, FloatRegisters* floatRegs, LoadBuffer* LDBuffer, StoreBuffer* SDBuffer, ReservationStationADD* RSAdd, ReservationStationMULT* RSMult);
    int GetOperandNum(string instructionType);
    int GetInstructionType(string instructionType);
    int GetOffset(string instructionOperand);
    void ReceiveData(string unitName, string value, int cycle);  // 从CDB那里接收数据
    bool isAllFree();
    void Tick(int cycle);
    void OutputInstructionTime();
};
