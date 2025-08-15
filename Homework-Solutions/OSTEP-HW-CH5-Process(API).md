
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

