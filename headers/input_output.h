#ifndef INPUT_OTPUT_H
#define INPUT_OTPUT_H

#include <stdio.h> 
#include "..\..\err_codes.h"
#include "differentiator.h"
#include "../headers/DSL.h"

struct line 
{
    char* str = NULL;
    size_t len = 0;
};

struct Data 
{
    line* lines = NULL;
    size_t quant = 0;
};

Data input_data (const char* file_name);

void dump_data (Data* data);

void clear_data (Data* data);

int GetFileSize(FILE* fp);

err print_tree (Node* head);

void print_data (Node* head);

err fprint_tree (FILE* out, Node* head);

err importTree (FILE* read ,Node* head);

err draw_tree (Node* tree);

err fill_buffer (FILE* read, char** buf);

char* tex_tree (Node* tree);

#endif // INPUT_OTPUT_H