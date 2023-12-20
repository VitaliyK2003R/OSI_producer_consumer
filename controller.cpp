#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int produced_count = 0;

void producer_handler(int signum) {
    std::cerr << "Produced: " << produced_count << std::endl;
}

int main() {
    signal(SIGUSR1, producer_handler);

    int pipe1[2], pipe0[2], pipe2[2];

    pipe(pipe1);
    pipe(pipe0);
    pipe(pipe2);

    pid_t pid1 = fork();

    if (pid1 == 0) {
        close(pipe1[0]);
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);

        execve("./producer", nullptr, nullptr);
        exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) {  // P2 process
        close(pipe0[1]);
        dup2(pipe0[0], STDIN_FILENO);
        close(pipe0[0]);

        close(pipe2[0]);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);

        execve("/usr/bin/bc", nullptr, nullptr);
        exit(EXIT_SUCCESS);
    }

    close(pipe1[1]);
    close(pipe0[0]);
    close(pipe2[1]);

    char buffer[1024];

    while (true) {
        ssize_t bytesRead = read(pipe1[0], buffer, sizeof(buffer));

        if (bytesRead == 0) {
            break;
        }

        write(pipe0[1], buffer, bytesRead);

        bytesRead = read(pipe2[0], buffer, sizeof(buffer));
        buffer[bytesRead] = '\0';

        std::string expression(buffer);
        std::cout << expression << std::endl;

        produced_count++;

        kill(getpid(), SIGUSR1);
    }

    close(pipe1[0]);
    close(pipe0[1]);
    close(pipe2[0]);

    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    return 0;
}
