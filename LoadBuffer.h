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
    void Tick();
};

class LoadBuffer {
private:
    // 该模块使用循环队列实现，若head==tail说明队列为空，若head==tail+1(mod LOADNUM)说明队列已满
    int head;
    int tail;
    LoadBufferLine loadbuffers[LOADNUM];

public:
    LoadBuffer();
    int IsFree();
    int LoadExecute(string instBase, int instOffset, int instDestination);
    void Tick();
};