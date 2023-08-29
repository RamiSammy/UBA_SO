#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

enum { WRITE, READ };
enum { PARENT, CHILD };

int pipes[2][2];

void bsend(pid_t dst, int msg) {
    pid_t ppid = getppid();
    int owner = ppid == dst ? CHILD : PARENT;
    write(pipes[owner][WRITE], &msg, sizeof(msg));
    if(owner == PARENT) printf("Padre envía a Hijo el valor %d\n", msg);
    else printf("Hijo envía a Padre el valor %d\n", msg);
}

int breceive(pid_t src) {
    pid_t ppid = getpid();
    int owner = ppid == src ? CHILD : PARENT;
    int valor;
    read(pipes[owner][READ], &valor, sizeof(valor));
}

void parent(pid_t child_pid) {
    for(int i = 0; i < 5; i++) {
        bsend(child_pid, i);
        sleep(1);
    }
        

    exit(0);
}

void child() {
    pid_t ppid = getppid();

    for(int i = 0; i < 5; i++){
        bsend(ppid, i);
        sleep(1);
    }
    
    exit(0);
}

int main(int argc, char const *argv[]) {
    pipe(pipes[0]);
    pipe(pipes[1]);
    
    pid_t pid = fork();

    if (pid == -1) exit(EXIT_FAILURE);
    if (pid == 0) child();
    else parent(pid);

    return 0;
}
