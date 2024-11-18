//
// Created by rodri on 17/11/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

void initSymbolTable(SymbolTable *table) {
    table->head = NULL;
    table->current_scope = 0;
}

bool addSymbol(SymbolTable *table, const char *name, DataType type) {
    // Check if symbol already exists in current scope
    Symbol *existing = findSymbol(table, name);
    if (existing && existing->scope == table->current_scope) {
        return false;
    }

    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = type;
    new_symbol->scope = table->current_scope;
    new_symbol->next = table->head;
    table->head = new_symbol;
    return true;
}

Symbol* findSymbol(SymbolTable *table, const char *name) {
    Symbol *current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void freeSymbolTable(SymbolTable *table) {
    Symbol *current = table->head;
    while (current) {
        Symbol *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    table->head = NULL;
}