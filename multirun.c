
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "0.0.0"
#endif

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
void reap_zombies();

int verbose = 0;
char* const* commands;
int nbr_processes = 0;
subprocess* subprocesses = NULL;
int closing = 0;

int main(int argc, char *const *argv) {
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
        default:
            printf("Unknown option: %c\n", (char)optopt);
            print_help();
            exit(-2);
        }
    }
    if (optind >= argc) {
        print_help();
        exit(-2);
    }
    commands = &argv[optind];
    nbr_processes = argc - optind;
    launch_processes();
}

void launch_processes() {
    subprocesses = malloc(sizeof(subprocess) * nbr_processes);

    for (int i = 0; i < nbr_processes; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // declare a new group
            setpgrp();
            // execute subcommand
            sub_exec(commands[i]);
            exit(-2); // should not happen
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

    if (signal(SIGINT, sig_receive) == SIG_ERR) {
        exit(-2);
    }
    if (signal(SIGTERM, sig_receive) == SIG_ERR) {
        exit(-2);
    }

    while (1) {
        int wstatus;
        pid_t pid = waitpid(-1, &wstatus, 0);

        if (pid == -1) {
            if (errno == ECHILD) {
                break; // no more children
            } if (errno == EINTR) {
                continue; // interrupted
            } else {
                fprintf(stderr, "multirun: error during wait: %d\n", errno);
                exit(-2);
            }
        } else if (pid == 0) {
            break; // no more children
        }

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
                        printf("multirun: command %s with pid %d exited abnormally\n", process->command, process->pid);
                    }
                } else {
                    if (verbose) {
                        printf("multirun: command %s with pid %d exited normally\n", process->command, process->pid);
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
        } else {
            if (verbose) {
                printf("multirun: subchild process with pid %d ended\n", pid);
            }
        }
    }

    // ensure all child died in all groups
    for (int i = 0; i < nbr_processes; i++) {
        while (1) {
            int wstatus;
            int pid = waitpid(-subprocesses[i].pid, &wstatus, 0);
            if (pid == -1) {
                if (errno == ECHILD) {
                    break; // no more children in group
                } if (errno == EINTR) {
                    continue; // interrupted
                } else {
                    fprintf(stderr, "multirun: error during wait: %d\n", errno);
                    exit(-2);
                }
            } else if (pid == 0) {
                break; // no more children
            }
            // some more child must still exit
        }
    }

    // check if there were errors
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
    printf("Version: %s\n", PROJECT_VERSION);
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
        exit(-2);
    }
}

void kill_all(int signal) {
    for (int i = 0; i < nbr_processes; i++) {
        int ret = kill(-subprocesses[i].pid, signal);
        if (ret != 0) {
            if (errno == ESRCH) {
                // ignore
            } else {
                fprintf(stderr, "multirun: error %d while killing processes\n", errno);
                exit(-2);
            }
        }
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
}
