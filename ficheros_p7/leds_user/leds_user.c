#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void write_to_device(char input) {
    int fd;
    fd = open("/dev/leds", O_WRONLY);

    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &input, 1) == -1) {
        perror("Error writing to file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

int main() {
    int num = 10;
    while (num-- > 0) {
        write_to_device('1');
        sleep(1);
        write_to_device('2');
        sleep(1);
        write_to_device('3');
        sleep(1);
    }

    return 0;
}
