#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void system_(const char *arg) {
    pid_t pid = fork();
    int status;

    if(pid == -1) exit(EXIT_FAILURE);
    if(pid == 0) {
        execv(sizeof(arg), arg);
    } else {
        waitpid(pid, &status, 0);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[]) {
    system_(argv[1]);
    exit(EXIT_SUCCESS);
}
