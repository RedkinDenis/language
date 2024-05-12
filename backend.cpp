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

    size_t start = 0;
    Stack* ram_stk;
};

static void compiler (Node* prog);

static vars create_vars ();

static void dump_vars (vars* vars);

static void delete_vars (vars* vars, RAM* ram);

static size_t add_var (RAM* ram, vars* vars, const char* name);

static var* check_var (vars* vars, const char* var, RAM* ram);

static char* op_to_str (unsigned char op);

static char* equation_assm (Node* tree, Stack* mem_stk, vars* vars, RAM* ram, int* mark_num);

static int is_assign (Node* tree);

static int is_arg (Node* node);

static RAM ram_ctor (size_t size);

static size_t search_free_cells (RAM* ram);

static char* unknown_node (Node* node);

static void ram_dtor (RAM* ram);

int main ()
{
    Node prog = {};
    FILE* input = fopen("prog_tree.txt", "rb");
    // printf("here");
    importTree(input, &prog);
    // printf("here");
    // draw_tree(&prog);

    // printf ("root - %d left - %d right - %d\n", prog.code, prog.left->code, prog.right->code);
    compiler(&prog);
}

void compiler (Node* prog)
{
    vars vars = create_vars();
    RAM ram = ram_ctor(RAM_SIZE);

    Stack stk = {};
    stack_ctor(&stk, 1);
    
    int mark_num = 1;
    char* res = equation_assm(prog, &stk, &vars, &ram, &mark_num);
    cut_end(res);

    FILE* assm = fopen("processor/prog_assm.txt", "wb");
    fprintf (assm, "%s", res);
    fclose(assm);
    
    stack_dtor(&stk);
    // delete_vars(&vars);
}

