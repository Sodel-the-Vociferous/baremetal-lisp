#define HASH_TABLE_SIZE 255

//Types. One day, these will be changed to pointers to symbols.
#define T 1
#define LIST 2
#define FIXNUM 3
#define BIGNUM 4
#define RATIO 5
#define SINGLE 6
#define BASE_CHAR 7
#define SIMPLE_VECTOR 8
#define ARRAY 9
#define COMPILED_FUNCTION 10
#define STRING 11
#define SYMBOL 12
#define FUNCTION 13
#define HASH_TABLE 14
#define PACKAGE 15
#define PROCINFO 16
#define STREAM 17

#define BITS32

#ifdef BITS32


/* CONS cell.
 *
 * A basic linked list node: the fundamental quanta of Lisp lists.
 * A CONS cell consists of two pointers: the CAR, and the CDR.
 ***************************************************************
 * Note: this is an artifact of the assembly instructions used *
 * to implement CONS cells on the first machine to host Lisp.  *
 ***************************************************************
 * In a sigle CONS cell, these are pointers to any two objects.
 * When part of a List, the CAR always points to some object,
 * and the CDR to the next cell in the list.
 */
typedef struct cons
{
  unsigned int type;;
  struct cons *car;
  struct cons *cdr;
} __attribute__((packed)) cons;

/* Fixnum integer
 *
 * An integer that can be stored and manipulated by the machine;
 * Alternately put, an integer that can be dealt with in C, with no
 * special infrastructure.
 */
typedef struct fixnum
{
  unsigned int type;;
  long num;
} __attribute__((packed)) fixnum;

/* Bignum integer
 * 
 * For number which are too large for the machine to handle without
 * some software infrastructure. Here, implemented as a series of
 * bignum object, each storing the same amount of data as a fixnum.
 * For calculations, the series is compounded. 
 * For reference, the first bignum is always the least significant 
 * (smallest) and the last bignum is always the most significant
 * (largest).
 */
typedef struct bignum
{
  unsigned int type;;
  long num;
  struct bignum *next;
} __attribute__((packed)) bignum;

//Ratios

/* Ratio
 *
 * Same as the ratios you learned in school: a numerator, and a
 * denominator. Both must be fixnums (integers).
 * Lisp will try to keep ratios as ratios during
 * mathematical operations. However, if a ratio interacts with
 * a floating point number, the result will be floating-point.
 */
typedef struct ratio
{
  unsigned int type;;
  struct fixnum *numerator;
  struct fixnum *denominator;
} __attribute__((packed)) ratio;

/* Floating-Point irrational number
 *
 * Floating point numbers, while finite, are treated as irrational
 * numbers. To convert them to ratios, the Lisp interpreter must be
 * explicitly told to make the conversion.
 * Consists of a 1 bit sign, 15-bit base, fixnum exponent, and
 * fixnum integer.
 */
typedef struct single
{
  unsigned int type;;
  unsigned short sign : 1;
  unsigned short base : 15;
  struct fixnum *exponent;
  struct fixnum *integer;
} __attribute__((packed)) single;

//Characters

/* Base character
 * This character has no bells and whistles, and is barely a Lisp object.
 * It consists only of an 8-bit ASCII character.
 */
typedef struct base_char
{
  unsigned int type;;
  char c;
}__attribute__((packed)) base_char;

/* Array
 * 
 * An array consists of a property list, a rank (which
 * is the number of dimensions), a length, and a C array 
 * of pointers to Lisp objects. The 'next' field is a pointer
 * to another array, which is treated as an extension to 
 * the current array. This means that arrays can be resized
 * with much less of a performance hiccup. Lookup times are
 * decreased by several CPU instructions for extended arrays,
 * meaning that there is virtually no practical problem.
 *
 * A vector is a one-dimensional array. Strings are vectors
 * of characters.
 */
typedef struct array
{
  unsigned int type;;
  struct cons *plist;
  struct fixnum *rank;
  struct fixnum *length;
  struct cons ***a;//Array
  struct array *next;//Just in case we want to expand it.
}__attribute__((packed)) array;

//Functions


/* Compiled Function
 *
 * This is a function that has been compiled into machine
 * code, either referenced from the C code, or compiled
 * by the as-of-yet non-existent Lisp compiler.
 * Consists of a property list, a lambda list (which holds
 * argument variable names), the lexical environment in which
 * the function was defined, and a pointer to the function proper.
 */
typedef struct compiled_function
{
  unsigned int type;;
  struct cons *plist;
  struct cons *lambda_list;
  struct cons *env;
  struct cons *(*fun)(cons* env);
}__attribute__((packed)) compiled_function;

/* Function
 *
 * Instead of being compiled, these functions are Lisp expressions
 * to be evaluated. Virtually identical to compiled functions,
 * except the pointer is to a linked list.
 */
