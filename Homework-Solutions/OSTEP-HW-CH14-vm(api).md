1. It thrown me an error "Segmentation fault (core dumped)
2. it shows me that "Program received signal SIGSEGV, Segmentation faul" when the program reached the printf statement.
3. it shows me that there's an error at `0x109168`: main (null.c:6), saying it's invalid read of size 4.
4. nothing happens out of expect, code is compiled and run successfully. 
```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int* arr = malloc(10 * sizeof(int));
    if(arr == NULL){
        printf("malloc failed");
        exit(1);
    }

    for(int i=0; i < 10; i++){
        arr[i] = i;
    }
    
    for(int i=0; i < 10; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}

```

- using gdb, nothing obscure happens too, all is good.
- using valgrind, it shows me explicitely the leak happned in my program, indicating that there're 40 bytes lost as well as the 400 bytes that I didn't free them.  

```
==25126== Memcheck, a memory error detector
==25126== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==25126== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==25126== Command: ./null
==25126== 
0 1 2 3 4 5 6 7 8 9 
==25126== 
==25126== HEAP SUMMARY:
==25126==     in use at exit: 40 bytes in 1 blocks
==25126==   total heap usage: 2 allocs, 1 frees, 1,064 bytes allocated
==25126== 
==25126== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==25126==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==25126==    by 0x1091C5: main (null.c:5)
==25126== 
==25126== LEAK SUMMARY:
==25126==    definitely lost: 40 bytes in 1 blocks
==25126==    indirectly lost: 0 bytes in 0 blocks
==25126==      possibly lost: 0 bytes in 0 blocks
==25126==    still reachable: 0 bytes in 0 blocks
==25126==         suppressed: 0 bytes in 0 blocks
==25126== 
==25126== For lists of detected and suppressed errors, rerun with: -s
==25126== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0
```

6. The program is compiled and ran normally when compile it with gcc, and run it (although I wrote  on a portion of memory that I didn't allocat), even when run it in the gdb, it didn't raise any error or warning
   - Using valgrind, it shows me "Invalid write of size 4"

```
==25534== Memcheck, a memory error detector
==25534== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==25534== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==25534== Command: ./null
==25534== 
==25534== Invalid write of size 4
==25534==    at 0x1091B4: main (null.c:6)
==25534==  Address 0x4a861d0 is 0 bytes after a block of size 400 alloc'd
==25534==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==25534==    by 0x1091A5: main (null.c:5)
==25534== 
Check point 
==25534== Invalid read of size 4
==25534==    at 0x1091D3: main (null.c:8)
==25534==  Address 0x4a861d0 is 0 bytes after a block of size 400 alloc'd
==25534==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==25534==    by 0x1091A5: main (null.c:5)
==25534== 
0
==25534== 
==25534== HEAP SUMMARY:
==25534==     in use at exit: 400 bytes in 1 blocks
==25534==   total heap usage: 2 allocs, 1 frees, 1,424 bytes allocated
==25534== 
==25534== 400 bytes in 1 blocks are definitely lost in loss record 1 of 1
==25534==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==25534==    by 0x1091A5: main (null.c:5)
==25534== 
==25534== LEAK SUMMARY:
==25534==    definitely lost: 400 bytes in 1 blocks
==25534==    indirectly lost: 0 bytes in 0 blocks
==25534==      possibly lost: 0 bytes in 0 blocks
==25534==    still reachable: 0 bytes in 0 blocks
==25534==         suppressed: 0 bytes in 0 blocks
==25534== 
==25534== For lists of detected and suppressed errors, rerun with: -s
==25534== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
```

7. yes it runs, but it didn't print the value of the array element
```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int* arr = malloc(100 * sizeof(int));
    for(int i=0; i < 100; i++) arr[i] = i;
    free(arr);
    printf("%d", arr[9]);
    return 0;
}

```
- when used valgrind, it shows me that there was an invalid read of size 4 at 0x4a86064
```
==32752== Memcheck, a memory error detector
==32752== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==32752== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==32752== Command: ./null
==32752== 
==32752== Invalid read of size 4
==32752==    at 0x1091EA: main (null.c:8)
==32752==  Address 0x4a86064 is 36 bytes inside a block of size 400 free'd
==32752==    at 0x484988F: free (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==32752==    by 0x1091E1: main (null.c:7)
==32752==  Block was alloc'd at
==32752==    at 0x4846828: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==32752==    by 0x1091A5: main (null.c:5)
==32752== 
9==32752== 
==32752== HEAP SUMMARY:
==32752==     in use at exit: 0 bytes in 0 blocks
==32752==   total heap usage: 2 allocs, 2 frees, 1,424 bytes allocated
==32752== 
==32752== All heap blocks were freed -- no leaks are possible
==32752== 
==32752== For lists of detected and suppressed errors, rerun with: -s
==32752== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

7.
```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int* arr = malloc(100 * sizeof(int));
    for(int i=0; i < 100; i++) arr[i] = i;
    free(arr+8);
    return 0;
}

```
the program is compiled successfully (gave warning only), then when I ran it: it gives me error
```
munmap_chunk(): invalid pointer
Aborted (core dumped)
```
No I didn't need a tool to know the type of the problem, but when I ran valgrind it showed me more context (and where exactly the error ocurred) which was very beneficial for debugging.

8.
```c
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;        
    size_t size;      
    size_t capacity;  
} Vector;

void vector_init(Vector *v) {
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}

void vector_push_back(Vector *v, int value) {
    if (v->size == v->capacity) {
        size_t new_capacity = (v->capacity == 0) ? 1 : v->capacity * 2;
        int *new_data = realloc(v->data, new_capacity * sizeof(int));
        if (!new_data) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
        v->data = new_data;
        v->capacity = new_capacity;
    }
    v->data[v->size] = value;
    v->size++;
}

void vector_free(Vector *v) {
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}

void vector_print(const Vector *v) {
    printf("[");
    for (size_t i = 0; i < v->size; i++) {
        printf("%d", v->data[i]);
        if (i < v->size - 1) printf(", ");
    }
    printf("]\n");
}

int main() {
    Vector v;
    vector_init(&v);

    for (int i = 1; i <= 10; i++) {
        vector_push_back(&v, i);
        vector_print(&v);
    }

    vector_free(&v);
    return 0;
}

```

9. I'll try :)
