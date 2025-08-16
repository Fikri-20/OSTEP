1.
- Using -s 1
  VA0: segmentation violation
  VA1: PA = 13884 + 261 = 1445 (decimal)
  VA2: segmentation violation
  VA3: segmentation violation
  VA4: segmentation violation

- Using -s 2
  VA0: PA = 15529 + 57 = 15586 (decimal)
  VA1: PA = 15529 + 86 = 15615 (decimal)
  VA2: Segmentation violation
  VA3: segmentation violation
  VA4: segmentation violation

- Using -s 3
  VA0: segmentation violation
  VA1: segmentation violation
  VA2: segmentation violation
  VA3: PA = 8916 + 67 = 8983 (decimal)
  VA4: PA = 8916 + 13 = 8929 (decimal)

2. I have to set the limit register to (at least) 930, in order to translate any of these virtual addresses successfully. (as the biggest virtual address is 929)

3. the max base register would be: 16kib - 100 = 16384 - 100 = 16284 (decimal)
4. done
5. A virtual address (VA) is valid if:
```
VA < limi
```
Otherwise, it causes a segmentation violation.

If you generate VAs randomly, sometimes they’ll be valid, sometimes not — and the fraction that are valid depends on how big the limit is compared to the total virtual address space size.

Example

If:

Address space size = 1024 bytes

Limit = 256

Then:

Only addresses 0–255 are valid.

That’s 256 / 1024 = 0.25 → 25% of random addresses will be valid.

If Limit = 1024:

All addresses are valid → 1024 / 1024 = 1.0 → 100% valid.

If Limit = 0:

No address is valid → 0 / 1024 = 0.0.

So the as bigger the limit, the bigger the fraction of randomly-generated adress is valid
So the relationship is almost linear.

![[fraction_valid_vs_limit.png]]