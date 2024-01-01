#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>

struct options {
    char *progname;
    int recurse;
};

struct options opt;

void list_dir_recurse(char *name);

void usage(void)
{
    printf("%s [options] [dirname]\n\n", opt.progname);
    printf("lists the contents of dirname (default .)\n");
    printf("options:\n"
           "\t-h:\tshow this help\n"
           "\t-R:\trecursive\n");
}

/* Apartado b */
void list_dir(char *name)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(name)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}

/* Apartado c */
void process_recurse(char *dirname, char *name)
{
    pid_t pid;
    char path[PATH_MAX];
    strcpy(path, dirname);
    strcat(path, "/");
    strcat(path, name);

    int status;
    pid = fork();
    if (pid == 0) {
        printf("%s:\n", path);
        execl(opt.progname, opt.progname, "-R", path, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else {
        while (pid != wait(&status));
    }
}

/* Apartado c */
void list_dir_recurse(char *name)
{
    DIR *d;
    struct dirent *de;

    if ((d = opendir(name)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    printf("\n");
    while ((de = readdir(d)) != NULL) {
        if (de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
            process_recurse(name, de->d_name);
        }
    }

    closedir(d);
}

int main(int argc, char **argv)
{
    char *dirname = ".";
    int op;
    opt.progname = argv[0];
    opt.recurse = 0;

    /* Apartado a: procesar opciones con getopt */

    while ((op = getopt(argc, argv, "hR")) != -1) {
        switch (op) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'R':
            opt.recurse = 1;
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    if (opt.recurse)
        printf("%s:\n", dirname);

    list_dir(dirname);

    if (opt.recurse)
        list_dir_recurse(dirname);

    return 0;
}
