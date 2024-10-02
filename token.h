#ifndef TOKEN_H
#define TOKEN_H

// Tipos de tokens
typedef enum {
    // Palavras-chave
    TOKEN_PROGRAM,
    TOKEN_VAR,
    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_BOOLEAN,
    TOKEN_PROCEDURE,
    TOKEN_BEGIN,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_READ,
    TOKEN_WRITE,

    // Operadores
    TOKEN_ASSIGN,    // :=
    TOKEN_PLUS,      // +
    TOKEN_MINUS,     // -
    TOKEN_MULTIPLY,  // *
    TOKEN_DIVIDE,    // /
    TOKEN_EQ,        // =
    TOKEN_NEQ,       // <>
    TOKEN_LT,        // <
    TOKEN_GT,        // >
    TOKEN_LTE,       // <=
    TOKEN_GTE,       // >=

    // Pontuação
    TOKEN_SEMICOLON, // ;
    TOKEN_COLON,     // :
    TOKEN_COMMA,     // ,
    TOKEN_DOT,       // .
    TOKEN_LPAREN,    // (
    TOKEN_RPAREN,    // )

    // Outros
    TOKEN_IDENTIFIER,        // Identificador
    TOKEN_INTEGER_LITERAL,   // Literal inteiro
    TOKEN_REAL_LITERAL,      // Literal real
    TOKEN_BOOLEAN_LITERAL,   // Literal booleano
    TOKEN_STRING_LITERAL,    // Literal de string

    // Tokens especiais
    TOKEN_EOF,               // Fim de arquivo
    TOKEN_ERROR,             // Erro
} TokenType;

// Estrutura do Token
typedef struct {
    TokenType type;
    char *lexeme;
    int line;
    int column;
} Token;

// Protótipos de função
Token *create_token(TokenType type, const char *lexeme, int line, int column);
void free_token(Token *token);

#endif // TOKEN_H
