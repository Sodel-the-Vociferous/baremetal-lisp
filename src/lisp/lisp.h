#define HASH_TABLE_SIZE 255

//Types
#define T 1
#define CONS 2
#define FIXNUM 3


#define BITS32

#ifdef BITS32

typedef struct cons
{
  unsigned short type;
  struct cons *car;
  struct cons *cdr;
} __attribute__((packed)) cons;


//Numbers
//Integers
typedef struct fixnum;
{
  unsigned short type;
  long num;
} __attribute__((packed)) fixnum;

typedef struct bignum;
{
  unsigned short type;
  long num;
  struct bignum *next;
} __attribute__((packed)) bignum;

//Ratios
typedef struct ratio
{
  unsigned short type;
  struct fixnum *numerator;
  struct fixnum *denominator;
} __attribute__((packed)) rato;

//Floating point
typedef struct single
{
  unsigned short type;
  unsigned short sign : 1;
  unsigned short base : 15;
  struct fixnum *exponent;
  struct fixnum *integer;
} __attribute__((packed)) single;


//Characters
//Standard character
typedef struct base_char
{
  unsigned short type;
  char c;
}__attribute__((packed)) base_char;

typedef struct vector
{
  unsigned short type;
  int size;
  unsigned short datatype;
  struct cons **v;
  struct vector *next;
}__attribute__((packed)) vector;

typedef struct array
{
  unsigned short type;
  unsigned short rank;
  struct vector **a;
  struct array *next;
}__attribute__((packed)) array;

#endif

extern cons *nil;
extern cons *t;
