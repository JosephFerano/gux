#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#define BUF_SIZE 512
#define PERM_FILE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void copy(char *from, char *to) {

    int fromfd = -1, tofd = -1;

    ssize_t nread;
    char *buf[BUF_SIZE];

    if ((fromfd = open(from, O_RDONLY)) == -1) {
        char *errb;
        asprintf(&errb, "Error opening file '%s'", from);
        perror(errb);
        free(errb);
        goto clean_up;
    }

    if ((tofd = open(to, O_WRONLY | O_CREAT, PERM_FILE)) == -1) {
        goto clean_up;
    }

    while ((nread = read(fromfd, buf, sizeof(buf))) > 0) {
        if ((write(tofd, buf, nread)) > 0) {
            goto clean_up;
        }
    }
    if (nread == -1) {
        goto clean_up;
    }

    clean_up:
    if (fromfd != -1) {
        (void) close(fromfd);
    }
    if (tofd != -1) {
        (void) close(tofd);
    }
}

int main(int argc, char *argv[]) {
    (void) argv;
    if (argc < 2) {
        fprintf(stderr, "Please provide a filename. Args provided: %i\n", argc);
        exit(1);
    }
    /* copy(argv[1], argv[2]); */
    struct statvfs *stats;
    (void) statvfs("/usr", stats);
}
