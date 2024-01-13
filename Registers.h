#pragma once
#include "BasicDefine.h"

class RegisterLine {
private:
    // 该模块中有Busy位、以及被占用对应的Reorder Buffer位置，还有寄存器的值
    int busy;
    int ROBPosition;
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示#2-#1这类值
    void Reset();
public:
    RegisterLine();
    void SetValue(string value);
    string GetValue();
    int IsBusy();
    string OffsetToString(int offset);
    void SetBusy(int busyValue);
    int GetROBPosition();
    void SetROBPosition(int position);
};

class Registers {
private:
    // 该模块共有F0、F2、F4……F30共16个寄存器，直接实例化RegisterLine
    RegisterLine registers[REGNUM];
public:
    Registers();
    void SetLineValue(string value, int reg);
    string GetLineValue(int reg);
    int IsBusy(int reg);
    void SetBusy(int busyValue, int reg);
    int GetROBPosition(int reg);
    void SetROBPosition(int position, int reg);
    void InsertOutput(vector<string>& table);
};