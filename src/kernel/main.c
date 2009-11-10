#include "defs.h"
#include "main.h"
#include "multiboot.h"

int kmain(struct multiboot_header *mboot)
{
  return 0xDEADBEEF;
}

struct mmap *newmmap(void *begin)
{
  
}
 
