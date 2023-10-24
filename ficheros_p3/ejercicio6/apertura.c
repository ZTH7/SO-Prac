#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BLOCK_SIZE 512

void testFile(char* path, int mode) {
	int file = open(path, mode | O_CREAT, 0666);
	if (file == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

	char *str = "Hello World";

	// Write the block of data to the output file
	ssize_t bytesWritten = write(file, str, strlen(str));

	if (bytesWritten == -1) {
        perror("Error writing to the file");
        close(file);
        exit(EXIT_FAILURE);
    }

	lseek(file, strlen(str), SEEK_SET);

	// Buffer to store data read from the file
    char buffer[BLOCK_SIZE];

	// Read in a loop until the end of the file
	ssize_t bytesRead = read(file, buffer, BLOCK_SIZE);

	if (bytesRead == -1) {
        perror("Error reading from the file");
        close(file);
        exit(EXIT_FAILURE);
    }

	// Close file descriptors when done
    close(file);
}

int main(int argc, char *argv[])
{
	int opt, mode = -1;
	char* input_file = NULL;

	if (argc == 1) {
		fprintf(stderr, "Usage: %s -f <file> [ -r | -w ]\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	while ((opt = getopt(argc, argv, "f:rw")) != -1)
	{
		switch (opt)
		{
		case 'f':
			input_file = optarg;
			break;
		case 'r':
			mode += 1;
			break;
		case 'w':
			mode += 2;
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (input_file == NULL) {
		fprintf(stderr, "Must specify one input file as an argument of -f\n");
		exit(EXIT_FAILURE);
	}
	else if (mode == -1) {
		fprintf(stderr, "Must indicate one of the following options: -r, -w\n");
		exit(EXIT_FAILURE);
	}

	testFile(input_file, mode);

	return 0;
}
