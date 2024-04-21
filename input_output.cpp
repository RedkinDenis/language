#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <ctype.h>

#include "headers/differentiator.h"
#include "headers/input_output.h"
#include "../UDL.h"

static void get_data (char* buf, int* ptr, Node* tree, int data_len);

static void goto_prace (char* buf, int* ptr);

static err print_tree__ (Node* head, int* tab);

static err fprint_tree__ (FILE* out, Node* head, int* tab);

static int check_var (const char* data_buffer);

static void draw_tree_1 (FILE* save, Node* tree, int* node_num);

static void draw_tree_2 (FILE* save, Node* tree);

int GetFileSize(FILE* fp)
{
    int startPos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, startPos, SEEK_SET);
    return fsize;
}

err print_tree (Node* head)
{
    CHECK_PTR(head);

    int tab = 0;
    return print_tree__(head, &tab);
}

err print_tree__ (Node* head, int* tab)
{
    CHECK_PTR(head);

    printf("(");

    printf("%*s", *tab * 4, "");
    print_data(head);
    if (head->type == VAR)
    {
        printf("var - %s %p ", head->data.var, head->data.var);
    }

    if (head->left != NULL)
    {
        *tab += 1;
        printf("\n");
        print_tree__(head->left, tab);
    }

    if (head->right != NULL)
    {
        printf("\n");
        print_tree__(head->right, tab);
        *tab -= 1;
    }

    printf(")");

    return SUCCESS;
}

void print_data (Node* head)
{
    if (head->type == NUM)
        printf("#%lf#", head->data.value);
        
    else if (head->type == OPERAND)
        printf("#%c#", head->data.operand); 
        
    else if (head->type == FUNCTION)
        printf("#%s#", head->data.function); 

    else if (head->type == VAR)
        printf("#%s#", head->data.var); 

    else if (head->type ==  EMPtY)
        printf("#null#");
    
    return;
}

err fprint_tree (FILE* out, Node* head)
{
    CHECK_PTR(out);
    CHECK_PTR(head);

    int tab = 0;
    return fprint_tree__(out, head, &tab);
}

err fprint_tree__ (FILE* out, Node* head, int* tab)
{
    CHECK_PTR(head);

    fprintf(out, "(");

    fprintf(out, "%*s", *tab * 4, "");

    fprintf(out, "*%s*", head->data);

    if (head->left != NULL)
    {
        *tab += 1;
        fprintf(out, "\n");
        fprint_tree__(out, head->left, tab);
    }

    if (head->right != NULL)
    {
        fprintf(out, "\n");
        fprint_tree__(out, head->right, tab);
        *tab -= 1;
    }

    fprintf(out, ")");

    return SUCCESS;
}

err fill_buffer (FILE* read, char** buf)
{
    CHECK_PTR(read);
    CHECK_PTR(buf);

    int fsize = GetFileSize(read);

    CALLOC(*buf, char, (fsize + 1));

    fread(*buf, sizeof(char), fsize, read);

    return SUCCESS;
}

void remove_enters (char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
        if (str[i] == '\n' || str[i] == '\r')
            str[i] = ' ';
}

err importTree (FILE* read, Node* tree)
{
    CHECK_PTR(read);
    CHECK_PTR(tree);

    char* buf = 0;
    fill_buffer(read, &buf);

    int level = 0, ptr = 0;

    goto_prace(buf, &ptr);

    if (buf[ptr] == '(')
    {
        ptr++;
        level++;
        get_data(buf, &ptr, tree, DATA_LEN);
    }

    while (level > 0)
    {
        if (buf[ptr] == '(')
        {
            ptr++;
            CHANGE_NODE(tree, tree->left);

            get_data(buf, &ptr, tree, DATA_LEN);
        }
        else if (buf[ptr] == ')')
        {
            tree = tree->parent;
            level--;

            if (level == 0)
                break;

            ptr++;

            goto_prace(buf, &ptr);

            if (buf[ptr] == '(')
            {
                ptr++;

                CHANGE_NODE(tree, tree->right);

                get_data(buf, &ptr, tree, DATA_LEN);
            }
        }
    }
    free(buf);

    return SUCCESS;
}

void goto_prace (char* buf, int* ptr)
{
    while((buf[*ptr] != '(') && (buf[*ptr] != ')'))
        *ptr += 1;
}

void get_data(char* buf, int* ptr, Node* tree, int data_len)
{
    char* data_buffer = (char*)calloc(DATA_LEN + 1, sizeof(char));
    while (buf[*ptr - 1] != '#')
        *ptr += 1;

    int i = 0;
    while ((buf[*ptr] != '#') && i <= data_len)
    {
        data_buffer[i] = buf[*ptr];
        *ptr += 1;
        i++;
    }

    // printf("data_buffer - %s\n", data_buffer);
    
    if (check_var(data_buffer))
    {
        tree->data.var = strdup(data_buffer);
        tree->type = VAR;

        free(data_buffer);
        goto_prace(buf, ptr);
        return;
    }
    else 
    {
        int x = sscanf(data_buffer, "%lf", &(tree->data.value));
        if (x == 1) 
        {
            tree->type = NUM;
            free(data_buffer);
            goto_prace(buf, ptr);
            return;
        }
    }
    if (strlen(data_buffer) == 1)
    {
        tree->data.operand = data_buffer[0];
        tree->type = OPERAND;
    }
    else
    {
        if (strcmp(data_buffer, "null") == 0)
        {
            //printf("here\n");
            tree->type = EMPtY;
        }
        else
            tree->type = FUNCTION;

        tree->data.function = (char*)calloc(strlen(data_buffer) + 1, sizeof(char));
        strcpy(tree->data.function, data_buffer);    
    }

    free(data_buffer);
    goto_prace(buf, ptr);
}

