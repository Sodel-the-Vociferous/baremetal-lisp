//Types
#define LIST 1
#define T 2
#define SYMBOL 3
#define STRING 4
#define INTEGER 5
#define FLOATING 6

typedef struct pair
{
  char type;
  struct pair *car;
  struct pair *cdr;
}__attribute__((packed)) pair;

typedef struct
{
  char type;
  char text[4];
  
  pair *cdr;
}__attribute__((packed)) symbol;

typedef struct
{
  char type;
  char text[4];
  
  pair *cdr;
}__attribute__((packed)) string;

typedef struct
{
  char type;
  long long num;
}__attribute__((packed)) integer;

typedef struct
{
  char type;
  double num;
}__attribute__((packed)) floating;

extern pair *t;
extern pair *nil;

void init ();
void *kallocate();
pair *whitespacep (char c);
string *strtolstr (char *str);
symbol *strtosym (char *str);
symbol *readsym (char *str);
inline pair *knull (pair *exp);
inline pair *klistp (pair *exp);
inline pair *ksymbolp (pair *exp);
inline pair *kcar (pair *exp);
inline pair *kcdr (pair *exp);
inline pair *keq (pair *exp1, pair *exp2);
inline pair *kequal (pair *exp1, pair *exp2);
inline pair *knumberp (pair *exp);
inline pair *katom (pair *exp);
inline pair *kquote (pair *exp);
inline pair *knth (int i, pair *l);
char *skipwhite (char *str);
pair *delimiterp(char c);
pair *ob_assoc(symbol *sym, pair *env);
pair *kread (char **str);
pair *eval (pair *exp, pair *env);
