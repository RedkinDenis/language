#ifndef DSL_H
#define DSL_H

enum step
{
    command = sizeof(char),
    number = 4,
    reg = sizeof(char)
};

#define STPOP(stk_type, reg)                     \
    res = stack_pop(&(proc->stk_type), &buf);  \
    if(res != SUCCESS)                         \
       return res;                             \
    memcpy(&reg, &buf, sizeof(elem_t));

#define STPUSH(stk_type, reg)                    \
    temp = (elem_t)reg;                        \
    memcpy(&buf, &temp, sizeof(elem_t));       \
    res = stack_push(&(proc->stk_type), &buf); \
    if(res != SUCCESS)                         \
        return res;

#define check_result(func)                          \
    if(res != SUCCESS)                              \
    {                                               \
        printf("\nerror in " #func " %d \n", res);  \
        return res;                                 \
    }

#define CHECK_MARK(cmd)                                       \
    if(strchr(data[i].str + strlen(cmd) + 1, ':') != NULL)        \
    {                                                             \
        num = 0;                                                  \
        for(int k = strlen(cmd); data[i].str[k - 1] != ':'; k++)  \
            num += data[i].str[k];                                \
                                                                  \
        num1 = num;                                               \
                                                                  \
        for(int j = 0; j < marks_len; j++)                        \
            if(num == marks[j].num)                               \
                num = marks[j].adress;                            \
                                                                  \
        if(num1 == num)                                           \
            return MARK_NOT_FOUND;                                \
    }                                                             \
    else                                                          \
        sscanf(data[i].str + strlen(cmd) + 1, "%d", &num);

#define PROC_DUMP(proc, func_name) proc_dump(proc, __LINE__, GET_VARIABLE_NAME(proc), __FILE__, GET_VARIABLE_NAME(func_name));

#define GET_VARIABLE_NAME(variable) #variable

#define STACK_DUMP(stk, func_name) stack_dump(&stk, __LINE__, GET_VARIABLE_NAME(stk), __FILE__, GET_VARIABLE_NAME(func_name));

#define PRINT_START                                                                                   \
    printf("-------------------------------------------------------------------------------------\n");  \
    printf("-----------------------------------START_OF_PROGRAMM---------------------------------\n");  \
    printf("-------------------------------------------------------------------------------------\n");

#define ASSM_JUMPS(string, enum)                                    \
    buffer[ptr] = enum;                                             \
    ptr += command;                                                 \
    CHECK_MARK(string)                                              \
    memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(int));    \
    ptr += number;

#define PROC_JUMPS(enum, operand)                                        \
    STPOP(cmd_stk, x1)                                                     \
    STPOP(cmd_stk, x2)                                                     \
    if(x1 operand x2)                                                    \
        memcpy(&proc->ip, &proc->data[proc->ip + command], sizeof(int)); \
    else                                                                 \
        proc->ip += (command + number);

#define DISASSM_JUMPS()                     \
    memcpy(&num, data + ptr, sizeof(int));  \
    fprintf(out, " %d", num);               \
    ptr += number;

#define MATH_COMM(enum, operand) \
    STPOP(cmd_stk, x1)             \
    STPOP(cmd_stk, x2)             \
    x = x2 operand x1;           \
    proc->ip += command;         \
    STPUSH(cmd_stk, x)

#define CHECK_PTR(ptr)           \
    do                           \
    {                            \
    if (ptr == NULL)             \
        return NULL_INSTEAD_PTR; \
    }while(0)                    \

#define CALLOC(elem, type, size)                \
    do                                          \
    {                                           \
    void* clTmp_ = calloc(size, sizeof(type));  \
    if (clTmp_ == NULL)                         \
        return CALLOC_ERROR;                    \
    elem = (type*)clTmp_;                       \
    }while(0)


#define REALLOC(elem, type, size)                                 \
    do                                                            \
    {                                                             \
        void* clTmp_ = realloc(stk->data, size * sizeof(elem_t)); \
        if(clTmp_ == NULL)                                        \
            return REALLOC_ERROR;                                 \
        stk->data = (type*)clTmp_;                                \
    }while(0)

#define FOPEN(var, name, mode)             \
    FILE* var = fopen(name, mode);         \
        if(var == NULL)                    \
            return FOPEN_ERROR;


#define PUSH_POP_DET(name, type1, type2, type3)                                                \
    do                                                                                          \
    {                                                                                           \
        size_t name_len = strlen(name) + 1;                                                     \
        if(*(data[i].str + name_len) == '[' && strchr(data[i].str + name_len, ']') != NULL)     \
        if(isdigit(*(data[i].str + name_len + strlen("["))))                                    \
        {                                                                                       \
            sscanf(data[i].str + name_len, "[%d]", &num);                                       \
            buffer[ptr] = type1;                                                                \
        }                                                                                       \
        else if(*(data[i].str + name_len + strlen("[a")) == 'x')                                \
        {                                                                                       \
            buffer[ptr] = type2;                                                                \
                                                                                                \
            num = reg_det(data[i].str + name_len);                                              \
                                                                                                \
            ptr += command;                                                                     \
            memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));                       \
            ptr += reg;                                                                         \
            break;                                                                              \
        }                                                                                       \
        if(*(data[i].str + name_len + strlen("a")) == 'x')                                      \
        {                                                                                       \
            buffer[ptr] = type3;                                                                \
                                                                                                \
            num = reg_det(data[i].str + name_len);                                              \
                                                                                                \
            ptr += command;                                                                     \
            memcpy(buffer + ptr * sizeof(buffer[0]), &num, sizeof(char));                       \
            ptr += reg;                                                                         \
            break;                                                                              \
        }                                                                                       \
    } while (0)                                                                
    

#endif //DSL_H
