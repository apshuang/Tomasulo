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
    void Tick();
};

class LoadBuffer {
private:
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod LOADNUM)˵����������
    int head;
    int tail;
    LoadBufferLine loadbuffers[LOADNUM];

public:
    LoadBuffer();
    int IsFree();
    int LoadExecute(string instBase, int instOffset, int instDestination);
    void Tick();
};