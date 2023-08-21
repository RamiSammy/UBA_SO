#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void abraham(__pid_t child) {
    write(1, "Soy abraham\n", 13);
    sleep(1);
    wait(child);
    printf("Abraham termino\n");
}

void homer() {
    printf("Soy Homer\n");

    __pid_t pid = fork();
    int status;
    
    if (pid == -1) exit(EXIT_FAILURE);
    else if (pid == 0) {
        bart();
    } else {
        waitpid(pid, &status, 0);
    }

    pid = fork();
    if (pid == -1) exit(EXIT_FAILURE);
    else if (pid == 0) {
        lisa();
    } else {
        waitpid(pid, &status, 0);
    }

    pid = fork();
    if (pid == -1) exit(EXIT_FAILURE);
    else if (pid == 0) {
        maggie();
    } else {
        waitpid(pid, &status, 0);
    }

    printf("Homer termino\n");
    exit(EXIT_SUCCESS);
}

void bart() {
    printf("Soy Bart\n");
    exit(EXIT_SUCCESS);
}

void lisa() {
    printf("Soy Lisa\n");
    exit(EXIT_SUCCESS);
}

void maggie() {
    printf("Soy Maggie\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[]) {
    __pid_t pid = fork();
    
    if(pid == -1) exit(EXIT_FAILURE);
    else if (pid == 0) homer(); // hijo tiene pid = 0
    else abraham(pid);

    exit(EXIT_SUCCESS); // Finalizan todos los procesos
}
