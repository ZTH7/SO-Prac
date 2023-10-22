#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define BLOCK_SIZE 512

void copy(int fdo, int fdd)
{
	// Buffer to store data read from the file
    char buffer[BLOCK_SIZE];

    // Read in a loop until the end of the file
    ssize_t bytesRead;
    while ((bytesRead = read(fdo, buffer, BLOCK_SIZE)) > 0) {
        // Write the block of data to the output file
        ssize_t bytesWritten = write(fdd, buffer, bytesRead);

        if (bytesWritten == -1) {
            perror("Error writing to output file");
            close(fdo);
            close(fdd);
            exit(EXIT_FAILURE);
        }
    }

	if (bytesRead == -1) {
        perror("Error reading from input file");
        close(fdo);
        close(fdd);
        exit(EXIT_FAILURE);
    }

	 // Close file descriptors when done
    close(fdo);
    close(fdd);
}

void copy_regular(char *orig, char *dest)
{
	int source_fd, dest_fd;

    // Abrir el archivo de origen para lectura
    source_fd = open(orig, O_RDONLY);
    if (source_fd == -1) {
        perror("Error al abrir el archivo de origen");
        exit(EXIT_FAILURE);
    }

    // Crear el archivo de destino para escritura
    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        perror("Error al abrir el archivo de destino");
        close(source_fd);
        exit(EXIT_FAILURE);
    }

    copy(source_fd, dest_fd);
}

void copy_symlink(const char *source, const char *destination) {
    // Obtener información sobre el enlace simbólico original
    struct stat source_info;
    if (lstat(source, &source_info) == -1) {
        perror("Error al obtener información del enlace simbólico original");
        exit(EXIT_FAILURE);
    }

    // Reservar memoria para la ruta apuntada por el enlace simbólico
    char *buffer = (char *)malloc(source_info.st_size + 1);
    if (buffer == NULL) {
        perror("Error al reservar memoria para la copia de la ruta");
        exit(EXIT_FAILURE);
    }

    // Leer la ruta apuntada por el enlace simbólico
    ssize_t read_size = readlink(source, buffer, source_info.st_size);
    if (read_size == -1) {
        perror("Error al leer el enlace simbólico original");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Añadir manualmente el caracter null de final de cadena
    buffer[read_size] = '\0';

    // Crear un nuevo enlace simbólico
    if (symlink(buffer, destination) == -1) {
        perror("Error al crear el nuevo enlace simbólico");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // Liberar la memoria
    free(buffer);
}

int main(int argc, char *argv[])
{

	if (argc != 3) {
		fprintf(stderr,"Usage: %s <input_file_name> <output_file_name>\n",argv[0]);
		exit(1);
	}

	char *file_path = argv[1], *output_path = argv[2];
    struct stat file_info;

    // Utiliza lstat para obtener información sobre el archivo
    if (lstat(file_path, &file_info) == -1) {
        perror("Error obteniendo información del archivo");
        return 1;
    }

    // Verifica el tipo de archivo
    if (S_ISREG(file_info.st_mode)) {

        printf("%s es un fichero regular.\n", file_path);
		copy_regular(file_path, output_path);

    } else if (S_ISLNK(file_info.st_mode)) {

        printf("%s es un enlace simbólico. \n", file_path);
		copy_symlink(file_path, output_path);

    } else {

        fprintf(stderr, "%s es otro tipo de archivo. \n", file_path);
		exit(EXIT_FAILURE);

    }

	return 0;
}
