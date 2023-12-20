#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

int main() {
    std::srand(std::time(0));

    int N = std::rand() % (180 - 120 + 1) + 120;

    for (int i = 0; i < N; ++i) {
        int X = std::rand() % 9 + 1;
        char op = "+-*/"[std::rand() % 4];
        int Y = std::rand() % 9 + 1;

        std::cout << X << ' ' << op << ' ' << Y << std::endl;
        sleep(1);
    }

    return 0;
}
