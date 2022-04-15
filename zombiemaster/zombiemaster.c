
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* const* argv) {
    int nbr_processes = 3;
    printf("Executing zombie master\n");
    for (int i = 0; i < nbr_processes; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("Executing secondary zombie master %d\n", i);
            for (int j = 0; j < nbr_processes; j++) {
                pid_t pid2 = fork();
                if (pid2 == 0) {
                    printf("Executing zombie %d.%d\n", i, j);
                    while (1) {
                        sleep(1);
                        printf("Hi! I'm zombie %d.%d and I die\n", i, j);
                        exit(-1);
                    }
                } else {
                    // nothing
                }
            }
            sleep(2);
            printf("Hi! I'm secondary zombie master %d and I die\n", i);
            exit(-1);
        } else {
            // nothing
        }
    }

    sleep(3);
    printf("Hi! I'm zombie master and I die\n");
    exit(0);
}
