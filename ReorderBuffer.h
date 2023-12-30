#pragma once
#include "LoadBuffer.h"
#include "Registers.h"
#include "ReservationStation.h"

static class InstuctionDecoder {
private:
    unordered_map<string, int> OperandNum; // 定义某条指令的操作数数量
    unordered_map<string, int> InstructionModule; // 定义某条指令的调用的模块，Load为1，Store为2，Adder为3，Multiplier为4
public:
    InstuctionDecoder();

    int GetOperandNum(string instructionType);
    int GetInstructionType(string instructionType);
    int GetOffset(string instructionOperand);
};

class ReorderBufferLine {
private:
    // 该模块中有Busy位、对应指令、指令执行状态，还有寄存器的值
    int id;
    int busy;
    string instructionType;
    string instuctionDestination;
    string instructionOperand1;
    string instructionOperand2;
    int state;
    int moduleNum; // 记录该ROB将指令发到了模块的哪一行
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示#2-#1这类值
    void Reset();
public:
    ReorderBufferLine();
    void SetID(int ID);
    int IsBusy();
    int NameToNum(string RegName);
    void Tick(TomasuloWithROB& tomasulo);
    void FetchAndIssue(TomasuloWithROB& tomasulo);
    void WriteResult(string value);
    string GetValue();
    void Commit(TomasuloWithROB& tomasulo);
    int GetState();
    string OffsetToString(int offset);
};



class ReorderBuffer {
private:
    // 该模块使用循环队列实现，若head==tail说明队列为空，若head==tail+1(mod ENTRYNUM)说明队列已满
    int head;
    int tail;
    int empty;
    ReorderBufferLine entry[ENTRYNUM];
public:
    ReorderBuffer();
    int IsFree();
    int IsEmpty();
    int IsFull(); // 这个只是必要条件，不是充分条件
    void SetEmpty(int emptyValue);
    void WriteResult(int entryLine, string value);
    void Tick(TomasuloWithROB& tomasulo);
    void BackTrack();
    string GetValue(int entryLine);
};


class TomasuloWithROB {
public:
    // 此处整合了前面所有模块，组成一个系统，并包括对应的cycle
    ReorderBuffer reorderBuffer;
    LoadBuffer loadBuffer;
    ReservationStationADD reservationStationADD;
    ReservationStationMULT reservationStationMULT;
    Registers registers;
    InstuctionDecoder instructionDecoder;
    int cycle;
    TomasuloWithROB();
    void Tick();
};