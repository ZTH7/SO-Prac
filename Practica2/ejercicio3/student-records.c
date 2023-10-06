#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

student_t* parse_recrods(char *path, int* nr_entries)
{
	FILE* file;
	char line[MAX_PASSWD_LINE+1];
	student_t* entries;
	student_t* cur_entry;
	char* token;
	char* lineptr;
	char* endptr;
	token_id_t token_id;
	int entry_idx=0;
	
	if ((file=fopen(path,"r"))==NULL) {
		fprintf(stderr, "The input file %s could not be opened: ",
				path);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	/* Figure out number of lines */
	fgets(line, MAX_PASSWD_LINE + 1, file);
	(*nr_entries) = strtol(line, &endptr, 10);

	entries=malloc(sizeof(student_t)*(*nr_entries));
	/* zero fill the array of structures */
	memset(entries,0,sizeof(student_t)*(*nr_entries));

	/* Parse file */
	while (fgets(line, MAX_PASSWD_LINE + 1, file) != NULL) {
		/* Point to the beginning of the line */
		lineptr=line;
		token_id=STUDENT_ID_IDX;
		cur_entry=&entries[entry_idx];

		while((token = strsep(&lineptr, ":"))!=NULL) {
			switch(token_id) {
			case STUDENT_ID_IDX:
				cur_entry->student_id=strtol(token, &endptr, 10);
				break;
			case NIF_IDX:
				strcpy(cur_entry->NIF,token);
				break;
			case FIRST_NAME_IDX:
				cur_entry->first_name=strdup(token);
				break;
			case LAST_NAME_IDX:
				cur_entry->last_name=strdup(token);
				break;
			default:
				break;
			}
			token_id++;
		}
		if (token_id!=NR_FIELDS_STUDENT) {
			fprintf(stderr, "Could not process all tokens from line %d of %s\n",entry_idx+1,path);
			return NULL;
		}
		entry_idx++;
	}
	
	return entries;
}

static void free_entries(student_t* entries, int nr_entries)
{
	int i=0;
	student_t* entry;

	for (i=0; i<nr_entries; i++) {
		entry=&entries[i]; /* Point to current entry */
		free(entry->first_name);
		free(entry->last_name);
	}

	free(entries);
}

char *loadstr(FILE *file)
{
	/* To be completed */
	int length = 0, num = 0;
	char *c = malloc(1);
	
	while ((num = fread(c, 1, 1, file)) == 1) {
		length++;
	}

	char *res = malloc(length + 1);
	fseek(file,0,SEEK_SET);
	fread(res, 1, length, file);

	return res;
}

int print_text_file(char *path)
{
	/* To be completed (part A) */
	int nr_entries;
	student_t* entries=parse_recrods(path,&nr_entries);

	if (!entries)
		return EXIT_FAILURE;

	for (int i=0; i<nr_entries; i++) {
		student_t* e=&entries[i];

		fprintf(stdout,"[Entry #%d]\n",i);
		fprintf(stdout,"\tstudent_id=%d\n\tNIF=%s\n\t"
		        "first_name=%s\n\tlast_name=%s\n",
		        e->student_id, e->NIF,
		        e->first_name, e->last_name);
	}

	free_entries(entries,nr_entries);

	return 0;
}

int write_binary_file(char *input_file, char *output_file)
{
	/* To be completed  (part B) */
	FILE *outfile;
	if ((outfile=fopen(output_file,"wx"))==NULL) {
		fprintf(stderr, "The output file %s could not be opened: ",
		        output_file);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	int nr_entries;
	student_t* entries=parse_recrods(input_file,&nr_entries);

	if (!entries)
		return EXIT_FAILURE;

	fwrite(&nr_entries, sizeof(int), 1, outfile);
	for (int i=0; i<nr_entries; i++) {
		student_t* e=&entries[i];
		fwrite(&e->student_id, sizeof(int), 1, outfile);
		fwrite(&e->NIF, sizeof(char), MAX_CHARS_NIF + 1, outfile);
		fwrite(e->first_name, sizeof(char), strlen(e->first_name) + 1, outfile);
		fwrite(e->last_name, sizeof(char), strlen(e->last_name), outfile);
	}

	free_entries(entries,nr_entries);

	return 0;
}

int print_binary_file(char *path)
{
	/* To be completed  (part C) */
	FILE *file;
	if ((file=fopen(path,"r"))==NULL) {
		fprintf(stderr, "The input file %s could not be opened: ",
				path);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	char *str=loadstr(file);

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
	while ((opt = getopt(argc, argv, "hpi:o:ba")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -p | -i file | -o <output_file> | -b ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
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
