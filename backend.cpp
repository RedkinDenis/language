#include "string.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"

#include "headers/differentiator.h"
#include "headers/encoding.h"
#include "headers/input_output.h"
#include "headers/stack.h"

#define RAM_SIZE 1024

struct var
{
    char* name = NULL;
    double value = 0;
    size_t adress = 0;
};

struct vars 
{
    var* arr = NULL;
    size_t capacity = 0;
    size_t qant = 0;
};

struct RAM_cell
{
    double value = 0;
    int busy = 0;
};

struct RAM
{
    RAM_cell* cells = NULL;
    size_t size = 0;
    size_t free_cells = 0;
};

static vars create_vars ();

static void dump_vars (vars* vars);

static void delete_vars (vars* vars);

static size_t add_var (RAM* ram, vars* vars, const char* name);

static var* check_var (vars* vars, const char* var);

static char* op_to_str (unsigned char op);

static char* equation_assm (Node* tree, Stack* mem_stk, vars* vars, RAM* ram);

static RAM ram_ctor (size_t size);

static size_t search_free_cells (RAM* ram);

static char* unknown_node (Node* node);

static void ram_dtor (RAM* ram);

int main ()
{

}

vars create_vars ()
{
    vars vars = {};
    vars.capacity = 5;
    vars.qant = 0;
    vars.arr = (var*)calloc(vars.capacity, sizeof(var));
    return vars;
}

size_t add_var (RAM* ram, vars* vars, const char* name)
{
    if (vars->capacity == vars->qant)
    {
        vars->capacity *= 2;
        vars->arr = (var*)realloc(vars->arr, vars->capacity);
    }
    vars->arr[vars->qant].name = strdup(name);

    size_t ram_adress = search_free_cells(ram);
    ram->cells[ram_adress].busy = 1;
    vars->arr[vars->qant].adress = ram_adress;

    ram->free_cells = ram_adress + 1;

    vars->qant++;

    return ram_adress;
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
        printf("\n%s adress: %u \n", vars->arr[i].name, vars->arr[i].adress);
}

var* check_var (vars* vars, const char* var)
{
    // printf ("%s ", var);
    for (int i = 0; i < vars->qant; i++)
    {
        if (strcmp(vars->arr[i].name, var) == 0)
        {
            // printf ("HERE ");
            return &(vars->arr[i]);
        }
    }
    return NULL;
}

char* op_to_str (unsigned char op)
{
    switch (op)
    {
        case opADD:
            return (char*)"ADD";
        case opSUB:
            return (char*)"SUB";
        case opMUL:
            return (char*)"MUL";
        case opDIV:
            return (char*)"DIV";
    }
    return NULL;
}

char* equation_assm (Node* tree, Stack* mem_stk, vars* vars, RAM* ram)
{
    int buf_size = 10000;
    char* buf = (char*)calloc(buf_size, sizeof(char));
    stack_push(mem_stk, (void**)&buf);

    if (tree->code == ASSIGN || tree->code == MAIN)
    {
        SPRINTF(buf, buf_size, "%s\n%s\n", equation_assm(tree->left, mem_stk, vars, ram), equation_assm(tree->right, mem_stk, vars, ram));
        return buf;
    }

    else if (tree->type == EMPtY)
        return (char*)"";

    else if (tree->code == IF)
    {
        SPRINTF(buf, buf_size, "%s\n%s\n", equation_assm(tree->left, mem_stk, vars, ram), equation_assm(tree->right, mem_stk, vars, ram));
        return buf;
    }
    else if (tree->code == MORE)
    {
        SPRINTF(buf, buf_size, "%s\n%s\nJNE MARK\n", equation_assm(tree->left, mem_stk, vars, ram), equation_assm(tree->right, mem_stk, vars, ram));
        return buf;
    }

    else if (tree->code != NEW)
    {
        if (tree->type == OPERAND)
        {
            SPRINTF(buf, buf_size, "%s \n%s \n%s \n", equation_assm(tree->left, mem_stk, vars, ram), equation_assm(tree->right, mem_stk, vars, ram), op_to_str(tree->data.operand));
            return buf;
        }
        else if (tree->type == NUM)
        {
            if ((tree->data.value - (int)tree->data.value) == 0)
            {
                SPRINTF(buf, buf_size, "PUSH %d", (int)tree->data.value);    
            }
            else 
            {
                SPRINTF(buf, buf_size, "PUSH %lf", tree->data.value);
            }
            return buf;
        }

        else if (tree->type == VAR)
        {
            // printf ("1 ");
            // printf ("var - %p ", tree->data.var);
            var* var = check_var(vars, tree->data.var);
            // printf ("2 ");
            if (var != NULL)
            {
                SPRINTF(buf, buf_size, "POP [%u]\n", var->adress);
            }
            else 
            {
                size_t adress = add_var(ram, vars, tree->data.var);
                SPRINTF(buf, buf_size, "PUSH [%u]\n", adress);
            }
            return buf;
        }
    }
    else 
    {
        SPRINTF(buf, buf_size, "%s\n%s", equation_assm(tree->left, mem_stk, vars, ram), equation_assm(tree->right, mem_stk, vars, ram));
        return buf;
    }

    return unknown_node(tree);
}

char* unknown_node (Node* node)
{
    char* ret = (char*)calloc(20, sizeof(char));
    sprintf(ret, "UNKN - %s", data_to_string(node));
    return ret;
}

RAM ram_ctor (size_t size)
{
    RAM ram = {};
    ram.cells = (RAM_cell*)calloc(size, sizeof(RAM_cell));
    ram.size = size;

    return ram;
}

size_t search_free_cells (RAM* ram)
{
    return ram->free_cells;
}

void ram_dtor (RAM* ram)
{
    free(ram->cells);
}