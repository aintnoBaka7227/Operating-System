#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

// signal handler function for sigint
void signal_handler_int(int sig) {
    printf("Yeah!\n");
}

// signal handler function for sighup
void signal_handle_hup(int sig) {
    printf("Ouch!\n");
}

int main(int argc, char *argv[]) {

    // warning and exit if argument insufficient
    if (argc != 2) {
        printf("invalid input!\n");
        return 1;
    }

    // handle each type of signal
    signal(SIGINT, signal_handler_int);
    signal(SIGHUP, signal_handle_hup);

    // convert input to int
    int n = atoi(argv[1]);

    // loop and print first n even nums
    // sleep() to free program for 5 seconds
    for (int i = 0; i < 2*n; i+=2) {
        printf("%d\n", i);
        sleep(5);
    }

    return 0;
}