#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define BUFFER_SIZE 4096

int main(int argc, char* argv[]) {
	FILE* file=NULL;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead;

	/* Read file byte by byte */
	while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        size_t bytesWritten = fwrite(buffer, 1, bytesRead, stdout);

        if (bytesWritten != bytesRead) {
            fclose(file);
            err(3, "Error al escribir en stdout");
        }
    }

	fclose(file);
	return 0;
}
