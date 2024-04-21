#include "headers/recursive_descent.h"
#include "headers/input_output.h"
#include "headers/encoding.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>

static Node* get_n (char** s);

static Node* get_t (char** s);

static Node* get_e (char** s);

static Node* get_p (char** s);

static Node* get_f (char** s);

static Node* get_d (char** s);

static Node* get_a (char** s);

static Node* get_c (char** s);

void skip_spaces (char** str)
{
    while (**str == ' ')
        *str += 1;
}

Node* get_g (const char* str)
{
    char** s = (char**)&str;
    skip_spaces(s);
    Node* val = get_c(s);

    skip_spaces(s);
    REQUIRE(';');

    return val;
}

Node* get_c (char** s)
{
    skip_spaces(s);
    if (**s == ';')
        return EMPtY_NODE;
        
    Node* val = get_a(s);
    skip_spaces(s);

    if (**s == ';')
    {
        *s += 1;
        val = create_node(OPERAND, &op_com, val, get_c(s), NEW);
    }
    skip_spaces(s);
    return val;
}

Node* get_a (char** s)
{
    Node* val = get_e(s);
    skip_spaces(s);

    if (**s == '=')
    {
        *s += 1;
        skip_spaces(s);
        Node* expr = get_e(s);
        val = create_node(OPERAND, &op_ass, expr, val, ASSIGN);
    }

    skip_spaces(s);
    return val;
}

Node* get_e (char** s)
{
    Node* val = get_t(s);
    skip_spaces(s);

    while (**s == '+' || **s == '-')
    {
        char op = **s;
        *s += 1;
        skip_spaces(s);
        Node* val2 = get_t(s);
        if (op == '+')  
            val = create_node(OPERAND, &op_add, val, val2, ADD);
        else 
            val = create_node(OPERAND, &op_sub, val, val2, SUB);
    }
    skip_spaces(s);

    return val;
}

Node* get_d (char** s)
{
    Node* val = get_f(s);

    while (**s == '^')
    {
        char op = **s;
        *s += 1;

        skip_spaces(s);
        REQUIRE('(');
        *s -= 1;
        skip_spaces(s);

        Node* val2 = get_f(s);

        val = create_node(OPERAND, &op_pow, val, val2);

        skip_spaces(s);
    }
    skip_spaces(s);

    return val;
}

Node* get_t (char** s)
{
    Node* val = get_d(s);

    while (**s == '*' || **s == '/')
    {
        char op = **s;
        *s += 1;
        Node* val2 = get_d(s);
        if (op == '*')  
            val = create_node(OPERAND, &op_mul, val, val2, MUL);
        else 
            val = create_node(OPERAND, &op_div, val, val2, DIV);
    }
    skip_spaces(s);

    return val;
}

Node* get_f (char** s)
{
    skip_spaces(s);
    operation foo = long_op_det(*s, s);
    skip_spaces(s);

    if (foo != opUNKNOWN)
    {
        Node* var = NULL;

        REQUIRE('(');
        switch (foo)
        {
            #define LONG_OP_DET(enum, string, vr)                               \
            case enum:                                                          \
                var = create_node(FUNCTION, vr, EMPtY_NODE, get_e(s));        \
                break;
            #include "headers/long_op.h"
            #undef LONG_OP_DET
        }
        skip_spaces(s);
        REQUIRE(')');
        skip_spaces(s);
        return var;
    }
    else
        return get_p(s);
}

Node* get_p (char** s)
{
    skip_spaces(s);
    if (**s == '(')
    {
        *s += 1;
        skip_spaces(s);
        Node* val = get_e(s);
        skip_spaces(s);

        REQUIRE(')');

        return val;
    }
    else 
    {
        Node* val = get_n(s);
        return val;
    }
}

Node* get_n (char** s)
{
    skip_spaces(s);
    if (**s == ';')
        return EMPtY_NODE;

    char* old_s = *s;

    Node* val = (Node*)calloc(1, sizeof(Node));
    val->type = NUM;

    skip_spaces(s);
    if (isalpha(**s))
    {
        int i = 0;
        while (isalpha((*s)[i]))
            i++;

        val->type = VAR;
        char* temp = (char*)calloc(i + 1, sizeof(char));
        strncpy(temp, *s, i);
        *s += i;

        val->data.var = temp;
        val->code = RAMPUSH;
    }
    else
    {
        val->code = PUSH;
        char sign = '+';
        if (**s == '-')
        {
            sign = '-';
            *s += 1;
        }

        while ('0' <= **s && **s <= '9')
        {
            if (sign == '+')
                val->data.value = val->data.value * 10 + (**s - '0');

            else 
                val->data.value = val->data.value * 10 - (**s - '0');

            *s += 1;
        }
        if (old_s == *s)
            SYNTAX_ERROR;
    }
    skip_spaces(s);
    return val;
}