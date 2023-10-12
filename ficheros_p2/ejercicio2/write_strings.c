#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	FILE* file = NULL;
	const char *modoApertura = "w";  // "w" indica escritura (crea un archivo si no existe, trunca el archivo si ya existe)

	/* To be completed */
	if (argc<2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "w")) == NULL)
		file = fopen(argv[1], modoApertura);

	for (int i = 2; argv[i] != NULL; ++i) {
		fwrite(argv[i], 1, strlen(argv[i]) + 1, file);
	}

	fclose(file);
	
	return 0;
}
