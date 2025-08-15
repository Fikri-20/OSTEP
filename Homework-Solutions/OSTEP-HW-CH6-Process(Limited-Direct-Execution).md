# Measure System Call Cost
I'll measure it as the authors suggest, by reading a 0-byte instruction many many times, then divides it by the number I call it.
```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#define ITERATIONS 1000000

int main(int argc, char* argv[]) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (long i = 0; i < ITERATIONS; i++) {
        if (read(fd, NULL, 0) < 0) {
            perror("read");
            exit(1);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    close(fd);

    double total_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double per_call = (total_time / ITERATIONS) * 1e9; // nanoseconds

    printf("Total time: %.6f seconds\n", total_time);
    printf("Average per system call: %.2f ns\n", per_call);

    return 0;
}

```

```Output
Total time: 0.220442 seconds  
Average per system call: 220.44 ns ~= 0.22 Microsecond
```

# Measure Context Switch Cost
```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>

#define ITERATIONS 1000000

void bind_to_cpu0() {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set); // Bind to CPU 0
    if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        perror("sched_setaffinity");
        exit(1);
    }
}

int main(void) {
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    bind_to_cpu0();

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        bind_to_cpu0();
        char buf = 'x';
        for (int i = 0; i < ITERATIONS; i++) {
            // Wait for parent
            if (read(pipe1[0], &buf, 1) != 1) {
                perror("child read");
                exit(1);
            }
            // Reply to parent
            if (write(pipe2[1], &buf, 1) != 1) {
                perror("child write");
                exit(1);
            }
        }
        exit(0);
    } else {
        // Parent process
        char buf = 'x';
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < ITERATIONS; i++) {
            // Send to child
            if (write(pipe1[1], &buf, 1) != 1) {
                perror("parent write");
                exit(1);
            }
            // Wait for reply
            if (read(pipe2[0], &buf, 1) != 1) {
                perror("parent read");
                exit(1);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double total_time = (end.tv_sec - start.tv_sec) +
                            (end.tv_nsec - start.tv_nsec) / 1e9;
        // 2 context switches per iteration
        double per_switch_us = (total_time / (ITERATIONS * 2)) * 1e6;

        printf("Total time: %.6f sec\n", total_time);
        printf("Average context switch: %.2f µs\n", per_switch_us);
    }

    return 0;
}

```

```Output
Total time: 6.920173 sec
Average context switch: 3.46 µs
```