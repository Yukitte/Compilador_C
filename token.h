#ifndef TOKEN_H
#define TOKEN_H

// Token types
typedef enum {
    // Keywords
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

    // Operators
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

    // Punctuation
    TOKEN_SEMICOLON, // ;
    TOKEN_COLON,     // :
    TOKEN_COMMA,     // ,
    TOKEN_DOT,       // .
    TOKEN_LPAREN,    // (
    TOKEN_RPAREN,    // )

    // Others
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER_LITERAL,
    TOKEN_REAL_LITERAL,
    TOKEN_BOOLEAN_LITERAL,
    TOKEN_STRING_LITERAL,

    // Special tokens
    TOKEN_EOF,
    TOKEN_ERROR,
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *lexeme;
    int line;
    int column;
} Token;

// Function prototypes
Token *create_token(TokenType type, const char *lexeme, int line, int column);
void free_token(Token *token);

#endif // TOKEN_H