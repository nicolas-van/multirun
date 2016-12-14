#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef struct {
    pid_t pid;
} subprocess;

void sub_exec(const char * command);

int main(int argc, const char** argv) {
    
    subprocess* subprocesses;
    int nbr = argc - 1;
    subprocesses = malloc(sizeof(subprocess) * nbr);
    int wstatus;
    
    for (int i = 0; i < nbr; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sub_exec(argv[i + 1]);
            exit(-1); // should not happen
        } else {
            subprocess new_sub;
            new_sub.pid = pid;
            subprocesses[i] = new_sub;
        }
    }
    
    for (int i = 0; i < nbr; i++) {
        wait(&wstatus);
    }
}

void sub_exec(const char* command) {
    int ret;
    ret = execlp("sh", "sh", "-c", command, (char*) NULL);
    if (ret != 0) {
        printf("Error launching the subprocess: %s\n", strerror(errno));
        exit(-1);
    }
}
