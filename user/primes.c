#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

void primes(int inputFd);

void generate(int n) {
    int parent2Child[2];
    char buffer[sizeof(int)];
    if (pipe(parent2Child) < 0) {
        printf("Pipe creation failed!\n");
        exit(1);
    }

    int pid = fork();

    if (pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    } else if (pid == 0) { 
        // Child process
        close(parent2Child[WRITE]);
        primes(parent2Child[READ]); 
        close(parent2Child[READ]);
        exit(0);
    } else if (pid > 0) { 
        // Parent process
        close(parent2Child[READ]);

        for (int i = 2; i <= n; i++) {
            *((int *)buffer) = i;
            if (write(parent2Child[WRITE], buffer, sizeof(int)) != sizeof(int)) {
                printf("generate: Write failed!\n");
                exit(1);
            }
        }

        close(parent2Child[WRITE]);
        wait(0);
        exit(0);
    }
}

void primes(int inputFd) {
    int prime;
    char buffer[sizeof(int)];

    if (read(inputFd, buffer, sizeof(int)) != sizeof(int)) {
        close(inputFd);
        exit(0);
    }

    prime = *((int *)buffer);
    printf("prime %d\n", prime);

    int parent2Child[2];
    if (pipe(parent2Child) < 0) {
        printf("Pipe creation failed!\n");
        exit(1);
    }

    int pid = fork();

    if (pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    } else if (pid == 0) { 
        // Child process
        close(parent2Child[WRITE]);
        close(inputFd);    
        primes(parent2Child[READ]);
        close(parent2Child[READ]);
        exit(0);
    } else if (pid > 0) { 
        // Parent process
        close(parent2Child[READ]);
        int num;
        while (read(inputFd, buffer, sizeof(int)) == sizeof(int)) {
            num = *((int *)buffer);
            if (num % prime != 0) {
                if (write(parent2Child[WRITE], buffer, sizeof(int)) != sizeof(int)) {
                    printf("primes: Write failed!\n");
                    exit(1);
                }
            }
        }
        close(parent2Child[WRITE]);
        close(inputFd);          
        wait(0);
        exit(0);
    }
}

int main() {
    generate(280);
    exit(0);
}