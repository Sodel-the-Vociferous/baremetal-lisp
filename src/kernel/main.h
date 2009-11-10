struct mmap
{
  int gran;
  int size;
  long *map;
  int oldest_free_index;
  int newest_free_index;
  int free[1024];
}__attribute__((packed));

