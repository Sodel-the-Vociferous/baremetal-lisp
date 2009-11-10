#include "defs.h"
#include "main.h"
#include "multiboot.h"

int kmain(multiboot_header_t *mboot)
{
  return 0xDEADBEEF;
}

struct mmap *newmmap(void *begin)
{
  
}
 
