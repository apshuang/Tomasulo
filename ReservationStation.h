#pragma once
#include "BasicDefine.h"


class TomasuloWithROB;

class ReservationStationLine {
private:
    // 该模块中有操作类型、两个源运算值/源运算位置（指在第几条Reorder Buffer处进行取值）
    // 还有Busy位、计算结果、执行状态、目的位置（写到第几条Reorder Buffer），已经剩余执行时间
    int busy;
    string Op;
    float Vj, Vk; // 如果是立即数就用这两个
    string Vj2, Vk2; // 如果是Mem[32 + Regs[x2]]这类值就用这个
    int Qj, Qk; // 这里只能是指向#2这种ROB位置，说明值尚未ready
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示#2-#1这类值
    int state;
    int destination; // 当其为FREE状态时，Destination值为-1
    int remainingTime; // 当其为FREE状态时，RemainingTime值为-1
    void Reset();
    void WriteBack(TomasuloWithROB& tomasulo);
public:
    ReservationStationLine();
    int IsBusy();
    void SetRSLine(string instOp, int instDest);
    void SetVj(string instVj);
    void SetVk(string instVk);
    void SetQj(int instQj);
    void SetQk(int instQk);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table, int id, int moduleDistinguish);
};

class ReservationStationADD {
private:
    // 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
    int tail;
    ReservationStationLine addModule[ADDNUM];
public:
    ReservationStationADD();
    int IsFree();
    int AddExecute(string instOp, int instDest);
    void SetVj(string instVj, int moduleNum);
    void SetVk(string instVk, int moduleNum);
    void SetQj(int instQj, int moduleNum);
    void SetQk(int instQk, int moduleNum);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table);
};

class ReservationStationMULT {
private:
    // 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
    int tail;
    ReservationStationLine multModule[MULTNUM];
public:
    ReservationStationMULT();
    int IsFree();
    int MultExecute(string instOp, int instDest);
    void SetVj(string instVj, int moduleNum);
    void SetVk(string instVk, int moduleNum);
    void SetQj(int instQj, int moduleNum);
    void SetQk(int instQk, int moduleNum);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table);
};