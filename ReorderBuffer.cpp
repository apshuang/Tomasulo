#include "LoadBuffer.cpp"
#include "Registers.cpp"
#include "ReservationStation.cpp"

class ReorderBufferLine {
private:
    // ��ģ������Busyλ����Ӧָ�ָ��ִ��״̬�����мĴ�����ֵ
    int id;
    int busy;
    string instructionType;
    string instuctionDestination;
    string instructionOperand1;
    string instructionOperand2;
    int state;
    float value;
    string valueString; // ���ڸ���ҵ�������ԣ����øñ������ڱ�ʾ#2-#1����ֵ
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
            // ��ȡ�ö�Ӧ��ָ��֮�󣬸���ָ�����ͷ���ȥ��Ӧ��ģ�鴦
            InstuctionDecoder decoder = tomasulo.instructionDecoder;
            int instModule = decoder.GetInstructionType(instructionType);
            switch (instModule) {
            case 1:
                int result = tomasulo.loadBuffer.LoadExecute(instructionOperand2, decoder.GetOffset(instructionOperand1), id);
                if (result != -1) {
                    state = EXEC;
                }
                else {
                    // �������ֵΪ-1��˵��LoadBuffer�ڲ�û�п�λ������ִ��ʧ��
                    // ִ��ʧ������״̬�仯����ô�´ν���������
                }
                break;
            default:
                break;

            }
        }

    }
    void FetchAndIssue(InstuctionDecoder& decoder) {
        // �����һ��decoder��ָ�룬�������ö�Ӧ�Ľ��뺯��
        string s = "";
        cin >> s;
        if (s == "")return; // �Ѿ�û���µ�ָ����
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
    // ��ģ��ʹ��ѭ������ʵ�֣���head==tail˵������Ϊ�գ���head==tail+1(mod ENTRYNUM)˵����������
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
            return -1; //��������
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
    unordered_map<string, int> OperandNum; // ����ĳ��ָ��Ĳ���������
    unordered_map<string, int> InstructionModule; // ����ĳ��ָ��ĵ��õ�ģ�飬LoadΪ1��StoreΪ2��AdderΪ3��MultiplierΪ4
public:
    InstuctionDecoder() {
        OperandNum.clear();
        InstructionModule.clear();

        //�ڱ�ʵ���У�ȫ����������������ָ��
        for (auto Instruction : OperandNum3Instuction) {
            OperandNum[Instruction] = 3;
        }

        InstructionModule["LD"] = LOAD; //Loadָ������Ϊ1
        InstructionModule["SD"] = STORE; //Storeָ������Ϊ2
        InstructionModule["ADDD"] = ADDER;
        InstructionModule["SUBD"] = ADDER; // ����ADDģ���ָ������Ϊ3
        InstructionModule["MULTD"] = MULTIPLIER;
        InstructionModule["DIVD"] = MULTIPLIER; // ����MULTģ���ָ������Ϊ4
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
    // �˴�������ǰ������ģ�飬���һ��ϵͳ����������Ӧ��cycle
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