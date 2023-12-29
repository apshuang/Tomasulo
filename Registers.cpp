#include "BasicDefine.h"


class RegisterLine {
private:
    // 该模块中有Busy位、以及被占用对应的Reorder Buffer位置，还有寄存器的值
    int busy;
    int ROBPosition;
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示#2-#1这类值
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
    // 该模块共有F0、F2、F4……F30共16个寄存器，直接实例化RegisterLine
    RegisterLine registers[REGNUM];
public:
    Registers() {
        for (int i = 0; i < REGNUM; i++) {
            registers[i] = RegisterLine();
        }
    }
};