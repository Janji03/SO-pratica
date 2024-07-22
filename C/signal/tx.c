#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }

    int rx_pid = atoi(argv[1]);
    char* message = argv[2];

    if(strlen(message) > 8) {
        fprintf(stderr, "Messaggio troppo lungo");
        return 1;
    }

    union sigval sv;
    char msg[9] = {0}; 
    strncpy(msg, message, 8);
    memcpy(&sv.sival_int, msg, 8); 

    if(sigqueue(rx_pid, SIGUSR1, sv) == -1) {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }

    return 0;
}