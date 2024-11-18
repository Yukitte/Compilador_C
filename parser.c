#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Avançar para o próximo token
static void advance(Parser *parser) {
    if (parser->current_token) {
        parser->current_token = parser->current_token->next;
    }
}

// Esperar o próximo token e verificar se é do tipo esperado
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

// Função semântica para verificação de tipo e declaração de variável
static bool parseVariableDeclaration(Parser *parser) {
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

        // Verificação semântica para garantir que a variável não foi declarada
        if (!addSymbol(parser->symbol_table, var_name, var_type)) {
            fprintf(parser->output_file, "Semantic Error: Variable %s already declared at line %d\n",
                    var_name, parser->current_token->token.line);
            parser->error_count++;
        }

        free(var_name);
        advance(parser);

        // Se encontramos um ponto e vírgula, avançamos
        if (parser->current_token && parser->current_token->token.type == TOKEN_SEMICOLON) {
            advance(parser);
        } else if (parser->current_token && parser->current_token->token.type != TOKEN_BEGIN) {
            return false;
        }
    }
    return true;
}

// Função para verificar a sintaxe do programa inteiro
static bool parseProgram(Parser *parser) {
    // Cabeçalho do programa
    if (!expect(parser, TOKEN_PROGRAM)) return false;
    if (!expect(parser, TOKEN_IDENTIFIER)) return false;
    if (!expect(parser, TOKEN_SEMICOLON)) return false;

    // Declarações de variáveis, se houver
    if (parser->current_token && parser->current_token->token.type == TOKEN_VAR) {
        advance(parser);
        if (!parseVariableDeclaration(parser)) return false;
    }

    // Bloco principal do programa
    if (!expect(parser, TOKEN_BEGIN)) return false;

    // Processamento de comandos dentro do bloco
    while (parser->current_token && parser->current_token->token.type != TOKEN_END) {
        // Aqui você pode adicionar a lógica de parsing para sentenças (ex: expressões, comandos)
        advance(parser);
    }

    if (!expect(parser, TOKEN_END)) return false;
    if (!expect(parser, TOKEN_DOT)) return false;

    return true;
}

// Inicialização do parser
void initParser(Parser *parser, TokenList *tokens, FILE *output_file) {
    parser->tokens = tokens;
    parser->current_token = tokens->head;
    parser->output_file = output_file;
    parser->error_count = 0;
    parser->symbol_table = (SymbolTable *)malloc(sizeof(SymbolTable));
    initSymbolTable(parser->symbol_table);
}



const char* getTokenTypeName(TokenType type) {
    switch (type) {
        // Palavras-chave
        case TOKEN_PROGRAM: return "Palavra reservada: program";
        case TOKEN_VAR: return "Palavra reservada: var";
        case TOKEN_INTEGER: return "Palavra reservada: integer";
        case TOKEN_REAL: return "Palavra reservada: real";
        case TOKEN_BOOLEAN: return "Palavra reservada: boolean";
        case TOKEN_PROCEDURE: return "Palavra reservada: procedure";
        case TOKEN_BEGIN: return "Palavra reservada: begin";
        case TOKEN_END: return "Palavra reservada: end";
        case TOKEN_IF: return "Palavra reservada: if";
        case TOKEN_THEN: return "Palavra reservada: then";
        case TOKEN_ELSE: return "Palavra reservada: else";
        case TOKEN_WHILE: return "Palavra reservada: while";
        case TOKEN_DO: return "Palavra reservada: do";
        case TOKEN_READ: return "Palavra reservada: read";
        case TOKEN_WRITE: return "Palavra reservada: write";

        // Operadores
        case TOKEN_ASSIGN: return "Operador: :=";
        case TOKEN_PLUS: return "Operador: +";
        case TOKEN_MINUS: return "Operador: -";
        case TOKEN_MULTIPLY: return "Operador: *";
        case TOKEN_DIVIDE: return "Operador: /";
        case TOKEN_EQ: return "Operador: =";
        case TOKEN_NEQ: return "Operador: <>";
        case TOKEN_LT: return "Operador: <";
        case TOKEN_GT: return "Operador: >";
        case TOKEN_LTE: return "Operador: <=";
        case TOKEN_GTE: return "Operador: >=";

        // Pontuação
        case TOKEN_SEMICOLON: return "Pontuação: ;";
        case TOKEN_COLON: return "Pontuação: :";
        case TOKEN_COMMA: return "Pontuação: ,";
        case TOKEN_DOT: return "Pontuação: .";
        case TOKEN_LPAREN: return "Pontuação: (";
        case TOKEN_RPAREN: return "Pontuação: )";

        // Outros
        case TOKEN_IDENTIFIER: return "Identificador";
        case TOKEN_INTEGER_LITERAL: return "Literal inteiro";
        case TOKEN_REAL_LITERAL: return "Literal real";
        case TOKEN_BOOLEAN_LITERAL: return "Literal booleano";
        case TOKEN_STRING_LITERAL: return "Literal de string";

        // Tokens especiais
        case TOKEN_EOF: return "Fim de arquivo (EOF)";
        case TOKEN_ERROR: return "Erro";

        default: return "Token desconhecido";
    }
}
void printTokenInfo(Parser *parser) {
    if (parser->current_token) {
        const char *token_type_name = getTokenTypeName(parser->current_token->token.type);
        fprintf(parser->output_file, "Token: %-20s, Lexema: %-30s, Linha: %-5d, Coluna: %-5d\n",
                token_type_name, parser->current_token->token.lexeme,
                parser->current_token->token.line, parser->current_token->token.column);
    }
}   

// Função principal de parsing, chama `parseProgram`
bool parse(Parser *parser) {
    return parseProgram(parser);
}

// Liberação de memória do parser
void freeParser(Parser *parser) {
    freeSymbolTable(parser->symbol_table);
    free(parser->symbol_table);
}
