#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXARG 10

int main(int argc, char *argv[]) {
    char *newargv[MAXARG];
    char buffer[512]; 
    int n, i, j;
    
    for (i = 1; i < argc && i < MAXARG - 1; i++) {
        newargv[i - 1] = argv[i];
    }

    while ((n = read(0, buffer, sizeof(buffer))) > 0) {
        buffer[n] = 0;

        i = argc - 1;
        j = 0;
        while (j < n) {
            while (j < n && (buffer[j] == ' ' || buffer[j] == '\n')) j++;
            if (j >= n) break;

            newargv[i++] = &buffer[j];

            while (j < n && buffer[j] != ' ' && buffer[j] != '\n') j++;
            buffer[j++] = 0;
        }

        newargv[i] = 0;

        if (fork() == 0) {
            exec(newargv[0], newargv);
            exit(1);
        } else {
            wait(0);
        }
    }
    exit(0);
}
