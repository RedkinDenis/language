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

static Node* get_ret (char** s);

static Node* get_a (char** s);

static char op_det (char** s);

static Node* get_c (char** s);

static Node* get_construction (char** s);

static void require (char** str, const char* srch);

static Node* get_args (char** s);

static Node* get_func (char** s);

char* try_get_func (char** s);

void skip_spaces (char** str)
{
    while (**str == ' ')
        *str += 1;
}

void require (char** str, const char* srch)
{
    size_t len = strlen(srch);
    for (int i = 0; i < len; i++)
    {
        if ((*str)[i] != srch[i])
        {
            SYNTAX_ERROR
            // printf ("here\n");
            return;
        }
    }
    *str += len;
    skip_spaces(str);
}

Node* get_func (char** s)
{
    Node* body = NULL;
    Node* args = NULL;
    Node* res = NULL;
    skip_spaces(s);
    if (**s != '\0')
    {
        int i = 0;
        while ((*s)[i] != ' ' && (*s)[i] != '(')
            i++;

        char* name = (char*)calloc(i + 1, sizeof(char));
        strncpy(name, *s, i);
        *s += i;
        skip_spaces(s);

        require(s, "(");

        Node* args = get_args(s);

        require(s, ")");

        REQUIRE('{');

        body = get_construction(s);
        skip_spaces(s);

        // else 
        // {
        //     double nul = 0;
        //     body->right = create_node(FUNCTION, (char*)"soslati", create_node(NUM, &nul, NULL, NULL, PUSH), EMPtY_NODE, RET);
        // }

        REQUIRE('}');

        res = create_node(LINKER, &op_link, create_node(FUNCTION, name, body, args, FUNC_PRIOR), get_func(s), LINK);

        return res;
    }
    return EMPtY_NODE;
}

Node* get_args (char** s)
{
    Node* arg = get_e(s);
    if (arg->type != EMPtY)
    {
        if (**s == ',')
        {
            *s += 1;
            return create_node(LINKER, &op_link, arg, get_args(s), LINK);
        }
        else
        {
            return create_node(LINKER, &op_link, arg, EMPtY_NODE, LINK);
        }
    }
    return arg;
}

Node* get_g (const char* str)
{
    char** s = (char**)&str;

    Node* val = get_func(s);

    REQUIRE('\0');

    return val;
}

Node* get_construction (char** s)
{
    skip_spaces(s);
    #define CONSTRUCTION_GENERATE(str, op_arg, code)                            \
    if (strncmp(str, *s, strlen(str)) == 0)                                     \
    {                                                                           \
        *s += strlen(str);                                                      \
        skip_spaces(s);                                                         \
                                                                                \
        REQUIRE('(');                                                           \
                                                                                \
        Node* condition = get_a(s);                                             \
                                                                                \
        REQUIRE(')');                                                           \
                                                                                \
        skip_spaces(s);                                                         \
        REQUIRE('{');                                                           \
                                                                                \
        Node* Do = get_construction(s);                                         \
                                                                                \
        REQUIRE('}');                                                           \
        skip_spaces(s);                                                         \
        Node* cnstr = create_node(FUNCTION, op_arg, condition, Do, code);       \
        return create_node(LINKER, &op_link, cnstr, get_construction(s), LINK); \
    }
    CONSTRUCTION_GENERATE("ezeli", op_if, IF)
    CONSTRUCTION_GENERATE("docole", op_while, WHILE)
    #undef CONSTRUCTION_GENERATE
    return get_c(s);
}

Node* get_c (char** s)
{
    skip_spaces(s);
    if (**s == ';')
        return EMPtY_NODE;
        
    Node* val = get_ret(s);
    skip_spaces(s);

    if (**s == ';')
    {
        *s += 1;
        val = create_node(LINKER, &op_link, val, get_construction(s), LINK);
    }
    skip_spaces(s);
    return val;
}

