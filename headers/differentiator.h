#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include <cstddef>
#include "..\..\err_codes.h"   
#include "../headers/DSLdiff.h"
#include "encoding.h"

#define DATA_LEN 40

enum data_t
{
    NUM = 1,
    VAR = 4,
    OPERAND = 2,
    FUNCTION = 3,
    EMPtY = 0
};

enum operation
{
    opUNKNOWN = 0, 
    opADD = '+', 
    opSUB = '-', 
    opMUL = '*', 
    opDIV = '/', 
    opLN = 'l', 
    opEXP = 'e',
    opPOW = '^',
    opSIN = 's',
    opCOS = 'c',
    opTG = 't',
    opASS = '=',
    opCOM = ';',
    opMORE = '>',
    opLESS = '<',
    opEQ = '~'
};

union data
{
    unsigned char operand;
    const char* var;
    char* function;
    double value;
};

struct Node
{
    data_t type = EMPtY;
    byte_codes code = DEFAULT;
    
    data data = {};
    
    Node* left = NULL;
    Node* right = NULL;
    
    Node* parent = NULL;

    int num_in_tree = 0;
};

static operation op_add = opADD;        
static operation op_mul = opMUL;        
static operation op_sub = opSUB;        
static operation op_div = opDIV;        
static operation op_pow = opPOW;   
static operation op_com = opCOM;  
static char* op_more = (char*)"more";  
static char* op_less = (char*)"less";     
static char* op_exp = (char*)"exp";   
static char* op_ln = (char*)"ln";     
static char* op_sin = (char*)"sin";   
static char* op_cos = (char*)"cos";   
static char* op_tg = (char*)"tg";     
static char* op_if = (char*)"if";     
static char* op_else = (char*)"else";     
static char* op_while = (char*)"while";    
static char* op_ass = (char*)"=";  
static char* op_eq = (char*)"~";     

err tree_kill (Node* head);

double calculator (Node* tree, int* var = NULL);

Node* diff (const Node* node, const char* part = "x");

Node* simplifier (Node* tree);

Node* create_node (data_t type, void* data, Node* left, Node* right, byte_codes code = DEFAULT);

operation long_op_det (char* str, char** s = NULL);

#endif //DIFFERENTIATOR_H