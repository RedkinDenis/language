#ifndef INPUT_OTPUT_H
#define INPUT_OTPUT_H

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

void clear_tabs(struct line* data, int nLines);

Data input_data (FILE* input);

void dump_data (Data* data);

void clear_data (Data* data);

int GetFileSize(FILE* fp);

#endif // INPUT_OTPUT_H