int check_var (const char* data_buffer)
{
    int len = (int)strlen(data_buffer);
    // printf("len - %d\n", len);
    if (isalpha(data_buffer[len - 1]))
    {
        if (len == 1)
            return 1;
        for (int i = len - 1; i >= 0; i--)
        {   
            if (('0' <= data_buffer[i]) && (data_buffer[i] <= '9'))
            {
                // printf("%s\n", data_buffer);
                return 1;
            }
        }
    }
    return 0;
}

err draw_tree (Node* tree)
{
    FOPEN(save, "drawTree.txt", "wb");

    fprintf(save, "digraph Tree {\n");

    int node_num = 0;
    draw_tree_1(save, tree, &node_num);
    draw_tree_2(save, tree);

    fprintf(save, "}");

    fclose(save);

    system("iconv -f WINDOWS-1251 -t UTF-8 drawTree.txt > buffer.txt");
    system("dot buffer.txt -Tpng -o drawTree.png");
    system("start drawTree.png");
    return SUCCESS;
}

char* data_to_string (Node* tree)
{
    char* data = 0;
    if (tree->type == NUM)
    {
        data = (char*)calloc(DATA_LEN + 1, sizeof(char));
        sprintf(data, "%f", tree->data);
    }
    else if (tree->type == OPERAND)
    {
        data = (char*)calloc(2, sizeof(char));
        sprintf(data, "%c", tree->data);
    }
    else if (tree->type == FUNCTION)
    {
        data = (char*)calloc(strlen(tree->data.function) + 1, sizeof(char));
        strcpy(data, tree->data.function);
    }    
    else if (tree->type == VAR)
    {
        data = (char*)calloc(strlen(tree->data.var) + 1, sizeof(char));
        strcpy(data, tree->data.var);
    }
    else if (tree->type == EMPtY)
        data = (char*)"null";

    return data;
}

char* type_to_str (Node* tree)
{
    char* type = (char*)calloc(20, sizeof(char));
    
    switch (tree->type)
    {
        case NUM:
            strcpy(type, "NUM");
            break;
        case OPERAND:
            strcpy(type, "OPERAND");
            break;
        case VAR:
            strcpy(type, "VAR");
            break;
        case FUNCTION:
            strcpy(type, "FUNCTION");
            break;
    }
    return type;
}

void draw_tree_1 (FILE* save, Node* tree, int* node_num)
{
    char* data = data_to_string(tree);
    char* buffer = (char*)calloc(strlen(data) + 1, sizeof(char));
    strcat(buffer, data);                                                

    tree->num_in_tree = *node_num;

    char* type = type_to_str(tree);

    if (tree->type == NUM || tree->type == VAR)
        fprintf(save, "    %d [shape = Mrecord, style = filled, fillcolor = lightgoldenrod1, label = %cTYPE: %s | DATA: %s | CODE: %d%c];\n", *node_num, '"', type, buffer, tree->code, '"');
    else if (tree->type != EMPtY)
        fprintf(save, "    %d [shape = Mrecord, style = filled, fillcolor = cyan, label = %cTYPE: %s | DATA: %s | CODE: %d%c];\n", *node_num, '"', type, buffer, tree->code, '"');
    else 
        *node_num -= 1;

    if (tree->left != NULL)
    {
        *node_num += 1;
        draw_tree_1(save, tree->left, node_num);
    }

    if (tree->right != NULL)
    {
        *node_num += 1;
        draw_tree_1(save, tree->right, node_num);
    }

    return;
}

void draw_tree_2 (FILE* save, Node* tree)
{
    if (tree->left != NULL)
    {
        fprintf(save, "    %d -> %d;\n", tree->num_in_tree, (tree->left)->num_in_tree);
        draw_tree_2(save, tree->left);
    }

    if (tree->right != NULL)
    {
        fprintf(save, "    %d -> %d;\n", tree->num_in_tree, (tree->right)->num_in_tree);
        draw_tree_2(save, tree->right);
    }
    return;
}

Data input_data (const char* file_name)
{
    FILE* input = fopen(file_name, "rb");
    int fsize = GetFileSize(input);

    char* buffer = NULL;
    fill_buffer(input, &buffer);

    char* buffer_cp = buffer;

    // printf ("Buffer: \n %s", buffer);

    fclose(input);

    Data data = {};

    for (int i = 0; i < fsize + 1; i++)
        if (buffer[i] == '\n' && buffer[i - 1] == '\r')
        {
            data.quant += 1;
            buffer[i] = '\0';
            buffer[i - 1] = '\0';
        }

    data.lines = (line*)calloc(data.quant, sizeof(line));

    if (buffer[fsize - 1] != '\n')
        data.quant += 1;

    for (int i = 0; i < data.quant; i++)
    {
        data.lines[i].str = strdup(buffer);
        data.lines[i].len = strlen(buffer);
        buffer += strlen(buffer);

        while (*buffer == '\0') 
            buffer++;
    }

    free(buffer_cp);

    return data;
}

void clear_data (Data* data)
{
    for (int i = 0; i < data->quant; i++)
        free(data->lines[i].str);

    free(data->lines);
    // free(data);
}

void dump_data (Data* data)
{
    printf ("qant - %d \n", data->quant);
    for (int i = 0; i < data->quant; i++)
        printf("%s \n", data->lines[i].str);
}