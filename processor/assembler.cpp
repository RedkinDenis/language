#include "headers/assembler.h"

int main(int argc, char* argv[])
{
    char* inpName = (char*)"processor/prog_assm.txt";
    char* outName = (char*)"processor/prog_assm.bin";

    check_arguments(argc, argv, inpName, outName);

    FILE* read = fopen(inpName, "rb");

    int fsize = GetFileSize(read);
    struct Data data = input_data(read);
    // dump_data(&data);

    fclose(read);

    FILE* out = fopen(outName, "wb");

    enum err res = assembler(out, data.lines, data.quant);

    // printf(" END ");
    // clear_data(&data);

    fclose(out);

}

void check_arguments(int argc, char* argv[], char* inpName, char* outName)
{
    if(argc == 3)
    {
        inpName = argv[1];
        outName = argv[2];
    }
    else if(argc == 2)
    {
        inpName = argv[1];
    }
}

enum byte_codes comm_det(const char* comm)
{
#define Define_Command(str, enum)     \
    if(strcmp(comm, str) == 0)        \
        return enum;
#include "headers/command.h"
#undef Define_Command

    return ERR;
}

enum byte_codes reg_det(const char* reg)
{
#define Define_Command(str, enum)     \
    if(strncmp(reg, str, 2) == 0)     \
        return enum;
#include "headers/registers.h"
#undef Define_Command

    return ERR;
}

enum err assembler(FILE* out, struct line* data, int nLines)
{
    CHECK_PTR(out);
    CHECK_PTR(data);

    int comm = 0;
    int num = 0;
    int num1 = 0;
    int len = nLines;

    int ptr = 0;

    for(int i = 0; i < nLines; i++)
    {
        while(*(data[i].str) == ' ')
        {
            data[i].str++;
            data[i].len -= 1;
        }


        for(int k = 0; k < data[i].len; k++)
        {
            if(data[i].str[k] == ' ')
            {
                data[i].str[k] = '\0';
                len++;
                k = data[i].len;
            }
            if(data[i].str[k] == '$' && (k != 0))
            {
                while(k < data[i].len)
                {
                    data[i].str[k] = '\0';
                    k++;
                }
            }
        }
    }

    void* temp = 0;

    struct mark* marks = 0;
    CALLOC(marks, struct mark, MARK_QUANTITY);

    int marks_len = 0;
    int n = 0;

    char* buffer = 0;
    CALLOC(buffer, char, len * 4);

    for(int i = 0; i < nLines; i++)
    {

        comm = comm_det(data[i].str);
        if(comm == ADD || comm == SUB || comm == MUL || comm == DIV || comm == HLT || comm == OUT || comm == RET || comm == OUTC || comm == SQRT || comm == IN)
            ptr += command;

        else if(comm == ERR)
        {
            if(strchr(data[i].str, ':') != NULL || strchr(data[i].str, '$') != NULL)
            {
                for(int j = 0; data[i].str[j - 1] != ':'; j++)
                    marks[marks_len].num += data[i].str[j];

                marks[marks_len].adress = ptr;
                marks_len++;
            }
        }
        else
        {
            if((strstr(data[i].str + strlen(data[i].str) + 1, "ax") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "bx") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "cx") != NULL) || (strstr(data[i].str + strlen(data[i].str) + 1, "dx") != NULL))
            {
                ptr += (command + reg);
                if(strchr(data[i].str + strlen(data[i].str) + 1, '+') != NULL)
                    ptr += 3;
            }
            else
                ptr += (command + number);
        }
    }

    ptr = 0;
    for(int i = 0; i < nLines; i++)
    {
        comm = comm_det(data[i].str);

        switch(comm & 0x1F)
        {
            #define CMD_GEN(str, NAME, assm_code, proc_code, dis_code)  \
            case NAME:                                                  \
                assm_code                                               \
                break;
            #include "headers/CMD_GEN.h"

            case ERR:
                if(strchr(data[i].str, ':') != NULL || strchr(data[i].str, '$') != NULL || strlen(data[i].str) == 0)
                    break;
                printf("line %d: %s\n", i, data[i].str);
                return UNKNOWN_COMMAND_NAME;
        }

        
    }

    // for(int i = 0; i < ptr; i++)
    //     printf(" %d", (uint8_t)buffer[i]);
    // printf("\n");
    fwrite(buffer, sizeof(char), ptr, out);

    return SUCCESS;
}
