#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include <err.h>
#include "defs.h"

/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

// Del ejercicio 2, read_strings.c
char* loadstr(FILE* file)
{
	char aux;
	int count = 0;

	// Cuenta el número de caracteres
	while (fread(&aux, sizeof(aux), 1, file) != 0 && aux != '\0') count++;
	if (count++ == 0) return NULL;

	// Retrocede el indicador de posición
	fseek(file, -count, SEEK_CUR);

	char* c = malloc(count);
	fread(c, sizeof(aux), count, file);

	return c;
}

student_t* read_student_text_file(FILE* f, int* n)
{
	student_t* students;
	fscanf(f, "%d%*c", n);

	students = malloc(sizeof(*students) * *n);
	for (int i = 0; i < *n; ++i)
	{
		char line[MAXLEN_LINE_FILE + 1];
		char* next = line;

		fgets(line, sizeof line, f);
		students[i].student_id = atoi(strsep(&next, ":\n"));
		strcpy(students[i].NIF, strsep(&next, ":\n"));
		students[i].first_name = strdup(strsep(&next, ":\n"));
		students[i].last_name  = strdup(strsep(&next, ":\n"));
	}
	return students;
}

student_t* read_student_binary_file(FILE* f, int* n)
{
	student_t* students;
	fread(n, sizeof *n, 1, f);

	students = malloc(sizeof(*students) * *n);
	for (int i = 0; i < *n; ++i)
	{
		char* NIF;
		fread(&students[i].student_id, sizeof students[i].student_id, 1, f);
		NIF = loadstr(f);
		strcpy(students[i].NIF, NIF);
		free(NIF);
		students[i].first_name = loadstr(f);
		students[i].last_name  = loadstr(f);
	}
	return students;
}

void print_student_entry(student_t* student)
{
		printf("\tstudent_id=%d\n", student->student_id);
		printf("\tNIF=%s\n", student->NIF);
		printf("\tfirst_name=%s\n", student->first_name);
		printf("\tlast_name=%s\n", student->last_name);
}

int print_text_file(char* path)
{
	int n;
	FILE* f = fopen(path, "rb");
	student_t* students = read_student_text_file(f, &n);
	fclose(f);

	for (int i = 0; i < n; ++i)
	{
		printf("[Entry #%d]\n", i);
		print_student_entry(&students[i]);

		free(students[i].first_name);
		free(students[i].last_name);
	}
	free(students);

	return 0;
}

void write_binary_entry(FILE* f, const student_t* student)
{
	fwrite(&student->student_id, sizeof student->student_id, 1, f);
	fwrite(student->NIF, 1, strlen(student->NIF) + 1, f);
	fwrite(student->first_name, 1, strlen(student->first_name) + 1, f);
	fwrite(student->last_name, 1, strlen(student->last_name) + 1, f);
}

void write_text_entry(FILE* f, const student_t* student)
{
	fprintf(f, "%d:%s:%s:%s\n", student->student_id,
								student->NIF,
								student->first_name,
								student->last_name);
}

int write_binary_file(char* input_file, char* output_file)
{
	int n;
	FILE* f = fopen(input_file, "r");
	student_t* students = read_student_text_file(f, &n);
	fclose(f);

	f = fopen(output_file, "wb");
	fwrite(&n, sizeof n, 1, f);
	for (int i = 0; i < n; ++i)
	{
		write_binary_entry(f, &students[i]);

		free(students[i].first_name);
		free(students[i].last_name);
	}
	free(students);

	return 0;
}

int print_binary_file(char* path)
{
	int n;
	FILE* f = fopen(path, "r");
	student_t* students = read_student_binary_file(f, &n);
	fclose(f);

	for (int i = 0; i < n; ++i)
	{
		printf("[Entry #%d]\n", i);
		print_student_entry(&students[i]);

		free(students[i].first_name);
		free(students[i].last_name);
	}
	free(students);

	return 0;
}

student_t* parse_argv_entries(char** entries, int n)
{
	student_t* students = malloc(sizeof *students * n);
	for (int i = 0; i < n; ++i)
	{
		char* next = entries[i];
		students[i].student_id = atoi(strsep(&next, ":"));
		strcpy(students[i].NIF, strsep(&next, ":"));
		students[i].first_name = strsep(&next, ":");
		students[i].last_name  = strsep(&next, ":");
	}
	return students;
}

void update_num_entries_txt(FILE* f, int n)
{
	char* buf;
	int n_orig;
	size_t fsize, content_sz;

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);

	freopen(NULL, "r+", f);
	fscanf(f, "%d", &n_orig);
	content_sz = fsize - ftell(f);
	buf = malloc(content_sz);
	fread(buf, 1, content_sz, f);

	fseek(f, 0, SEEK_SET);
	fprintf(f, "%d", n_orig + n);
	fwrite(buf, 1, content_sz, f);
	free(buf);
}

void update_num_entries_bin(FILE* f, int n)
{
	int n_orig;
	freopen(NULL, "r+", f);
	fread(&n_orig, sizeof n_orig, 1, f);
	n_orig += n;
	fseek(f, 0, SEEK_SET);
	fwrite(&n_orig, sizeof n_orig, 1, f);
}

int append_entries(char* path, char** entries, int n)
{
	FILE* f = fopen(path, "a");
	student_t* students = parse_argv_entries(entries, n);

	if (strcmp(&path[strlen(path) - 4], ".txt") == 0)
	{
		for (int i = 0; i < n; ++i)
			write_text_entry(f, &students[i]);
		update_num_entries_txt(f, n);
	}
	else
	if (strcmp(&path[strlen(path) - 4], ".bin") == 0)
	{
		for (int i = 0; i < n; ++i)
			write_binary_entry(f, &students[i]);
		update_num_entries_bin(f, n);
	}
	else
		errx(EXIT_FAILURE, "Formato no reconocido\n");

	free(students);
	printf("%d records written succesfully to existing text file\n", n);

	return 0;
}

int main(int argc, char** argv)
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:po:ba")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i <file> | -o <output_file> | -b | -a ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		case 'o':
			options.action = WRITE_BINARY_ACT;
			options.output_file = optarg;
			break;
		case 'b':
			options.action = PRINT_BINARY_ACT;
			break;
		case 'a':
			options.action = APPEND_ENTRIES_ACT;
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
	case APPEND_ENTRIES_ACT:
		/* Optional 2 */
		ret_code = append_entries(options.input_file, &argv[optind], argc - optind);
		break;
	default:
		break;
	}
	exit(ret_code);
}
