#include "BasicDefine.h"

class ReservationStationLine {
private:
    // 该模块中有操作类型、两个源运算值/源运算位置（指在第几条Reorder Buffer处进行取值）
    // 还有Busy位、计算结果、执行状态、目的位置（写到第几条Reorder Buffer），已经剩余执行时间
    int busy;
    string Op;
    float Vj, Vk; // 如果是立即数就用这两个
    string Vj2, Vk2; // 如果是Mem[32 + Regs[x2]]这类值就用这个
    int Qj, Qk;
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示#2-#1这类值
    int state;
    int destination; // 当其为FREE状态时，Destination值为-1
    int remainingTime; // 当其为FREE状态时，RemainingTime值为-1
    void Reset() {
        busy = 0;
        Op = "";
        Vj = 0;
        Vk = 0;
        Vj2 = "";
        Vk2 = "";
        Qj = 0;
        Qk = 0;
        value = 0;
        valueString = "";
        state = FREE;
        destination = -1;
        remainingTime = -1;
    }
public:
    ReservationStationLine() {
        Reset();
    }
};

class ReservationStationADD {
private:
    // 该模块使用循环队列实现，若head==tail说明队列为空，若head==tail+1(mod ADDNUM)说明队列已满
    int head;
    int tail;
    ReservationStationLine addModule[ADDNUM];
public:
    ReservationStationADD() {
        head = 0;
        tail = 0;
        for (int i = 0; i < ADDNUM; i++) {
            addModule[i] = ReservationStationLine();
        }
    }
    int isFree() {
        if (head == ((tail + 1) % ADDNUM)) {
            return -1; //队列已满
        }
        else {
            tail += 1;
            tail %= ADDNUM;
            return tail;
        }
    }
};

class ReservationStationMULT {
private:
    // 该模块使用循环队列实现，若head==tail说明队列为空，若head==tail+1(mod MULTNUM)说明队列已满
    int head;
    int tail;
    ReservationStationLine multModule[MULTNUM];
public:
    ReservationStationMULT() {
        head = 0;
        tail = 0;
        for (int i = 0; i < MULTNUM; i++) {
            multModule[i] = ReservationStationLine();
        }
    }
    int isFree() {
        if (head == ((tail + 1) % MULTNUM)) {
            return -1; //队列已满
        }
        else {
            tail += 1;
            tail %= MULTNUM;
            return tail;
        }
    }
};