#include "headers/differentiator.h"
#include "headers/input_output.h"
#include "headers/recursive_descent.h"
#include "headers/encoding.h"

#include "..\UDL.h"
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
// #include <windows.h>

static void dump (Node* tree);

static Node* copy_subtree (Node* sub_tree);

static void calc_simplifier (Node* tree, int* changed);

static void second_simplifier (Node* tree, int* changed);

static Node* diff_mul (const Node* node, const char* part);

static Node* diff_div (const Node* node, const char* part);

static Node* diff_exp (const Node* node, const char* part);

static Node* diff_pow (const Node* node, const char* part);

static Node* diff_ln (const Node* node, const char* part);

static Node* diff_sin (const Node* node, const char* part);

static Node* diff_cos (const Node* node, const char* part);

static Node* diff_tg (const Node* node, const char* part);

// int main ()
// {
//     FOPEN(read, "expressions.txt", "rb");
//     char* buffer = NULL;
    
//     fill_buffer(read, &buffer);

//     Node* tree = get_g(buffer);
//     simplifier(tree);
//     // draw_tree(tree);
//     // Sleep(1000);
//     // draw_tree(simplifier(diff(tree)));

//     // Node* Diff = diff(tree, "x");

//     diff_tex(tree);

//     // tree_kill(Diff);
//     tree_kill(tree);
// } 

operation long_op_det (char* str, char** s)
{
    #define LONG_OP_DET(enum, string, vr)                \
    if (strncmp(string, str, strlen(string)) == 0)       \
    {                                                    \
        if (s != NULL)                                   \
            *s += strlen(string);                        \
        return enum;                                     \
    }
    #include "headers/long_op.h"
    #undef LONG_OP_DET

    return opUNKNOWN;
}

double calculator (Node* tree, int* var)
{
    if (tree == NULL)
        return 10;

    if (tree->type == VAR)
        *var = 10; 

    if (var != NULL && *var == 1)
        return 10;

    if (tree->type == OPERAND)
    {
        switch (tree->data.operand)
        {
            #define OPERATION(str, op)                                                \
            case str:                                                                 \
                return calculator(tree->left, var) op calculator(tree->right, var);   \
                break;      
            #include "headers/operations.h"
            #undef OPERATION

            case opDIV:
            {
                float x1 = calculator(tree->left, var);
                float x2 = calculator(tree->right, var);

                if (x2 != 0)
                    return x1 / x2;
                
                printf("ERROR: dividing by zero\n");
                break;
            }

            case opPOW:
                return pow(calculator(tree->left, var), calculator(tree->right, var));
            
            default:
                printf("unknown operator ");
                print_data(tree);   printf(" \n");
                break;            
        }
    }
    else if (tree->type == FUNCTION)
    {
        switch (long_op_det(tree->data.function))
        {
            case opLN:
                return log(calculator(tree->right, var));
            case opEXP:
                return exp(calculator(tree->right, var));
            case opSIN:
                return sin(calculator(tree->right, var));
            case opCOS:
                return cos(calculator(tree->right, var));
            case opTG:
                return tan(calculator(tree->right, var));        
        }
    }

    if (tree->type == VAR)
        *var = 1;    

    if (var != NULL && *var == 1)
        return 10;

    return tree->data.value;
}

Node* simplifier (Node* tree)
{
    int changed = 1;
    while (changed > 0)
    {
        changed = 0;
        calc_simplifier(tree, &changed);
        second_simplifier(tree, &changed);
    }     

    return tree;
}

void calc_simplifier (Node* tree, int* changed)
{
    int var = 0;
    double temp = calculator(tree, &var);
    if (var == 0 && tree->type != NUM && tree->type != EMPtY)
    {
        // print_data(tree);
        tree->data.value = temp;
        tree->type = NUM;

        FREE_SUBTREE(tree->left);
        FREE_SUBTREE(tree->right);

        *changed += 1;
    }
    
    if (tree->left != NULL)
        calc_simplifier(tree->left, changed);

    if (tree->right != NULL)
        calc_simplifier(tree->right, changed);
}