vars create_vars ()
{
    vars vars = {};
    vars.capacity = 100;
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

void delete_vars (vars* vars, RAM* ram)
{
    for (int i = 0; i < vars->qant; i++)
    {
        if (vars->arr[i].value == 1 && vars->arr[i].adress >= ram->start)
        {
            free(vars->arr[i].name);
            vars->arr[i].adress = 0;
            vars->arr[i].value = 0;
        }
    }
}

void dump_vars (vars* vars)
{
    printf("----DUMP_VARS----\n\n");
    for (int i = 0; i < vars->qant; i++)
        printf("\n%s adress: %u \n", vars->arr[i].name, vars->arr[i].adress);
    printf("\n----DUMP_END-----\n\n");
}

var* check_var (vars* vars, const char* var, RAM* ram)
{
    // printf ("%s ", var);
    for (int i = 0; i < vars->qant; i++)
    {
        if (strcmp(vars->arr[i].name, var) == 0 && vars->arr[i].adress >= ram->start)
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
            return (char*)"add";
        case opSUB:
            return (char*)"sub";
        case opMUL:
            return (char*)"mul";
        case opDIV:
            return (char*)"div";
    }
    return NULL;
}

char* str_toupper (char* str)
{
    char* res = strdup(str);
    for (int i = 0; i < strlen(str); i++)
        res[i] = toupper(str[i]);
    return res;
}

#define ASSM_LEFT char* left = equation_assm(tree->left, mem_stk, vars, ram, mark_num)
#define ASSM_RIGHT char* right = equation_assm(tree->right, mem_stk, vars, ram, mark_num)

char* equation_assm (Node* tree, Stack* mem_stk, vars* vars, RAM* ram, int* mark_num)
{
    // if (tree->left != NULL && tree->right != NULL)
    //     printf("code - %d left - %d right - %d\n", tree->code, tree->left->code, tree->right->code);
    // else 
    //     printf("code - %d\n", tree->code);

    int buf_size = 10000;
    char* buf = (char*)calloc(buf_size, sizeof(char));
    stack_push(mem_stk, &buf, sizeof(char*));

    if (tree->code == ASSIGN)
    {
        ASSM_LEFT; ASSM_RIGHT;
        SPRINTF(buf, buf_size, "%s\n%s", left, right);
        return buf;
    }

    else if (tree->code == MAIN)
    {
        ASSM_LEFT; ASSM_RIGHT;
        SPRINTF(buf, buf_size, "push 0\npop ax\n%s\n%s\nhlt", left, right);
        return buf;
    }

    else if (tree->code == PRINT)
    {
        ASSM_LEFT;
        SPRINTF(buf, buf_size, "%s\nout", left);
        return buf;
    }

    else if (tree->code == SQRT)
    {
        ASSM_LEFT;
        SPRINTF(buf, buf_size, "%s\nsqrt", left);
        return buf;
    }

    else if (tree->type == EMPtY)
        return (char*)"";

    else if (tree->code == CALL_FUNC)
    {
        ASSM_LEFT; char* name = str_toupper(tree->data.function);
        SPRINTF(buf, buf_size, "%s\n%s", left, name);
        return buf;
    }

    else if (tree->code == CALL_OWN_FUNC)
    {
        ASSM_LEFT; 
        SPRINTF(buf, buf_size, "%s\npush ax\npush 10\nadd\npop ax\ncall %s:\npush ax\npush 10\nsub\npop ax", left, tree->data.function);
        return buf;
    }

    else if (tree->code == OWNFUNC)
    {
        stack_push(ram->ram_stk, &ram->start, sizeof(ram->start));
        ram->start += ram->free_cells;
        ASSM_RIGHT; ASSM_LEFT;
        SPRINTF(buf, buf_size, "%s:\n%s\n%s", tree->data.function, right, left);
        return buf;
    }

    else if (tree->code == RET)
    {
        ASSM_LEFT;
        SPRINTF(buf, buf_size, "%s\nret", left);

        delete_vars(vars, ram);
        stack_pop(ram->ram_stk, &ram->start, sizeof(ram->start));
        return buf;
    }

    else if (tree->code == IF)
    {
        ASSM_LEFT; ASSM_RIGHT;
        *mark_num += 1;
        SPRINTF(buf, buf_size, "%s MARK%d:\n%s\nMARK%d:", left, *mark_num, right, *mark_num);
        return buf;
    }

    else if (tree->code == WHILE)
    {
        ASSM_LEFT; ASSM_RIGHT;
        *mark_num += 1;
        SPRINTF(buf, buf_size, "CICLE%d:\n%s MARK%d:\n%s\njump CICLE%d:\nMARK%d:",*mark_num, left, *mark_num, right, *mark_num, *mark_num);
        return buf;
    }

#define CONDITIONS(cond, str)                                                                                                                           \
    else if (tree->code == cond)                                                                                                                        \
    {                                                                                                                                                   \
        ASSM_LEFT; ASSM_RIGHT;                                                                                                                          \
        SPRINTF(buf, buf_size, "%s\n%s\n%s", left, right, str);                                                                                         \
        return buf;                                                                                                                                     \
    }
    CONDITIONS(LESS, "jae")
    CONDITIONS(MORE, "jbe")
    CONDITIONS(EQ, "jne")
#undef CONDITIONS

    else if (tree->code != LINK)
    {
        if (tree->type == OPERAND)
        {
            ASSM_LEFT; ASSM_RIGHT;
            SPRINTF(buf, buf_size, "%s\n%s\n%s", left, right, op_to_str(tree->data.operand));
            return buf;
        }
        else if (tree->type == NUM)
        {
            if ((tree->data.value - (int)tree->data.value) == 0)
            {
                SPRINTF(buf, buf_size, "push %d", (int)tree->data.value);    
            }
            else 
            {
                SPRINTF(buf, buf_size, "push %lf", tree->data.value);
            }
            return buf;
        }

        else if (tree->type == VAR)
        {
            // printf ("var now - %s\n", tree->data.var);
            // dump_vars(vars);
            var* var = check_var(vars, tree->data.var, ram);
            // if (var != NULL)
            if (is_assign(tree) || is_arg(tree))
            {
                size_t adress = 0;
                if (var == NULL)
                    adress = add_var(ram, vars, tree->data.var);
                else 
                    adress = var->adress;

                SPRINTF(buf, buf_size, "pop [ax + %u]", adress);
            }
            else 
            {
                // printf("var - %p\n", var);
                // dump_vars(vars);
                SPRINTF(buf, buf_size, "push [ax + %u]", var->adress);
            }
            return buf;
        }
    }
    else 
    {
        ASSM_LEFT; ASSM_RIGHT;
        if (is_arg(tree))
        {
            SPRINTF(buf, buf_size, "%s\n%s", right, left);
        }
        else 
        {
            SPRINTF(buf, buf_size, "%s\n%s", left, right);
        }
        return buf;
    }

    return unknown_node(tree);
} 

int is_arg (Node* node)
{
    // printf("start chek arg\n");
    Node* temp = NULL;
    while (node->code != OWNFUNC)
    {
        if (node->parent == NULL) {
            // printf("end chek arg\n");
            return 0;
        }
        
        temp = node;
        node = node->parent;
    }
    if (node->right == temp) {
        // printf("end chek arg\n");
        return 1;
    }
    // printf("end chek arg\n");
    return 0;
}

int is_assign (Node* tree)
{
    if (tree->parent->right == tree && tree->parent->code == ASSIGN)
        return 1;
    return 0;
}

char* unknown_node (Node* node)
{
    char* ret = (char*)calloc(20, sizeof(char));
    sprintf(ret, "UNKN - %s CODE - %d", data_to_string(node), node->code);
    return ret;
}

RAM ram_ctor (size_t size)
{
    RAM ram = {};
    ram.cells = (RAM_cell*)calloc(size, sizeof(RAM_cell));
    ram.size = size;

    stack_ctor(ram.ram_stk, 1);

    return ram;
}

size_t search_free_cells (RAM* ram)
{
    return ram->free_cells;
}

void ram_dtor (RAM* ram)
{
    free(ram->cells);
    stack_dtor(ram->ram_stk);
    free(ram);
}
