#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid;

void handle_alarm(int signo) {
    // Este manejador enviará una señal SIGKILL al proceso hijo
    // cuando reciba una señal de alarma (SIGALRM).
    kill(pid, SIGKILL);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int status;

    // Crear el proceso hijo
    pid = fork();

    if (pid == -1) {
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Código del proceso hijo
        execvp(argv[1], &argv[1]);  // Cambiar la imagen del proceso al comando especificado
        perror("Error en execvp");  // En caso de error en execvp
        exit(EXIT_FAILURE);
    } else {
        // Código del proceso padre
		// Ignorar la señal SIGINT
        //signal(SIGINT, SIG_IGN);

        // Establecer el manejador de la señal SIGALRM
        struct sigaction sa;
        sa.sa_handler = handle_alarm;
        sigaction(SIGALRM, &sa, NULL);

        // Programar una alarma para que envíe una señal después de 5 segundos
        alarm(5);

        pause(); // Espera a que acaben las señales

        // Esperar a que el hijo termine
        //waitpid(pid, &status, 0);

        // Si solo se hace un wait (sin pause) devuelve el estado del último proceso hijo
        wait(&status);

        /*Por eso, se puede meter dentro de un bucle hasta que sea pid del hijo
             while (wait(&status) != pid)
         */

        // Comprobar la causa de la finalización del hijo
        if (WIFEXITED(status)) {
            printf("El hijo ha terminado normalmente.\n");
        } else if (WIFSIGNALED(status)) {
            printf("El hijo ha sido terminado por una señal.\n");
        }
    }

    return 0;
}
