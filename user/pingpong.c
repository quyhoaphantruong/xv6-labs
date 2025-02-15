#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

int main() {
    int parent2Child[2];
    int child2Parent[2];
    char buffer[1];
    
    if (pipe(parent2Child) < 0 || pipe(child2Parent) < 0) {
        printf("created pipe failed !\n"); 
        exit(1);
    }
    
    int pid = fork();
    
    if (pid < 0) {
        printf("Fork failed!\n"); 
        exit(1);
    } else if (pid == 0) {
        // Child process
        close(parent2Child[WRITE]);
        close(child2Parent[READ]);
        
        if (read(parent2Child[READ], buffer, 1) != 1) {
            printf("child: read failed\n"); 
            exit(1);
        }

        printf("%d: received ping\n", getpid());
        
        // write to parent
        if (write(child2Parent[WRITE], buffer, 1) != 1) {
            printf("child: write failed\n"); 
            exit(1);
        }

        close(parent2Child[READ]);
        close(child2Parent[WRITE]);

    } else if (pid > 0) {
        // Parent process
        close(parent2Child[READ]);
        close(child2Parent[WRITE]);

        buffer[0] = 'H';
        if (write(parent2Child[WRITE], buffer, 1) != 1) {
            printf("parent: write failed\n");
            exit(1);
        }

        if (read(child2Parent[READ], buffer, 1) != 1) {
            printf("parent: read failed\n"); 
            exit(1);
        }

        printf("%d: received pong\n", getpid()); 
        
        close(parent2Child[WRITE]); 
        close(child2Parent[READ]);
        wait(0);
        exit(0);
    }
}