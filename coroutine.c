#include "coroutine.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void coroutine_finish() {
  fprintf(stderr, "TODO: coroutine_finish is not implemented\n");
  exit(69);
}

static void overflow_fail() {
  fprintf(stderr, "ERROR: Too many coroutines\n");
  exit(69);
}

typedef struct coroutine_context {
  void *rsp;
  void *rbp;
  void *rip;
} coroutine_context;

#define COROUTINES_CAPACITY 10
#define STACK_CAPACITY (4 * 1024)

static size_t contexts_count = 0;
static size_t contexts_current = 0;
static uint8_t stacks[COROUTINES_CAPACITY][STACK_CAPACITY] __attribute__((aligned(16)));
static coroutine_context contexts[COROUTINES_CAPACITY];

void volatile coroutine_go(void (*f)(void)) {
  if (contexts_count >= COROUTINES_CAPACITY)
    overflow_fail();
  size_t current = contexts_count++;
  void *stack_top = &(stacks[current][STACK_CAPACITY]);
  *((void **)(stack_top - 8)) = coroutine_finish;
  contexts[current].rsp = stack_top - 8;
  contexts[current].rbp = NULL;
  contexts[current].rip = (void *)f;
}

__attribute__((naked)) void volatile coroutine_init() {
  if (contexts_count >= COROUTINES_CAPACITY)
    overflow_fail();
  size_t current = contexts_count++;
  asm volatile("pop %0" : "=r"(contexts[current].rip)::"memory");
  asm volatile("mov %%rsp, %0" : "=r"(contexts[current].rsp)::"memory");
  asm volatile("mov %%rbp, %0" : "=r"(contexts[current].rbp)::"memory");
  asm volatile("jmp *%0" ::"r"(contexts[current].rip));
  __builtin_unreachable();
}

__attribute__((naked)) void volatile coroutine_yield() {
  asm volatile("pop %0" : "=r"(contexts[contexts_current].rip)::"memory");
  asm volatile("mov %%rsp, %0" : "=r"(contexts[contexts_current].rsp)::"memory");
  asm volatile("mov %%rbp, %0" : "=r"(contexts[contexts_current].rbp)::"memory");
  contexts_current = (contexts_current + 1) % contexts_count;
  asm volatile("mov  %0, %%rsp" ::"r"(contexts[contexts_current].rsp) : /* don't tell them we messed with rsp*/);
  asm volatile("mov  %0, %%rbp" ::"r"(contexts[contexts_current].rbp) : /* don't tell them we messed with rbp*/);
  asm volatile("jmp *%0" ::"r"(contexts[contexts_current].rip));
  __builtin_unreachable();
}
