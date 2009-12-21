#define XCOLUMNS 80
#define YROWS 25
#define SCREENSTART 0xB8000

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define GREY 7
#define DGRAY 8
#define LBLUE 9
#define LGREEN 10
#define LCYAN 11
#define LRED 12
#define LMAGENTA 13
#define LBROWN 14
#define WHITE 15

typedef struct sw_screen
{
  uchar fgattrib : 4;
  uchar bgattrib : 4;
  uint x;
  uint y;
  ushort *screen;
}__attribute__((packed)) sw_screen;

extern void inithw_screen();
extern void cls();
extern void draw_screen(struct sw_screen *s);
