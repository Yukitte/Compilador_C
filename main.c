#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "tabelaHash.c"

#define MAX_IDENTIFIER_LENGTH 100
#define MAX_NUMBER_LENGTH 20
#define MAX_STRING_LENGTH 1000

HashTable* keyword_table;

// Função para converter o tipo TokenType em string
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_PROGRAM: return "TOKEN_PROGRAM";
        case TOKEN_VAR: return "TOKEN_VAR";
        case TOKEN_INTEGER: return "TOKEN_INTEGER";
        case TOKEN_REAL: return "TOKEN_REAL";
        case TOKEN_BOOLEAN: return "TOKEN_BOOLEAN";
        case TOKEN_PROCEDURE: return "TOKEN_PROCEDURE";
        case TOKEN_BEGIN: return "TOKEN_BEGIN";
        case TOKEN_END: return "TOKEN_END";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_THEN: return "TOKEN_THEN";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_DO: return "TOKEN_DO";
        case TOKEN_READ: return "TOKEN_READ";
        case TOKEN_WRITE: return "TOKEN_WRITE";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_MULTIPLY: return "TOKEN_MULTIPLY";
        case TOKEN_DIVIDE: return "TOKEN_DIVIDE";
        case TOKEN_EQ: return "TOKEN_EQ";
        case TOKEN_NEQ: return "TOKEN_NEQ";
        case TOKEN_LT: return "TOKEN_LT";
        case TOKEN_GT: return "TOKEN_GT";
        case TOKEN_LTE: return "TOKEN_LTE";
        case TOKEN_GTE: return "TOKEN_GTE";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_DOT: return "TOKEN_DOT";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_INTEGER_LITERAL: return "TOKEN_INTEGER_LITERAL";
        case TOKEN_REAL_LITERAL: return "TOKEN_REAL_LITERAL";
        case TOKEN_BOOLEAN_LITERAL: return "TOKEN_BOOLEAN_LITERAL";
        case TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";
        case TOKEN_EOF: return "TOKEN_EOF";
        case TOKEN_ERROR: return "\e[0;31mTOKEN_ERROR\e[0m";
        default: return "UNKNOWN_TOKEN";
    }
}

// Inicializa a tabela de palavras-chave
void init_keyword_table() {
    keyword_table = create_table(CAPACITY);
    ht_insert(keyword_table, "program", "TOKEN_PROGRAM");
    ht_insert(keyword_table, "var", "TOKEN_VAR");
    ht_insert(keyword_table, "integer", "TOKEN_INTEGER");
    ht_insert(keyword_table, "real", "TOKEN_REAL");
    ht_insert(keyword_table, "boolean", "TOKEN_BOOLEAN");
    ht_insert(keyword_table, "procedure", "TOKEN_PROCEDURE");
    ht_insert(keyword_table, "begin", "TOKEN_BEGIN");
    ht_insert(keyword_table, "end", "TOKEN_END");
    ht_insert(keyword_table, "if", "TOKEN_IF");
    ht_insert(keyword_table, "then", "TOKEN_THEN");
    ht_insert(keyword_table, "else", "TOKEN_ELSE");
    ht_insert(keyword_table, "while", "TOKEN_WHILE");
    ht_insert(keyword_table, "do", "TOKEN_DO");
    ht_insert(keyword_table, "read", "TOKEN_READ");
    ht_insert(keyword_table, "write", "TOKEN_WRITE");
    ht_insert(keyword_table, "true", "TOKEN_BOOLEAN_LITERAL");
    ht_insert(keyword_table, "false", "TOKEN_BOOLEAN_LITERAL");
}

// Obtém o token correspondente à palavra-chave
TokenType get_keyword_token(const char* lexeme) {
    char* token_str = ht_search(keyword_table, lexeme);
    if (token_str == NULL) {
        return TOKEN_IDENTIFIER;
    }
    if (strcmp(token_str, "TOKEN_BOOLEAN_LITERAL") == 0) {
        return TOKEN_BOOLEAN_LITERAL;
    }
    return (TokenType)atoi(token_str);
}

// Cria um novo token
Token* create_token(TokenType type, const char* lexeme, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = strdup(lexeme);
    token->line = line;
    token->column = column;
    return token;
}

// Libera a memória de um token
void free_token(Token* token) {
    if (token) {
        free(token->lexeme);
        free(token);
    }
}

