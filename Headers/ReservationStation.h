#pragma once
#include "BasicDefine.h"

class CommonDataBus;

class ReservationStationLine {
private:
    // ��ģ�����в������͡�����Դ����ֵ/Դ����λ�ã�ָ�ڵڼ���Reorder Buffer������ȡֵ��
    // ����Busyλ����������ִ��״̬��Ŀ��λ�ã�д���ڼ���Reorder Buffer�����Ѿ�ʣ��ִ��ʱ��
    int busy;
    string Op;
    string Vj, Vk; // �ڱ���Ŀ������ֵ����string��ʾ�������û׼���õ�ֵ����д��Load2�ȣ���ô�ͽ�������Qj��Qk���ɣ���output��ʱ������������֣�
    string valueString;
    int remainingTime; // ����ΪFREE״̬ʱ��RemainingTimeֵΪ-1
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
    void ReceiveData(string unitName, string value);  // ��CDB�����������
};

class ReservationStationADD {
private:
    // ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
    int tail;
    ReservationStationLine* addModule[ADDNUM];
public:
    ReservationStationADD();
    int IsFree();
    string AddIssue(string instOp, string instVj, string instVk, float arrived);
    void Tick();
    void ReceiveData(string unitName, string value);  // ��CDB�����������
    bool isAllFree();
    void InsertOutput(vector<string>& table);
};

class ReservationStationMULT {
private:
    // ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
    int tail;
    ReservationStationLine* multModule[MULTNUM];
public:
    ReservationStationMULT();
    int IsFree();
    string MultIssue(string instOp, string instVj, string instVk, float arrived);
    void Tick();
    void ReceiveData(string unitName, string value);  // ��CDB�����������
    bool isAllFree();
    void InsertOutput(vector<string>& table);
};