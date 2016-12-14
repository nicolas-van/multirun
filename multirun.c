#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#define LOG(mess) printf("%s", mess); printf("\n");
#else
#define LOG(mess)
#endif

typedef struct {
    pid_t pid;
} subprocess;

void sub_exec(const char * command);
void kill_all(subprocess* subprocesses, int nbr, int signal);
void sig_receive(int signum);

int main(int argc, const char** argv) {
    
    subprocess* subprocesses;
    int nbr = argc - 1;
    subprocesses = malloc(sizeof(subprocess) * nbr);
    int wstatus;
    int error = 0;
    int counter = 0;
    int closing = 0;
    struct sigaction ssig;
    
    for (int i = 0; i < nbr; i++) {
        LOG("Launching command:");
        LOG(argv[i + 1]);
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
    
    ssig.sa_handler = sig_receive;
    if (sigaction(SIGINT, &ssig, NULL))
        exit(-1);
    if (sigaction(SIGTERM, &ssig, NULL))
        exit(-1);
    
    while (counter < nbr) {
        wait(&wstatus);
        if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
            LOG("One of the subprocesses exited");
            counter += 1;
            if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0){
                error = 1;
            } else if (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) != SIGINT && WTERMSIG(wstatus) != SIGTERM) {
                error = 1;
            }
            if (! closing) {
                closing = 1;
                LOG("Sending SIGTERM to all subprocesses");
                kill_all(subprocesses, nbr, SIGTERM);
            }
        } else {
            LOG("Received unhandled signal from subprocess");
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

void sig_receive(int signum) {
    //printf("Received signal %d\n", signum);
};
