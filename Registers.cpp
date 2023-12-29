#include "BasicDefine.h"


class RegisterLine {
private:
    // ��ģ������Busyλ���Լ���ռ�ö�Ӧ��Reorder Bufferλ�ã����мĴ�����ֵ
    int busy;
    int ROBPosition;
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
    void Reset() {
        busy = 0;
        ROBPosition = 0;
        value = 0;
        valueString = "";
    }
public:
    RegisterLine() {
        Reset();
    }
};

class Registers {
private:
    // ��ģ�鹲��F0��F2��F4����F30��16���Ĵ�����ֱ��ʵ����RegisterLine
    RegisterLine registers[REGNUM];
public:
    Registers() {
        for (int i = 0; i < REGNUM; i++) {
            registers[i] = RegisterLine();
        }
    }
};