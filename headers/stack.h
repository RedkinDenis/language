#ifndef STACK_H
#define STACK_H

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "../../err_codes.h"
#include "DSLdiff.h"

// typedef void* void*;

static void* poison = NULL;

struct Stack
{
    void** data = NULL;
    size_t capacity = 0;
    size_t size = 0;
};

err stack_pop(struct Stack* stk, void** pop_el);

err stack_dump(struct Stack* stk, int LINE, const char* stk_name, const char* file_name, const char* func_name);

err stack_ctor(struct Stack* stk, size_t capacity);

err stack_push(struct Stack* stk, void** x);

err stack_dtor(struct Stack* stk);

err capacity_down(struct Stack* stk);

err capacity_up(struct Stack* stk);

#endif //STACK_H
