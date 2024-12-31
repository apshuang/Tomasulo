#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <unordered_map>
#include <string>
#include <queue>
#include <iomanip>
using namespace std;

#define LOADNUM 2
#define STORENUM 2
#define ADDNUM 3  // 这个是reservationStation的数量
#define MULTNUM 2
#define ENTRYNUM 6
#define REGNUM 16
#define LOADCYCLE 2
#define STORECYCLE 2
#define ADDCYCLE 2
#define SUBCYCLE 2
#define MULTCYCLE 6
#define DIVCYCLE 12

#define ISSUENUM 2
#define ADDERNUM 1  // 这个是functionUnit的数量
#define MULTIPLIERNUM 1
#define LOADUNITNUM 1  // 为了方便，我们假设load指令和store指令使用的内存单元各有1个
#define STOREUNITNUM 1

enum ALLSTATE {
	FREE = 0,
	ISSUE = 1,
	EXEC = 2,
	WRITE = 3,
	COMMIT = 4
};

enum INSTRUCTIONTYPE {
	LOAD = 1,
	STORE = 2,
	ADDER = 3,
	MULTIPLIER = 4,
	BRANCH = 5
};

const vector<string> StateOutput = { "Free", "Issue", "Exec", "Write", "Commit" };
const vector<string> OperandNum3Instuction = { "LD", "SD","ADDD","SUBD","MULTD","DIVD" };
const unordered_map<string, int> ExecTime = {
		{"ADDD", ADDCYCLE},
		{"SUBD", SUBCYCLE},
		{"MULTD", MULTCYCLE},
		{"DIVD", DIVCYCLE},
		{"fadd.d", ADDCYCLE},
		{"fsub.d", SUBCYCLE},
		{"fmul.d", MULTCYCLE},
		{"fdiv.d", DIVCYCLE},
		{"addi", ADDCYCLE}
};
const unordered_map<string, string> OperatorSign = {
	{"ADDD", "+"},
	{"SUBD", "-"},
	{"MULTD", "*"},
	{"DIVD", "/"},
	{"fadd.d", "+"},
	{"fsub.d", "-"},
	{"fmul.d", "*"},
	{"fdiv.d", "/"},
	{"addi", "+"}
};


struct Instruction {
	string inst;
	int issueComplete;
	int executeComplete;

	Instruction(string ins, int issue, int exec) {
		inst = ins;
		issueComplete = issue;
		executeComplete = exec;
	}
};


inline string OffsetToString(int offset) {
	// 将数字形式的offset改成string形式的
	if (offset == 0)return "0";
	string result = "";
	while (offset) {
		result += (offset % 10) + '0';
		offset /= 10;
	}
	reverse(result.begin(), result.end());
	return result;
}

inline bool checkReady(vector<string> toCheck) {
	for (auto it : toCheck) {
		if (it.find("Load") != string::npos) return false;
		if (it.find("Add") != string::npos)return false;
		if (it.find("Mult") != string::npos)return false;
	}
	return true;
}

inline string centerString(const string& str, int width) {
	int totalPadding = width - str.length();
	int rightPadding = totalPadding / 2;
	int leftPadding = totalPadding - rightPadding;
	return string(leftPadding, ' ') + str + string(rightPadding, ' ');
}


inline void printHeader(const string& name, int width) {
	cout << string(width, '-') << endl;
	cout << "|" << centerString(name, width - 2) << "|" << endl;
	cout << string(width, '-') << "\n";
}