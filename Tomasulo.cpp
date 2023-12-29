#include "ReorderBuffer.cpp"

int main() {
    FILE* stream;
    freopen_s(&stream, "input.txt", "r", stdin);
    for (int i = 0; i < 10; i++) {
        int d = 0;
        cin >> d;
        cout << d;
    }
    fclose(stdin);
    return 0;
}
