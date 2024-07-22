#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handle_signal(int sig, siginfo_t *info, void *context) {
    char msg[9] = {0};
    memcpy(msg, &info->si_value.sival_int, 8); 
    printf("Messaggio: %s\n", msg);
}

int main() {
    int pid = getpid();
    printf("%d\n", pid);

    struct sigaction sa;
    
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_signal;
    
    if(sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while(1) {
        pause();
    }

    return 0;
}