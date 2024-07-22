#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }

    int iterations;
    double interval;
    char buffer[1024];

    if(sscanf(argv[1], "%d,%lf,%1023s", &iterations, &interval, buffer) != 3) {
        fprintf(stderr, "Formato non corretto");
        return 1;
    }

    int tfd = timerfd_create(CLOCK_REALTIME, 0);
    if(tfd == -1) {
        perror("timerfd_create");
        return 1;
    }

    struct itimerspec new_value;
    new_value.it_value.tv_sec = (int)interval;
    new_value.it_value.tv_nsec = (interval - (int)interval) * 1e9;
    new_value.it_interval.tv_sec = new_value.it_value.tv_sec;
    new_value.it_interval.tv_nsec = new_value.it_value.tv_nsec;

    if(timerfd_settime(tfd, 0, &new_value, NULL) == -1) {
        perror("timerfd_settime");
        close(tfd);
        return EXIT_FAILURE;
    }

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for(int i = 0; i < iterations; i++) {
        uint64_t expirations;
        ssize_t s = read(tfd, &expirations, sizeof(expirations));
        if(s != sizeof(expirations)) {
            perror("read");
            close(tfd);
            return 1;
        }

        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        double elapsed_time = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_nsec - start_time.tv_nsec) / 1e9;

        printf("%.3f %s\n", elapsed_time, buffer);
    }

    close(tfd);

    return 0;
}