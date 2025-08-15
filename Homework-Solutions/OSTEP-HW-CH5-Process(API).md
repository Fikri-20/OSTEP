
# Homework (Simulation)
1.

```
                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: a forks c
                               a
                               ├── b
                               └── c
Action: c EXITS
                               a
                               └── b
Action: a forks d
                               a
                               ├── b
                               └── d
Action: a forks e
                               a
                               ├── b
                               ├── d
                               └── e
```

2.
the larger fork_percentege I set, the larger the tree goes in depth (more forks).

```
                           Process Tree:
                               a

Action: a forks b
                               a
                               └── b
Action: b forks c
                               a
                               └── b
                                   └── c
Action: b forks d
                               a
                               └── b
                                   ├── c
                                   └── d
Action: b forks e
                               a
                               └── b
                                   ├── c
                                   ├── d
                                   └── e
Action: d forks f
                               a
                               └── b
                                   ├── c
                                   ├── d
                                   │   └── f
                                   └── e
Action: a forks g
                               a
                               ├── b
                               │   ├── c
                               │   ├── d
                               │   │   └── f
                               │   └── e
                               └── g
Action: b forks h
                               a
                               ├── b
                               │   ├── c
                               │   ├── d
                               │   │   └── f
                               │   ├── e
                               │   └── h
                               └── g
Action: c forks i
                               a
                               ├── b
                               │   ├── c
                               │   │   └── i
                               │   ├── d
                               │   │   └── f
                               │   ├── e
                               │   └── h
                               └── g
Action: f forks j
                               a
                               ├── b
                               │   ├── c
                               │   │   └── i
                               │   ├── d
                               │   │   └── f
                               │   │       └── j
                               │   ├── e
                               │   └── h
                               └── g
Action: j forks k
                               a
                               ├── b
                               │   ├── c
                               │   │   └── i
                               │   ├── d
                               │   │   └── f
                               │   │       └── j
                               │   │           └── k
                               │   ├── e
                               │   └── h
                               └── g
```

3. Yes, that's easy :)
fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -t

```
                           Process Tree:
                               a

Action?
                               a
                               └── b
Action?
                               a
                               └── b
                                   └── c
Action?
                               a
                               └── b
                                   ├── c
                                   └── d
Action?
                               a
                               └── b
                                   └── d
Action?
                               a
                               ├── b
                               │   └── d
                               └── e
```			       
a forks b, b forks c, b forks d, c exits, a forks e.

  	     	      	       
4.

Yes, easy
```
                           Process Tree:
                               a

Action: a forks b
Action: b forks c
Action: c forks d
Action: a forks e
Action: a forks f

                        Final Process Tree?
```
```
			a
			|__ b
			|   |__ c
			|       |__ d
			|__ e
			|__ f

```

5.
fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -t -F
```						 	   
                           Process Tree:
                               a

Action?
Action?
Action?
Action?
Action?

                        Final Process Tree:
                               a
                               ├── b
                               └── d
			       └── e
```

    1. a forks b
    2. a forks c
    3. a forks d
    4. c exits
    5. d forks e

Actually I can't tell exactly the eact actions that took place, as in this example it could d that forks another process (e.g., f) and exits, so here is also a valid 5-actions tree.
So I can exactly determine the actions if the number of actions = the number of nodes in the tree (all of them are forks), otherwise I can't *exactly* determine, but I could give valid potential solutions.


# Homework (Code)
1.
```c
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h> 

int main(int argc, char* argv[]){

    int x = 100;
    int rc = fork(); 

    if(rc < 0){
        fprintf(stderr, "Fork is failed!");
        exit(1);
    }else if(rc == 0){
        printf("Value of X in child process: %d\n", x);
        x = 30; 
        printf("Value of X after Change: %d\n", x);
        exit(0);
    }else{
        int wait_rc = wait(NULL);
        printf("Value of X in parent process: %d\n", x);
        x = 40; 
        printf("Value of X after Change: %d\n", x);
        exit(0);
    }

    return 0;
}
```
- x has the same initial value in both: the parent process, and the child process. As the child process has the an exact copy of the parent process (including the same address space contents) so x is exist in both processes data.

