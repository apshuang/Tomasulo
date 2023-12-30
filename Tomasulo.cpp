#include "ReorderBuffer.h"

int main() {
    FILE* stream;
    freopen_s(&stream, "input.txt", "r", stdin);
    FILE* stream2;
    freopen_s(&stream2, "output.txt", "w", stdout);
    TomasuloWithROB tomasulo;
    for (int i = 1; i <= 40; i++) {
        tomasulo.Tick();
    }
    fclose(stdin);
    fclose(stdout);
    return 0;
}
