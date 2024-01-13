#pragma once
#include "BasicDefine.h"

class TomasuloWithROB;

class LoadBufferLine {
private:
    // ��ģ������Busyλ��Loadָ��ĵ�ַ���Լ�����ȡ�õ�ֵ���Լ�ʣ��ʱ��
    int busy; // ��λΪ1ʱΪbusy������Ϊ����
    string base; // Base��Ϊ��ַ���˴���¼Loadָ���ַ��Ӧ�ļĴ�������
    int offset; // OffsetΪƫ��������Base��ͬ��ɵ�ַ
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾMem[32 + Regs[x2]]����ֵ
    int remainingTime;
    int destination; // ��עӦд��ROB�е���һ��entry
    void Reset();
    void WriteBack(TomasuloWithROB& tomasulo);

public:
    LoadBufferLine();

    void SetLoadBuffer(string instBase, int instOffset, int instDestination);
    int IsBusy();
    string OffsetToString(int offset);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table, int id);
};

class LoadBuffer {
private:
    // ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
    int tail;
    LoadBufferLine loadbuffers[LOADNUM];

public:
    LoadBuffer();
    int IsFree();
    int LoadExecute(string instBase, int instOffset, int instDestination);
    void Tick(TomasuloWithROB& tomasulo);
    void InsertOutput(vector<string>& table);
};


class StoreBufferLine {
private:
    // ��ģ������Busyλ��Loadָ��ĵ�ַ���Լ�����ȡ�õ�ֵ���Լ�ʣ��ʱ��
    int busy; // ��λΪ1ʱΪbusy������Ϊ����
    string base; // Base��Ϊ��ַ���˴���¼Loadָ���ַ��Ӧ�ļĴ�������
    int offset; // OffsetΪƫ��������Base��ͬ��ɵ�ַ
    int valueROB; //�ҵ���Ӧ��ROBȥȡ��
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾMem[32 + Regs[x2]]����ֵ
    int remainingTime;
    int ROBPosition; // ��עӦָ��ROB�е���һ��entry
    void Reset();

public:
    StoreBufferLine();

    void SetStoreBuffer(string instBase, int instOffset, string instValue, int instROBPosition);
    int IsBusy();
    void Tick(TomasuloWithROB& tomasulo);
};

class StoreBuffer {
private:
    // ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
    int tail;
    StoreBufferLine storebuffers[STORENUM];

public:
    StoreBuffer();
    int IsFree();
    int LoadExecute(string instBase, int instOffset, string instValue, int instDestination);
    void Tick(TomasuloWithROB& tomasulo);
};