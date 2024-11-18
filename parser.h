//
// Created by rodri on 17/11/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include "symbol_table.h"

// Linked list node for tokens
typedef struct TokenNode {
    Token token;
    struct TokenNode *next;
} TokenNode;

// Linked list for managing tokens
typedef struct {
    TokenNode *head;
    TokenNode *tail;
} TokenList;

typedef struct {
    TokenList *tokens;
    TokenNode *current_token;
    SymbolTable *symbol_table;
    FILE *output_file;
    int error_count;
} Parser;

void initParser(Parser *parser, TokenList *tokens, FILE *output_file);
bool parse(Parser *parser);
void freeParser(Parser *parser);

#endif