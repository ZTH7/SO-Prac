#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define MAX_THREADS 5

// Estructura para los argumentos del hilo
typedef struct {
    int numero_hilo;
    char prioridad;
} ArgumentosHilo;

// Función que ejecutarán los hilos
void *thread_usuario(void *arg) {
    // Obtener los argumentos del hilo
    ArgumentosHilo *argumentos = (ArgumentosHilo *)arg;

    // Copiar los argumentos en variables locales
    int numero_hilo = argumentos->numero_hilo;
    char prioridad = argumentos->prioridad;

    // Liberar la memoria dinámica reservada para los argumentos
    free(argumentos);

    // Averiguar el identificador del hilo
    pthread_t id_hilo = pthread_self();

    // Imprimir un mensaje con la información del hilo
    printf("Hilo ID: %lu, Numero de Hilo: %d, Prioridad: %c\n", id_hilo, numero_hilo, prioridad);

    // Terminar el hilo
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // Array para almacenar los identificadores de los hilos
    pthread_t hilos[MAX_THREADS];

    // Crear e iniciar los hilos
    for (int i = 0; i < MAX_THREADS; i++) {
        // Crear argumentos para el hilo usando memoria dinámica
        ArgumentosHilo *argumentos = (ArgumentosHilo *)malloc(sizeof(ArgumentosHilo));
        if (argumentos == NULL) {
            perror("Error al asignar memoria para los argumentos");
            exit(EXIT_FAILURE);
        }

        // Inicializar los argumentos
        argumentos->numero_hilo = i + 1;  // Hilo 1, 2, 3, ...
        argumentos->prioridad = (i % 2 == 0) ? 'P' : 'N';  // P para pares, N para impares

        // Crear el hilo y pasarle los argumentos
        if (pthread_create(&hilos[i], NULL, thread_usuario, (void *)argumentos) != 0) {
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }
    }

    // El main espera a que todos los hilos terminen
    for (int i = 0; i < MAX_THREADS; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            perror("Error al esperar al hilo");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
