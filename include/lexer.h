
#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_CREATE,
    TOKEN_PRINT,
    TOKEN_INPUT,

    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_END,

    TOKEN_REPEAT,
    TOKEN_WHILE,

    TOKEN_FUNCTION,
    TOKEN_CALL,
    TOKEN_RETURN,

    TOKEN_TRUE,
    TOKEN_FALSE,

    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    TOKEN_APPEND,
    TOKEN_DELETE,
    TOKEN_SET,

    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    TOKEN_ASSIGN,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,

    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char value[100];
    int line;
} Token;

void tokenize(const char *source);

#endif
