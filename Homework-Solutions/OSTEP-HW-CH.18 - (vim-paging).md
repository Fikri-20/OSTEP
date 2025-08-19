# Homework (Simulation)
1.
- As the address space grows, the page table increases (more page
table entries). On the other hand, as the page size increase, the page
table decreases (less page table entries).
- increasing the page table leads to un-used space, let's assume we
have 1MB page size, and the program takes only 512Kb, then the second
half of the 1MB is un-used.

2. As you increase -u, more pages in the virtual address space are
mapped to physical memory. The page table goes from having mostly
invalid entries (lots of unused virtual pages) to having all valid
entries (every virtual page backed by a physical frame). This makes
the virtual-to-physical translation more complete and reduces the
likelihood of accessing invalid memory.

3.
- the first case is unrealistic, as the page size is very very low (8
byte) which will lead to very big number of page entries, so much
memory!
- the last case is unrealistic too, as the page size is very big, this
leads to wasted space, as many processes will not use the 1MB page
entirly!

4. It throws me an error saying that the physical memory must be
greater than address space size
```
fikri@Fikri:~/Desktop/OS/ostep-homework/vm-paging$ python3 ./paging-linear-translate.py -P 1m -a 512m -p 256m -v -s 3 -c
ARG seed 3
ARG address space size 512m
ARG phys mem size 256m
ARG page size 1m
ARG verbose True
ARG addresses -1

Error: physical memory size must be GREATER than address space size (for this simulation)

```

