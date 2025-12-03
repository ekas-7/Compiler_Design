#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child Process running...\n");
    } else if (pid > 0) {
        wait(NULL);
        printf("Parent Process after child termination.\n");
    } else {
        printf("Fork failed!\n");
    }

    return 0;
}
