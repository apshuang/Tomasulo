#include "LoadBuffer.cpp"
#include "Registers.cpp"
#include "ReservationStation.cpp"

class ReorderBufferLine {
private:
    // 该模块中有Busy位、对应指令、指令执行状态，还有寄存器的值
    int id;
    int busy;
    string instructionType;
    string instuctionDestination;
    string instructionOperand1;
    string instructionOperand2;
    int state;
    float value;
    string valueString; // 由于该作业的特殊性，设置该变量用于表示#2-#1这类值
    void Reset() {
        busy = 0;
        instructionType = "";
        instuctionDestination = "";
        instructionOperand1 = "";
        instructionOperand2 = "";
        state = FREE;
        value = 0;
        valueString = "";
    }
public:
    ReorderBufferLine() {
        Reset();
    }
    void SetID(int ID) {
        id = ID;
    }
    void Tick(TomasuloWithROB& tomasulo) {
        switch (state) {
        case FREE:
            FetchAndIssue(tomasulo.instructionDecoder);
            break;
        case ISSUE:
            // 在取得对应的指令之后，根据指令类型发射去对应的模块处
            InstuctionDecoder decoder = tomasulo.instructionDecoder;
            int instModule = decoder.GetInstructionType(instructionType);
            switch (instModule) {
            case 1:
                int result = tomasulo.loadBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), id);
                if (result != -1) {
                    state = EXEC;
                }
                else {
                    // 如果返回值为-1，说明LoadBuffer内部没有空位，所以执行失败
                    // 执行失败则不做状态变化，那么下次将继续尝试
                }
                break;
            default:
                break;

            }
        }

    }
    void FetchAndIssue(InstuctionDecoder& decoder) {
        // 需接受一个decoder的指针，用来调用对应的解码函数
        string s = "";
        cin >> s;
        if (s == "")return; // 已经没有新的指令了
        instructionType = s;
        int operandNum = decoder.GetOperandNum(instructionType);
        switch (operandNum) {
        case 1:
            cin >> instuctionDestination;
            break;
        case 2:
            cin >> instuctionDestination;
            cin >> instructionOperand1;
            break;
        case 3:
            cin >> instuctionDestination;
            cin >> instructionOperand1;
            cin >> instructionOperand2;
            break;
        default:
            break;
        }
        state = ISSUE;
    }
};



class ReorderBuffer {
private:
    // 该模块使用循环队列实现，若head==tail说明队列为空，若head==tail+1(mod ENTRYNUM)说明队列已满
    int head;
    int tail;
    ReorderBufferLine entry[ENTRYNUM];
public:
    ReorderBuffer() {
        head = 0;
        tail = 0;
        for (int i = 0; i < ENTRYNUM; i++) {
            entry[i] = ReorderBufferLine();
            entry[i].SetID(i);
        }

    }
    int isFree() {
        if (head == ((tail + 1) % ENTRYNUM)) {
            return -1; //队列已满
        }
        else {
            tail += 1;
            tail %= ENTRYNUM;
            return tail;
        }
    }
};


static class InstuctionDecoder {
private:
    unordered_map<string, int> OperandNum; // 定义某条指令的操作数数量
    unordered_map<string, int> InstructionModule; // 定义某条指令的调用的模块，Load为1，Store为2，Adder为3，Multiplier为4
public:
    InstuctionDecoder() {
        OperandNum.clear();
        InstructionModule.clear();

        //在本实验中，全部都是三操作数的指令
        for (auto Instruction : OperandNum3Instuction) {
            OperandNum[Instruction] = 3;
        }

        InstructionModule["LD"] = LOAD; //Load指令类型为1
        InstructionModule["SD"] = STORE; //Store指令类型为2
        InstructionModule["ADDD"] = ADDER;
        InstructionModule["SUBD"] = ADDER; // 调用ADD模块的指令类型为3
        InstructionModule["MULTD"] = MULTIPLIER;
        InstructionModule["DIVD"] = MULTIPLIER; // 调用MULT模块的指令类型为4
    }

    int GetOperandNum(string instructionType) {
        return OperandNum[instructionType];
    }
    int GetInstructionType(string instructionType) {
        return InstructionModule[instructionType];
    }
    int GetOffset(string instructionOperand) {
        return stoi(instructionOperand);
    }
};

class TomasuloWithROB {
public:
    // 此处整合了前面所有模块，组成一个系统，并包括对应的cycle
    ReorderBuffer reorderBuffer;
    LoadBuffer loadBuffer;
    ReservationStationADD reservationStationADD;
    ReservationStationMULT reservationStationMULT;
    Registers registers;
    InstuctionDecoder instructionDecoder;
    int cycle;
    TomasuloWithROB() {
        reorderBuffer = ReorderBuffer();
        loadBuffer = LoadBuffer();
        reservationStationADD = ReservationStationADD();
        reservationStationMULT = ReservationStationMULT();
        registers = Registers();
        instructionDecoder = InstuctionDecoder();
        cycle = 0;
    }
};