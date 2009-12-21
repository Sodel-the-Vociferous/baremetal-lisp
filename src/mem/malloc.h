/* Map of memory usage. Flat, and very, very temporary.
 */

#ifndef MALLOC_H
#define MALLOC_H

/* struct mmap */
/* { */
/*   void* type; */
/*   int gran;/\* Granularity in bytes *\/ */
/*   int size;/\* Size in bytes, divided by granularity. *\/ */
/*   long *map;/\* Pointer to the actual map. *\/ */
/*   int oldest_free_index; */
/*   int newest_free_index; */
/*   int free[1024]; */
/* }__attribute__((packed)); */

extern void *firstfree;

extern void *malloc(int sz);
#endif
