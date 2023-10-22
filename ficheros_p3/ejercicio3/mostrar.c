#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1

void mostrar_archivo(const char *nombre_archivo, off_t offset, int mostrar_ultimos) {
    int fd = open(nombre_archivo, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Obtener la longitud del archivo
    off_t longitud = lseek(fd, 0, SEEK_END); // Toma un descriptor de archivo, un desplazamiento (offset) y una referencia a la posición desde donde se hará el desplazamiento.
    if (longitud == -1) {
        perror("Error al obtener la longitud del archivo");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Calcular la nueva posición según los parámetros proporcionados
    off_t nueva_posicion;
    if (mostrar_ultimos) {
        nueva_posicion = longitud - offset;
    } else {
        nueva_posicion = offset;
    }

    // Situar el marcador de posición en la posición calculada
    if (lseek(fd, nueva_posicion, SEEK_SET) == -1) {
        perror("Error al situar el marcador de posición");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Leer byte a byte y escribir en la salida estándar
    char buffer[BUFFER_SIZE];
    ssize_t bytes_leidos;

    while ((bytes_leidos = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_leidos);
    }

    if (bytes_leidos == -1) {
        perror("Error al leer el archivo");
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    int opcion;
    int mostrar_ultimos = 0;  // 1 si se proporcionó el flag -e, 0 en caso contrario
    off_t offset = 0;        // Desplazamiento desde el comienzo o el final del archivo

    while ((opcion = getopt(argc, argv, "en:")) != -1) {
        switch (opcion) {
            case 'e':
                mostrar_ultimos = 1;
                break;
            case 'n':
                offset = atoll(optarg);
                break;
            default:
                fprintf(stderr, "Uso: %s [-e] [-n N] archivo\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Se esperaba el nombre de un archivo como argumento\n");
        exit(EXIT_FAILURE);
    }

    const char *nombre_archivo = argv[optind];

    // Realizar la operación principal
    mostrar_archivo(nombre_archivo, offset, mostrar_ultimos);

    return 0;
}
