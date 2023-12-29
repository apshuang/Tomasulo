#pragma once
#include "BasicDefine.h"

class ReservationStationLine {
private:
    // ��ģ�����в������͡�����Դ����ֵ/Դ����λ�ã�ָ�ڵڼ���Reorder Buffer������ȡֵ��
    // ����Busyλ����������ִ��״̬��Ŀ��λ�ã�д���ڼ���Reorder Buffer�����Ѿ�ʣ��ִ��ʱ��
    int busy;
    string Op;
    float Vj, Vk; // ���������������������
    string Vj2, Vk2; // �����Mem[32 + Regs[x2]]����ֵ�������
    int Qj, Qk;
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
    int state;
    int destination; // ����ΪFREE״̬ʱ��DestinationֵΪ-1
    int remainingTime; // ����ΪFREE״̬ʱ��RemainingTimeֵΪ-1
    void Reset();
public:
    ReservationStationLine();
};

class ReservationStationADD {
private:
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod ADDNUM)˵����������
    int head;
    int tail;
    ReservationStationLine addModule[ADDNUM];
public:
    ReservationStationADD();
    int isFree();
};

class ReservationStationMULT {
private:
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod MULTNUM)˵����������
    int head;
    int tail;
    ReservationStationLine multModule[MULTNUM];
public:
    ReservationStationMULT();
    int isFree();
};