- when changes x value in both processes, it doesn't conflict nor affect each other. That's because each process is standalone process, has its own address space (although they both have the same *content* at first, but not same locations). so x in the parent proces isn't the same in the child process. No shared Memory between the two.

2.
```c
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <fcntl.h> 
#include <string.h> 

int main(int argc, char* argv[]){

    int fd = open("example.txt", O_RDWR, 0x644);
    int rc = fork(); 
    if(rc < 0){
        fprintf(stderr, "Fork is failed!");
        exit(1);
    }else if(rc == 0){
        printf("CHILD: Inherited file descriptor %d\n", fd);
        char* msg = "hello, from the child\n";
        ssize_t bytes_written = write(fd, msg, strlen(msg));
        if(bytes_written == -1){
            perror("write");
        }
        exit(0);
    }else{
        printf("PARENT: Parent file descriptor %d\n", fd);
        char* msg = "hello, from the parent\n";
        ssize_t bytes_written = write(fd, msg, strlen(msg));
        if(bytes_written == -1){
            perror("write");
        }
        exit(0);
    }

    return 0;
}
```
- yes, both processes (parent & chid) can access the file descriptor, conculding the forked process inherit the openend files.
When the two processes 
- when the two processes write to the file at the same time (concurrently), they manage to write to the file one after the other, no corrpution or conflict of writing occurs, because the file writing is atomically, means it occurs one all (completely). So no problems here.


3. Ok without using wait() sys call, the CPU scheduler is the one determines who run frist as these are two different processes. we could turn around the processor by using *signals*, the child will send a signal to the parent after printing hello, when signal is sent, a signal handler is executed (assuring that now we could run the parent routine), then parent run

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t child_ready = 0;

void signal_handler(int sig) {
    child_ready = 1;
}

int main() {
    signal(SIGUSR1, signal_handler);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("hello\n");
        kill(getppid(), SIGUSR1);
        exit(0);
    } else {
        while (!child_ready) {
            pause();
        }
        printf("goodbye\n");
    }
    
    return 0;
}
```

4.

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main() {    
    pid_t pid = fork();
    
    if(pid < 0){
        fprintf(stderr, "Fork Failed!");
        exit(1);
    }
    if (pid == 0) {
        printf("------ Child Space --------\n");
        printf("hello\n");
        execl("/bin/ls", "ls", "-l", NULL);
        printf("This will never printed!");
        exit(1);
    } else {
        printf("------ Parent Space --------\n");
        printf("parent here\n");
	exit(0);
    }
    
    return 0;
}
```

why there're variants of exec? Due to:
    1. form of passing inputs to the new program: it could passed as list (l), or as vector (v), ..etc.
    2. If you want to add new parameter to the new program, like executing in an environment (e)

here are a summary of almost all variants of exec in linux:
```c
// execl - list args, full path, inherit env
execl("/bin/cat", "cat", "file.txt", NULL);

// execle - list args, full path, custom env
char *env[] = {"PATH=/bin", NULL};
execle("/bin/cat", "cat", "file.txt", NULL, env);

// execlp - list args, search PATH, inherit env
execlp("cat", "cat", "file.txt", NULL);

// execv - vector args, full path, inherit env
char *args[] = {"cat", "file.txt", NULL};
execv("/bin/cat", args);

// execvp - vector args, search PATH, inherit env
char *args[] = {"cat", "file.txt", NULL};
execvp("cat", args);

// execvpe - vector args, search PATH, custom env
char *args[] = {"cat", "file.txt", NULL};
char *env[] = {"PATH=/bin", NULL};
execvpe("cat", args, env);
```

