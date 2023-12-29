#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <unordered_map>
#include <string>
using namespace std;

#define LOADNUM 2
#define ADDNUM 3
#define MULTNUM 2
#define ENTRYNUM 6
#define REGNUM 16
#define LOADCYCLE 2
#define STORECYCLE 2
#define ADDCYCLE 2
#define SUBCYCLE 2
#define MULTCYCLE 10
#define DIVCYCLE 20

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
    MULTIPLIER = 4
};

const vector<string> StateOutput = { "Free", "Issue", "Exec", "Write", "Commit" };
const vector<string> OperandNum3Instuction = { "LD", "SD","ADDD","SUBD","MULTD","DIVD" };
