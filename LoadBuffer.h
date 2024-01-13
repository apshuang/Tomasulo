#pragma once
#include "BasicDefine.h"

class TomasuloWithROB;

class LoadBufferLine {
private:
    // 该模块中有Busy位，Load指令的地址，以及具体取得的值，以及剩余时间
    int busy; // 该位为1时为busy，否则为空闲
    string base; // Base意为基址，此处记录Load指令基址对应的寄存器名字
    int offset; // Offset为偏移量，与Base共同组成地址
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示Mem[32 + Regs[x2]]这类值
    int remainingTime;
    int destination; // 标注应写到ROB中的哪一个entry
    void Reset();
    void WriteBack(TomasuloWithROB& tomasulo);

public:
    LoadBufferLine();

    void SetLoadBuffer(string instBase, int instOffset, int instDestination);
    int IsBusy();
    string OffsetToString(int offset);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table, int id);
};

class LoadBuffer {
private:
    // 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
    int tail;
    LoadBufferLine loadbuffers[LOADNUM];

public:
    LoadBuffer();
    int IsFree();
    int LoadExecute(string instBase, int instOffset, int instDestination);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table);
};


class StoreBufferLine {
private:
    // 该模块中有Busy位，Load指令的地址，以及具体取得的值，以及剩余时间
    int busy; // 该位为1时为busy，否则为空闲
    string base; // Base意为基址，此处记录Load指令基址对应的寄存器名字
    int offset; // Offset为偏移量，与Base共同组成地址
    int valueROB; //找到对应的ROB去取数
    string valueString; // 由于该作业的特殊性，设置该变量用于表示Mem[32 + Regs[x2]]这类值
    int remainingTime;
    int ROBPosition; // 标注应指向ROB中的哪一个entry
    void Reset();

public:
    StoreBufferLine();

    void SetStoreBuffer(string instBase, int instOffset, string instValue, int instROBPosition);
    int IsBusy();
    void Tick(TomasuloWithROB& tomasulo);
};

class StoreBuffer {
private:
    // 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
    int tail;
    StoreBufferLine storebuffers[STORENUM];

public:
    StoreBuffer();
    int IsFree();
    int LoadExecute(string instBase, int instOffset, string instValue, int instDestination);
    void Tick(TomasuloWithROB& tomasulo);
};