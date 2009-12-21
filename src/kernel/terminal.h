typedef struct lisp_terminal
{
  stream *stdin;
  stream *stdout;
  struct sw_screen *screen;
}__attribute__((packed)) lisp_terminal;

extern lisp_terminal *newlisp_terminal();
extern cons *termmain(lisp_terminal *t);
extern lisp_terminal *current_terminal;
