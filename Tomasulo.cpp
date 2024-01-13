#include "ReorderBuffer.h"

int main() {
    //这里是Visual Studio文件输入输出写法，在其他IDE上可能需要修改
    FILE* stream;
    freopen_s(&stream, "input2.txt", "r", stdin);
    FILE* stream2;
    freopen_s(&stream2, "output2.txt", "w", stdout);

    TomasuloWithROB tomasulo;
    int cycle = 0;
    while(1){
        cycle++;
        int stop = tomasulo.Tick();
        if (stop || cycle >= 1000)break;
    }
    tomasulo.OutputAll();
    fclose(stdin);
    fclose(stdout);
    return 0;
}
