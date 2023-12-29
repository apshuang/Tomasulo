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
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod ADDNUM)˵����������
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
            return -1; //��������
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
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod MULTNUM)˵����������
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
            return -1; //��������
        }
        else {
            tail += 1;
            tail %= MULTNUM;
            return tail;
        }
    }
};