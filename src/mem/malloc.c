void *firstfree;

void *malloc(int sz)
{
  void *to_ret = firstfree;
  firstfree += sz;
  return to_ret;
}
