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
    subprocesses = malloc(sizeof(subprocess) * argc - 1);
    
    for (int i = 1; i < argc; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sub_exec(argv[i]);
            exit(-1);
        } else {
            subprocess new_sub;
            new_sub.pid = pid;
            subprocesses[i - 1] = new_sub;
        }
    }

}

void sub_exec(const char* command) {
    char* command_copy;
    char* current;
    char* args[100];
    int argn = 0;
    int ret;
    
    command_copy = malloc(sizeof(char) * (strlen(command) + 1));
    strcpy(command_copy, command);
    current = command_copy;
    
    for (int i = 0; i < 100; i++) {
        char* token = strtok(current, " ");
        current = NULL;
        if (token == NULL)
            break;
        args[i] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(args[i], token);
        argn += 1;
    }
    args[argn] = NULL;
    
    ret = execvp(args[0], args);
    if (ret != 0) {
        printf("Error launching the subprocess: %s\n", strerror(errno));
        exit(-1);
    }
}
