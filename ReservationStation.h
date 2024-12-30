#pragma once
#include "BasicDefine.h"

class CommonDataBus;

class ReservationStationLine {
private:
    // 该模块中有操作类型、两个源运算值/源运算位置（指在第几条Reorder Buffer处进行取值）
    // 还有Busy位、计算结果、执行状态、目的位置（写到第几条Reorder Buffer），已经剩余执行时间
    int busy;
    string Op;
    string Vj, Vk; // 在本项目中所有值都用string表示，如果是没准备好的值，会写作Load2等，那么就将其视作Qj、Qk即可（在output的时候亦会有所区分）
    string valueString;
    int remainingTime; // 当其为FREE状态时，RemainingTime值为-1
    float arrivedTime;
    string unitName;
    void Reset();
public:
    ReservationStationLine(string name);
    int IsBusy();
    string GetName();
    string GetOp();
    string GetVj();
    string GetVk();
    int GetRemainingTime();
    void SetExecute();
    float GetArrivedTime();
    void SetRSLine(string instOp, string instVj, string instVk, float arrived);
    void Tick();
    bool isReady();
    void ReceiveData(string unitName, string value);  // 从CDB那里接收数据
};

class ReservationStationADD {
private:
    // 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
    int tail;
    ReservationStationLine* addModule[ADDNUM];
public:
    ReservationStationADD();
    int IsFree();
    string AddIssue(string instOp, string instVj, string instVk, float arrived);
    void Tick();
    void ReceiveData(string unitName, string value);  // 从CDB那里接收数据
    bool isAllFree();
    void InsertOutput(vector<string>& table);
};

class ReservationStationMULT {
private:
    // 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
    int tail;
    ReservationStationLine* multModule[MULTNUM];
public:
    ReservationStationMULT();
    int IsFree();
    string MultIssue(string instOp, string instVj, string instVk, float arrived);
    void Tick();
    void ReceiveData(string unitName, string value);  // 从CDB那里接收数据
    bool isAllFree();
    void InsertOutput(vector<string>& table);
};