// Obtém o próximo token do arquivo
Token* get_next_token(FILE* file, int* line, int* column) {
    int ch = fgetc(file);
    while (isspace(ch)) {
        if (ch == '\n') {
            (*line)++;
            *column = 0;
        } else {
            (*column)++;
        }
        ch = fgetc(file);
    }

    if (ch == EOF) {
        return create_token(TOKEN_EOF, "EOF", *line, *column);
    }

    char lexeme[MAX_STRING_LENGTH + 1];
    int lexeme_len = 0;

    // Lida com comentários
    if (ch == '{') {
        while ((ch = fgetc(file)) != EOF && ch != '}') {
            if (ch == '\n') {
                (*line)++;
                *column = 0;
            } else {
                (*column)++;
            }
        }
        if (ch == EOF) {
            return create_token(TOKEN_ERROR, "Comentario nao fechado", *line, *column);
        }
        (*column)++;
        return get_next_token(file, line, column); // Chamada recursiva para pegar o próximo token
    }

    if (isalpha(ch) || ch == '_') {
        // Identificador ou palavra-chave
        while (isalnum(ch) || ch == '_') {
            lexeme[lexeme_len++] = ch;
            (*column)++;
            ch = fgetc(file);
        }
        ungetc(ch, file);
        lexeme[lexeme_len] = '\0';
        TokenType type = get_keyword_token(lexeme);
        return create_token(type, lexeme, *line, *column - lexeme_len + 1);
    }

    if (isdigit(ch)) {
        // Número (inteiro ou real)
        while (isdigit(ch) || ch == '.') {
            lexeme[lexeme_len++] = ch;
            (*column)++;
            ch = fgetc(file);
        }
        ungetc(ch, file);
        lexeme[lexeme_len] = '\0';
        TokenType type = (strchr(lexeme, '.') != NULL) ? TOKEN_REAL_LITERAL : TOKEN_INTEGER_LITERAL;
        return create_token(type, lexeme, *line, *column - lexeme_len + 1);
    }

    if (ch == '\'') {
        // Literal de string
        lexeme[lexeme_len++] = ch;
        (*column)++;
        while ((ch = fgetc(file)) != EOF && ch != '\'') {
            if (lexeme_len >= MAX_STRING_LENGTH) {
                return create_token(TOKEN_ERROR, "Literal de string muito longo", *line, *column);
            }
            lexeme[lexeme_len++] = ch;
            (*column)++;
            if (ch == '\n') {
                return create_token(TOKEN_ERROR, "Literal de string não fechado", *line, *column);
            }
        }
        if (ch == EOF) {
            return create_token(TOKEN_ERROR, "Literal de string não fechado", *line, *column);
        }
        lexeme[lexeme_len++] = ch;
        lexeme[lexeme_len] = '\0';
        (*column)++;
        return create_token(TOKEN_STRING_LITERAL, lexeme, *line, *column - lexeme_len + 1);
    }

    // Tokens de um único caractere e operadores
    lexeme[0] = ch;
    lexeme[1] = '\0';
    (*column)++;

    switch (ch) {
        case '+': return create_token(TOKEN_PLUS, lexeme, *line, *column);
        case '-': return create_token(TOKEN_MINUS, lexeme, *line, *column);
        case '*': return create_token(TOKEN_MULTIPLY, lexeme, *line, *column);
        case '/': return create_token(TOKEN_DIVIDE, lexeme, *line, *column);
        case '=': return create_token(TOKEN_EQ, lexeme, *line, *column);
        case '<':
            ch = fgetc(file);
            if (ch == '=') {
                lexeme[1] = '=';
                lexeme[2] = '\0';
                (*column)++;
                return create_token(TOKEN_LTE, lexeme, *line, *column - 1);
            } else if (ch == '>') {
                lexeme[1] = '>';
                lexeme[2] = '\0';
                (*column)++;
                return create_token(TOKEN_NEQ, lexeme, *line, *column - 1);
            } else {
                ungetc(ch, file);
                return create_token(TOKEN_LT, lexeme, *line, *column);
            }
        case '>':
            ch = fgetc(file);
            if (ch == '=') {
                lexeme[1] = '=';
                lexeme[2] = '\0';
                (*column)++;
                return create_token(TOKEN_GTE, lexeme, *line, *column - 1);
            } else {
                ungetc(ch, file);
                return create_token(TOKEN_GT, lexeme, *line, *column);
            }
        case ':':
            ch = fgetc(file);
            if (ch == '=') {
                lexeme[1] = '=';
                lexeme[2] = '\0';
                (*column)++;
                return create_token(TOKEN_ASSIGN, lexeme, *line, *column - 1);
            } else {
                ungetc(ch, file);
                return create_token(TOKEN_COLON, lexeme, *line, *column);
            }
        case ';': return create_token(TOKEN_SEMICOLON, lexeme, *line, *column);
        case ',': return create_token(TOKEN_COMMA, lexeme, *line, *column);
        case '.': return create_token(TOKEN_DOT, lexeme, *line, *column);
        case '(': return create_token(TOKEN_LPAREN, lexeme, *line, *column);
        case ')': return create_token(TOKEN_RPAREN, lexeme, *line, *column);
    }

    // Caractere não reconhecido
    return create_token(TOKEN_ERROR, lexeme, *line, *column);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo: %s\n", argv[1]);
        return 1;
    }

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s.lex", argv[1]);
    FILE* output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Erro ao criar o arquivo de saída: %s\n", output_filename);
        fclose(input_file);
        return 1;
    }

    init_keyword_table();

    int line = 1, column = 0;
    Token* token;

    do {
        token = get_next_token(input_file, &line, &column);

        // Imprimindo no arquivo de saída
        fprintf(output_file, "Token: %-20s, Lexema: %-15s, Linha: %d, Coluna: %d\n",
            token_type_to_string(token->type), token->lexeme, token->line, token->column);

        // Imprimindo no terminal
        printf("Token: %-20s, Lexema: %-15s, Linha: %d, Coluna: %d\n",
            token_type_to_string(token->type), token->lexeme, token->line, token->column);

        free_token(token);
    } while (token->type != TOKEN_EOF && token->type != TOKEN_ERROR);

    fclose(input_file);
    fclose(output_file);
    free_table(keyword_table);

    return 0;
}
