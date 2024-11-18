
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "parser.h"

// Function to initialize the token list
void initTokenList(TokenList *list) {
    list->head = NULL;
    list->tail = NULL;
}

// Function to create a new token node
TokenNode *createTokenNode(TokenType type, const char *lexeme, int line, int column) {
    TokenNode *newNode = (TokenNode *)malloc(sizeof(TokenNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->token.type = type;
    newNode->token.lexeme = strdup(lexeme);
    newNode->token.line = line;
    newNode->token.column = column;
    newNode->next = NULL;
    return newNode;
}

// Function to add a token to the list
void addToken(TokenList *list, TokenType type, const char *lexeme, int line, int column) {
    TokenNode *node = createTokenNode(type, lexeme, line, column);
    if (!list->head) {
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

// Function to free the token list
void freeTokenList(TokenList *list) {
    TokenNode *current = list->head;
    while (current) {
        TokenNode *temp = current;
        current = current->next;
        free(temp->token.lexeme);
        free(temp);
    }
}

// Function to tokenize input source code
void tokenizeSource(const char *source, TokenList *list) {
    int line = 1, column = 1;
    char buffer[256];
    int bufferIndex = 0;

    for (const char *c = source; *c; ++c) {
        if (*c == ' ' || *c == '\t' || *c == '\n' || *c == ';' || *c == ':' || *c == ',' || *c == '(' || *c == ')' || *c == '.') {
            // If buffer has content, process it
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';  // Null-terminate the buffer

                // Check if the buffer is a keyword, identifier, or literal
                if (strcmp(buffer, "program") == 0) {
                    addToken(list, TOKEN_PROGRAM, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "var") == 0) {
                    addToken(list, TOKEN_VAR, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "begin") == 0) {
                    addToken(list, TOKEN_BEGIN, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "end") == 0) {
                    addToken(list, TOKEN_END, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "integer") == 0) {
                    addToken(list, TOKEN_INTEGER, buffer, line, column - bufferIndex);
                } else {
                    // Default to identifier
                    addToken(list, TOKEN_IDENTIFIER, buffer, line, column - bufferIndex);
                }

                bufferIndex = 0;  // Reset buffer
            }

            // Process the current character as a separate token if necessary
            if (*c == ';') {
                addToken(list, TOKEN_SEMICOLON, ";", line, column);
            } else if (*c == ':') {
                addToken(list, TOKEN_COLON, ":", line, column);
            } else if (*c == ',') {
                addToken(list, TOKEN_COMMA, ",", line, column);
            } else if (*c == '(') {
                addToken(list, TOKEN_LPAREN, "(", line, column);
            } else if (*c == ')') {
                addToken(list, TOKEN_RPAREN, ")", line, column);
            } else if (*c == '.') {
                addToken(list, TOKEN_DOT, ".", line, column);
            }

            if (*c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        } else {
            // Accumulate character into buffer
            if (bufferIndex < sizeof(buffer) - 1) {
                buffer[bufferIndex++] = *c;
            } else {
                fprintf(stderr, "Token buffer overflow at line %d, column %d\n", line, column);
                exit(EXIT_FAILURE);
            }
            column++;
        }
    }

    // Add EOF token
    addToken(list, TOKEN_EOF, "EOF", line, column);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open source file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read file content into a buffer
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    // Initialize token list
    TokenList tokenList;
    initTokenList(&tokenList);

    // Tokenize the source code
    tokenizeSource(buffer, &tokenList);
    free(buffer);

    FILE *output_file = fopen("output.lex", "w");
    if (!output_file) {
        perror("Error opening output file");
        freeTokenList(&tokenList);
        return EXIT_FAILURE;
    }

    // Print lexical analysis results
    fprintf(output_file, "Lexical Analysis Results:\n");
    for (TokenNode *node = tokenList.head; node; node = node->next) {
        fprintf(output_file, "Type: %-5d, Lexeme: %-30s, Line: %-5d, Column: %-5d\n",
                node->token.type, node->token.lexeme, node->token.line, node->token.column);
    }

    // Perform syntactic and semantic analysis
    Parser parser;
    initParser(&parser, &tokenList, output_file);

    fprintf(output_file, "\nSyntactic and Semantic Analysis Results:\n");
    if (parse(&parser)) {
        if (parser.error_count == 0) {
            fprintf(output_file, "Analysis completed successfully with no errors.\n");
        } else {
            fprintf(output_file, "Analysis completed with %d errors.\n", parser.error_count);
        }
    } else {
        fprintf(output_file, "Analysis failed with fatal errors.\n");
    }

    freeParser(&parser);
    fclose(output_file);

    // Print tokens
    printf("Tokens:\n");
    for (TokenNode *node = tokenList.head; node; node = node->next) {
        printf("Type: %-5d, Lexeme: %-30s, Line: %-5d, Column: %-5d\n",
               node->token.type, node->token.lexeme, node->token.line, node->token.column);
    }

    // Free token list
    freeTokenList(&tokenList);

    return EXIT_SUCCESS;
}
