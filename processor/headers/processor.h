#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#include "input_output.h"
#include "encoding.h"
#include "C:\Users\vp717\Desktop\ilab\err_codes.h"
#include "DSL.h"
#include "stack.h"

struct processor
{
    char* data = {};
    int data_size;
    int ip = 0;
    elem_t* reg = 0;   //registers ax, bx, cx, dx
    elem_t* RAM = 0;
    struct Stack cmd_stk = {};
    struct Stack call_stk = {};
};

#endif //PROCESSOR_H
