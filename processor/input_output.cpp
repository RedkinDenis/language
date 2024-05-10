#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>

#include "headers/input_output.h"
#include "../../err_codes.h"
#include "headers/DSL.h"

static err fill_buffer (FILE* read, char** buf);

int GetFileSize(FILE* fp)
{
    int startPos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, startPos, SEEK_SET);
    return fsize;
}

err fill_buffer (FILE* read, char** buf)
{
    CHECK_PTR(read);
    CHECK_PTR(buf);

    int fsize = GetFileSize(read);

    CALLOC(*buf, char, (fsize + 1));

    fread(*buf, sizeof(char), fsize, read);

    return SUCCESS;
}

void clear_tabs(struct line* data, int nLines)
{
    for(int i = 0; i < nLines; i++)
        while(data[i].str[0] == ' ')
        {
            data[i].str[0] = '\0';
            data[i].str++;
        }
}

Data input_data (FILE* input)
{
    int fsize = GetFileSize(input);

    char* buffer = NULL;
    fill_buffer(input, &buffer);

    char* buffer_cp = buffer;

    // printf ("Buffer: \n%s\n end of buffer\n", buffer);

    Data data = {};

    for (int i = 0; i < fsize + 1; i++)
        if (buffer[i] == '\n' && buffer[i - 1] == '\n')
                data.quant -= 1;

    for (int i = 0; i < fsize + 1; i++)
    {
        if (buffer[i] == '\n')
        {
            data.quant += 1;
            buffer[i] = '\0';
        }
    }

    data.lines = (line*)calloc(data.quant, sizeof(line));

    if (buffer[fsize - 1] != '\n')
        data.quant += 1;

    for (int i = 0; i < data.quant; i++)
    {
        // printf("%d - %s\n", i, buffer);
        data.lines[i].str = strdup(buffer);
        data.lines[i].len = strlen(buffer);
        buffer += strlen(buffer);

        while (*buffer == '\0') 
            buffer++;
    }

    free(buffer_cp);

    return data;
}

void clear_data (Data* data)
{
    for (int i = 0; i < data->quant; i++)
        free(data->lines[i].str);

    free(data->lines);
    free(data);
}

void dump_data (Data* data)
{
    printf ("qant - %d \n", data->quant);
    for (int i = 0; i < data->quant; i++)
        printf("%s: len = %d \n", data->lines[i].str, data->lines[i].len);
}