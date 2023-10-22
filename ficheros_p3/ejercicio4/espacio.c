#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks) {
    struct stat st;

    if (lstat(fname, &st) == -1) {
        perror("lstat");
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        return get_size_dir(fname, blocks);
    } else {
        *blocks = st.st_blocks;
        return 0;
    }
}

/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dname)) == NULL) {
        perror("opendir");
        return -1;
    }

    *blocks = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", dname, entry->d_name);

            size_t entry_blocks;
            if (get_size(full_path, &entry_blocks) == 0) {
                *blocks += entry_blocks;
            }
        }
    }

    closedir(dir);
    return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [<file2> ...]\n", argv[0]);
        return 1;
    }

    size_t total_blocks = 0;

    if (strcmp(argv[1], ".") == 0) {
        // Handle the case when '.' is passed
        if (get_size_dir(".", &total_blocks) != 0) {
            fprintf(stderr, "Error calculating size for '.'\n");
            return 1;
        }
        printf("%zuK .\n", total_blocks * 512 / 1024);
    } else {
        // Handle the case when '*' or other files are passed
        for (int i = 1; i < argc; i++) {
            size_t file_blocks;
            if (get_size(argv[i], &file_blocks) == 0) {
                total_blocks += file_blocks;
                printf("%zuK %s\n", file_blocks * 512 / 1024, argv[i]);
            }
        }

        if (argc > 2) {
            // Print the total size if more than one file is passed
            printf("%zuK total\n", total_blocks * 512 / 1024);
        }
    }

    return 0;
}
