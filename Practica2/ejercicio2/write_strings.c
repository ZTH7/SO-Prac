#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	/* To be completed */
	FILE *file;
	char *espacio = ".";
	if (argc > 1) {
		if ((file=fopen(argv[1],"wx"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        argv[1]);
				perror(NULL);
				exit(EXIT_FAILURE);
			}
	}
	else {
		fprintf(stderr, "Parameters missing");
				perror(NULL);
				exit(EXIT_FAILURE);
	}
	for(int i = 2; i < argc; i++) {
		fwrite(argv[i], 1, strlen(argv[i]), file);
		fwrite(espacio, 1, 1, file);
	}

	fclose(file);
	return 0;
}
