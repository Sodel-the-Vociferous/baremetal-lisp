#define HASH_TABLE_SIZE 255

//Types
#define T 1
#define CONS 2
#define FIXNUM 3
#define BIGNUM 4
#define RATIO 5
#define SINGLE 6
#define BASE_CHAR 7
#define VECTOR 8
#define ARRAY 9
#define COMPILED_FUNCTION 10
#define STRING 11
#define SYMBOL 12
#define FUNCTION 13
#define HASH_TABLE 14
#define PACKAGE 15
#define PROCINFO 16

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
typedef struct fixnum
{
  unsigned short type;
  long num;
} __attribute__((packed)) fixnum;

typedef struct bignum
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
} __attribute__((packed)) ratio;

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
  struct cons *plist;
  int size;
  unsigned short datatype;
  struct cons **v;
  struct vector *next;
}__attribute__((packed)) vector;

typedef struct array
{
  unsigned short type;
  struct cons *plist;
  unsigned short rank;
  struct vector **a;
  struct array *next;
}__attribute__((packed)) array;

//Functions
typedef struct compiled_function
{
  unsigned short type;
  struct cons *plist;
  struct cons *lambda_list;
  struct cons *env;
  struct cons *(*function)(struct cons*);
}__attribute__((packed)) compiled_function;

typedef struct function
{
  unsigned short type;
  struct cons *plist;
  struct cons *lambda_list;
  struct cons *env;
  struct cons *function;
}__attribute__((packed)) function;

typedef struct symbol
{
  unsigned short type;
  struct vector *name;
  struct cons *plist;
  struct package *home_package;
  struct cons *value;
  struct cons *function;
}__attribute__((packed)) symbol;

typedef struct hash_table
{
  unsigned short type;
  struct cons *plist;
  struct cons **table;
}__attribute__((packed)) hash_table;

typedef struct package
{
  unsigned short type;
  struct cons *plist;
  struct vector *name;
  struct vector *global;//Change to hash table one day
}__attribute__((packed)) package;

typedef struct procinfo//Global stuff for each Lisp 'process'.
{
  unsigned short type;
  struct symbol *package;
  //Just for speed. :)
  //This is the *pakage* binding.
  //The current package can now be swiftly looked-up, without having to look up the symbol in :cl.
  struct cons *packages;
  //Packages that are defined for this process. A list unique to each process.
  //common-lisp is always first package, cl-user second, keywprd third, followed by the rest.
}__attribute__((packed)) procinfo;

#endif

extern cons *nil;
extern cons *t;


procinfo *init();
cons *newcons();
fixnum *newfixnum();
bignum *newbignum();
ratio *newratio(fixnum *n, fixnum *d);
single *newsingle();
base_char *newbase_char();
vector *newvector(int size);
package *newpackage();
vector *strtolstr(char *str);
cons *null (cons *env);
cons *numberp(cons *env);
cons *fcons(cons *env);
cons *car(cons *env);
cons *rplaca(cons *env);
cons *cdr(cons *env);
cons *rplacd(cons *env);
symbol *fintern(vector *name, package *p);
cons *bchareq(base_char *a, base_char *b);
cons *bcharequal(base_char *a, base_char *b);
cons *bstringeq(vector *a, vector *b);
cons *bstringequal(vector *a, vector *b);
cons *eq (cons *env);
cons *eql (cons *env);
cons *feql (cons *a, cons *b);
cons *equal (cons *env);
cons *lookup(char *name, cons *env);
cons *eval(cons *exp, cons *env);
cons *extend_env(cons *env);
cons *envbind(cons *env, cons *binding);
cons *evalambda(cons *lambda_list, cons *args, cons *env);
