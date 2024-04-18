#include "string.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"

#include "headers/differentiator.h"
#include "headers/input_output.h"
#include "headers/recursive_descent.h"
#include "headers/stack.h"

struct var
{
    char* name = NULL;
    double value = 0;
};

struct vars 
{
    var* arr = NULL;
    size_t capacity = 0;
    size_t qant = 0;
};

static void compiler (Data* prog);

static vars create_vars ();

static void dump_vars (vars* vars);

static void delete_vars (vars* vars);

static void add_var (vars* vars, char* name, double value);

static char* get_name (char* str);

static char* op_to_str (unsigned char op);

static char* equation_assm (Node* tree, Stack* mem_stk, vars* vars);

int main ()
{
    Data programm = input_data("prog.txt");
    dump_data(&programm);
    // clear_data(&programm);

    compiler(&programm);
}

void goto_expression (char** str)
{
    while (*(*str - 1) != '=')
        *str += 1;
    skip_spaces(str);
}

void compiler (Data* prog)
{
    vars vars = create_vars();
    for (int line = 0; line < prog->quant; line++)
    {
        if (strstr(prog->lines[line].str, "="))
        {
            Stack stk = {};
            stack_ctor(&stk, 1);

            char* name = get_name(prog->lines[line].str);
            double var = 0;
            goto_expression(&prog->lines[line].str);

            Node* expression = get_g(prog->lines[line].str);

            int v = 0;
            var = calculator(expression, &v);
            if (v == 0)
                add_var(&vars, name, var);

            printf("\n%s", equation_assm(expression, &stk, &vars));
            stack_dtor(&stk);
        }
    }
    // dump_vars(&vars);
    delete_vars(&vars);
}

char* get_name (char* str)
{
    skip_spaces(&str);

    int len = 0;
    while (isalpha(str[len]))
        len++;

    char* name = (char*)calloc(len + 1, sizeof(char));
    strncpy(name, str, len);
    return name;
}

vars create_vars ()
{
    vars vars = {};
    vars.capacity = 1;
    vars.qant = 0;
    vars.arr = (var*)calloc(1, sizeof(var));
    return vars;
}

void add_var (vars* vars, char* name, double value)
{
    if (vars->capacity == vars->qant)
    {
        vars->capacity *= 2;
        vars->arr = (var*)realloc(vars->arr, vars->capacity);
    }
    vars->arr[vars->qant].name = strdup(name);
    vars->arr[vars->qant].value = value;
    vars->qant++;
}

void delete_vars (vars* vars)
{
    for (int i = 0; i < vars->qant; i++)
        free(vars->arr[i].name);
    free(vars->arr);
}

void dump_vars (vars* vars)
{
    for (int i = 0; i < vars->qant; i++)
        printf("%s - %lf \n", vars->arr[i].name, vars->arr[i].value);
}

char* op_to_str (unsigned char op)
{
    switch (op)
    {
        case ADD:
            return (char*)"ADD";
        case SUB:
            return (char*)"SUB";
        case MUL:
            return (char*)"MUL";
        case DIV:
            return (char*)"DIV";
    }
    return NULL;
}

char* equation_assm (Node* tree, Stack* mem_stk, vars* vars)
{
    int buf_size = 10000;
    char* buf = (char*)calloc(buf_size, sizeof(char));
    stack_push(mem_stk, &buf);

    if (tree->type == OPERAND)
    {
        SPRINTF(buf, buf_size, "%s \n%s \n%s \n", op_to_str(tree->data.operand), equation_assm(tree->left, mem_stk, vars), equation_assm(tree->right, mem_stk, vars));
        return buf;
    }
    else if (tree->type == FUNCTION)
    {
        SPRINTF(buf, buf_size, "%s(%s) ", tree->data.function, equation_assm(tree->right, mem_stk, vars));

        return buf;
    }
    
    else if (tree->type == NUM)
    {
        if ((tree->data.value - (int)tree->data.value) == 0)
        {
            SPRINTF(buf, buf_size, "%d", (int)tree->data.value);    
        }
        else 
        {
            SPRINTF(buf, buf_size, "%lf", tree->data.value);
        }
        return buf;
    }

    else if (tree->type == VAR)
    {
        // printf("HERE!!!!\n");
        for (int i = 0; i , vars->qant; i++)
        {
            if (strncmp(vars->arr[i].name, tree->data.var, strlen(vars->arr[i].name)) == 0)
            {
                tree->type = NUM;
                tree->data.value = vars->arr[i].value;
                return equation_assm(tree, mem_stk, vars);
            }
        }
        return (char*)"UNKNOWN VAR";
    }

    return (char*)"UNKNOWN";
}