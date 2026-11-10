
#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_STRING,
    AST_BOOLEAN,
    AST_VARIABLE,

    AST_BINARY,
    AST_COMPARE,
    AST_LOGICAL,
    AST_UNARY,

    AST_CREATE,
    AST_INPUT,
    AST_PRINT,

    AST_ARRAY,
    AST_INDEX,
    AST_SET_INDEX,
    AST_APPEND,
    AST_DELETE,

    AST_IF,
    AST_BLOCK,
    AST_REPEAT,
    AST_WHILE,

    AST_FUNCTION,
    AST_CALL,
    AST_RETURN,
    AST_BUILTIN

} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;

    double number;
    char text[100];
    char operator;

    ASTNode *left;
    ASTNode *right;
    ASTNode *next;
    ASTNode *else_branch;
};

ASTNode *ast_new(ASTNodeType type);
void ast_free(ASTNode *node);

#endif
