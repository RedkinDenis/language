#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include "input_output.h"
#include "encoding.h"
#include "..\..\..\err_codes.h"
#include "stack.h"
#include "DSL.h"

void check_arguments(int argc, char* argv[], char* inpName, char* outName);

enum err assembler(FILE* out, struct line* data, int nLines);

enum byte_codes comm_det(const char* comm);

#define MARK_QUANTITY 20

struct mark
{
    int num = 0;
    int adress = 0;
};

#endif //ASSEMBLER_H