typedef struct function
{
  unsigned int type;;
  struct cons *plist;
  struct cons *lambda_list;
  struct cons *env;
  struct cons *fun;
}__attribute__((packed)) function;

/* Symbol
 * 
 * Symbols are comprised of a name in the form of a string, a
 * property list, the symbol's home-package, a value, a function,
 * and a class. There is only one symbol with a given name per
 * package.
 */
typedef struct symbol
{
  unsigned int type;;
  struct array *name;
  struct cons *plist;
  struct package *home_package;
  struct cons *value;
  struct function *fun;
  struct cons *class;
}__attribute__((packed)) symbol;

/* Package
 *
 * A package is a collection of symbols and values.
 * They are used to prevent function and variable name
 * collisions when using libraries, and external code.
 * Each package consists of a property list, a name in
 * the form of a string, and a dynamic environment which
 * holds all symbols and their dynamic bindings.
 */
typedef struct package
{
  unsigned int type;;
  struct cons *plist;
  struct array *name;
  struct array *global;//Change to hash table one day
}__attribute__((packed)) package;


/* Process Information
 *
 * These keep separate processes separate. The list of packages
 * accessible by a process resides in these structures.
 * The package list is currently a linked list, but may one
 * day be changed to a hash table.
 * "package_sym" is just a pointer to the *PACKAGE* symbol
 * in the Common Lisp package. The only reson it is there
 * is to remove the need to search through the Common Lisp
 * package to find the value of the current package in use
 * every evaluation cycle. The first element of an environment
 * list is a procinfo.
 */
typedef struct procinfo//Global stuff for each Lisp 'process'.
{
  unsigned int type;;
  struct symbol *package_s;
  struct cons *packages;
}__attribute__((packed)) procinfo;


/* Stream
 *
 * A stream is a source or sink of bytes or characters.
 * Used for communication, or to move through a series of
 * elements.
 * Consists of a property list, a read index, a write index, 
 * and pointers to up to two vectors.
 *
 * Reading will move through the read vector until it 
 * a) catches up with write ((read_index == write_index) &&
 * (rv == wv)), meaning that the stream has no new input, or 
 * b) reaches the end of the vector. If it reaches the end,
 * and it still hasn't caught up with write, it moves into 
 * the vector extension pointed to by the vector's "next".
 * (See array documentation.) If there is no "next", the
 * stream has "dried up".
 *
 * Write will append an object to the stream. If it reaches 
 * the end of the vector, it will extend it, via "next".
 */
typedef struct stream
{
  unsigned int type;;
  struct cons *plist;
  int read_index;
  int write_index;
  struct array *rv;//read vector
  struct array *wv;//write vector
}__attribute__((packed)) stream;

/* Composite Stream
 *
 * A collection of streams, and a property list. These can 
 * be used to implement two-way streams, and broadcasts.
 */
typedef struct composite_stream
{
  unsigned int type;;
  struct cons *plist;
  struct array *streams;
}__attribute__((packed)) composite_stream;


#endif

extern cons *nil;
extern cons *t;

extern procinfo *init();
extern cons *newcons();
extern fixnum *newfixnum(long i); 
extern bignum *newbignum();
extern ratio *newratio(fixnum *n, fixnum *d);
extern single *newsingle();
extern base_char *newbase_char();
extern array *newarray(long rank, int length);
extern array *newsimple_vector(int size);
extern package *newpackage();
extern compiled_function *newcompiled_function();
extern function *newfunction();
extern stream *newstream();
extern base_char *ctolc(char c);
extern array *strtolstr(char *str);
extern cons *null (cons *a);
extern cons *numberp(cons *a);
extern cons *fcons(cons *a, cons *b);
extern cons *car(cons *a);
extern cons *rplaca(cons *a, cons *new);
extern cons *cdr(cons *a);
extern cons *rplacd(cons *a, cons *new);
extern symbol *intern(array *name, package *p);
extern cons *chareq(base_char *a, base_char *b);
extern cons *charequal(base_char *a, base_char *b);
extern cons *stringeq(array *a, array *b);
extern cons *stringequal(array *a, array *b);
extern package *find_package(array *name, procinfo *pinfo);
extern cons *eq (cons *a, cons *b);
extern cons *eql (cons *a, cons *b);
extern cons *equal (cons *a, cons *b);
extern cons *lookup(char *name, cons *env);
extern cons *mkpair(cons *key, cons *value);
extern cons *eval(cons *exp, cons *env);
extern cons *extend_env(cons *env);
extern cons *envbind(symbol *sym, cons *value, cons *env);
extern cons *evalambda(cons *lambda_list, cons *args, cons *env);
extern cons *assoc(cons *key, cons *plist);
extern symbol *defun(symbol *sym, cons *lambda_list, cons *form, cons *env);
extern base_char *read_char(stream *str);
extern base_char *peek_char(stream *str);
extern cons *unread_char(base_char *c, stream *str);

