#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void signal_handler_int(int sig) {
    printf("\nYeah!\n");
    fflush(stdout);
}

void signal_handle_hup(int sig) {
    printf("\nOuch!\n");
    fflush(stdout);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("invalid input!\n");
        return 1;
    }

    signal(SIGINT, signal_handler_int);
    signal(SIGHUP, signal_handle_hup);

    int n = atoi(argv[1]);

    for (int i = 0; i < 2*n; i+=2) {
        printf("%d\n", i);
        fflush(stdout);
        sleep(5);
    }

    return 0;
}