#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct {
    pid_t pid;
} subprocess;

void sub_exec(const char * command);
void kill_all(subprocess* subprocesses, int nbr, int signal);
void sig_handler(int signo);

int main(int argc, const char** argv) {
    
    subprocess* subprocesses;
    int nbr = argc - 1;
    subprocesses = malloc(sizeof(subprocess) * nbr);
    int wstatus;
    int error = 0;
    int counter = 0;
    int closing = 0;
    
    signal(SIGINT, sig_handler);
    
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
    
    while (counter < nbr) {
        wait(&wstatus);
        if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
            counter += 1;
            if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0){
                error = 1;
            } else if (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) != SIGINT && WTERMSIG(wstatus) != SIGTERM) {
                error = 1;
            }
            if (! closing) {
                closing = 1;
                kill_all(subprocesses, nbr, SIGTERM);
            }
        }
    }
    if (error == 1) {
        printf("One or more of the provided commands ended abnormally\n");
        return -1;
    } else {
        return 0;
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

void kill_all(subprocess* subprocesses, int nbr, int signal) {
    for (int i = 0; i < nbr; i++) {
        kill(subprocesses[i].pid, signal);
    }
}

void sig_handler(int signo) {
    signal(signo, sig_handler);
    if (signo == SIGINT)
        printf("received %d\n", signo);
}
