#ifndef COROUTINE_H
#define COROUTINE_H

void coroutine_go(void (*f)(void));
void coroutine_yield(void);
void coroutine_init(void);
#endif // COROUTINE_H
