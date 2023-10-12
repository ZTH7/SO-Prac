#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
	/* To be completed */
	// Obtener la posición actual en el archivo
    long posicionInicial = ftell(file);

    // Contar el número de caracteres en la cadena
    int longitud = 0;
    char caracter;
    while (fread(&caracter, sizeof(char), 1, file) == 1 && caracter != '\0') {
        longitud++;
    }

	if (longitud == 0) {
		return NULL;
	}
    // Restaurar la posición inicial en el archivo
    fseek(file, posicionInicial, SEEK_SET);

    // Reservar memoria para la cadena
    char* cadena = (char*)malloc((longitud + 1) * sizeof(char));
	

    // Leer la cadena completa
    fread(cadena, sizeof(char), longitud, file);

    return cadena;
}

int main(int argc, char *argv[])
{
	/* To be completed */
	if (argc != 2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	FILE* file;

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	char* cadena;
	while((cadena = loadstr(file)) != NULL) {
		printf("%s\n", cadena);
		free(cadena);
	}

	return 0;
}
