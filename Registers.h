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
};

class Registers {
private:
    // ��ģ�鹲��F0��F2��F4����F30��16���Ĵ�����ֱ��ʵ����RegisterLine
    RegisterLine registers[REGNUM];
public:
    Registers();
};