1. done (simple type `man free` in the shell`)
2.
```
total mem: 13835MB
free: 5992MB 
```
Yes thats numbers match my intution. As my Main memory is supposed to be 16GB, the GPU & hardware takes 2.2GB of the system before Botting the system, so the remaining RAM is almost 13.8GB. 

3.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep()

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <megabytes> [seconds]\n", argv[0]);
        return 1;
    }

    int megabytes = atoi(argv[1]);
    int seconds = (argc >= 3) ? atoi(argv[2]) : 0; // 0 means run forever

    size_t size = (size_t)megabytes * 1024 * 1024;
    char *arr = malloc(size);
    if (!arr) {
        perror("malloc");
        return 1;
    }

    printf("Allocated %d MB of memory.\n", megabytes);

    // Touch memory indefinitely or for given time
    time_t start = time(NULL);
    while (seconds == 0 || (time(NULL) - start) < seconds) {
        for (size_t i = 0; i < size; i++) {
            arr[i] = (char)(i % 256);
        }
        // Optional: small sleep to slow CPU usage
        // usleep(1000); 
    }

    free(arr);
    printf("Memory freed.\n");

    return 0;
}

```

4.
- The memory changes according to the number of MB I allocated in the program, this is reflected on the `used` panel on free response.
- when I kill the process, all the numbers reset again to the default memory numbers.
```
Before
fikri@Fikri:~/Desktop/OS/OSTEP/Homework-Solutions$ free -m 
               total        used        free      shared  buff/cache   available
Mem:           13825        3353       10178         187         791       10472
Swap:           2047        1945         102

Program Command : /vm 1000 (allocating 1000MB) 

After

fikri@Fikri:~/Desktop/OS/OSTEP/Homework-Solutions$ free -m 
               total        used        free      shared  buff/cache   available
Mem:           13825        4376        9122         185         822        9449
Swap:           2047        1936         111

Notice: Used increased 1000MB, and also available decrease by around 1000MB
```

- Using large amounts of memory, I tested allocating 10000MB (10GB) and it's been allocated successfully, remains just 500MB avaialbe, and the system was working properly.
- I tried to allocat 11500MB, although my total avaialbe memory is around 10GB. when I do so, the program ran successfully, allocating address space of 11500MB successfully, when the process run I monitor the memory using the `free` command. The avaialbe memory is decreased as the processor touches memory places of the allocatd memory. nearly reaching 0 avaialbe memory! hence the OS killed the process (process I ran) and restore the memory back.
- Conclusion: the OS will allocate for the process an address space as it allocated (even it allocates 50GB, when you'v only avaialbe 10GB), but when the processor touches these virtual addresses and try to translate it to the physical memory and notice there's no physical memory to hold all these memory at one, it will kill the process! that's one solution, the other solution is to kill other processes running trying to free space for this heavily process! (but the former was the one works on my linux mint machine)

5. done.
6. I ran pmap on the current program I just showed above, and here's the result
```
Address           Kbytes     RSS   Dirty Mode  Mapping
000057a8a9fb9000       4       4       0 r---- vm
000057a8a9fba000       4       4       0 r-x-- vm
000057a8a9fbb000       4       4       0 r---- vm
000057a8a9fbc000       4       4       4 r---- vm
000057a8a9fbd000       4       4       4 rw--- vm
000057a8c68ab000     132       4       4 rw---   [ anon ]
0000758b345ff000 1024004 1024004 1024004 rw---   [ anon ]
0000758b72e00000     160     156       0 r---- libc.so.6
0000758b72e28000    1568     972       0 r-x-- libc.so.6
0000758b72fb0000     316     124       0 r---- libc.so.6
0000758b72fff000      16      16      16 r---- libc.so.6
0000758b73003000       8       8       8 rw--- libc.so.6
0000758b73005000      52      16      16 rw---   [ anon ]
0000758b731b0000      12       8       8 rw---   [ anon ]
0000758b731ca000       8       4       4 rw---   [ anon ]
0000758b731cc000       4       4       0 r---- ld-linux-x86-64.so.2
0000758b731cd000     172     172       0 r-x-- ld-linux-x86-64.so.2
0000758b731f8000      40      32       0 r---- ld-linux-x86-64.so.2
0000758b73202000       8       8       8 r---- ld-linux-x86-64.so.2
0000758b73204000       8       8       8 rw--- ld-linux-x86-64.so.2
00007ffdfc00d000     132      16      16 rw---   [ stack ]
00007ffdfc1e2000      16       0       0 r----   [ anon ]
00007ffdfc1e6000       8       4       0 r-x--   [ anon ]
ffffffffff600000       4       0       0 --x--   [ anon ]
---------------- ------- ------- ------- 
total kB         1026688 1025576 1024100
```
24 entity! containing: stack, heap, code, Shared Libraries: libc.so, ld-linux-x86-64.so.2, and more.

Conclusion:

### 1. Your program doesn't contain everything it needs

When you compile a C program, unless you compile statically, the binary doesn't include the actual machine code for printf or malloc.
Instead, the compiler just says:

"This function lives in the shared library libc.so.6 — link it at runtime."

### 2. Dynamic linker loads it into your process

When you start your program:

The kernel loads your executable into memory.

The dynamic linker (ld-linux-x86-64.so.2) looks at the binary’s dynamic section.

It sees: "This binary needs libc.so.6."

It maps (mmap) the shared object file libc.so.6 into your process's address space.

This way, multiple programs share the same copy of glibc in memory (read-only code is shared; writable data is private to each process).

### 3. Why inside your process's address space?

Because your process needs to call those functions.
When your code calls printf:

It's actually a jump to an address inside the mapped libc.so.6 region.

That address must be inside your process's virtual memory space — otherwise, the CPU would throw a segmentation fault.


8. I doubled the allocated space (2500MB) actually the same entities is still shown, the only entities that changed are the headp entities, but all the other stuff are the same

```
fikri@Fikri:~/Desktop/OS/OSTEP/Homework-Solutions$ pmap -x 17364
17364:   ./vm 2500
Address           Kbytes     RSS   Dirty Mode  Mapping
00005ab88f3d8000       4       4       0 r---- vm
00005ab88f3d9000       4       4       0 r-x-- vm
00005ab88f3da000       4       4       0 r---- vm
00005ab88f3db000       4       4       4 r---- vm
00005ab88f3dc000       4       4       4 rw--- vm
00005ab8b489a000     132       4       4 rw---   [ anon ]
00007fe104dff000 2560004 2560004 2560004 rw---   [ anon ]
00007fe1a1200000     160     156       0 r---- libc.so.6
00007fe1a1228000    1568     976       0 r-x-- libc.so.6
00007fe1a13b0000     316     124       0 r---- libc.so.6
00007fe1a13ff000      16      16      16 r---- libc.so.6
00007fe1a1403000       8       8       8 rw--- libc.so.6
00007fe1a1405000      52      16      16 rw---   [ anon ]
00007fe1a142b000      12       8       8 rw---   [ anon ]
00007fe1a1445000       8       4       4 rw---   [ anon ]
00007fe1a1447000       4       4       0 r---- ld-linux-x86-64.so.2
00007fe1a1448000     172     172       0 r-x-- ld-linux-x86-64.so.2
00007fe1a1473000      40      40       0 r---- ld-linux-x86-64.so.2
00007fe1a147d000       8       8       8 r---- ld-linux-x86-64.so.2
00007fe1a147f000       8       8       8 rw--- ld-linux-x86-64.so.2
00007ffe38e7a000     132      12      12 rw---   [ stack ]
00007ffe38edf000      16       0       0 r----   [ anon ]
00007ffe38ee3000       8       4       0 r-x--   [ anon ]
ffffffffff600000       4       0       0 --x--   [ anon ]
---------------- ------- ------- ------- 
total kB         2562688 2561584 2560096
```