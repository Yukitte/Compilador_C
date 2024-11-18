//
// Created by rodri on 17/11/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static void advance(Parser *parser) {
    if (parser->current_token) {
        parser->current_token = parser->current_token->next;
    }
}

static bool expect(Parser *parser, TokenType type) {
    if (parser->current_token && parser->current_token->token.type == type) {
        advance(parser);
        return true;
    }
    fprintf(parser->output_file, "Syntax Error: Expected token type %d at line %d, column %d\n",
            type, parser->current_token ? parser->current_token->token.line : -1,
            parser->current_token ? parser->current_token->token.column : -1);
    parser->error_count++;
    return false;
}

static bool parseVariableDeclaration(Parser *parser) {
    // Parse variable declarations
    while (parser->current_token && parser->current_token->token.type == TOKEN_IDENTIFIER) {
        char *var_name = strdup(parser->current_token->token.lexeme);
        advance(parser);

        if (!expect(parser, TOKEN_COLON)) return false;

        DataType var_type = TYPE_UNKNOWN;
        if (parser->current_token) {
            if (parser->current_token->token.type == TOKEN_INTEGER) {
                var_type = TYPE_INTEGER;
            } else if (parser->current_token->token.type == TOKEN_REAL) {
                var_type = TYPE_REAL;
            } else if (parser->current_token->token.type == TOKEN_BOOLEAN) {
                var_type = TYPE_BOOLEAN;
            }
        }

        if (var_type == TYPE_UNKNOWN) {
            fprintf(parser->output_file, "Semantic Error: Invalid variable type at line %d\n",
                    parser->current_token->token.line);
            parser->error_count++;
            return false;
        }

        if (!addSymbol(parser->symbol_table, var_name, var_type)) {
            fprintf(parser->output_file, "Semantic Error: Variable %s already declared at line %d\n",
                    var_name, parser->current_token->token.line);
            parser->error_count++;
        }

        free(var_name);
        advance(parser);

        if (parser->current_token && parser->current_token->token.type == TOKEN_SEMICOLON) {
            advance(parser);
        } else if (parser->current_token && parser->current_token->token.type != TOKEN_BEGIN) {
            return false;
        }
    }
    return true;
}

static bool parseProgram(Parser *parser) {
    // Parse program header
    if (!expect(parser, TOKEN_PROGRAM)) return false;
    if (!expect(parser, TOKEN_IDENTIFIER)) return false;
    if (!expect(parser, TOKEN_SEMICOLON)) return false;

    // Parse variable declarations if present
    if (parser->current_token && parser->current_token->token.type == TOKEN_VAR) {
        advance(parser);
        if (!parseVariableDeclaration(parser)) return false;
    }

    // Parse main program block
    if (!expect(parser, TOKEN_BEGIN)) return false;

    // Parse statements until END
    while (parser->current_token && parser->current_token->token.type != TOKEN_END) {
        // Here you would add statement parsing logic
        advance(parser);
    }

    if (!expect(parser, TOKEN_END)) return false;
    if (!expect(parser, TOKEN_DOT)) return false;

    return true;
}

void initParser(Parser *parser, TokenList *tokens, FILE *output_file) {
    parser->tokens = tokens;
    parser->current_token = tokens->head;
    parser->output_file = output_file;
    parser->error_count = 0;
    parser->symbol_table = (SymbolTable *)malloc(sizeof(SymbolTable));
    initSymbolTable(parser->symbol_table);
}

bool parse(Parser *parser) {
    return parseProgram(parser);
}

void freeParser(Parser *parser) {
    freeSymbolTable(parser->symbol_table);
    free(parser->symbol_table);
}
