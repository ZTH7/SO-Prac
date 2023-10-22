#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr,"Usage: %s <input_file_name> <output_file_name>\n",argv[0]);
		exit(1);
	}

	const char *in_path = argv[1], *out_path = argv[2];

	int input = open(in_path, O_RDONLY);

	if (input == -1) {
		perror("Error opening file for reading");
        exit(EXIT_FAILURE);
	}

	int out = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (out == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

	copy(input, out);

	return 0;
}
