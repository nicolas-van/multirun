/*
Copyright (c) 2016 Nicolas Vanhoren

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <getopt.h>

typedef struct {
    pid_t pid;
    const char* command;
    int up;
    int error;
} subprocess;

void print_help();
void launch_processes();
void sub_exec(const char * command);
void kill_all(int signal);
void sig_receive(int signum);

int verbose = 0;
char* const* commands;
int nbr_processes = 0;
subprocess* subprocesses = NULL;
int closing = 0;

int main(int argc, char* const* argv) {
    int opt;

    while ((opt = getopt(argc, argv, "vh")) != -1) {
        switch (opt) {
        case 'v':
            verbose = 1;
            break;
        case 'h':
            print_help();
            exit(0);
            break;
        default: /* '?' */
            print_help();
            exit(-1);
        }
    }
    if (optind >= argc) {
        print_help();
        exit(-1);
    }
    commands = &argv[optind];
    nbr_processes = argc - optind;
    launch_processes();
}

void launch_processes() {
    int wstatus;
    struct sigaction ssig;

    subprocesses = malloc(sizeof(subprocess) * nbr_processes);

    for (int i = 0; i < nbr_processes; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sub_exec(commands[i]);
            exit(-1); // should not happen
        } else {
            if (verbose) {
                printf("multirun: launched command %s with pid %d\n", commands[i], pid);
            }
            subprocess new_sub;
            new_sub.pid = pid;
            new_sub.command = commands[i];
            new_sub.up = 1;
            new_sub.error = 0;
            subprocesses[i] = new_sub;
        }
    }

    ssig.sa_handler = sig_receive;
    if (sigaction(SIGINT, &ssig, NULL))
        exit(-1);
    if (sigaction(SIGTERM, &ssig, NULL))
        exit(-1);

    while (1) {
        pid_t pid = wait(&wstatus);
        subprocess* process = NULL;
        for (int i = 0; i < nbr_processes; i++) {
            if (subprocesses[i].pid == pid) {
                process = &subprocesses[i];
                break;
            }
        }
        if (process != NULL) {
            // check if process is down
            if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
                process->up = 0;
                if ((WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
                    || (WIFSIGNALED(wstatus) && WTERMSIG(wstatus) != SIGINT && WTERMSIG(wstatus) != SIGTERM)) {
                    process->error = 1;
                    if (verbose) {
                        printf("multirun: command %s with pid %d exited abnormally\n", process->command, pid);
                    }
                } else {
                    if (verbose) {
                        printf("multirun: command %s with pid %d exited normally\n", process->command, pid);
                    }
                }
                if (! closing) {
                    closing = 1;
                    // activate Goebbels mode
                    if (verbose) {
                        printf("multirun: sending SIGTERM to all subprocesses\n");
                    }
                    kill_all(SIGTERM);
                }
            }
        }
        // check if all processes are stopped
        int running = 0;
        for (int i = 0; i < nbr_processes; i++) {
            if (subprocesses[i].up) {
                running = 1;
                break;
            }
        }
        if (! running)
            break;
    }
    // check if there are errors
    int error = 0;
    for (int i = 0; i < nbr_processes; i++) {
        if (subprocesses[i].error) {
            error = 1;
            break;
        }
    }
    if (error) {
        fprintf(stderr, "multirun: one or more of the provided commands ended abnormally\n");
        exit(-1);
    } else {
        if (verbose) {
            printf("multirun: all subprocesses exited without errors\n");
        }
        exit(0);
    }
}

void print_help() {
    printf("Usage: multirun <options> command...\n");
    printf("\n");
    printf("multirun is a small utility to run multiple commands concurrently. ");
    printf("If one of these commands terminate it will kill all the others.\n");
    printf("\n");
    printf("Options:\n");
    printf("    -v verbose mode\n");
    printf("    -h display help\n");
}

void sub_exec(const char* command) {
    int ret;
    const char* pre = "exec ";
    char* ccommand;
    ccommand = malloc((sizeof(char) * (strlen(pre) + strlen(command))) + 1);
    ccommand[0] = 0;
    strcat(ccommand, pre);
    strcat(ccommand, command);
    ret = execlp("sh", "sh", "-c", ccommand, (char*) NULL);
    if (ret != 0) {
        fprintf(stderr, "multirun: error launching the subprocess: %s\n", strerror(errno));
        exit(-1);
    }
}

void kill_all(int signal) {
    for (int i = 0; i < nbr_processes; i++) {
        kill(subprocesses[i].pid, signal);
    }
}

void sig_receive(int signum) {
    if (verbose) {
        printf("multirun: received signal %s, propagating to all subprocesses\n", strsignal(signum));
    }
    if (signum == SIGTERM) {
        closing = 1;
    }
    kill_all(signum);
};
