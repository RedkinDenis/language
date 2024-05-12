#include "string.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"

#include "headers/differentiator.h"
#include "headers/encoding.h"
#include "headers/input_output.h"
#include "headers/stack.h"

static char* back_compile (Node* tree, Stack* mem_stk);

static char* unknown_node (Node* node);

static void compiler (Node* prog);

static const char* op_to_str (char op);

#define BACK_LEFT char* left = back_compile(tree->left, mem_stk)
#define BACK_RIGHT char* right = back_compile(tree->right, mem_stk)

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
    Stack stk = {};
    stack_ctor(&stk, 1);
    
    char* res = back_compile(prog, &stk);
    cut_end(res);

    FILE* assm = fopen("back_compile.txt", "wb");
    fprintf (assm, "%s", res);
    fclose(assm);
    
    stack_dtor(&stk);
}

char* back_compile (Node* tree, Stack* mem_stk)
{
    // printf("code - %d\n", tree->code);
    int buf_size = 100000;
    char* buf = (char*)calloc(buf_size, sizeof(char));
    stack_push(mem_stk, &buf, sizeof(char*));

    if (tree->code == ASSIGN)
    {
        BACK_LEFT; BACK_RIGHT;
        SPRINTF(buf, buf_size, "%s narekayu %s;\n", right, left);
        return buf;
    }

    else if (tree->code == MAIN)
    {
        BACK_LEFT; BACK_RIGHT;
        SPRINTF(buf, buf_size, "zapovedi (%s)\nGospodi,\n%s\namini\n", right, left);
        return buf;
    }

    else if (tree->code == PRINT)
    {
        BACK_LEFT;
        SPRINTF(buf, buf_size, "print(%s);\n", left);
        return buf;
    }

    else if (tree->code == SQRT)
    {
        BACK_LEFT;
        SPRINTF(buf, buf_size, "sqrt(%s)", left);
        return buf;
    }

    else if (tree->type == EMPtY)
        return (char*)"";

    else if (tree->code == CALL_FUNC || tree->code == CALL_OWN_FUNC)
    {
        BACK_LEFT; char* name = tree->data.function;
        SPRINTF(buf, buf_size, "%s(%s)", name, left);
        return buf;
    }

    else if (tree->code == OWNFUNC)
    {
        BACK_RIGHT; BACK_LEFT;
        SPRINTF(buf, buf_size, "%s (%s)\nGospodi,\n%samini\n", tree->data.function, right, left);
        return buf;
    }

    else if (tree->code == RET)
    {
        BACK_LEFT;
        SPRINTF(buf, buf_size, "soslati %s;\n", left);
        return buf;
    }

    else if (tree->code == IF)
    {
        BACK_LEFT; BACK_RIGHT;
        SPRINTF(buf, buf_size, "ezeli (%s)\nGospodi,\n%samini\n", left, right);
        return buf;
    }

    else if (tree->code == WHILE)
    {
        BACK_LEFT; BACK_RIGHT;
        SPRINTF(buf, buf_size, "docole (%s)\nGospodi,\n%samini\n", left, right);
        return buf;
    }

#define CONDITIONS(cond, str)                                                                                                                           \
    else if (tree->code == cond)                                                                                                                        \
    {                                                                                                                                                   \
        BACK_LEFT; BACK_RIGHT;                                                                                                                          \
        SPRINTF(buf, buf_size, "%s %s %s", right, str, left);                                                                                         \
        return buf;                                                                                                                                     \
    }
    CONDITIONS(LESS, "menee")
    CONDITIONS(MORE, "veliche")
    CONDITIONS(EQ, "spodobno")
#undef CONDITIONS

    else if (tree->code != LINK)
    {
        if (tree->type == OPERAND)
        {
            BACK_LEFT; BACK_RIGHT;
            SPRINTF(buf, buf_size, "%s %s %s", left, op_to_str(tree->data.operand), right);
            return buf;
        }
        else if (tree->type == NUM)
        {
            // printf ("here1\n");
            if ((tree->data.value - (int)tree->data.value) == 0)
            {
                SPRINTF(buf, buf_size, "%d", (int)tree->data.value);    
            }
            else 
            {
                SPRINTF(buf, buf_size, "%lf", tree->data.value);
            }
            // printf ("here2 buf - %s\n", buf);
            return buf;
        }

        else if (tree->type == VAR)
        {
            SPRINTF(buf, buf_size, "%s", tree->data.var);
            return buf;
        }
    }
    else 
    {
        BACK_LEFT; BACK_RIGHT;
        // if (is_arg(tree))
        // {
        //     SPRINTF(buf, buf_size, "%s\n%s", right, left);
        // }
        // else 
        // {
            SPRINTF(buf, buf_size, "%s%s", left, right);
        // }
        return buf;
    }

    return unknown_node(tree);
}

char* unknown_node (Node* node)
{
    char* ret = (char*)calloc(20, sizeof(char));
    sprintf(ret, "UNKN - %s CODE - %d", data_to_string(node), node->code);
    return ret;
}

const char* op_to_str (char op)
{
    if (op == '+')
        return "prisoedinenie";
    if (op == '-')
        return "otchugdenie";
    if (op == '*')
        return "mnogiti";
    if (op == '/')
        return "delitisa";
    return "unknown";
}