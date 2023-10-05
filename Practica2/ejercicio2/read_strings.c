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
	int length = 0, num = 0;
	char *c = malloc(1);
	
	while ((num = fread(c, 1, 1, file)) == 1) {
		length++;
	}

	fseek(file,0,SEEK_SET);

	char *res = malloc(length + 1);

	fread(res, 1, length, file);

	return res;
}

int main(int argc, char *argv[])
{
	/* To be completed */
	FILE *file;
	char *str, *token;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	str = loadstr(file);

	while((token = strsep(&str, "."))!=NULL) {
		printf("%s\n", token);
	}

	return 0;
}