void second_simplifier (Node* tree, int* changed)
{    
    if (tree->type == OPERAND && tree->data.operand == '*')
    {
        if ((tree->left->type == NUM && tree->left->data.value == 0) || (tree->right->type == NUM && tree->right->data.value == 0))
        {
            tree->type = NUM;
            tree->data.value = 0;
            
            FREE_SUBTREE(tree->left);
            FREE_SUBTREE(tree->right);

            *changed += 1;
        }
        else if (tree->left->type == NUM && tree->left->data.value == 1)
        {
            FREE_SUBTREE(tree->left);
            memcpy(tree, tree->right, sizeof(Node));
            *changed += 1;
        }

        else if (tree->right->type == NUM && tree->right->data.value == 1)
        {
            FREE_SUBTREE(tree->right);
            memcpy(tree, tree->left, sizeof(Node));
            *changed += 1;
        }
    }
    else if (tree->type == OPERAND && tree->data.operand == '/')
    {
        if ((tree->left->type == NUM && tree->left->data.value == 0))
        {
            tree->type = NUM;
            tree->data.value = 0;
            
            FREE_SUBTREE(tree->left);
            FREE_SUBTREE(tree->right);

            *changed += 1;
        }
    }
    else if (tree->type == OPERAND && (tree->data.operand == '+' || tree->data.operand == '-'))
    {
        if (tree->left->type == NUM && tree->left->data.value == 0)
        {
            FREE_SUBTREE(tree->left);
            memcpy(tree, tree->right, sizeof(Node));
            *changed += 1;
        }

        else if (tree->right->type == NUM && tree->right->data.value == 0)
        {
            FREE_SUBTREE(tree->right);
            memcpy(tree, tree->left, sizeof(Node));
            *changed += 1;
        }
    }
    
    if (tree->left != NULL)
        second_simplifier(tree->left, changed);

    if (tree->right != NULL)
        second_simplifier(tree->right, changed);
}

Node* copy_subtree (Node* sub_tree)
{
    Node* copy = (Node*)calloc(1, sizeof(Node));
    memcpy(copy, sub_tree, sizeof(Node));

    if (sub_tree->left != NULL)
        copy->left = copy_subtree(sub_tree->left);
    
    if (sub_tree->right != NULL)
        copy->right = copy_subtree(sub_tree->right);

    return copy;
}

Node* create_node (data_t type, void* data, Node* left, Node* right, byte_codes code)
{
    Node* newNode = (Node*)calloc(1, sizeof(Node));

    newNode->type = type;
    newNode->code = code;

    switch (type)
    {
        case NUM:
            newNode->data.value = *(double*)data;
            break;
        case OPERAND:
            newNode->data.operand = *(char*)data;
            break;
        case FUNCTION:
            newNode->data.function = (char*)calloc(strlen((char*)data), sizeof(char));
            strcpy(newNode->data.function, (char*)data);
            break;
        case EMPtY:
            // printf("here\n");
            break;
    }
 
    newNode->left = left;
    newNode->right = right;

    return newNode;
}

Node* diff (const Node* node, const char* part)
{
    switch (node->type)
    {
        case NUM:
        {
            double temp = 0;
            return create_node(NUM, &temp, NULL, NULL);
        }
        case VAR:
        {
            if (strcmp(part, node->data.var) == 0)
            {
                double temp = 1;
                return create_node(NUM, &temp, NULL, NULL);
            }
            else
            {
                double temp = 0;
                return create_node(NUM, &temp, NULL, NULL);
            }
        }
        case OPERAND:
            switch (node->data.operand)
            {
                case opADD:
                    return create_node(OPERAND, &op_add, diff(node->left, part), diff(node->right, part));
                
                case opSUB:
                    return create_node(OPERAND, &op_sub, diff(node->left, part), diff(node->right, part));
                
                case opMUL:
                    return diff_mul(node, part);
                    
                case opDIV:
                    return diff_div(node, part);

                case opPOW:
                    return diff_pow(node, part);
            }
        case (FUNCTION):
            switch (long_op_det(node->data.function))
            {
                case opLN:
                    return diff_ln(node, part);
    
                case opEXP:
                    return diff_exp(node, part);  

                case opSIN:
                    return diff_sin(node, part);  

                case opCOS:
                    return diff_cos(node, part);  

                case opTG:
                    return diff_tg(node, part);     
            }
    }
    return NULL;
}

Node* diff_mul (const Node* node, const char* part)
{
    Node* du = DIFF_LEFT;    Node* cu = COPY_LEFT;
    Node* dv = DIFF_RIGHT;   Node* cv = COPY_RIGHT;
    
    Node* res = create_node(OPERAND, &op_add, create_node(OPERAND, &op_mul, du, cv), create_node(OPERAND, &op_mul, cu, dv));

    return res;
}