Node* get_ret (char** s)
{
    if (strncmp(*s, "soslati", 6) == 0)
    {
        *s += strlen("soslati");
        skip_spaces(s);
        return create_node(FUNCTION, (char*)"soslati", get_e(s), EMPtY_NODE, RET);
    }
    else if (strncmp(*s, "nachertati", 5) == 0)
    {
        *s += strlen("nachertati");
        skip_spaces(s);
        return create_node(FUNCTION, (char*)"nachertati", get_e(s), EMPtY_NODE, PRINT);
    }
    else if (strncmp(*s, "sqrt", 4) == 0)
    {
        *s += strlen("sqrt");
        skip_spaces(s);
        return create_node(FUNCTION, (char*)"sqrt", get_e(s), EMPtY_NODE, SQRT);
    }
    return get_a(s);
}

char op_det (char** s)
{
#define OP_DET(str, ret)                    \
    if (strncmp(*s, str, strlen(str)) == 0) \
        return ret;                 
    OP_DET("spodobno", '~')
    OP_DET("veliche", '>')
    OP_DET("menee", '<')
    OP_DET("narekayu", '=')
#undef OP_DET
    return '\0';
}

Node* get_a (char** s)
{
    Node* val = get_e(s);
    skip_spaces(s);
    Node* expr = NULL;

    char op = op_det(s);
    // char op = **s;

    switch (op)
    {
        #define OP_GEN(opT, op_arg, com, str)                                \
        case opT:                                                       \
            *s += strlen(str);                                                    \
            skip_spaces(s);                                             \
            expr = get_e(s);                                            \
            val = create_node(FUNCTION, op_arg, expr, val, com);        \
            break;
        OP_GEN(opASS, op_ass, ASSIGN, "narekayu")
        OP_GEN(opMORE, op_less, LESS, "veliche")
        OP_GEN(opLESS, op_more, MORE, "menee")
        OP_GEN(opEQ, op_eq, EQ, "spodobno")
        #undef OP_GEN
    }
    skip_spaces(s);
    return val;
}

Node* get_e (char** s)
{
    Node* val = get_t(s);
    skip_spaces(s);

    // while (**s == '+' || **s == '-')
    while (strncmp(*s, "prisoedinenie", 13) == 0 || strncmp(*s, "otchugdenie", 11) == 0 )
    {
        char op = **s;
        if (op == 'p')
            *s += 13;
        else
            *s += 11;
        skip_spaces(s);
        Node* val2 = get_t(s);
        if (op == 'p')  
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

    while (strncmp(*s, "mnogiti", 7) == 0 || strncmp(*s, "delitisa", 8) == 0 )
    {
        char op = **s;
        if (op == 'm')
            *s += 7;
        else
            *s += 8;
        skip_spaces(s);
        Node* val2 = get_d(s);
        if (op == 'm')  
            val = create_node(OPERAND, &op_mul, val, val2, MUL);
        else 
            val = create_node(OPERAND, &op_div, val, val2, DIV);
    }
    skip_spaces(s);

    return val;
}

char* try_get_func (char** s)
{
    if (isalpha(**s))
    {
        int i = 0;
        while (isalpha((*s)[i]) && (*s)[i] != ' ' && (*s)[i] != '(')
            i++;

        int sp = i;
        while ((*s)[sp] == ' ')
            sp++;
        
        if ((*s)[sp] != '(')
            return NULL;

        char* name = (char*)calloc(i +  1, sizeof(char));
        strncpy(name, *s, i);
        *s += sp + 1;
        skip_spaces(s);
        return name;
    }
    return NULL;
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
                var = create_node(FUNCTION, vr, get_e(s), EMPtY_NODE, CALL_FUNC);        \
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
    {
        char* name = try_get_func(s);
        
        if (name == NULL)
            return get_p(s);

        Node* res = NULL;
        if (strncmp(name, "sqrt", 4) == 0)
            res = create_node(FUNCTION, name, get_args(s), EMPtY_NODE, SQRT);
        else
            res = create_node(FUNCTION, name, get_args(s), EMPtY_NODE, CALL_OWN_FUNC);
        REQUIRE(')');

        return res;
    }
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
    if (**s == '\0')
        return EMPtY_NODE;
    if (**s == '}')
        return EMPtY_NODE;

    char* old_s = *s;

    Node* val = (Node*)calloc(1, sizeof(Node));
    val->type = NUM;

    skip_spaces(s);
    if (isalpha(**s))
    {
        int i = 0;
        while (isalpha((*s)[i]) || isalnum((*s)[i]))
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
            return EMPtY_NODE;
    }
    skip_spaces(s);
    return val;
}