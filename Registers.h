#pragma once
#include "BasicDefine.h"

class RegisterLine {
private:
    // ��ģ������Busyλ���Լ�ռ�ø�register�еĹ��ܲ��������мĴ�����ֵ
    int busy;
    string valueString;  // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
    void Reset();
public:
    RegisterLine();
    void SetValue(string value);
    string GetValue();
    int IsBusy();
    void ReceiveData(string unitName, string value);  // ��CDB�����������
    void SetBusy(int busyValue);
};

class FloatRegisters {
private:
    // ��ģ�鹲��F0��F2��F4����F30��F����R���Ǹ���Ĵ�������16������Ĵ�����ֱ��ʵ����RegisterLine
    RegisterLine* registers[REGNUM];
public:
    FloatRegisters();
    void SetLineValue(string value, int reg);
    string GetLineValue(int reg);
    int IsBusy(int reg);
    void SetBusy(int busyValue, int reg);
    void ReceiveData(string unitName, string value);  // ��CDB�����������
    void InsertOutput(vector<string>& table);
};


class IntegerRegisters {
private:
    // ��ģ�鹲��x0��x1��x2����x15��16���Ĵ�����ֱ��ʵ����RegisterLine
    RegisterLine* registers[REGNUM];
public:
    IntegerRegisters();
    void SetLineValue(string value, int reg);
    string GetLineValue(int reg);
    int IsBusy(int reg);
    void SetBusy(int busyValue, int reg);
    void ReceiveData(string unitName, string value);  // ��CDB�����������
    void InsertOutput(vector<string>& table);
};