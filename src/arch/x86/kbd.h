struct keyboard_status
{
  ushort lctrl :1;
  ushort rctrl :1;
  ushort lalt :1;
  ushort ralt :1;
  ushort lshift :1;
  ushort rshift :1;
  ushort caps :1;
  ushort insert :1;
  ushort numlck :1;
  ushort scrlck :1;
  ushort other :1;
};

struct keyboard
{
  struct keyboard_status status;
  char buffer[256];
  uchar widx;//Write index.Yes, these are SUPPOSED to "wrap around" to 0 again. Simple solutions! Yay!
  uchar ridx;//Read index. Current read position.
  uchar wrap;//Whether the reader needs to catch up by wrapping.
};
