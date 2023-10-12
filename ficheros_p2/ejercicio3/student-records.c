#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

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
    fread(cadena, sizeof(char), longitud + 1, file);

    return cadena;
}

int print_text_file(char *path)
{
	/* To be completed (part A) */

    FILE* input = fopen(path, "r");

    if (input == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    int n;

    if (fscanf(input, "%d%*c", &n) != 1) {
        fprintf(stderr, "Error al leer el número desde el archivo\n");
        fclose(input);
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        char* token;
        char lineptr[MAXLEN_LINE_FILE];  // Utilizamos una matriz en lugar de asignar memoria dinámica
        student_t cur_entry;

        // Leer una línea completa antes de separarla en tokens
        if (fgets(lineptr, sizeof(lineptr), input) == NULL) {
            fprintf(stderr, "Error al leer la línea desde el archivo\n");
            fclose(input);
            return 1;
        }

        // Eliminar el salto de línea al final de la línea si existe
        size_t len = strlen(lineptr);
        if (len > 0 && lineptr[len - 1] == '\n') {
            lineptr[len - 1] = '\0';
        }

        token_id_t token_id = STUDENT_ID_IDX;

        char* saved_lineptr = strdup(lineptr);  // Guardamos una copia para evitar afectar la original

        while ((token = strsep(&saved_lineptr, ":")) != NULL) {
            switch (token_id) {
            case STUDENT_ID_IDX:
                if (sscanf(token, "%d", &cur_entry.student_id) != 1) {
                    fprintf(stderr, "Warning: Wrong format for ID field in line %d\n", i);
                    cur_entry.student_id = 0;
                }
                break;
            case NIF_IDX:
                // Usar strndup para copiar solo los primeros 19 caracteres
                cur_entry.NIF[sizeof(cur_entry.NIF) - 1] = '\0';
                strncpy(cur_entry.NIF, token, sizeof(cur_entry.NIF) - 1);
                break;
            case FIRST_NAME_IDX:
                cur_entry.first_name = strdup(token);
                break;
            case LAST_NAME_IDX:
                cur_entry.last_name = strdup(token);
                break;
            default:
                break;
            }

            token_id++;
        }

		fprintf(stdout,"[Entry #%d]\n",i);
        fprintf(stdout, "\tstudent_id=%d\n", cur_entry.student_id);
        fprintf(stdout, "\tNIF=%s\n", cur_entry.NIF);
        fprintf(stdout, "\tfirst_name=%s\n", cur_entry.first_name);
        fprintf(stdout, "\tlast_name=%s\n", cur_entry.last_name);

        // Liberar la memoria asignada para la línea
        free(saved_lineptr);
    }

    fclose(input);

    return 0;

}

int print_binary_file(char *path)
{
	/* To be completed  (part C) */
	FILE* in;
    in = fopen(path, "rb");
    int n;
    student_t* students;

    // Leer los datos del fichero binario
    fread(&n, sizeof(n), 1, in);
    students = malloc(sizeof(*students) * n);

    for(int i = 0; i < n; ++i) {
        fread(&students[i].student_id, sizeof(students[i].student_id), 1, in);
        strcpy(students[i].NIF, loadstr(in));
        students[i].first_name = loadstr(in);
        students[i].last_name = loadstr(in);
    }

     // Imprimirlos por pantalla
    for (int i = 0; i < n; ++i)
	{
		printf("[Entry #%d]\n", i);
		printf("\tstudent_id=%d\n", students[i].student_id);
        printf("\tNIF=%s\n", students[i].NIF);
        printf("\tfirst_name=%s\n", students[i].first_name);
        printf("\tlast_name=%s\n", students[i].last_name);
		
        free(students[i].first_name);
		free(students[i].last_name);
	}
	free(students);

	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part B) */
	FILE *in, *out;
    const char *modoApertura = "r";  // Modo binario para el archivo de entrada
    int n;
    student_t* students;

    /* Open file */
    if ((in = fopen(input_file, modoApertura)) == NULL) {
        printf("El archivo de entrada %s no se pudo abrir", input_file);
        return 1;
    }

    out = fopen(output_file, "wb");  // Modo binario para el archivo de salida

    fscanf(in, "%d%*c", &n); // Leemos el numero de estudiantes
    students = malloc(sizeof (*students) * n);

    for (int i = 0; i < n; ++i) { // Leemos los datos de los estudiantes
        char line[MAXLEN_LINE_FILE];
        char* next = line;
        fgets(line, sizeof(line), in);

        students[i].student_id = atoi(strsep(&next, ":\n"));
        strcpy(students[i].NIF, strsep(&next, ":\n"));
        students[i].first_name = strdup(strsep(&next, ":\n"));
        students[i].last_name = strdup(strsep(&next, ":\n"));
    }

    fclose(in);

    // Escribimos el archivo binario
    fwrite(&n, sizeof(n), 1, out);

    for (int i = 0; i < n; ++i) {
        fwrite(&students[i].student_id, sizeof(students[i].student_id), 1, out);
        fwrite(students[i].NIF, 1, strlen(students[i].NIF) + 1, out);
        fwrite(students[i].first_name, 1, strlen(students[i].first_name) + 1, out);
	    fwrite(students[i].last_name, 1, strlen(students[i].last_name) + 1, out);
        free(students[i].first_name);
		free(students[i].last_name);
    }

    free(students);

    fclose(out);
    return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:po:b")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file | -o <output_file> | -b ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		/**
		 **  To be completed ...
		 **/
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		case 'o':
			options.output_file = optarg;
			options.action = WRITE_BINARY_ACT;
			break;
        case 'b':
            options.action = PRINT_BINARY_ACT;
            break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
