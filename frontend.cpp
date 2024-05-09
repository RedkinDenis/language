#include "string.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"

#include "headers/differentiator.h"
#include "headers/input_output.h"
#include "headers/recursive_descent.h"
#include "headers/stack.h"

static void compiler (char* prog);

int main ()
{
    FILE* prog = fopen("qadr.txt", "rb");
    char* programm = NULL;
    fill_buffer(prog, &programm);
    remove_enters(programm);
    printf("%s\n", programm);

    compiler(programm);
}

void compiler (char* prog)
{
    Node* programm = get_g(prog);
    draw_tree(programm);
    // print_tree(programm);

    FILE* out = fopen("prog_tree.txt", "wb");

    fprint_tree(out, programm);
    fclose(out);

    // Stack stk = {};
    // stack_ctor(&stk, 1);
    
    // char* res = equation_assm(programm, &stk, &vars, &ram);
    // printf ("assm:\n%s\n", res);
    // stack_dtor(&stk);
    // delete_vars(&vars);
}
