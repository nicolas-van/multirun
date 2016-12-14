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
            exit(-1); // should not happen
        } else {
            subprocess new_sub;
            new_sub.pid = pid;
            subprocesses[i - 1] = new_sub;
        }
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
