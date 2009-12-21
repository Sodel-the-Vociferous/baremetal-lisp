#include "defs.h"

unsigned char *memcpy(uchar *dest, const uchar *src, uint count)
{/* Copy count bytes of data from src' to dest, and return dest.
  */
    const char *source = (const char *)src;
    char *destination = (char *)dest;
    for(;count!=0;count--)
    {
        *destination++ = *source++;
    }
    return dest;
}

unsigned char *memset(uchar *dest, uchar val, uint count)
{/* Set count bytes from location dest to value val.
  */
    char *temp = (char *)dest;
    for(; count !=0; count--)
    {
        *temp++ = val;
    }
    return dest;
}

unsigned short *memsetw(ushort *dest, ushort val, uint count)
{/* Set count bytes from location dest to value val.
  */
    ushort *temp = (ushort*)dest;
    for(; count !=0; count--)
    {
        *temp++ = val;
    }
    return dest;
}

/* Read from an I/O port.
 */
uchar inb (ushort _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/* Write to an I/O port.
 */
void outb (ushort _port, uchar _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}
