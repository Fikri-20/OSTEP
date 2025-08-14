
# Homework (Simulation)
1.
fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -s 10 -c 

ARG seed 10
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

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

2. the larger fork_percentege I set, the larger the tree goes in depth (more forks).
fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -a 10 -f 0.9 -c

ARG seed -1
ARG fork_percentage 0.9
ARG actions 10
ARG action_list 
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True

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
3. Yes, that's easy :)
fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -t

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree True
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve False

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
a forks b, b forks c, b forks d, c exits, a forks e.

  	     	      	       
4. Yes, easy

fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -F

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree False
ARG just_final True
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve False

                           Process Tree:
                               a

Action: a forks b
Action: b forks c
Action: c forks d
Action: a forks e
Action: a forks f

                        Final Process Tree?
			a
			|__ b
			|   |__ c
			|       |__ d
			|__ e
			|__ f

5.
fikri@Fikri:~/Desktop/OS/ostep-homework/cpu-api$ ./fork.py -t -F

ARG seed -1
ARG fork_percentage 0.7
ARG actions 5
ARG action_list 
ARG show_tree True
ARG just_final True
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve False

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