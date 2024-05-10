#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "input_output.h"
#include "encoding.h"
#include "DSL.h"
#include "C:\Users\vp717\Desktop\ilab\err_codes.h"

void check_arguments(int argc, char* argv[], char* inpName, char* outName);

err fill_data(char** data, FILE* read, int fsize);

err dis_assembler(FILE* out, int fsize, char* data);

#endif //DISASSEMBLER_H