5.
```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main() {    
    pid_t pid = fork();
    
    if(pid < 0){
        fprintf(stderr, "Fork Failed!");
        exit(1);
    }
    if (pid == 0) {
        printf("------ Child Space --------\n");
        printf("CHILD PID: %d\n", getpid());
    }else{
        int rc_wait = wait(NULL);
        printf("------ Parent Space --------\n");
        printf("PID: %d\n", getpid());
        printf("rc_wait: %d\n", rc_wait);
    }
    
    return 0;
}
```
- wait returns the PID of the child process.
- if put wait() in the child process it returns -1, as it's supposed to return the PID of the child process.

6. waitpid() is more general, where we wait a specific child process, e.g., wait specific process with that PID, wait any child in the *same process group*. waitpid() is similar to wait() when sitting pid parameter of waitpid() to -1 (indicating wait for any child process.
   - waitpid() is very beneficial for servers, shells, and any application managing multiple child processes!

7. After closing the standard output, the child process couldn't print anything to the screen

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main() {    
    pid_t pid = fork();
    
    if(pid < 0){
        fprintf(stderr, "Fork Failed!");
        exit(1);
    }
    if (pid == 0) {
        printf("------ Child Space --------\n");        
        printf("CHILD PID: %d\n", getpid());
        close(STDOUT_FILENO); 
        printf("garbage \n");
    }else{
        int rc_wait = wait(NULL);
        printf("------ Parent Space --------\n");
        printf("PARENT PID: %d\n", getpid());
    }
    
    return 0;
}
```
8.
```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int pipefd[2];
    pid_t child1, child2;
    
    printf("=== pipe() System Call Demo ===\n");
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    printf("Read end: fd %d, Write end: fd %d\n", pipefd[0], pipefd[1]);
    
    child1 = fork();
    if (child1 == 0) {
        printf("Child1 (Writer) PID: %d\n", getpid());
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        printf("Hello from Child1!\n");
        printf("This is line 2\n");
        printf("This is line 3\n");
        printf("End of data from Child1\n");
        
        exit(0);
    }
    
    child2 = fork();
    if (child2 == 0) {
        printf("Child2 (Reader) PID: %d\n", getpid());        
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        char buffer[256];
        printf("Child2 reading from pipe:\n");
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            printf("Child2 received: %s", buffer);
        }
        printf("Child2: End of input reached\n");
        exit(0);
    }
    
    printf("Parent created Child1: %d, Child2: %d\n", child1, child2);
    
    close(pipefd[0]);
    close(pipefd[1]);
    printf("Parent closed both pipe ends\n");
    
    // Wait for both children
    waitpid(child1, NULL, 0);
    printf("Child1 finished\n");
    
    waitpid(child2, NULL, 0);
    printf("Child2 finished\n");
    
    printf("All done!\n");
    return 0;
}#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int pipefd[2];
    pid_t child1, child2;
    
    printf("=== pipe() System Call Demo ===\n");
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    printf("Read end: fd %d, Write end: fd %d\n", pipefd[0], pipefd[1]);
    
    child1 = fork();
    if (child1 == 0) {
        printf("Child1 (Writer) PID: %d\n", getpid());
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        printf("Hello from Child1!\n");
        printf("This is line 2\n");
        printf("This is line 3\n");
        printf("End of data from Child1\n");
        
        exit(0);
    }
    
    child2 = fork();
    if (child2 == 0) {
        printf("Child2 (Reader) PID: %d\n", getpid());        
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        char buffer[256];
        printf("Child2 reading from pipe:\n");
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            printf("Child2 received: %s", buffer);
        }
        printf("Child2: End of input reached\n");
        exit(0);
    }
    
    printf("Parent created Child1: %d, Child2: %d\n", child1, child2);
    
    close(pipefd[0]);
    close(pipefd[1]);
    printf("Parent closed both pipe ends\n");
    
    // Wait for both children
    waitpid(child1, NULL, 0);
    printf("Child1 finished\n");
    
    waitpid(child2, NULL, 0);
    printf("Child2 finished\n");
    
    printf("All done!\n");
    return 0;
}
```