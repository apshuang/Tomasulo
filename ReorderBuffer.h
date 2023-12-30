#pragma once
#include "LoadBuffer.h"
#include "Registers.h"
#include "ReservationStation.h"

static class InstuctionDecoder {
private:
    unordered_map<string, int> OperandNum; // ����ĳ��ָ��Ĳ���������
    unordered_map<string, int> InstructionModule; // ����ĳ��ָ��ĵ��õ�ģ�飬LoadΪ1��StoreΪ2��AdderΪ3��MultiplierΪ4
public:
    InstuctionDecoder();

    int GetOperandNum(string instructionType);
    int GetInstructionType(string instructionType);
    int GetOffset(string instructionOperand);
};

class ReorderBufferLine {
private:
    // ��ģ������Busyλ����Ӧָ�ָ��ִ��״̬�����мĴ�����ֵ
    int id;
    int busy;
    string instructionType;
    string instuctionDestination;
    string instructionOperand1;
    string instructionOperand2;
    int state;
    int moduleNum; // ��¼��ROB��ָ�����ģ�����һ��
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
    void Reset();
public:
    ReorderBufferLine();
    void SetID(int ID);
    int IsBusy();
    int NameToNum(string RegName);
    void Tick(TomasuloWithROB& tomasulo);
    void FetchAndIssue(TomasuloWithROB& tomasulo);
    void WriteResult(string value);
    string GetValue();
    void Commit(TomasuloWithROB& tomasulo);
    int GetState();
    string OffsetToString(int offset);
};



class ReorderBuffer {
private:
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod ENTRYNUM)˵����������
    int head;
    int tail;
    int empty;
    ReorderBufferLine entry[ENTRYNUM];
public:
    ReorderBuffer();
    int IsFree();
    int IsEmpty();
    int IsFull(); // ���ֻ�Ǳ�Ҫ���������ǳ������
    void SetEmpty(int emptyValue);
    void WriteResult(int entryLine, string value);
    void Tick(TomasuloWithROB& tomasulo);
    void BackTrack();
    string GetValue(int entryLine);
};


class TomasuloWithROB {
public:
    // �˴�������ǰ������ģ�飬���һ��ϵͳ����������Ӧ��cycle
    ReorderBuffer reorderBuffer;
    LoadBuffer loadBuffer;
    ReservationStationADD reservationStationADD;
    ReservationStationMULT reservationStationMULT;
    Registers registers;
    InstuctionDecoder instructionDecoder;
    int cycle;
    TomasuloWithROB();
    void Tick();
};