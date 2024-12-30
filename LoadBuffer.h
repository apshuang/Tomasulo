#pragma once
#include "BasicDefine.h"

class CommonDataBus;

class LoadBufferLine {
private:
	// 该模块中有Busy位，Load指令的地址，以及具体取得的值，以及剩余时间
	int busy; // 该位为1时为busy，否则为空闲
	string base; // Base意为基址，此处记录Load指令基址对应的寄存器名字
	string offset; // Offset为偏移量，与Base共同组成地址
	string valueString; // 由于该作业的特殊性，设置该变量用于表示Mem[32 + Regs[x2]]这类值
	int remainingTime;
	float arrivedTime;  // 保证先到的先服务，float是为了区分双发射，双发射的第二条我们设为time+0.5
	string unitName;  // 指的是这个LBLine是第几个
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
	void ReceiveData(string unitName, string value);  // 从CDB那里接收数据
};

class LoadBuffer {
private:
	// 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
	int tail;
	LoadBufferLine* loadbuffers[LOADNUM];

	int IsFree();
public:
	LoadBuffer();
	
	string GetName(int index);
	string LoadIssue(string instBase, string instOffset, float arrived);  // 直接返回模块名
	void Tick();
	void ReceiveData(string unitName, string value);  // 从CDB那里接收数据，并分发给各行
	bool isAllFree();
	void InsertOutput(vector<string>& table);
};


class StoreBufferLine {
private:
	// 该模块中有Busy位，Load指令的地址，以及具体取得的值，以及剩余时间
	int busy; // 该位为1时为busy，否则为空闲
	string base; // Base意为基址，此处记录Load指令基址对应的寄存器名字
	string offset; // Offset为偏移量，与Base共同组成地址
	string valueString; // 由于该作业的特殊性，设置该变量用于表示Mem[32 + Regs[x2]]这类值
	int remainingTime;
	float arrivedTime;
	string unitName;  // 指的是这个LBLine是第几个
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
	void ReceiveData(string unitName, string value);  // 从CDB那里接收数据
	void Tick();
};

class StoreBuffer {
private:
	// 该模块使用伪循环队列实现，但是不关心head，只关心tail（因为要尽量负载均衡）
	int tail;
	StoreBufferLine* storebuffers[STORENUM];

	int IsFree();
public:
	StoreBuffer();
	string GetName(int index);
	string StoreIssue(string instBase, string instOffset, string instValue, float arrived);  // 直接返回模块名
	void Tick();
	void ReceiveData(string unitName, string value);  // 从CDB那里接收数据，并分发给各行
	bool isAllFree();
	void InsertOutput(vector<string>& table);
};