Node* diff_div (const Node* node, const char* part)
{
    Node* du = DIFF_LEFT;    Node* cu = COPY_LEFT;
    Node* dv = DIFF_RIGHT;   Node* cv = COPY_RIGHT;

    Node* nominator = create_node(OPERAND, &op_sub, create_node(OPERAND, &op_mul, du, cv), create_node(OPERAND, &op_mul, cu, dv));
    Node* denominator = create_node(OPERAND, &op_mul, copy_subtree(cv), copy_subtree(cv));

    Node* res = create_node(OPERAND, &op_div, nominator, denominator);
    return res;
}

Node* diff_ln (const Node* node, const char* part)
{
    double temp = 1;
                    
    Node* dln = create_node(OPERAND, &op_div, create_node(NUM, &temp, NULL, NULL), COPY_RIGHT);
    Node* dx = DIFF_RIGHT;

    Node* res = create_node(OPERAND, &op_mul, dln, dx);
    return res;
}

Node* diff_pow (const Node* node, const char* part)
{
    if (node->left->type == NUM)
    {
        Node* cx = copy_subtree((Node*)node);
        Node* dx = DIFF_RIGHT;
        Node* lna = create_node(FUNCTION, op_ln, EMPtY_NODE, COPY_LEFT);
        Node* res = create_node(OPERAND, &op_mul, lna, create_node(OPERAND, &op_mul, cx, dx));

        return res;
    }
    else if (node->right->type == NUM)
    {
        double temp = 1;
        Node* degree_down = create_node(NUM, &temp, NULL, NULL);
        Node* degree = create_node(OPERAND, &op_sub, COPY_RIGHT, degree_down); 

        Node* cx = COPY_LEFT;
        Node* dx = DIFF_LEFT;

        Node* res = create_node(OPERAND, &op_mul, COPY_RIGHT, create_node(OPERAND, &op_mul, dx, create_node(OPERAND, &op_pow, cx, degree)));

        return res;
    }
    else 
    {
        Node* middle = create_node(FUNCTION, op_exp, EMPtY_NODE, create_node(OPERAND, &op_mul, COPY_RIGHT, create_node(FUNCTION, op_ln, EMPtY_NODE, COPY_LEFT)));

        return diff(middle, part);
    }
}

Node* diff_exp (const Node* node, const char* part)
{
    Node* cx = copy_subtree((Node*)node);
    Node* res = create_node(OPERAND, &op_mul, cx, DIFF_RIGHT);
    return res;   
}

Node* diff_sin (const Node* node, const char* part)
{
    Node* dsin = create_node(FUNCTION, op_cos, EMPtY_NODE, COPY_RIGHT);
    Node* dx = DIFF_RIGHT;

    Node* res = create_node(OPERAND, &op_mul, dsin, dx);
    return res;   
}

Node* diff_cos (const Node* node, const char* part)
{

    double temp = 1;
    Node* unit = create_node(NUM, &temp, NULL, NULL);
    Node* Sin = create_node(FUNCTION, op_sin, EMPtY_NODE, COPY_RIGHT);
    
    Node* dcos = create_node(OPERAND, &op_sub, unit, Sin);
    Node* dx = DIFF_RIGHT;

    Node* res = create_node(OPERAND, &op_mul, dcos, dx);
    return res;   
}

Node* diff_tg (const Node* node, const char* part)
{

    double one = 1;
    Node* unit = create_node(NUM, &one, NULL, NULL);
    Node* Cos = create_node(FUNCTION, op_cos, EMPtY_NODE, COPY_RIGHT);

    double two = 2;
    Node* Two = create_node(NUM, &two, NULL, NULL);
    Node* cos2 = create_node(FUNCTION, &op_pow, Cos, Two);
    
    Node* dtg = create_node(OPERAND, &op_div, unit, cos2);
    Node* dx = DIFF_RIGHT;

    Node* res = create_node(OPERAND, &op_mul, dtg, dx);
    return res;     
}

void dump(Node *tree)
{
    printf("\n---------------NODE_DUMP-------------\n");
    printf("type - %d\n", tree->type);
    print_data(tree);
    printf("\n---------------DUMP_END--------------\n");
}

err tree_kill (Node* head)
{
    CHECK_PTR(head);

    if (head->left != NULL)
        return tree_kill(head->left);

    if (head->right != NULL)
        return tree_kill(head->right);

    if (head->type == FUNCTION)
        free(head->data.function);

    if (head->type == VAR)
        free((char*)head->data.var);

    free(head);
    return SUCCESS;
}