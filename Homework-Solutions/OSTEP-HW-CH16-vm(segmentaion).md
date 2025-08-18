# Homework (Simulation)
1.

```
fikri@Fikri:~/Desktop/OS/OSTEP/Homework/vm-segmentation$ python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0
ARG seed 0
ARG address space size 128
ARG phys mem size 512
Segment register information:

  Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
  Segment 0 limit                  : 20

  Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
  Segment 1 limit                  : 20

Virtual Address Trace
  VA  0: 0x0000006c (decimal:  108) --> PA or segmentation violation?
  VA  1: 0x00000061 (decimal:   97) --> PA or segmentation violation?
  VA  2: 0x00000035 (decimal:   53) --> PA or segmentation violation?
  VA  3: 0x00000021 (decimal:   33) --> PA or segmentation violation?
  VA  4: 0x00000041 (decimal:   65) --> PA or segmentation violation?

```


the address space is 128, which means the left-most bit will be allocated for the segment selection, and the right-most 6-bits will be left for the address offset. 

Hence,
- VA0: 108 -> `1101100` -> the 7th bit from the left = 1-> referencing segment 1 -> the other 6 bits = 44. I want to reach virtual address 44, so the offset = |44 - 64| = 20, 20  <= limit (20), then this address is valid. PA = 512 - 20 = 492.
- VA1: 97 -> `1100001` -> the 7th bit from the left = 1 -> referencing segment 1 -> the other 6 bits = 1. I want to reach virtual address 1, so the offset = |1 - 64| = 63, 63 > limit (20), this will lead to segmentation violation.  
- VA2: 97 -> `0110101` -> the 7th bit from the left = 0 -> referencing segment 1 -> the other 6 bits = 53. I want to reach virtual address 53, so the offset = |53 - 0 + 1| = 54, 54 > limit (20), this will lead to segmentation violation. 
- VA3: 33 -> `0100001` -> the 7th bit from the left = 0 -> referencing segment 1 -> the other 6 bits = 33. I want to reach virtual address 33, so the offset = |33 - 0 + 1| = 34, 33 > limit (20), this will lead to segmentation violation.
- VA4: 65 -> `1000001` -> the 7th bit from the left = 1 -> referencing segment 1 -> the other 6 bits = 1. I want to reach virtual address 1, so the offset = |1 - 64| = 63, 64 > limit (20), this will lead to segmentation violation. 
Note: in the seg0 calc, I compute |final virt. add - lowest virt. add (0) + 1 | because the 0 is reserving a place int he size of the virtual address space, as well as we do in the seg1, |final virt. add - biggest virt. add (63) + 1|
the other two options are the same. 

2. 
- highest legal virtual address in seg0: 19
- lowest legal virtual address in seg1: 108
- lowest illegal address in the entire virtual address space: 20
- highest illegal address in the entire virtual address space: 107
We could check these numbers by running 

```
fikri@Fikri:~/Desktop/OS/OSTEP/Homework/vm-segmentation$ python3 ./segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2 -A 19,108,20,107 -c
ARG seed 2
ARG address space size 128
ARG phys mem size 512

Segment register information:

  Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
  Segment 0 limit                  : 20

  Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
  Segment 1 limit                  : 20

Virtual Address Trace
  VA  0: 0x00000013 (decimal:   19) --> VALID in SEG0: 0x00000013 (decimal:   19)
  VA  1: 0x0000006c (decimal:  108) --> VALID in SEG1: 0x000001ec (decimal:  492)
  VA  2: 0x00000014 (decimal:   20) --> SEGMENTATION VIOLATION (SEG0)
  VA  3: 0x0000006b (decimal:  107) --> SEGMENTATION VIOLATION (SEG1)

```

3.
```
segmentation.py -a 16 -p 128
-A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
--b0 0 --l0 2 --b1 16 --l1 2
```

4. yes, we'll make the distance between seg0 and seg1 is only 10% of the entire virtual address, e.g.,
```
./segmentation.py -a 100 -p 512 -n 1000 --b0 0 --l0 45 --b1 100 --l1 45 -c

```

5. yes we'll set the limit register (l) to 0

```
/segmentation.py -a 100 -p 512 -n 10 --b0 0 --l0 0 --b1 100 --l1 0 -c

```
