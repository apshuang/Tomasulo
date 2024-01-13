#pragma once
#include "BasicDefine.h"


class TomasuloWithROB;

class ReservationStationLine {
private:
    // ��ģ�����в������͡�����Դ����ֵ/Դ����λ�ã�ָ�ڵڼ���Reorder Buffer������ȡֵ��
    // ����Busyλ����������ִ��״̬��Ŀ��λ�ã�д���ڼ���Reorder Buffer�����Ѿ�ʣ��ִ��ʱ��
    int busy;
    string Op;
    float Vj, Vk; // ���������������������
    string Vj2, Vk2; // �����Mem[32 + Regs[x2]]����ֵ�������
    int Qj, Qk; // ����ֻ����ָ��#2����ROBλ�ã�˵��ֵ��δready
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
    int state;
    int destination; // ����ΪFREE״̬ʱ��DestinationֵΪ-1
    int remainingTime; // ����ΪFREE״̬ʱ��RemainingTimeֵΪ-1
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
    // ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
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
    // ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
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