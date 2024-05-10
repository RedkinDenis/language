#include "headers/processor.h"

static void check_arguments(int argc, char* argv[], char* inpName);

static err fill_proc(struct processor* proc, FILE* read, int fsize);

static err executor(struct processor* proc);

static err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name);

static err proc(struct processor* cmd_stk);

static err proc_free(struct processor* proc);

int main(int argc, char* argv[])
{
    char* inpName = (char*)"processor/prog_assm.bin";

    check_arguments(argc, argv, inpName);

    FOPEN(read, inpName, "rb")

    int fsize = GetFileSize(read) / sizeof(char);

    struct processor processor = {};

    fill_proc(&processor, read, fsize);

    proc(&processor);

    proc_free(&processor);
}

void check_arguments(int argc, char* argv[], char* inpName)
{
    if(argc == 2)
    {
        inpName = argv[1];
    }
}

err executor(struct processor* proc)
{
    elem_t x = 0, x1 = 0, x2 = 0, buf = 0, temp = 0;
    char cmd = 0;

    int liter = 0;
    err res;

    int running = 1;

    while(running)
    {
        // printf("\n=========================================================================\n");
        // PROC_DUMP(proc, executor)

        cmd = proc->data[proc->ip];
        switch(cmd & 0x1F)
        {
            #define CMD_GEN(str, NAME, assm_code, proc_code, dis_code) \
            case NAME:                                                 \
                proc_code                                              \
                break;
            #include "headers/CMD_GEN.h"

            default:
                return UNKNOWN_COMMAND_NAME;
                break;
        }

        // STACK_DUMP(proc->cmd_stk, executor)
        // printf("\n=========================================================================\n");
    }
    return SUCCESS;
}

err proc(struct processor* proc)
{
    CHECK_PTR(proc);

    return executor(proc);
}

err fill_proc(struct processor* proc, FILE* read, int fsize)
{
    CHECK_PTR(proc);
    CHECK_PTR(read);

    void* temp = 0;

    CALLOC(proc->data, char, fsize + 1);

    CALLOC(proc->RAM, elem_t, 100);

    err res = stack_ctor(&proc->cmd_stk, 5);
    if(res != SUCCESS)
        return res;

    res = stack_ctor(&proc->call_stk, 1);
    if(res != SUCCESS)
        return res;

    CALLOC(proc->reg, elem_t, 4);

    int x = fread(proc->data, sizeof(char), fsize, read);

    proc->data_size = fsize;

    assert(x == fsize);
    return SUCCESS;
}

err proc_dump(struct processor* proc, int LINE, const char* proc_name, const char* file_name, const char* func_name)
{
    CHECK_PTR(proc);
    CHECK_PTR(proc_name);
    CHECK_PTR(file_name);
    CHECK_PTR(func_name);

    printf("\n------PROCESSOR-------");
    printf("\n------DUMP_BEGIN------\n");

    printf("\nprocessor name: %s\n", proc_name);
    printf("\nDUMP was called from function: %s \n"
                             "from file: %s \n"
                             "from line: %d \n", func_name, file_name, LINE);
    printf("\nprocessor adress: %d\n", proc);
    if(proc->data != NULL)
    {
        printf("processor->data adress: %d\n", proc->data);

        printf("ax - %f \n"
               "bx - %f \n"
               "cx - %f \n"
               "dx - %f \n", proc->reg[0], proc->reg[1], proc->reg[2], proc->reg[3]);


        int i = 0;
        while(i < proc->data_size)
        {
            if(i == proc->ip)
                printf("[%d] ", proc->data[i]);

            else
                printf("%d ", proc->data[i]);
            i++;
        }
        printf("\n");
    }
    else
        return PROC_NOT_INIT;

    printf("ip = %d", proc->ip);

    printf ("\nRAM: ");
    for (int i = 0; i < 10; i++)
        printf("%f ", proc->RAM[i]);
    printf("\n\n");

    printf("\n-------DUMP_END-------\n\n");
    return SUCCESS;
}

err proc_free(struct processor* proc)
{
    CHECK_PTR(proc);

    free(proc->data);
    free(proc->RAM);
    stack_dtor(&proc->cmd_stk);
    proc->ip = 0;
    proc->data_size = 0;

    free(proc->reg);

    return SUCCESS;
}
