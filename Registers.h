#pragma once
#include "BasicDefine.h"

class RegisterLine {
private:
    // ��ģ������Busyλ���Լ���ռ�ö�Ӧ��Reorder Bufferλ�ã����мĴ�����ֵ
    int busy;
    int ROBPosition;
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
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
    // ��ģ�鹲��F0��F2��F4����F30��16���Ĵ�����ֱ��ʵ����RegisterLine
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