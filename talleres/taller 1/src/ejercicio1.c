#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int n;
int numero_maldito;
int numero_hijo;
int pids[10];

int generate_random_number(){
	return (rand() % n);
}

void signal_handler() {
    int rng = generate_random_number();
    if (rng == numero_maldito) {
        printf("Ultimas palabras ...\n");
        exit(EXIT_SUCCESS);
    }
    return;
}

void signal_handler_status() {
    // pid_t pid = wait(NULL); Para obtener el pid del proceso que acaba de cambiar
    printf("Cambio de estado el hijo %d\n",numero_hijo);
    pids[numero_hijo] = -2;
}

void hijo() {
    signal(SIGTERM, signal_handler);
    while(1);
}

void jugar(int k) {
    signal(SIGCHLD, signal_handler_status);

    // Crea los hijos
    for(int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) exit(EXIT_FAILURE);
        if (pid == 0) hijo();
        else pids[i] = pid;
    }

    printf("Empieza la ruleta\n");

    // Empieza la ruleta
    for(int i = 0; i < k; i++) {
        printf("Ronda %d\n",i+1);
        for(int j = 0; j < n; j++) {
            if(pids[j] != -2) {
                numero_hijo = j;
                kill(pids[j], SIGTERM);
                sleep(1);
            }   
        }
    }

    // Notifica los ganadores y termina de matar
    printf("Ganadores:\n");
    for(int i = 0; i < n; i++) {
        if (pids[i] != -2) {
            printf("Hijo #%d - %d\n",pids[i],i+1);
            kill(pids[i], SIGKILL);
        }
    }
}

int main(int argc, char const *argv[]){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL));
    
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
	numero_maldito = atoi(argv[3]);

    // Completar
    jugar(rondas);

    exit(0);
}