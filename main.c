#include <stdio.h>
#include <stdbool.h>
#include "coroutine.h"

void counter(void)
{
  for (int i = 0; i < 10; ++i) {
    printf("%d\n", i);
    coroutine_yield();
  }
}

int main()
{
  coroutine_init();
  coroutine_go(counter);
  coroutine_go(counter);
  while (true) coroutine_yield();
  return 0;
}
