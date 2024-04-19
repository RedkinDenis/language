#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <cstddef>
#include "..\..\err_codes.h"   
#include "../headers/DSLdiff.h"

#define DATA_LEN 40

enum data_t
{
    NUM = 1,
    VAR = 4,
    OPERAND = 2,
    FUNCTION = 3,
    DEFUALT = 0
};

enum operation
{
    ERR = 0, 
    ADD = '+', 
    SUB = '-', 
    MUL = '*', 
    DIV = '/', 
    LN = 'l', 
    EXP = 'e',
    POW = '^',
    SIN = 's',
    COS = 'c',
    TG = 't',
    ASS = '=',
    COM = ';'
};

union data
{
    unsigned char operand;
    char* var;
    char* function;
    double value;
};

struct Node
{
    data_t type = DEFUALT;
    
    data data = {};
    
    Node* left = NULL;
    Node* right = NULL;
    
    Node* parent = NULL;

    int num_in_tree = 0;
};

static operation op_add = ADD;        
static operation op_mul = MUL;        
static operation op_sub = SUB;        
static operation op_div = DIV;        
static operation op_pow = POW; 
static operation op_ass = ASS;  
static operation op_com = COM;     
static char* op_exp = (char*)"exp";   
static char* op_ln = (char*)"ln";     
static char* op_sin = (char*)"sin";   
static char* op_cos = (char*)"cos";   
static char* op_tg = (char*)"tg";   

err tree_kill (Node* head);

double calculator (Node* tree, int* var = NULL);

Node* diff (const Node* node, const char* part = "x");

Node* simplifier (Node* tree);

Node* create_node (data_t type, void* data, Node* left, Node* right);

operation long_op_det (char* str, char** s = NULL);

#endif //DIFFERENTIATOR_H