#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

void exec_cmd(char* cmd, char** p) {
	execvp(cmd, p);
}

void close_other_pipes(int child_id, size_t count, int** pipes) {
	for(int i = 0; i < count ; i++){
		if(i != child_id - 1) close(pipes[i][PIPE_READ]);
		if(i != child_id) close(pipes[i][PIPE_WRITE]);
	}
}

void child(int child_id, int** pipes, int last_child_id, char* cmd, char** p) {
	if (last_child_id > 0) {
		// Lo desconecto de los otros pipes
		close_other_pipes(child_id, last_child_id + 1, pipes);

		// Si es el primer programa cierro la lectura del pipe
		if(child_id == 0) {
			close(pipes[child_id][PIPE_READ]);
			dup2(pipes[child_id][PIPE_WRITE], STD_OUTPUT);
		}

		// Si es el último programa cierro la escritura del pipe 
		if(child_id == last_child_id) {
			close(pipes[child_id][PIPE_WRITE]);
			dup2(pipes[child_id][PIPE_READ], STD_INPUT);
		}

		// Conecto la escritura y la lectura con el standard output
		if(0 < child_id && child_id < last_child_id) {
			dup2(pipes[child_id][PIPE_WRITE], STD_OUTPUT);
			dup2(pipes[child_id - 1][PIPE_READ], STD_INPUT);
		} 
	}

	exec_cmd(cmd, p);
}

static int run(char ***progs, size_t count) {	
	int r, status;

	//Reservo memoria para el arreglo de pids
	pid_t *children = malloc(sizeof(*children) * count);
	int (*pipes)[2];
	pipes = malloc(sizeof(*pipes) * (count - 1));

	// Genero los pipes
	for(int i = 0; i < count; i++) pipe(pipes[i]);

	// Genero los procesos
	for(int i = 0; i < count; i++) {
		pid_t pid = fork();
		if(pid < 0) exit(EXIT_FAILURE);
		else if (pid == 0) child(i, pipes, count - 1, progs[i][0], progs[i]);
		else children[i] = pid;
	}

	// Cierro los pipes de los hijos
	for(int i = 0; i < count; i++) {
		close(pipes[i][PIPE_WRITE]);
		close(pipes[i][PIPE_READ]);
	}

	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}


int main(int argc, char **argv) {
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	char*** programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
