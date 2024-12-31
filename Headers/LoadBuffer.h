#pragma once
#include "BasicDefine.h"

class CommonDataBus;

class LoadBufferLine {
private:
	// ��ģ������Busyλ��Loadָ��ĵ�ַ���Լ�����ȡ�õ�ֵ���Լ�ʣ��ʱ��
	int busy; // ��λΪ1ʱΪbusy������Ϊ����
	string base; // Base��Ϊ��ַ���˴���¼Loadָ���ַ��Ӧ�ļĴ�������
	string offset; // OffsetΪƫ��������Base��ͬ��ɵ�ַ
	string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾMem[32 + Regs[x2]]����ֵ
	int remainingTime;
	float arrivedTime;  // ��֤�ȵ����ȷ���float��Ϊ������˫���䣬˫����ĵڶ���������Ϊtime+0.5
	string unitName;  // ָ�������LBLine�ǵڼ���
	void Reset();

public:
	LoadBufferLine(string name);
	string GetName();
	string GetAddress();
	int GetRemainingTime();
	void SetExecute();
	float GetArrivedTime();
	void SetLoadBuffer(string instBase, string instOffset, float arrived);
	int IsBusy();
	void Tick();
	bool isReady();
	void ReceiveData(string unitName, string value);  // ��CDB�����������
};

class LoadBuffer {
private:
	// ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
	int tail;
	LoadBufferLine* loadbuffers[LOADNUM];

	int IsFree();
public:
	LoadBuffer();
	
	string GetName(int index);
	string LoadIssue(string instBase, string instOffset, float arrived);  // ֱ�ӷ���ģ����
	void Tick();
	void ReceiveData(string unitName, string value);  // ��CDB����������ݣ����ַ�������
	bool isAllFree();
	void InsertOutput(vector<string>& table);
};


class StoreBufferLine {
private:
	// ��ģ������Busyλ��Loadָ��ĵ�ַ���Լ�����ȡ�õ�ֵ���Լ�ʣ��ʱ��
	int busy; // ��λΪ1ʱΪbusy������Ϊ����
	string base; // Base��Ϊ��ַ���˴���¼Loadָ���ַ��Ӧ�ļĴ�������
	string offset; // OffsetΪƫ��������Base��ͬ��ɵ�ַ
	string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾMem[32 + Regs[x2]]����ֵ
	int remainingTime;
	float arrivedTime;
	string unitName;  // ָ�������LBLine�ǵڼ���
	void Reset();
	
public:
	StoreBufferLine(string name);
	string GetName();
	string GetAddress();
	int GetRemainingTime();
	void SetExecute();
	float GetArrivedTime();
	void SetStoreBuffer(string instBase, string instOffset, string instValue, float arrived);
	int IsBusy();
	bool isReady();
	void ReceiveData(string unitName, string value);  // ��CDB�����������
	void Tick();
};

class StoreBuffer {
private:
	// ��ģ��ʹ��αѭ������ʵ�֣����ǲ�����head��ֻ����tail����ΪҪ�������ؾ��⣩
	int tail;
	StoreBufferLine* storebuffers[STORENUM];

	int IsFree();
public:
	StoreBuffer();
	string GetName(int index);
	string StoreIssue(string instBase, string instOffset, string instValue, float arrived);  // ֱ�ӷ���ģ����
	void Tick();
	void ReceiveData(string unitName, string value);  // ��CDB����������ݣ����ַ�������
	bool isAllFree();
	void InsertOutput(vector<string>& table);
};