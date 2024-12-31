#include "Headers/Tomasulo.h"

int main() {
    //这里是Visual Studio文件输入输出写法，在其他IDE上可能需要修改
    FILE* stream;
    freopen_s(&stream, "Datas/doubleIssue_withoutROB_input.txt", "r", stdin);
    FILE* stream2;
    freopen_s(&stream2, "Datas/doubleIssue_withoutROB_output.txt", "w", stdout);

    Tomasulo* tomasulo = new Tomasulo();
    int cycle = 0;
    while(1){
        cycle++;
        bool stop = tomasulo->Tick();
        if (stop || cycle >= 1000)break;
    }
    tomasulo->OutputInstructionTime();
    //tomasulo->FoldingOutput();
    fclose(stdin);
    fclose(stdout);
    return 0;
}
